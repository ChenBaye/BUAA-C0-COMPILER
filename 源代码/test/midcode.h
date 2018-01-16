#ifndef _MIDCODE_H_
#define _MIDCODE_H_
#include<stdio.h> 
#include<ctype.h>
#include<string.h>
#include<stdlib.h>

#define ADDOP			1	//�Ӳ���*			
#define SUBOP			2	//������*			
#define MULOP			3	//�˲���*
#define DIVOP			4	//������*
#define INTFUNCDEF		5	//int��������*
#define CHARFUNCDEF		6	//char��������*
#define VOIDFUNCDEF		7	//void��������*
	
#define	FUNCCALL		8	//��������*
#define PARAOP			9	//�������ݲ���*
#define	FUNCRET			10	//��������ֵ*

#define CONSTINTDEF		11	//int��������*
#define CONSTCHARDEF	12	//char��������*
#define VARINTDEF		13	//int��������*
#define VARCHARDEF		14	//char��������*
#define	INTARRDEF		15	//int���鶨��*
#define CHARARRDEF		16	//char���鶨��*
#define	VARASSIGN		17	//��������ֵ*
#define	ARRASSIGN		18	//������Ԫ�ظ�ֵ*
#define ARRUSE			19	//��������Ԫ�ص�ֵ*


#define PRINTSTROP		20	//printf �ַ���*
#define PRINTINTOP		21	//printf integer*
#define PRINTCHAROP		22	//printf char*
#define	SCANINTOP		23	//scanf int
#define SCANCHAROP		24	//scanf char
#define RETEXPOP		25	//return ���ʽ*
#define RETNULLOP		26	//return ��*
#define	SETLABEL		27	//���ñ�ǩ*
#define BIGEQLOP		28	//		>=*
#define SMALLEQLOP		29	//		<=*
#define EQUALOP			30	//		==*
#define NOTEQLOP		31	//		!=*
#define BIGOP			32	//		>*
#define SMALLOP			33	//		<*

#define GOTOOP			34	//��������ת*
#define BNZOP			35	//����������ת not zero*
#define BZOP			36	//������������ת zero*
#define INTPARA			37	//int�β�*
#define CHARPARA		38	//char�β�*
#define PRINTLINE		39	//����

#define opsize			200
struct MIDCODE{
	int type;
	char op1[opsize];
	char op2[opsize];
	char result[opsize];
};



struct MIDCODE mid[2048];	//���ڴ洢�м����
struct MIDCODE com[2048];	//�����Ż��м����
int v[2048]={0};			//���ɾ�����м����(1����ɾ��)
int midlen=0;				//�洢���м���������(�������������壬�������������棩				



int tempmid_len=0;			//�м���볤��
FILE* midfile;			//�����м������ļ�ָ��
int tempvar_count=0;	//��¼��ʱ��������
int label_count=0;		//��¼label����


//�����Ƿ�������
int isrealnum(char *s){
	if(isdigit(s[0]) || s[0]=='+' || s[0]=='-'){
		return 1;
	}
	return 0;
}

//�����ϲ��Ż������codeһ���м����
void constcombine(MIDCODE* code){
	int i=0;
	if(code->type<=4 && code->type>=1){
		if(isrealnum(code->op1) && isrealnum(code->op2)){
			int x=atoi(code->op1);
			int y=atoi(code->op2);
			switch(code->type){
				case ADDOP:x=x+y;break;
				case SUBOP:x=x-y;break;
				case MULOP:x=x*y;break;
				case DIVOP:x=x/y;break;
			}
			code->type=VARASSIGN;
			memset(code->op1,0,opsize);
			memset(code->op2,0,opsize);
			sprintf(code->op2,"%d",x);
			strcpy(code->op1,code->result);
			memset(code->result,0,opsize);
		}
	}
	
}


