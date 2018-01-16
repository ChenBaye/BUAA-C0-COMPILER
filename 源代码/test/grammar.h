#ifndef _GRAMMAR_H_
#define	_GRAMMAR_H_

#include"getsym.h"
#include"midcode.h"
#include"error.h"
#include<stdio.h> 
#include<ctype.h>
#include<string.h>
#include <stdlib.h>


//���ű����
#define tabsize 256
struct tabentry{
	char id[tokensize];//ȡǰtokensize���ַ�
	int type;	//0-int, 1-char ,2-const int, 3-const char,4-int����,5-char����,
				//6-����int�ĺ���,7-����char�ĺ���,8-����void�ĺ���,9-int�Ͳ���,10-char�Ͳ���
	int len;	//���鳤�Ȼ�����������������ֵ
	int lev;	//ֻ����Ϊ0(ȫ�ֱ�ʶ��:������ȫ�ֳ�����ȫ�ֱ���)��1(�ֲ���ʶ��:�ֲ��������ֲ�����)
	int size;	//��������ռ�ֽڴ�С�����в�������������ռ�ֽڴ�С֮��
	int addr;	//��ʶ���洢��ַƫ��(����ڱ�����)
};

								//������ʱ�������ĸ�����Ϣ
char id[tokensize]={'\0'};
int type=0;
int len=0;
int addr=0;
int lev=0;
int size=0;

struct tabentry tab[tabsize];		//���ű�
int tablen=0;						//�Ѿ�����tablen����ʶ��
//int globalindex[tabsize]={0};		//����ȫ�ֱ�����������tab������
int globallen=0;					//�Ѿ�����globallen��ȫ����,ȫ�ֱ���һ�����ڷ��ű��ǰgloballen��
int funcindex[tabsize]={0};			//����������tab������
int	funclen=0;						//�Ѿ�����funclen������

char token1[tokensize]={'\0'};	//����Ԥ��ǰ����Ϣ
int symid1=0;
char token2[tokensize]={'\0'};	//����Ԥ��ǰ����Ϣ
int symid2=0;

//*********************************
//��������
//*********************************
int sizecalcu(int type,int len);							//�����ʶ����Ӧ����ռ�ڴ��С
void inserttab(char* id,int type,int len,int lev,int size,int addr);	//����ű�(���������׶�)
int searchtab(char* id,int whichtype);						//����ű�(����ʹ�ý׶�)
void constdef();			//����������
void conststa();			//������˵��
void variabledef();			//�����������
void compoundsta();			//���������
void hasretfunc();			//�����з���ֵ����
void noretfunc();			//�����޷���ֵ����
void mainfunction();		//����������
int  integer();				//�����������
void program();				//�����﷨����
int  paralist();			//���������
int	 expression(char* temp);//������ʽ(����ֵ��ʾ����0-int �� 1-char)
void stalist();				//���������
void sta();					//�������
int  term(char* temp);		//������(����ֵ��ʾ����0-int �� 1-char)
int  factor(char* temp);	//��������(����ֵ��ʾ����0-int �� 1-char)
void valueofpara(int index);//����ֵ������(����Ϊ�����ڷ��ű������)
void returnsta();			//���������
void printsta();			//����д���
void scansta();				//��������
void ifsta();				//����if-else
void switchsta();			//����switch-case
void forsta();				//����forѭ��
void assignsta();			//����ֵ���
void callretfunc(int index);	//�����з���ֵ�����ĵ���(����Ϊ�����ڷ��ű������)
void callnoretfunc(int index);	//�����޷���ֵ��������(����Ϊ�����ڷ��ű������)
int	 isstahead(int symid);	//�����ܵĵ�һ������
void condition();			//��������
void casesta(char* temp_exp,char* label,char* default_label,int switch_type);//����������
void defaultsta(char* default_label);			//ȱʡ�������
void casesubsta(char* temp_exp,char* mylabel,char* nextlabel,int switch_type);			//����������


//������������ֽ���,Ϊ���ڴ���int��char��Ϊ4�ֽ�
int sizecalcu(int type,int len){
	if(type==1 || type==3 || type==10){				//const char,char,char�Ͳ���
		return sizeof(int);
	}
	else if(type==0 || type==2 || type==9){			//const int,int,int�Ͳ���
		return sizeof(int);
	}
	else if(type==4){								//int����
		return len*sizeof(int);
	}
	else{											//char����
		return len*sizeof(int);
	}
}




//����ű�
void inserttab(char* id,int type,int len,int lev,int size,int addr){
	if(tablen>=tabsize){
		//error:tab����
		errormessage(OUTOFTAB_ERROR,line,no);
		return;
	}
	//1.ȫ������������ȫ������������������
	//2.ͬһ�����ľֲ�������������
	else{														//����Ƿ�����
		if(lev==0 || type==6 || type==7 || type==8){			//��ʶ����ȫ����������
			for(int i=0;i<tablen;i++){
				if(tab[i].lev!=0){
					continue;
				}
				if(strcmp(tab[i].id,id)==0){						//����ȫ�����������ͣ�������ȫ����)��������
					//error:ȫ������������ȫ������������������
					errormessage(GLOBALNAME_ERROR,line,no);
					return;
				}
			}
		}
		else{													//��ʶ���Ǿֲ���������,˵����ʱ����һ��������
			for(int i=funcindex[funclen-1]+1;i<tablen;i++){		//funcindex����Ǻ�����ʶ����������Ҫ��1
				if(strcmp(tab[i].id,id)==0){
					//error:�ֲ�������������							//����,ͬһ�����ľֲ����䡢����������
					errormessage(LOCALNAME_ERROR,line,no);
					return;
				}
			}
		}

	}
	//û������������ű�
	tab[tablen].addr=addr;		//addr
	strcpy(tab[tablen].id,id);	//id
	tab[tablen].len=len;		//len
	tab[tablen].lev=lev;		//lev
	tab[tablen].type=type;		//type
	tab[tablen].size=size;		//size
	//���ŷֱ���funcindex��globalindex
	if(lev==0 && type>=0 && type<=5){		//ȫ�ֳ�������,����Ϊconst int��const char��int��char��int�����char����
		//globalindex[globallen]=tablen;		//type =0 1 2 3 4 5֮һ����Ϊȫ�ֳ�������
		globallen++;
	}
	else if(type==6 || type==7 || type==8){	//���ֺ���
		funcindex[funclen]=tablen;
		funclen++;
	}

	tablen++;								//tablen��1
	return;
}


//��ʹ�÷���ʱ��ѯ���ű�,��������   ��ѯ��֪id����Ϊ ����-1, ������������ʶ��(������)-2,����-3
int searchtab(char* id,int whichtype){		
	if(whichtype==1){				//�Ǻ���
		for(int i=0;i<funclen;i++){
			if(strcmp(tab[funcindex[i]].id,id)==0){
				return funcindex[i];
			}
		}
	}
	else if(whichtype==2){						//�ǳ�����������ʶ��
												//�ȱ���ͬһ����(�����)�µľֲ�����(��Ҫ�ų�����)
		if(funclen!=0){							//�������κκ�����funclen==0���������Ӧ������Ѿ�����
			for(int i=funcindex[funclen-1]+1;i<tablen;i++){
				if(strcmp(tab[i].id,id)==0 && tab[i].type!=4 && tab[i].type!=5){
					return i;
				}
			}
		}
		else{
			//error:�Ա�����ʹ�ò��ں����У����򲻿������е�����
		}
												//���û�ҵ����ٱ���ȫ�ֳ�������(��Ҫ�ų�����)
		for(int i=0;i<globallen;i++){
			if(strcmp(tab[i].id,id)==0 && tab[i].type!=4 && tab[i].type!=5){
				return i;
			}
		}
	}
	else{										//������,������������ͬ
		if(funclen!=0){		
			for(int i=funcindex[funclen-1]+1;i<tablen;i++){
				if(strcmp(tab[i].id,id)==0 && (tab[i].type==4 || tab[i].type==5)){
					return i;
				}
			}
		}
		for(int i=0;i<globallen;i++){
			if(strcmp(tab[i].id,id)==0 && (tab[i].type==4 || tab[i].type==5)){
				return i;
			}
		}
	}
	//error:û���ҵ���ʶ��,δ����ı�ʶ��������㺯������
	return -1;

}