//���ƴ����Ż� ��x=y,��x��y����
void copyconv(){
	int i=0;
	int j=0;
	for(i=0;i<midlen;i++){
		char before[200]={'\0'};
		char now[200]={'\0'};


		//�ȵ��ó����ϲ��Ż�����
		constcombine(&com[i]);

		if(com[i].type==VARASSIGN){	//������ֵ
			strcpy(before,com[i].op1);
			strcpy(now,com[i].op2);
			for(j=i+1;j<midlen;j++){
				if((com[j].type>=INTFUNCDEF && com[j].type<=VOIDFUNCDEF) || j==midlen-1 || com[j].type==FUNCCALL || (com[j].type>=GOTOOP  && com[j].type<=BZOP) || com[j].type==SETLABEL){
					//����label ���� ��ת��� ���� �������� ���� ������ͷ ���� �м����Ľ�β
					//���ƴ����ͽ�����
					//if(com[i].op1[0]=='$'){	//�����ֵ�������м����,���ɾ������
					//	v[i]=1;
					//}
					break;
				}
				else if(com[j].type==VARASSIGN){								//����������ֵ�����before��now�Ƿ����¸�ֵ
					if(strcmp(com[j].op2,before)==0){
						memset(com[j].op2,0,200);
						strcpy(com[j].op2,now);
						if(com[i].op1[0]=='$'){									//�����ֵ�������м����,���ɾ������
							v[i]=1;
						}
					}
					if(strcmp(com[j].op1,before)==0 || strcmp(com[j].op1,now)==0){//before��now�����¸�ֵ
						break;
					}
				}
				else if(com[j].type<=DIVOP && com[j].type>=ADDOP){			//����+ - * /,�������
					if(strcmp(com[j].op1,before)==0){					//op1��before
						memset(com[j].op1,0,200);
						strcpy(com[j].op1,now);
						if(com[i].op1[0]=='$'){	//�����ֵ�������м����,���ɾ������
							v[i]=1;
						}
					}
					if(strcmp(com[j].op2,before)==0){					//op2��before
						memset(com[j].op2,0,200);
						strcpy(com[j].op2,now);
						if(com[i].op1[0]=='$'){	//�����ֵ�������м����,���ɾ������
							v[i]=1;
						}
					}
					if(strcmp(com[j].result,before)==0 || strcmp(com[j].result,now)==0){//���before��now�����¸�ֵ
						break;
					}
				}
				else if(com[j].type==ARRASSIGN){				//����������Ԫ�ظ�ֵ op1[op2] = result
					if(strcmp(com[j].result,before)==0){					//result��before
						memset(com[j].result,0,200);
						strcpy(com[j].result,now);
						if(com[i].op1[0]=='$'){	//�����ֵ�������м����,���ɾ������
							v[i]=1;
						}
					}
					if(strcmp(com[j].op2,before)==0){					//op2��before
						memset(com[j].op2,0,200);
						strcpy(com[j].op2,now);
						if(com[i].op1[0]=='$'){	//�����ֵ�������м����,���ɾ������
							v[i]=1;
						}
					}
				}

				else if(com[j].type>=BIGEQLOP && com[j].type<=SMALLOP){			//����>= == <= != > <
					if(strcmp(com[j].op1,before)==0){					//op1��before
						memset(com[j].op1,0,200);
						strcpy(com[j].op1,now);
						if(com[i].op1[0]=='$'){	//�����ֵ�������м����,���ɾ������
							v[i]=1;
						}
					}
					if(strcmp(com[j].op2,before)==0){					//op2��before
						memset(com[j].op2,0,200);
						strcpy(com[j].op2,now);
						if(com[i].op1[0]=='$'){	//�����ֵ�������м����,���ɾ������
							v[i]=1;
						}
					}
				}
				else if(com[j].type==PARAOP || com[j].type==PRINTINTOP || com[j].type==PRINTCHAROP || com[j].type==RETEXPOP){
					//printint, printchar ,push para,return value
					if(strcmp(com[j].op1,before)==0){					//op1��before
						memset(com[j].op1,0,200);
						strcpy(com[j].op1,now);
						if(com[i].op1[0]=='$'){	//�����ֵ�������м����,���ɾ������
							v[i]=1;
						}
					}
				}

				else if(com[j].type==ARRUSE){							//result = op1[op2]
					if(strcmp(com[j].op2,before)==0){					//op2��before
						memset(com[j].op2,0,200);
						strcpy(com[j].op2,now);
						if(com[i].op1[0]=='$'){							//�����ֵ�������м����,���ɾ������
							v[i]=1;
						}
					}
				}

			}
		}
	}
}


//�����ʱ������Ӧ�ַ���
void gettemp(char* str){		
	tempvar_count++;
	memset(str,0,200);
	sprintf(str,"$t_%d",tempvar_count);	//���ַ���д��str
}


//��ñ�ǩ��Ӧ�ַ���
void getlabel(char * str){		
	memset(str,0,200);
	label_count++;
	sprintf(str,"Label_%d",label_count);	//���ַ���д��str
}


//�����м����
void midcode(int type,char* op1,char* op2,char* result){
	//���������\����\������������
	/*if(!((type>=CONSTINTDEF && type<=CHARARRDEF) || type==INTPARA || type==CHARPARA)){
		mid[midlen].type=type;
		if(op1!=NULL){
			strcpy(mid[midlen].op1,op1);
		}
		if(op2!=NULL){
			strcpy(mid[midlen].op2,op2);
		}
		if(result!=NULL){
			strcpy(mid[midlen].result,result);
		}
		midlen++;
	
	}*/

	com[midlen].type=type;
	if(op1!=NULL){
		strcpy(com[midlen].op1,op1);
	}
	if(op2!=NULL){
		strcpy(com[midlen].op2,op2);
	}
	if(result!=NULL){
		strcpy(com[midlen].result,result);
	}
	midlen++;
	
}