//��ӡ���ű�
void printtab(){
	for(int i=0;i<tablen;i++){
		printf("%d:id=%s type=%d len=%d lev=%d size=%d addr=%d\n",i,tab[i].id,tab[i].type,tab[i].len,tab[i].lev,tab[i].size,tab[i].addr);
	}
}





//����ͷ�������� if for switch { id scanf printf return ;
int isstahead(int symid){
	if(symid==IFSYM || symid==FORSYM || symid==SWITCHSYM || symid==LBRACE || symid==ID || symid==SCANFSYM || symid==PRINTFSYM || symid==RETURNSYM || symid==SEMICOLON){
		return 1;
	}
	else{
		return 0;
	}
}


//��������䣾   ::=  return[��(�������ʽ����)��]  
//���������
void returnsta(){
	//���뺯��ʱ���Ѷ���return
	char temp[tokensize];			//�洢��ʱ����

	getsym();
	if(symid==LPAR){				//����(		����Ϊreturn(���ʽ);
		getsym();				//�ٶ�һ��
		expression(temp);		//���ô�����ʽ����
		if(symid==RPAR){
			midcode(RETEXPOP,temp,NULL,NULL);			//�����м����
			getsym();
		}
		else{
			//error:Ӧ��)
			errormessage(RPAR_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//����id if for switch scanf return printf ; { }
			return;
		}
	}
	else{					//����Ϊ		return;
		midcode(RETNULLOP,NULL,NULL,NULL);		//�����м���� return null	
	}

	//printf("this is a return statement\n");
	fprintf(grammarfile,"%s","this is a return statement\n");
}


//��д��䣾    ::= printf ��(�� ���ַ�����,�����ʽ�� ��)��| printf ��(�����ַ����� ��)��| printf ��(�������ʽ����)��
//����д���
void printsta(){
	//�������ʱ�Ѿ�����printf
	int exp_type=0;				//���ʽ������
	char str[tokensize]={'\0'};
	char temp[tokensize]={'\0'};		//�洢��ʱ����
	getsym();
	if(symid==LPAR){
		getsym();
		if(symid==ASTRING){		//�������ַ�����
			strcpy(str,token);	//tempֻ�����ַ�����ֵ
			midcode(PRINTSTROP,str,NULL,NULL);
			getsym();
			if(symid==COMMA){	//����,						
				getsym();
				exp_type=expression(temp);	//���ñ��ʽ�������
				if(symid==RPAR){//����)
					if(exp_type==1){		//���ʽ��char����
						midcode(PRINTCHAROP,temp,NULL,NULL);			//�����м����
					}
					else{
						midcode(PRINTINTOP,temp,NULL,NULL);			//�����м����
					}
					
					getsym();
				}
				else{
					//error:Ӧ��)
					errormessage(RPAR_ERROR,line,no);
					skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//����id if for switch scanf return printf ; { }
					return;
				}
			}
			else if(symid==RPAR){	//����)
				getsym();
			}
			else{
				//error:Ӧ��)
				errormessage(RPAR_ERROR,line,no);
				skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//����id if for switch scanf return printf ; { }
				return;
			}
		}
		else{					//�����Ǳ��ʽ
			exp_type=expression(temp);
			if(symid==RPAR){	//����)
				if(exp_type==1){		//���ʽ��char����
					midcode(PRINTCHAROP,temp,NULL,NULL);			//�����м����
				}
				else{
					midcode(PRINTINTOP,temp,NULL,NULL);			//�����м����
				}
				getsym();
			}
			else{
				//error:Ӧ��)
				errormessage(RPAR_ERROR,line,no);
				skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//����id if for switch scanf return printf ; { }
				return;
			}
		}
	}
	else{
		//error:Ӧ��(
		errormessage(LPAR_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//����if for switch scanf return printf ; { }
		return;
	}

	midcode(PRINTLINE,NULL,NULL,NULL);	//����
	//printf("this is a print statement\n");
	fprintf(grammarfile,"%s","this is a print statement\n");
}

//������䣾    ::=  scanf ��(������ʶ����{,����ʶ����}��)��
//�����
void scansta(){
	//���뺯��ʱ���Ѿ�����scanf
	getsym();
	if(symid==LPAR){				//����(
		do{
			getsym();
			if(symid==ID){
				int index=searchtab(token,2);	//����ű�
				if(index==-1){
					//error:δ�����ʶ��
					errormessage(UNDEFID_ERROR,line,no);
					skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//���� if for switch scanf return printf ; { }
					return;
				}
				else if(tab[index].type==0 || tab[index].type==9){
					midcode(SCANINTOP,token,NULL,NULL);				//�����м����
					//int ���������
					getsym();
				}
				else if(tab[index].type==1 || tab[index].type==10){
					midcode(SCANCHAROP,token,NULL,NULL);			//�����м����
					//int char���������
					getsym();
				}
				else{
					//error:�������ʹ���ӦΪint char ����
					errormessage(VARIABLE_ERROR,line,no);
					skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//���� if for switch scanf return printf ; { }
					return;
				}
			
			}
			else{
				//error:Ӧ��id
				errormessage(ID_ERROR,line,no);
				return;
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//���� if for switch scanf return printf ; { }
			}
		}while(symid==COMMA);			//����������һ����ʶ��

		if(symid==RPAR){				//����)
			getsym();
		}
		else{
			//error:Ӧ��)
			errormessage(RPAR_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//����id if for switch scanf return printf ; { }
			return;
		}
	}
	else{
		//error:Ӧ��(
		errormessage(LPAR_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� if for switch scanf return printf ; { }
		return;
	}

	//printf("this is a scan statement\n");
	fprintf(grammarfile,"%s","this is a scan statement\n");
}



//����ֵ��䣾   ::=  ����ʶ�����������ʽ��|����ʶ������[�������ʽ����]��=�����ʽ��
//����ֵ���
void assignsta(){
	//���뺯��ʱ�Ѿ�������= ��[
	char op1[tokensize]={'\0'};
	char op2[tokensize]={'\0'};
	char result[tokensize]={'\0'};
	strcpy(id,token1);				//���������
	strcpy(op1,token1);
	//printtab();
	//printf("%s\n",id);

	if(symid==ASSIGN){				//= int��char����
		int index=searchtab(id,2);	//����ű�

		if(index==-1){
			//error:δ��������
			errormessage(UNDEFID_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//���� if for switch scanf return printf ; { }
			return;
		}
		else if(tab[index].type==0 || tab[index].type==1 || tab[index].type==9 || tab[index].type==10){	//= int��char����
			getsym();
			expression(op2);				//���ñ��ʽ�������
			midcode(VARASSIGN,op1,op2,NULL);		//�����м����
		}
		else{
			//error:���Ǳ���
			errormessage(VARIABLE_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//���� if for switch scanf return printf ; { }
			return;
		}
	}
	else{							//[ int��char����
		int index=searchtab(id,3);	//����ű�
		if(index==-1){
			//error:δ��������
			errormessage(UNDEFID_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//���� if for switch scanf return printf ; { }
			return;
		}
		else if(tab[index].type==4 || tab[index].type==5){	//��������int��char����
			getsym();
			expression(op2);			//���ñ��ʽ�������
			if(symid==RBRACKET){		//����]
				getsym();
				if(symid==ASSIGN){		//����=
					getsym();
					expression(result);		//���ñ��ʽ�������
					midcode(ARRASSIGN,op1,op2,result);		//�����м����
				}
				else{
					//error:Ӧ��=
					errormessage(ASSIGN_ERROR,line,no);
					skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//����if for switch scanf return printf ; { }
					return;
				}
			}
			else{
				//error:Ӧ��]
				errormessage(RBRACKET_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//����id if for switch scanf return printf ; { }
				return;
			}
		}
		else{
			//error:��������
			errormessage(NOTARR_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//����id if for switch scanf return printf ; { }
			return;
		}
			
	}

	//printf("this is a assign statement\n");
	fprintf(grammarfile,"%s","this is a assign statement\n");
}

//��������    ::=  �����ʽ������ϵ������������ʽ���������ʽ�� //���ʽΪ0����Ϊ�٣�����Ϊ��
//��������
void condition(){
	//���뺯��ʱ���Ѿ���ȡ���׷���
	char op1[tokensize]={'\0'};
	char op2[tokensize]={'\0'};
	int temp_type=0;
	expression(op1);			//���ñ��ʽ�������
	//������ϵ�����
	if(symid==EQUAL || symid==BIGEQL || symid==BIG || symid==SMALL || symid==SMALLEQL || symid==NOTEQL){
		temp_type=(symid==EQUAL)?EQUALOP:
				 (symid==BIGEQL)?BIGEQLOP:
				 (symid==BIG)?BIGOP:
				 (symid==SMALL)?SMALLOP:
				 (symid==SMALLEQL)?SMALLEQLOP:
				 NOTEQLOP;
		//printf("this is a relation operator\n");
		fprintf(grammarfile,"%s","this is a relation operator\n");
		getsym();
		expression(op2);//���ñ��ʽ�������
		midcode(temp_type,op1,op2,NULL);			//�����м����
	}
	else{
		//���ʽ��������
	}

		
	//printf("this is a condition\n");
	fprintf(grammarfile,"%s","this is a condition\n");
}


//��������䣾  ::=  if ��(������������)������䣾��else����䣾��
//����if-else���
void ifsta(){
	//���뺯��ʱ�Ѿ�����if
	char label1[tokensize]={'\0'};
	char label2[tokensize]={'\0'};
	getsym();
	if(symid==LPAR){				//����(
		getsym();			
		condition();				//���������������
									
															//�����м����											//����һ��label���
		getlabel(label1);						//����label�ַ���,����label1��
		getlabel(label2);
		midcode(BZOP,label1,NULL,NULL);			//����Ϊ��ʱ��ת��label1������else��Ϊelse�Ŀ�ͷ������Ϊif-else���Ľ�β��


		if(symid==RPAR){			//����)
			getsym();
			sta();					//������䴦�����


			midcode(GOTOOP,label2,NULL,NULL);			//�����е��˴�˵������Ϊ�棬����else
			midcode(SETLABEL,label1,NULL,NULL);			//����label1		

			if(symid==ELSESYM){		//����else
				getsym();
				sta();
			}
			midcode(SETLABEL,label2,NULL,NULL);			//����label2
		}
		else{
			//error:Ӧ��)
			errormessage(RPAR_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//���� if for switch scanf return printf ; { }
			return;
		}
	}
	else{
		//error:����(
		errormessage(LPAR_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//����id if for switch scanf return printf ; { }
		return;
	}

	//printf("this is an if-else \n");
	fprintf(grammarfile,"%s","this is an if-else \n");
}


//��ѭ����䣾   ::=  for��(������ʶ�����������ʽ��;��������;����ʶ����������ʶ����(+|-)����������)������䣾
//����forѭ�� �ȴ��� ����ʶ�����������ʽ�� ���ٴ��� ����ʶ����������ʶ����(+|-)�����������ٴ���<����>
void forsta(){
	//���뺯��ʱ���Ѿ�������for
	struct MIDCODE temp_code2[2];			//�ݴ�Ӽ������������м����
	char temp_num[tokensize]={'\0'};				//i=0
	int temp_type=0;								//label1:
	int index1=0;									//ѭ����									
	int index2=0;									//i++								
	char temp1[tokensize]={'\0'};					//����					
	char temp2[tokensize]={'\0'};					//BNZ label1			
	char label1[tokensize]={'\0'};	
	char label2[tokensize]={'\0'};
	char label3[tokensize]={'\0'};
											
	getlabel(label1);				//��ȡ1��label		
	getlabel(label2);				//��ȡ1��label	
	getlabel(label3);				//��ȡ1��label	
	gettemp(temp2);														

	getsym();															
	if(symid!=LPAR){				//����(	
		//error:Ӧ��(
		errormessage(LPAR_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� if for switch scanf return printf ; { }
		return;
	}
	getsym();														
	if(symid!=ID){					//������ʶ��	
		//error:Ӧ��id
		errormessage(ID_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� if for switch scanf return printf ; { }
		return;
	}
	index1=searchtab(token,2);
	if(index1==-1){
		//error:δ�������������
		errormessage(UNDEFID_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� if for switch scanf return printf ; { }
		return;
	}
	else if(tab[index1].type!=0 && tab[index1].type!=1 && tab[index1].type!=9 && tab[index1].type!=10 ){
		//error:����int char����������
		errormessage(VARIABLE_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� if for switch scanf return printf ; { }
		return;
	}

	getsym();
	if(symid!=ASSIGN){		//����=
		//error:Ӧ��=
		errormessage(ASSIGN_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� if for switch scanf return printf ; { }
		return;
	}
	getsym();
	expression(temp1);		//���ñ��ʽ�������
	midcode(VARASSIGN,tab[index1].id,temp1,NULL);		//�����м���룬i=temp1
	midcode(GOTOOP,label2,NULL,NULL);					//�����м���룬goto label2
	midcode(SETLABEL,label1,NULL,NULL);					//�����м���룬set label1
	if(symid!=SEMICOLON){		//����;
		//error:Ӧ��;
		errormessage(SEMICOLON_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� if for switch scanf return printf ; { }
		return;
	}

	getsym();
	condition();			//���������������

	midcode(BZOP,label3,NULL,NULL);					//�����м���룬BZ label3


	if(symid!=SEMICOLON){	//����;
		//error:Ӧ��;
		errormessage(SEMICOLON_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� if for switch scanf return printf ; { }
		return;
	}

	
	getsym();
	if(symid!=ID){		//����id
		//error:Ӧ��id;
		errormessage(ID_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//����id if for switch scanf return printf ; { }
		return;

	}
		
	if(strcmp(token,tab[index1].id)!=0){
		//error:forѭ������������ʶ��
		errormessage(DIFFVAR_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//����id if for switch scanf return printf ; { }
		return;
	}
							
	getsym();
	if(symid!=ASSIGN){	//����=
		//error:Ӧ��=
		errormessage(ASSIGN_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� if for switch scanf return printf ; { }
		return;
	}
	getsym();
	if(symid!=ID){	//����id
		//error:Ӧ��id
		errormessage(ID_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� if for switch scanf return printf ; { }
		return;
	}
									
	if(strcmp(token,tab[index1].id)!=0){
		//error:forѭ������������ʶ��
		errormessage(DIFFVAR_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� if for switch scanf return printf ; { }
		return;
	}
	getsym();
	if(symid!=PLUS && symid!=SUB){		//����+��-
		//error:ȱ��+ -
		errormessage(ADDSUB_ERROR,line,no);
		skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//����id if for switch scanf return printf ; { }
		return;
	}
	temp_type=(symid==PLUS)?ADDOP:SUBOP;	
	getsym();
	if(symid!=ANUMBER || num==0){	//���� �޷��ŷ�������
		//error:��������ӦΪ�޷��ŷ�������
		errormessage(STEP_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� if for switch scanf return printf ; { }
		return;
	}
	sprintf(temp_num,"%d",num);		//����ת����

//ʹ��temp_code2��ʱ���沽���Ӽ������������м����
	temp_code2[0].type=temp_type;
	strcpy(temp_code2[0].op1,tab[index1].id);				//�����м���룬temp2=i+/-num
	strcpy(temp_code2[0].op2,temp_num);
	strcpy(temp_code2[0].result,temp2);
	
	temp_code2[1].type=VARASSIGN;
	strcpy(temp_code2[1].op1,tab[index1].id);		//�����м���룬i=temp2
	strcpy(temp_code2[1].op2,temp2);

	getsym();
	if(symid!=RPAR){
		//error:ȱ��)
		errormessage(RPAR_ERROR,line,no);
		skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//����id if for switch scanf return printf ; { }
		return;
	}

	midcode(SETLABEL,label2,NULL,NULL);					//�����м���룬set label2
	getsym();
	sta();
														
	
	//���ݴ�Ĵ���д���ļ�
	midcode(temp_code2[0].type,temp_code2[0].op1,temp_code2[0].op2,temp_code2[0].result);
	midcode(temp_code2[1].type,temp_code2[1].op1,temp_code2[1].op2,temp_code2[1].result);

	midcode(GOTOOP,label1,NULL,NULL);					//�����м���룬goto label1
	midcode(SETLABEL,label3,NULL,NULL);					//�����м���룬set label3

	//printf("this is a for-loop\n");
	fprintf(grammarfile,"%s","this is a for-loop\n");
}


//�������䣾  ::=  switch ��(�������ʽ����)�� ��{�����������ȱʡ����}��
//����switch-case
void switchsta(){
	//���뺯��ʱ�Ѿ�����switch
	int switch_type=0;				//switch�ı��ʽ����
	char label[tokensize]={'\0'};
	char default_label[tokensize]={'\0'};		//default_label��case����丳ֵ
	char temp_exp[tokensize]={'\0'};
	gettemp(temp_exp);		//�������ܱ��ʽֵ����ʱ����
	getlabel(label);
	getsym();
	if(symid==LPAR){		//����(
		getsym();
		switch_type=expression(temp_exp);	//���ñ��ʽ�������
		if(symid==RPAR){
			getsym();
			if(symid==LBRACE){		//����{
				getsym();
				if(symid==CASESYM){		//����case
					casesta(temp_exp,label,default_label,switch_type);		//��������������
					if(symid==DEFAULTSYM){		//����default
						defaultsta(default_label);			//����ȱʡ�������
						if(symid==RBRACE){		//����}
							midcode(SETLABEL,label,NULL,NULL);		//����label
							getsym();
						}
						else{
							//error:ӦΪ}
							errormessage(RBRACE_ERROR,line,no);
							skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
							//���� case default if for switch scanf return printf ; { }
							return;
						}
					}
					else{
						//error:ȱʧdefault
						errormessage(DEFAULT_ERROR,line,no);
						skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
						//���� case default if for switch scanf return printf ; { }
						return;
					}
				}
				else{
					//error:ȱʧcase
					errormessage(CASE_ERROR,line,no);
					skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//���� case default if for switch scanf return printf ; { }
					return;
				}
			}
			else{
				//error:ӦΪ{
				errormessage(LBRACE_ERROR,line,no);
				skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//���� case default if for switch scanf return printf ; { }
				return;
			}
		}
		else{
			//error:ӦΪ)
			errormessage(RPAR_ERROR,line,no);
			skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//���� case default if for switch scanf return printf ; { }
			return;
		}
	}
	else{
		//error:Ӧ��(
		errormessage(LPAR_ERROR,line,no);
		skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� case default if for switch scanf return printf ; { }
		return;
	}

	//printf("this is a switch-case statement\n");
	fprintf(grammarfile,"%s","this is a switch-case statement\n");
}


//�������   ::=  ���������䣾{���������䣾}
//���������
void casesta(char* temp_exp,char* label,char* default_label,int switch_type){
	//���뺯��ʱ�Ѷ�ȡ��case
	char mylabel[tokensize]={'\0'};
	char nextlabel[tokensize]={'\0'};
	getlabel(mylabel);			//�Ȼ�õ�һ��case��label
	do{
		casesubsta(temp_exp,mylabel,nextlabel,switch_type);
		midcode(GOTOOP,label,NULL,NULL);			//case����ֱ��������β

		memset(mylabel,0,tokensize);				//���½�nextlabel��ֵ����mylabel������nextlabel��ʼ��
		strcpy(mylabel,nextlabel);
		
		memset(default_label,0,tokensize);			//��Ԥ�Ϻ�ʱ����default��default_label���������һ��nextlabel
		strcpy(default_label,nextlabel);

		memset(nextlabel,0,tokensize);
	}while(symid==CASESYM);		//������һ��case���

	//printf("this is a case statement\n");
	fprintf(grammarfile,"%s","this is a case statement\n");
}

//���������䣾  ::=  case��������������䣾
//������������,mylabel������һ��case��������ͷ���룩��nextlabelδ����,temp_expΪ���ʽ����ʱ����
void casesubsta(char* temp_exp,char* mylabel,char* nextlabel,int switch_type){
	//���뺯���Ѷ���case
	int number=0;
	char temp_num[tokensize]={'\0'};
	midcode(SETLABEL,mylabel,NULL,NULL);		//�����ñ�case��label
	getsym();
	if(symid==PLUS || symid==SUB || symid==ANUMBER){		//����+ - ��ǰ������
		number=integer();							//integer��Ԥ��һ������
		if(switch_type==1){						//switchΪchar caseΪint
			//���ͳ�ͻ
			errormessage(TYPECONFL_ERROR,line,no);
		}
	}
	else if(symid==ACHAR){
		number=num;
		if(switch_type==0){
			//���ͳ�ͻ
			errormessage(TYPECONFL_ERROR,line,no);
		}
	}
	else{
		//error:ӦΪ����
		errormessage(CONSTVALUE_ERROR,line,no);
		skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� case default if for switch scanf return printf ; { }
		return;
	}
	sprintf(temp_num,"%d",number);
	midcode(EQUALOP,temp_exp,temp_num,NULL);		//������ϵ������ʽ
	getlabel(nextlabel);
	midcode(BZOP,nextlabel,NULL,NULL);

	getsym();
	if(symid==COLON){		//����:
		getsym();
		sta();				//������䴦�����
	}
	else{
		//error:Ӧ��:
		errormessage(COLON_ERROR,line,no);
		skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� case default if for switch scanf return printf ; { }
		return;
	}


	//printf("this is a case substatement\n");
	fprintf(grammarfile,"%s","this is a case substatement\n");
}

//��ȱʡ��   ::=  default : ����䣾
//����ȱʡ���
void defaultsta(char* default_label){
	//���뺯��ʱ�Ѿ���ȡ����default
	midcode(SETLABEL,default_label,NULL,NULL);		//����default label
	getsym();
	if(symid==COLON){
		getsym();
		sta();
	}
	else{
		//error:Ӧ��:
		errormessage(COLON_ERROR,line,no);
		skip(10,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//����default if for switch scanf return printf ; { }
		return;
	}

	//printf("this is a default statement\n");
	fprintf(grammarfile,"%s","this is a default statement\n");
}

//���з���ֵ����������䣾 ::= ����ʶ������(����ֵ��������)��
//�����з���ֵ�����ĵ���
void callretfunc(int index){
	char op1[tokensize]={'\0'};
	//���뺯��ʱ���Ѿ�������ʶ�� �� (
	if(symid==ID){					//������ǰ�����ķ�����Ϣ,��Ϊ������ܱ� ����д������ �� ���Ӵ������ ����
		strcpy(token1,token);		//�����Ӵ���������ʱ���Ѷ���( �Һ�������ʶ���ѱ�����token1��symid1
		symid1=symid;
		getsym();
		if(symid==LPAR){			//����(

		}
		else{
			//error:ȱ��(
			errormessage(LPAR_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//���� if for switch scanf return printf ; { }
			return;
		}
	}


	getsym();
	valueofpara(index);					//����ֵ�����д������

	midcode(FUNCCALL,tab[index].id,NULL,NULL);	//call function
	if(symid==RPAR){				//��ȡ��)
		getsym();
	}
	else{
		//error:Ӧ��)
		errormessage(RPAR_ERROR,line,no);
		skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� if for switch scanf return printf ; { }
		return;
	}

	//printf("this is a call of function\n");
	fprintf(grammarfile,"%s","this is a call of function\n");
}		

//���޷���ֵ����������䣾 ::= ����ʶ������(����ֵ��������)��
//�����޷���ֵ�����ĵ���,ֻ���ܱ�����е���
void callnoretfunc(int index){
	//���뺯��ʱ���Ѿ�����(
	getsym();
	valueofpara(index);				//����ֵ�����д������
	midcode(FUNCCALL,tab[index].id,NULL,NULL);	//call function
	if(symid==RPAR){				//����)
		getsym();
	}
	else{
		//error:Ӧ��)
		errormessage(RPAR_ERROR,line,no);
		skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� if for switch scanf return printf ; { }
		return;
	}

	//printf("this is a call of function\n");
	fprintf(grammarfile,"%s","this is a call of function\n");
}		


//������˵���� ::=  const���������壾;{ const���������壾;}
//������˵��
void conststa(){		
	//���뺯��ʱ�Ѿ�����const
	do{
		getsym();
		if(symid==INTSYM || symid==CHARSYM){	//�������ͱ�ʶ��
			constdef();							//���ô������������
			if(symid==SEMICOLON){				//�����ֺ�
				getsym();						//�����¶�һ������,�����const������ѭ��
			}
			else{
				//error:Ӧ���зֺ�
				errormessage(SEMICOLON_ERROR,line,no);
				skip(5,SEMICOLON,CONSTSYM,INTSYM,CHARSYM,VOIDSYM);
				//���� ; const int char void 
			}
		}
		else{
			//error:Ӧ��int �� char
			errormessage(TYPE_ERROR,line,no);
			skip(5,SEMICOLON,CONSTSYM,INTSYM,CHARSYM,VOIDSYM);
			//���� ; const int char void 
		}
	}while(symid==CONSTSYM);					//������һ��const���������壾;
	//printf("this is a conststatement\n");
	fprintf(grammarfile,"%s","this is a conststatement\n");
}


//���������壾 ::=int����ʶ��������������{,����ʶ��������������}
//              | char����ʶ���������ַ���{,����ʶ���������ַ���}
//����������
void constdef(){	
	//���뺯��ʱ�Ѿ�����int��char
	char temp_num[tokensize]={'\0'};
	switch(symid){
		case INTSYM:	type=2;break;		//const int
		case CHARSYM:	type=3;break;		//const char
	}
	getsym();								//�ٶ�һ��,Ӧ���Ǳ�ʶ��

	while(true){
		if(symid==ID){
			strcpy(id,token);					//�ڶ�=֮ǰ�������ʶ��
			getsym();
			if(symid==ASSIGN){					//�ٶ�һ��,Ӧ����=
				if(type==2){					//const int
					getsym();	
					len=integer();						//���ô�����������
					inserttab(id,type,len,lev,4,addr);		//����ű�
					addr+=4;									//daddr
					memset(temp_num,0,tokensize);
					sprintf(temp_num,"%d",len);
					midcode(CONSTINTDEF,id,temp_num,NULL);						//�����м����
				}
				else{									//const char
					getsym();	
					if(symid==ACHAR){
						len=num;						//�����ַ�
						inserttab(id,type,len,lev,4,addr);	//����ű�
						addr+=4;							//��ַ��4���ַ�Ҳռ��4�ֽ�
						memset(temp_num,0,tokensize);
						sprintf(temp_num,"%d",len);
						midcode(CONSTCHARDEF,id,temp_num,NULL);					//�����м����	
					}
					else{
						//error:ӦΪһ���ַ�
						errormessage(CHAR_ERROR,line,no);
						skip(5,SEMICOLON,CONSTSYM,INTSYM,CHARSYM,VOIDSYM);
						//���� ; const int char void 
						break;
					}
				}
			}
			else{
				//error:Ӧ����=
				errormessage(ASSIGN_ERROR,line,no);
				skip(5,SEMICOLON,CONSTSYM,INTSYM,CHARSYM,VOIDSYM);
				//���� ; const int char void 
				break;
			}
			getsym();							//�ٶ�һ������
			if(symid==COMMA){					//������������滹��
				getsym();
			}
			else{								//�����˳�����
				break;
			}
		}
		else{
			//error:Ӧ���Ǳ�ʶ��
			errormessage(ID_ERROR,line,no);
			skip(5,SEMICOLON,CONSTSYM,INTSYM,CHARSYM,VOIDSYM);
			//���� ; const int char void 
			break;
		}
	}
	//printf("this is a constdefine \n");
	fprintf(grammarfile,"%s","this is a constdefine \n");
}

//��������        ::= �ۣ������ݣ��޷�������������
//��������,�����������ӳ���ͬ���ó����ڽ�βû��Ԥ��һ������
int integer(){
	//���뺯��ʱ���Ѿ���ȡ�˵�һ������
	int head=0;				//+----head=1 ------head=-1
	if(symid==PLUS || symid==SUB){		//����+,-
		head=(symid==PLUS)?1:-1;
		getsym();
		if(symid==ANUMBER){
			if(head!=0 && num==0){
				//error:0�з���
				errormessage(PLUSZERO_ERROR,line,no);
				return 0;
			}
			return head*num;
		}
		else{
			//error:ȱ������
			errormessage(INTEGER_ERROR,line,no);
			return 0;
		}
	}	
	else if(symid==ANUMBER){
		return num;
	}
	else{
		//error:ȱ������
		errormessage(INTEGER_ERROR,line,no);
		return 0;
	}

	//printf("this is an integer\n");
}


//���������壾  ::= (int|char)(����ʶ����|����ʶ������[�����޷�����������]��)
//							 {,(����ʶ����|����ʶ������[�����޷�����������]��)};
//�����������
void variabledef(){					
	//���ú���ʱ�Ѷ���[ , ;
	strcpy(id,token2);							//��ȡ��ʱ����ı�ʶ��
	int temp_type=(symid1==INTSYM)?0:1;			//��ȡ��ʱ��������� ����ʱ��Ϊ��������int--type=0��char--type=1��������������
	len=0;
	type=temp_type;
	while(true){
		char temp_num[tokensize]={'\0'};
		if(symid==COMMA){							//����, ���б�������
			len=(type==0 || type==1)?0:len;			//int char������lenΪ0
			size=sizecalcu(type,len);
			inserttab(id,type,len,lev,size,addr);		//����ű�
			addr+=size;
			sprintf(temp_num,"%d",len);
			midcode((type==0)?VARINTDEF:(type==1)?VARCHARDEF:(type==4)?INTARRDEF:CHARARRDEF,id,temp_num,NULL);//�����м����
			
			type=temp_type;							//�����type���temp_type
			getsym();								//Ӧ�ö��� ��ʶ��
			if(symid==ID){
				strcpy(id,token);					//�����ʶ��
				getsym();
				if(symid==LBRACKET || symid==COMMA || symid==SEMICOLON){//�ֶ���[ , ;
					continue;
				}
				else{
					//error:ȱ��[ , ;
					errormessage(SEMICOLON_ERROR,line,no);
					skip(4,SEMICOLON,INTSYM,CHARSYM,VOIDSYM);
					//���� ; int char void 
					return;
				}
			}
			else{
				//error:Ӧ��id
				errormessage(ID_ERROR,line,no);
				skip(4,SEMICOLON,INTSYM,CHARSYM,VOIDSYM);
				//���� ; int char void 
				return;
			}
		}
		else if(symid==LBRACKET){					//����[ ˵��������
			getsym();
			if(symid==ANUMBER && num!=0){			//����ĳ������޷��ŷ�������
				len=num;							//��¼���鳤��
				type=(temp_type==0)?4:5;			//int����-4  char����-5
				size=sizecalcu(type,len);			//��¼�����ֽ���
				getsym();
				if(symid==RBRACKET){				//]
					getsym();
					if(symid==COMMA || symid==SEMICOLON){//�ֶ��� , ;
						//�˴��Ȳ�����ű�����, ;����
						continue;
					}
					else{
						//error:ȱ��, ;
						errormessage(SEMICOLON_ERROR,line,no);
						skip(4,SEMICOLON,INTSYM,CHARSYM,VOIDSYM);
						//���� ; int char void 
						return;
					}
				}
				else{
					//error:Ӧ��]
					errormessage(RBRACKET_ERROR,line,no);
					skip(4,SEMICOLON,INTSYM,CHARSYM,VOIDSYM);
					//���� ; int char void 
					return;
				}
			}
			else{
				//error://����ĳ������޷��ŷ�������
				errormessage(ARRLEN_ERROR,line,no);
				skip(4,SEMICOLON,INTSYM,CHARSYM,VOIDSYM);
				//���� ; int char void 
				return;
			}
		}
		else if(symid==SEMICOLON){					//����; ����
			len=(type==0 || type==1)?0:len;			//int char������lenΪ0

			size=sizecalcu(type,len);
			inserttab(id,type,len,lev,size,addr);		//����ű�
			addr+=size;
			sprintf(temp_num,"%d",len);			//����ת����
			midcode((type==0)?VARINTDEF:(type==1)?VARCHARDEF:(type==4)?INTARRDEF:CHARARRDEF,id,temp_num,NULL);//�����м����
			
			type=temp_type;							//�����type���temp_type
			getsym();								//Ԥ��һ������
			break;
		}
		else{
			//error:Ӧ��;
			errormessage(SEMICOLON_ERROR,line,no);
			skip(4,SEMICOLON,INTSYM,CHARSYM,VOIDSYM);
			//���� ; int char void 
			return;
		}
	}

	//printf("this is a variabledefine\n");
	fprintf(grammarfile,"%s","this is a variabledefine\n");
}

//�����ʽ��    ::= �ۣ������ݣ��{���ӷ�����������}
//������ʽ
int expression(char* temp){
	//char temp1[tokensize]={'\0'};
	char temp2[tokensize]={'\0'};
	char temp_num[tokensize]={'\0'};
	int temp_type=0;
	int exp_type=0;
	int term_sum=0;
	//gettemp(temp);

	//������ʽʱ���Ѷ�ȡ��һ���ַ�
	if(symid==PLUS || symid==SUB){		//����+ -
		temp_type=(symid==PLUS)?ADDOP:SUBOP;
		getsym();
		exp_type=term(temp);							//����������
		sprintf(temp_num,"%d",0);
		midcode(temp_type,temp_num,temp,temp);
		term_sum++;
	}
	else{
		exp_type=term(temp);							//ֱ�ӵ���������
	}



	while(symid==PLUS || symid==SUB){	//�ֶ���+ -
		temp_type=(symid==PLUS)?ADDOP:SUBOP;
		getsym();						//����Ӧ���Ǳ��ʽ
		term(temp2);
		midcode(temp_type,temp,temp2,temp);				//�����м���� temp1=temp1+/-temp2
		term_sum++;
	}

	//printf("this is a expression\n");
	fprintf(grammarfile,"%s","this is a expression\n");
	return (exp_type==1 && term_sum==0)?1:0;			//ֻ��һ������Ϊchar���޷��ţ��ŷ���1
}

//���     ::= �����ӣ�{���˷�������������ӣ�}
//������(����ֵ��ʾ����0-int �� 1-char)
int term(char* temp){
	//char temp1[tokensize]={'\0'};
	char temp2[tokensize]={'\0'};
	int temp_type=0;
	int term_type=0;
	int factor_sum=0;
	//gettemp(temp);
	//�������ʱ�Ѿ���ȡ�˵�һ������
	term_type=factor(temp);						//�������Ӵ������
	while(symid==MUL || symid==DIV){				//����* /
		temp_type=(symid==MUL)?MULOP:DIVOP;
		getsym();									//�ٶ�һ��
		factor(temp2);								//�������Ӵ������
		midcode(temp_type,temp,temp2,temp);				//�����м���� temp1=temp1*//temp2
		factor_sum++;
	}

	//midcode(VARASSIGN,temp,temp1,NULL);						//�����м���� temp=temp1
	//printf("this is a term\n");
	fprintf(grammarfile,"%s","this is a term\n");
	return (term_type==1 && factor_sum==0)?1:0;			//���ֻ��һ�����ӣ�������Ϊchar���ŷ���1
}

//�����ӣ�   nn ::= ����ʶ����������ʶ������[�������ʽ����]������������|���ַ��������з���ֵ����������䣾|��(�������ʽ����)��   
//��������(����ֵ��ʾ����0-int �� 1-char)
int factor(char* temp){
	char temp_num[tokensize]={'\0'};
	char op1[tokensize]={'\0'};
	char op2[tokensize]={'\0'};
	int factor_type=0;
	int value=0;
	//���뺯��ʱ���Ѿ���ȡ�˵�һ������
	int index=0;
	gettemp(temp);
	if(symid==ID){		//���ӿ���Ϊ����ʶ����������ʶ������[�������ʽ����]��|���з���ֵ����������䣾
		strcpy(token1,token);			//������ǰ�����ķ�����Ϣ
		strcpy(op1,token);
		symid1=symid;
		getsym();
		if(symid==LBRACKET){		//����[�����ӿ���Ϊ����ʶ������[�������ʽ����]��

		//��� �е����飬�Ƿ����������
			index=searchtab(token1,3);	
			if(index==-1){
				//error:δ��������
				errormessage(UNDEFID_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//���� if for switch scanf return printf ; { }
				return factor_type;
			}
			else if(tab[index].type!=4 && tab[index].type!=5){
				//error:��������
				errormessage(NOTARR_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//���� if for switch scanf return printf ; { }
				return factor_type;
			}

			if(tab[index].type==5){		//char����
				factor_type=1;			//�����Ӷ�Ϊchar����
			}

			getsym();
			expression(op2);		//���ñ��ʽ��������
			if(symid==RBRACKET){//����]
				midcode(ARRUSE,op1,op2,temp);			//�����м���� temp=op1[op2]
				getsym();
			}
			else{
				//error:Ӧ��]
				errormessage(RBRACKET_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//���� if for switch scanf return printf ; { }
				return factor_type;
			}
		}
		else if(symid==LPAR){		//����(��/���ӿ���Ϊ���з���ֵ����������䣾,������



			index=searchtab(token1,1);	//���
			if(index==-1){
				//error:δ�����з���ֵ����
				errormessage(UNDEFID_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//���� if for switch scanf return printf ; { }
				return factor_type;
			}
			else if(tab[index].type!=6 && tab[index].type!=7){
				//error:�����з���ֵ����
				errormessage(HASRETFUNC_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//���� if for switch scanf return printf ; { }
				return factor_type;
			}

			if(tab[index].type==7){			//char�ͺ���,factor_type=1
				factor_type=1;
			}


			callretfunc(index);			//�����з���ֵ�������ô������
			midcode(FUNCRET,temp,NULL,NULL);		//�����м���룬temp=RET
		}
		else{						//���ӿ���Ϊ����ʶ����



			index=searchtab(token1,2);	//���
			if(index==-1){
				//error:δ���峣��������������
				errormessage(UNDEFID_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//���� if for switch scanf return printf ; { }
				return factor_type;
			}
			else if(tab[index].type!=0 && tab[index].type!=1 && tab[index].type!=2 && tab[index].type!=3 && tab[index].type!=9 && tab[index].type!=10){
				//error:���ǳ�����������
				errormessage(FACTOR_ERROR,line,no);
			}

			//char�ͱ���������
			if(tab[index].type==1 || tab[index].type==3 || tab[index].type==10){
				factor_type=1;
			}

			//����ǳ���,ֱ���ó�����ֵ
			if(tab[index].type==2 || tab[index].type==3){
				sprintf(temp_num,"%d",tab[index].len);
				midcode(VARASSIGN,temp,temp_num,NULL);			//�����м����temp=temp_num
			}
			//������ǳ���
			else{
				midcode(VARASSIGN,temp,op1,NULL);			//�����м����temp=op1
			}
			//֮ǰ�Ѿ�Ԥ��������Ҫ��Ԥ��
		}
	}
	else if(symid==PLUS || symid==SUB || symid==ANUMBER){	//���ӿ���Ϊ����
		value=integer();					//����������������
		getsym();
		sprintf(temp_num,"%d",value);
		midcode(VARASSIGN,temp,temp_num,NULL);			//�����м���� temp=num
	}
	else if(symid==ACHAR){			//�����ַ�,����Ϊ�ַ�
		sprintf(temp_num,"%d",num);		//�ַ�����ascii��
		midcode(VARASSIGN,temp,temp_num,NULL);			//�����м���� temp=num
		getsym();
		factor_type=1;
	}
	else if(symid==LPAR){			//����(,����Ϊ��(�������ʽ����)��   
		getsym();
		factor_type=expression(op1);				//���ñ��ʽ��������
		if(symid==RPAR){			//����)
			midcode(VARASSIGN,temp,op1,NULL);				//�����м����,temp=op1
			getsym();
		}
		else{
			//error:Ӧ��)
			errormessage(RPAR_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//���� if for switch scanf return printf ; { }
			return factor_type;
		}
	}
	else{
		//error:���Ӵ���
		errormessage(FACTOR_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//���� if for switch scanf return printf ; { }
		return factor_type;
	}

	
	//printf("this is a factor\n");
	fprintf(grammarfile,"%s","this is a factor\n");
	return factor_type;
}


//��ֵ������   ::= �����ʽ��{,�����ʽ��}�����գ�
//ֵ������
void valueofpara(int index){
	//���뺯��ʱ�Ѿ���ȡ�˵�һ������
	char temp[tokensize]={'\0'};
	int type=0;
	int k=1;					//��ǰΪ��k������
	if(symid==RPAR){			//����)��˵��û�в���
		if(tab[index].len==0){
			//����
		}
		else{
			//error:������Ŀ����
			errormessage(PARACOUNT_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//����id if for switch scanf return printf ; { }
			return;
		}
	}
	else{
		//�ȷ�����һ�����ʽ
		type=expression(temp);			//���ñ��ʽ�������� 0-int 1-char
		if((type==0 && tab[index+k].type!=9) || (type==1 && tab[index+k].type!=10)){
			//error:���Ͳ�һ��
			errormessage(TYPECONFL_ERROR,line,no);
		}						
		midcode(PARAOP,temp,NULL,NULL);			//�����м���� push temp


		while(symid==COMMA){			//����,
			k++;							//��ǰΪk+1������
			getsym();
			type=expression(temp);			//���ñ��ʽ�������� 0-int 1-char
			if((type==0 && tab[index+k].type!=9) || (type==1 && tab[index+k].type!=10)){
				//error:���Ͳ�һ��
				errormessage(TYPECONFL_ERROR,line,no);
			}
			midcode(PARAOP,temp,NULL,NULL);			//�����м���� push temp
			//�˴�Ҫ�ж�exp������tab[index+k]�������Ƿ�һ��
		}			

		if(k!=tab[index].len){
			//error:������Ŀ����
			errormessage(PARACOUNT_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//���� if for switch scanf return printf ; { }
			return;
		}
	}
	//printf("this is a value of paralist\n");
	fprintf(grammarfile,"%s","this is a value of paralist\n");

}

//��������䣾   ::=  �ۣ�����˵�����ݣۣ�����˵�����ݣ�����У�
//���������
void compoundsta(){			
	//���뺯��ʱ���Ѿ���ȡ�˵�һ���ַ�
	if(symid==CONSTSYM){		//����const
		conststa();				//���ñ���������������
	}
	while(true){				//��������������˵��
		if(symid==INTSYM || symid==CHARSYM){
			strcpy(token1,token);			//������ǰ�����ķ�����Ϣ
			symid1=symid;
			getsym();
			if(symid==ID){					//�ֶ�����ʶ��
				strcpy(token2,token);		//������ǰ�����ķ�����Ϣ
				symid2=symid;
				getsym();
				if(symid==LBRACKET || symid==SEMICOLON || symid==COMMA){
					variabledef();				//���ñ������崦�����,����ʱ����;
				}
				else{
					//error:Ӧ��[ , ;
					errormessage(SEMICOLON_ERROR,line,no);
					skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//����if for switch scanf return printf ; { }
					return;
				}
			}
			else{
				//error:Ӧ��id
				errormessage(ID_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//����if for switch scanf return printf ; { }
				return;
			}
		}
		else{
			break;
		}
	}

	stalist();			//���ô�������г���
	//printf("this is a compound statement\n");
	fprintf(grammarfile,"%s","this is a compound statement\n");
}



//����䣾    ::= ��������䣾����ѭ����䣾| ��{��������У���}�������з���ֵ����������䣾; 
//| ���޷���ֵ����������䣾;������ֵ��䣾;��������䣾;����д��䣾;�����գ�;|�������䣾����������䣾;
//�������
void sta(){
	//����ʱ�Ѿ���ȡ�׷���
	int index=0;
	if(symid==IFSYM){			//����if��Ϊ�������
		ifsta();
	}
	else if(symid==FORSYM){		//����for��Ϊѭ�����
		forsta();
	}
	else if(symid==SWITCHSYM){	//����switch��Ϊ������
		switchsta();
	}
	else if(symid==LBRACE){						//����{��Ϊ�����
		getsym();
		stalist();								//��������д������
		if(symid==RBRACE){						//����}
			getsym();
		}
		else{
			//error:Ӧ�� }
			errormessage(RBRACE_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//����id if for switch scanf return printf ; { }
			return;
		}
	}
	else if(symid==ID){							//����id,Ϊ�з���ֵ����������� �� �޷���ֵ����������� �� ��ֵ���
		strcpy(token1,token);					//������ǰ�����ķ�����Ϣ
		symid1=symid;
		getsym();
		if(symid==ASSIGN || symid==LBRACKET){	//����[��ֵ��=,����Ϊ��ֵ���
			assignsta();						//���ø�ֵ��䴦�����
		}
		else if(symid==LPAR){					//����(,Ϊ��������
			//�˴�Ӧ����ű��ж����޷���ֵ,������Ϊ�з���ֵ
			index=searchtab(token1,1);

			if(index==-1){
				//error:δ���庯��
				errormessage(UNDEFID_ERROR,line,no);
				getsym();
				skip(10,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//����id if for switch scanf return printf ; { }
				return;
			}
			else if(tab[index].type!=6 && tab[index].type!=7 && tab[index].type!=8){//�������ֺ���
				//error:���Ǻ���(�������)
				errormessage(STA_ERROR,line,no);
				getsym();
				skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//����id if for switch scanf return printf ; { }
				return;
			}


			if(tab[index].type==6 || tab[index].type==7){
				callretfunc(index);					//�����з���ֵ�����������
			}
			else if(tab[index].type==8){
				callnoretfunc(index);				//�����޷���ֵ�����������
			}
		}
		else{
			//error:ȱ��=�򣨻�[
			errormessage(ASSIGN_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//����if for switch scanf return printf ; { }
			return;
		}

		if(symid==SEMICOLON){	//����������ӷֺ�
			getsym();
		}
		else{
			//error:Ӧ��;
			errormessage(SEMICOLON_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//����id if for switch scanf return printf ; { }
			return;
		}
	}
	else if(symid==SCANFSYM){	//����scanf,Ϊ�����
		scansta();
		if(symid==SEMICOLON){	//������ӷֺ�
			getsym();
		}
		else{
			//error:Ӧ��;
			errormessage(SEMICOLON_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//����id if for switch scanf return printf ; { }
			return;
		}
	}
	else if(symid==PRINTFSYM){	//����printf,Ϊд���
		printsta();
		if(symid==SEMICOLON){	//д����ӷֺ�
			getsym();
		}
		else{
			//error:Ӧ��;
			errormessage(SEMICOLON_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//����id if for switch scanf return printf ; { }
			return;
		}
	}
	else if(symid==RETURNSYM){	//����return��Ϊ�������
		returnsta();
		if(symid==SEMICOLON){	//��������ӷֺ�
			getsym();
		}
		else{
			//error:Ӧ��;
			errormessage(SEMICOLON_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//����id if for switch scanf return printf ; { }
			return;
		}
	}
	else if(symid==SEMICOLON){	//����䣬������;
		getsym();
	}
	else{
		//error:���ͷ������
		errormessage(STA_ERROR,line,no);
		return;
	}

	//printf("this is a statement\n");
	fprintf(grammarfile,"%s","this is a statement\n");
}



//������У�   ::= ������䣾��
//��������г���
void stalist(){
	//����ʱ�Ѿ���ȡ���׷���
	while(isstahead(symid)==1){
		sta();
	};
	//printf("this is a statement list\n");
	fprintf(grammarfile,"%s","this is a statement list\n");
}


//���з���ֵ�������壾  ::= (int|char)����ʶ������(������������)�� ��{����������䣾��}��
//�����з���ֵ��������
void hasretfunc(){				
	//���뺯��ʱ,�Ѿ�������(
	char temp_num[tokensize]={'\0'};
	int midtype;
	strcpy(id,token2);				//���溯����
	type=(symid1==INTSYM)?6:7;		//���溯������
	inserttab(id,type,0,0,0,0);		//��������޸�len(��������) size(������ռ�ֽ�)
	addr=0;							//���뺯��addr=0
	midtype=(type==6)?INTFUNCDEF:CHARFUNCDEF;			//�����м����
	midcode(midtype,id,NULL,NULL);

	lev=1;							//�ں�����lev=1
	getsym();	
	len=paralist();						//���ò����������
	tab[funcindex[funclen-1]].len=len;

	if(symid!=RPAR){					//����)
		//error:Ӧ��}
		errormessage(RPAR_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//����const int char id if for switch scanf return printf semicolon { }
	}
	else{
		getsym();
	}
	
	if(symid!=LBRACE){				//����{
		//error:ȱ��{
		errormessage(LBRACE_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//����const int char id if for switch scanf return printf semicolon { }
	}
	else{
		getsym();
	}
	
	compoundsta();		//���ø���������
	if(symid!=RBRACE){			//����}
		//error:Ӧ��}
		errormessage(RBRACE_ERROR,line,no);
		skip(3,INTSYM,VOIDSYM,CHARSYM);
		lev=0;
		addr=0;
		return;
	}
	//���¼��㺯����size
	size=0;
	for(int i=funcindex[funclen-1]+1;i<tablen;i++){	//���������Ĳ�����������������ͳ���ܵ�size��С
		size+=tab[i].size;
	}
	tab[funcindex[funclen-1]].size=size;				//���ܵĴ�С��ֵ�������ı���
	lev=0;
	addr=0;
	getsym();
			
		

	//Ĭ�����к�����β������return 0
	sprintf(temp_num,"%d",0);
	midcode(RETEXPOP,temp_num,NULL,NULL);
	//printf("this is a function with return value\n");
	fprintf(grammarfile,"%s","this is a function with return value\n");
}


//��������    ::=  �����ͱ�ʶ��������ʶ����{,�����ͱ�ʶ��������ʶ����}|���գ�
//���������ĳ���,���ز����ĸ���
int paralist(){
	//���뺯��ʱ�Ѿ���ȡ�ˣ����ͱ�ʶ���� �� )
	int k=0;//��¼�ж��ٸ�����
	int midtype=0;
	if(symid==RPAR){	//�޲���
		//printf("this is a paralist\n");
		fprintf(grammarfile,"%s","this is a paralist\n");
		return k;
	}

	while(true){
		if(symid==INTSYM || symid==CHARSYM){			//����int��char
			type=(symid==INTSYM)?9:10;					//int�Ͳ�����Ӧtype=9��char�Ͳ�����Ӧtype=10
			len=0;
			size=sizecalcu(type,len);					//������ռ�ֽ�
			getsym();
			if(symid==ID){			//������ʶ��
				strcpy(id,token);
				inserttab(id,type,0,lev,size,addr);
				addr+=size;
				k++;

				midtype=(type==9)?INTPARA:CHARPARA;		//�����м����
				midcode(midtype,id,NULL,NULL);

				getsym();
				if(symid==COMMA){	//����, ˵�����в���
					getsym();
					continue;
				}
				else{				//û��,ֱ������
					break;
				}
			}
			else{
				//error:Ӧ�б�ʶ��
				errormessage(ID_ERROR,line,no);
				skip(2,RPAR,LBRACE);
				//����{ )
				return k;
			}
		}
		else{
			//error:ȱ�����ͱ�ʶ��int,char

			errormessage(TYPE_ERROR,line,no);
			skip(2,RPAR,LBRACE);
			//����{ )
			return k;
		}
	}

	//printf("this is a paralist\n");
	fprintf(grammarfile,"%s","this is a paralist\n");
	return k;
}

//���޷���ֵ�������壾  ::=void����ʶ������(������������)����{����������䣾��}��
//�����޷���ֵ��������
void noretfunc(){				
	//���뺯��ʱ�Ѿ���ȡ�ˣ���ʶ����
	char temp_num[tokensize]={'\0'};
	strcpy(id,token);				//���溯����
	type=8;		//���溯������
	inserttab(id,type,0,0,0,0);		//��������޸�len(��������) size(������ռ�ֽ�)
	midcode(VOIDFUNCDEF,id,NULL,NULL);			//�����м���룬void id();
	addr=0;							//���ຯ��ƫ��ֵΪ0
	lev=1;							//�ں�����lev=1
	getsym();	

	if(symid!=LPAR){	//�ٶ���
		//error:Ӧ��(
		errormessage(LPAR_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//����const int char id if for switch scanf return printf semicolon { }
	}
	getsym();
	len=paralist();
	tab[funcindex[funclen-1]].len=len;	//��¼��������

	if(symid!=RPAR){    //���� )
		//error:Ӧ��)
		errormessage(RPAR_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//����const int char id if for switch scanf return printf semicolon { }
	}
	else{
		getsym();
	}

	
	if(symid!=LBRACE){
		//error:Ӧ��{
		errormessage(LBRACE_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//����const int char id if for switch scanf return printf ; { }
	}
	else{
		getsym();
	}
	compoundsta();		//���ø���������
	if(symid!=RBRACE){			//����}
		//error:Ӧ��}
		errormessage(RBRACE_ERROR,line,no);
		skip(3,INTSYM,VOIDSYM,CHARSYM);
		//����int char void
		addr=0;							//���ຯ��ƫ��ֵΪ0
		lev=0;
	}
	size=0;
	for(int i=funcindex[funclen-1]+1;i<tablen;i++){	//���������Ĳ�����������������ͳ���ܵ�size��С
		size+=tab[i].size;
	}
	tab[funcindex[funclen-1]].size=size;				//���ܵĴ�С��ֵ�������ı���
	addr=0;							//���ຯ��ƫ��ֵΪ0
	lev=0;
	getsym();

				

	//Ĭ�����к�����β������return 0
	sprintf(temp_num,"%d",0);
	midcode(RETEXPOP,temp_num,NULL,NULL);
	//printf("this is a function without return value\n");
	fprintf(grammarfile,"%s","this is a function without return value\n");

}

//����������    ::= void main��(����)�� ��{����������䣾��}��
//����������
void mainfunction(){			
	//���뺯��ʱ�Ѷ���main
	midcode(VOIDFUNCDEF,"main",NULL,NULL);			//�����м���룬void id();
	inserttab(token,8,0,0,0,0);		//����ű�
	addr=0;
	lev=1;

	getsym();
	if(symid!=LPAR){				//��ȡ(
		//error:ȱ��(
		errormessage(LPAR_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//����const int char id if for switch scanf return printf semicolon { }
	}
	else{
		getsym();
	}	
	
	if(symid!=RPAR){			//��ȡ)
		//error:ȱ��)
		errormessage(RPAR_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//����const int char id if for switch scanf return printf semicolon { }
	}
	else{
		getsym();
	}
	
	if(symid!=LBRACE){		//��ȡ{
		//error:ȱ��{
		errormessage(LBRACE_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//����const int char id if for switch scanf return printf semicolon { }
	}
	else{
		getsym();
	}
	compoundsta();		//���ø���������

	if(symid!=RBRACE){	//����}
		errormessage(RBRACE_ERROR,line,no);
		addr=0;
		lev=0;
		return;
	}
	size=0;
	for(int i=funcindex[funclen-1]+1;i<tablen;i++){	//���������Ĳ�����������������ͳ���ܵ�size��С
		size+=tab[i].size;
	}
	tab[funcindex[funclen-1]].size=size;				//���ܵĴ�С��ֵ�������ı���
	addr=0;
	lev=0;
	midcode(SETLABEL,"Label_end",NULL,NULL);			//���ó��������label
	
		
	
	//printf("this is a main function\n");
	fprintf(grammarfile,"%s","this is a main function\n");
}


//�﷨��������
void program(){
	getsym();
	if(symid==CONSTSYM){				//����const
		conststa();						//���ó���˵���������
	}
	int flag=0;							//flag=0˵�����ڱ���˵���׶�,flag=1˵�����ڱ���˵���׶�
	while(true){						//ѭ���������б���˵�����Լ���һ�� �з���ֵ��������
		if(symid==INTSYM || symid==CHARSYM){//����Ϊ����˵�����з���ֵ��������,��Ҫ�������2���ַ�	
			strcpy(token1,token);			//������ǰ�����ķ�����Ϣ
			symid1=symid;
			getsym();
			if(symid==ID){					//�ֶ�����ʶ��
				strcpy(token2,token);		//������ǰ�����ķ�����Ϣ
				symid2=symid;
				getsym();
				if(symid==LBRACKET || symid==SEMICOLON || symid==COMMA){	//����[ , ; ˵���Ǳ���˵��
					if(flag!=0){
						//error:��������׶��л��б���˵��
						errormessage(VARDEF_ERROR,line,no);
					}
					variabledef();				//���ñ������崦�����
					continue;
				}
				else{	

					if(symid!=LPAR){
						//error:ȱ��(
						errormessage(LPAR_ERROR,line,no);
						skip(3,LPAR,ID,RPAR);	//����( ,ID ,)
					}

					flag=1;
					hasretfunc();			//û�ж���[ , ;,˵�����з���ֵ��������
											//��Ȼ�Ѿ����� �з���ֵ�������� ,��ô����˵���Ѿ�����,
											//�������������з���ֵ�������塢�޷���ֵ�������塢void main
				}
			}
			else{
				//error:ȱ�ٱ�ʶ��
				errormessage(ID_ERROR,line,no);
				skip(2,ID,MAINSYM);
			}
		}
		else if(symid==VOIDSYM){				//����void
			getsym();
			if(symid==MAINSYM){					//����main
				mainfunction();	
				break;							//main���������Ӧ�����κη���
			}
			else if(symid==ID){		
				noretfunc();					//�����޷���ֵ�����������
			}
			else{
				//error: Ӧ��id��main
				errormessage(ID_ERROR,line,no);
				skip(2,ID,MAINSYM);
			}
		}
		else{
			//error:Ӧ��char int ��void
			errormessage(FUNCTYPE_ERROR,line,no);
			skip(3,VOIDSYM,INTSYM,CHARSYM);
			//return;
		}
	}
	//printf("this is a program\n");
	fprintf(grammarfile,"%s","this is a program\n");
}










#endif