//��com���Ż����м���뿽��mid�в�д��midcode.txt�ļ�
void writemidcode(){
	int i=0;
	int max=midlen;
	//midlen����
	midlen=0;
	for(i=0;i<max;i++){
		if(v[i]==1){			//�м���뱻ɾ��
			continue;
		}
		//���������\����\������������
		if(!((com[i].type>=CONSTINTDEF && com[i].type<=CHARARRDEF) || com[i].type==INTPARA || com[i].type==CHARPARA)){
			mid[midlen].type=com[i].type;
			if(com[i].op1!=NULL){
				strcpy(mid[midlen].op1,com[i].op1);
			}
			if(com[i].op2!=NULL){
				strcpy(mid[midlen].op2,com[i].op2);
			}
			if(com[i].result!=NULL){
				strcpy(mid[midlen].result,com[i].result);
			}
			midlen++;
		}
		switch(com[i].type){
				case ADDOP: 
					fprintf(midfile,"%s = %s + %s\n",com[i].result,com[i].op1,com[i].op2);		//result = op1 + op2
					break;
				case SUBOP:
					fprintf(midfile,"%s = %s - %s\n",com[i].result,com[i].op1,com[i].op2);		//result = op1 - op2
					break;
				case MULOP:
					fprintf(midfile,"%s = %s * %s\n",com[i].result,com[i].op1,com[i].op2);		//result = op1 * op2
					break;
				case DIVOP:
					fprintf(midfile,"%s = %s / %s \n",com[i].result,com[i].op1,com[i].op2);		//result = op1 / op2
					break;
				case INTFUNCDEF:
					fprintf(midfile,"int %s()\n",com[i].op1);				//int op1()
					break;
				case CHARFUNCDEF:
					fprintf(midfile,"char %s()\n",com[i].op1);				//char op1()
					break;
				case VOIDFUNCDEF:
					fprintf(midfile,"void %s()\n",com[i].op1);				//void op1()
					break;
				case FUNCCALL:
					fprintf(midfile,"call %s\n",com[i].op1);				//call op1
					break;
				case PARAOP:
					fprintf(midfile,"push %s\n",com[i].op1);				//push op1(op1������һ�����ֶ���id)
					break;
				case FUNCRET:
					fprintf(midfile,"%s = RET\n",com[i].op1);				//op1=RET(op1����һ�������ķ���ֵ)
					break;
				case CONSTINTDEF:
					fprintf(midfile,"const int %s = %s\n",com[i].op1,com[i].op2);	//const int op1 = op2
					break;
				case CONSTCHARDEF:
					fprintf(midfile,"const char %s = %s\n",com[i].op1,com[i].op2);//const char op1 = op2
					break;
				case VARINTDEF:
					fprintf(midfile,"var int %s\n",com[i].op1);			//var int op1
					break;
				case VARCHARDEF:
					fprintf(midfile,"var char %s\n",com[i].op1);			//var char op1
					break;
				case INTARRDEF:
					fprintf(midfile,"var int %s[%s]\n",com[i].op1,com[i].op2);	//var int op1[op2]
					break;
				case CHARARRDEF:
					fprintf(midfile,"var char %s[%s]\n",com[i].op1,com[i].op2);	//var char op1[op2]
					break;
				case VARASSIGN:
					fprintf(midfile,"%s = %s\n",com[i].op1,com[i].op2);					//op1 = op2
					break;
				case ARRASSIGN:
					fprintf(midfile,"%s[%s] = %s\n",com[i].op1,com[i].op2,com[i].result);	//op1[op2] = result
					break;
				case ARRUSE:
					fprintf(midfile,"%s = %s[%s]\n",com[i].result,com[i].op1,com[i].op2);	//result = op1[op2]
					break;
				case PRINTSTROP:
					fprintf(midfile,"print \"%s\"\n",com[i].op1);				//print str
					break;
				case PRINTINTOP:
					fprintf(midfile,"print %s\n",com[i].op1);					//print integer
					break;
				case PRINTCHAROP:
					fprintf(midfile,"print \'%s\'\n",com[i].op1);					//print char
					break;
				case SCANINTOP:
					fprintf(midfile,"scan int %s\n",com[i].op1);				//scan int op1
					break;
				case SCANCHAROP:
					fprintf(midfile,"scan char %s\n",com[i].op1);				//scan char op1
					break;
				case RETEXPOP:
					fprintf(midfile,"ret %s\n",com[i].op1);					//ret op1
					break;
				case RETNULLOP:
					fprintf(midfile,"ret null\n");						//ret null
					break;
				case SETLABEL:
					fprintf(midfile,"%s:\n",com[i].op1);						//op1(label)
					break;
				case BIGEQLOP:
					fprintf(midfile,"%s >= %s\n",com[i].op1,com[i].op2);				//op1 >= op2
					break;
				case SMALLEQLOP:
					fprintf(midfile,"%s <= %s\n",com[i].op1,com[i].op2);				//op1 <= op2
					break;
				case EQUALOP:
					fprintf(midfile,"%s == %s\n",com[i].op1,com[i].op2);				//op1 == op2
					break;
				case NOTEQLOP:
					fprintf(midfile,"%s != %s\n",com[i].op1,com[i].op2);				//op1 != op2
					break;
				case BIGOP:
					fprintf(midfile,"%s > %s\n",com[i].op1,com[i].op2);				//op1 > op2
					break;
				case SMALLOP:
					fprintf(midfile,"%s < %s\n",com[i].op1,com[i].op2);				//op1 < op2
					break;
				case GOTOOP:
					fprintf(midfile,"GOTO %s\n",com[i].op1);					//GOTO op1
					break;
				case BNZOP:
					fprintf(midfile,"BNZ %s\n",com[i].op1);					//BNZ op1
					break;
				case BZOP:
					fprintf(midfile,"BZ %s\n",com[i].op1);						//BZ op1
					break;
				case INTPARA:
					fprintf(midfile,"para int %s\n",com[i].op1);				//para int op1��������
					break;
				case CHARPARA:
					fprintf(midfile,"para char %s\n",com[i].op1);				//para char op1
					break;
				case PRINTLINE:
					fprintf(midfile,"printline\n");						//printline
					break;
				default:;
				}
	}

}


#endif