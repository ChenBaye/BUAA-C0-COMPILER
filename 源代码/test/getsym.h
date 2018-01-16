#ifndef _GETSYM_H_
#define _GETSYM_H_

#include "error.h"
#include<stdio.h> 
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
#include<stdarg.h>
//*******************************************
//������
//*******************************************
#define nrw 14//		number of reserved words
#define CONSTSYM 0//	const
#define INTSYM  1//		int
#define CHARSYM 2//		char
#define VOIDSYM 3//		void
#define IFSYM 4//		if
#define ELSESYM 5//		else
#define SWITCHSYM 6//	switch
#define CASESYM 7//		case
#define DEFAULTSYM 8//	default
#define RETURNSYM 9//	return
#define FORSYM 10//		for
#define SCANFSYM 11//	scanf
#define PRINTFSYM 12//	printf
#define MAINSYM 13//	main
//*******************************************
//�����ַ�
//*******************************************
#define PLUS 14//		+
#define SUB 15//		-
#define MUL 16//		*
#define DIV 17//		/
#define	LPAR 18//		(
#define RPAR 19//		)
#define LBRACKET 20//	[
#define RBRACKET 21//	]
#define LBRACE 22//		{
#define RBRACE 23//		}
#define ASSIGN 24//		=
#define SEMICOLON 25//	;
#define DOUBLEQUO 26//	"
#define SINGLEQUO 27//	'
#define BIG 26//		>
#define SMALL 29//		<
#define COLON 30//		:
#define COMMA 31//		,
//*******************************************
//����ַ���ɵĲ�����
//*******************************************
#define BIGEQL 32//		>=
#define SMALLEQL 33//	<=
#define EQUAL 34//		==
#define NOTEQL 35//		!=
//*******************************************
//����
//*******************************************
#define ID 36//			identify
#define ANUMBER 37//	�޷�������
#define ACHAR 38//		a char
#define ASTRING 39//	string

#define isplus(c)	c=='+'?1:0		//+
#define issub(c)	c=='-'?1:0		//-
#define ismul(c)	c=='*'?1:0		//*
#define isdiv(c)	c=='/'?1:0		// /
#define islpar(c)	c=='('?1:0		//(
#define isrpar(c)	c==')'?1:0		//)
#define islbt(c)	c=='['?1:0		//[
#define isrbt(c)	c==']'?1:0		//]
#define islbe(c)	c=='{'?1:0		//{
#define isrbe(c)	c=='}'?1:0		//}
#define isassi(c)	c=='='?1:0		//=
#define issemi(c)	c==';'?1:0		//;
#define isdoubq(c)	c=='\"'?1:0		//"
#define issingq(c)	c=='\''?1:0		//'
#define isbig(c)	c=='>'?1:0		//>
#define issmall(c)	c=='<'?1:0		//<
#define isnot(c)	c=='!'?1:0		//!
#define iscolon(c)	c==':'?1:0		//:
#define iscomma(c)	c==','?1:0		//,
#define isspace(c)	c==' '?1:0		//space
#define istab(c)	c==9?1:0		//tab
#define isnewline(c) c=='\n'?1:0	//newline
#define	isstrch(c)	(c==32 || c==33 || (c>=35 && c<=126))?1:0//�ַ����еĺϷ��ַ�
#define isletter(c)	(c=='_' || isalpha(c))?1:0				//��ĸ:'_','a'->'z','A'->'Z'

#define tokensize 200				//token�����С
#define symsize 20					//sym�����С



//����������
const char* resword[nrw]={
	"const",	//0
	"int",		//1
	"char",		//2
	"void",		//3
	"if",		//4
	"else",		//5
	"switch",	//6
	"case",		//7
	"default",	//8
	"return",	//9
	"for"	,	//10
	"scanf",	//11
	"printf",	//12
	"main"		//13
};
//�����ֶ�Ӧ���Ƿ�
const char* ressym[nrw]={
	"CONSTSYM",		//0
	"INTSYM",		//1
	"CHARSYM",		//2
	"VOIDSYM",		//3
	"IFSYM",		//4
	"ELSESYM",		//5
	"SWITCHSYM",	//6
	"ASESYM",		//7
	"DEFAULTSYM",	//8
	"RETURNSYM",	//9
	"FORSYM",		//10
	"SCANFSYM",		//11
	"PRINTFSYM",	//12
	"MAINSYM"		//13
};

char token[tokensize]={'\0'};		//����������ַ���
int tokenlen=0;						//token���ַ�������
int num=0;							//�����ȡ������
int line=1;							//��ǰ����
int	no=0;							//��ǰ�ַ�Ϊ�����е�no���ַ�
char sym[symsize]={'\0'};			//����token�ķ������Ƿ�
int symid=0;						//����token�ķ��Ŷ�Ӧ����
char c;								//������ʱ�������ַ�
FILE *fp=NULL;						//Դ������ļ�ָ��
FILE *grammarfile=NULL;				//�﷨������������
int count=1;						//��¼���ǵڼ�������
//int countinline=1;					//��¼���Ǳ��еڼ�������



//��������
int isreserve();		//�Ƿ�Ϊ������
char getch();			//��һ���ַ�
void printtoken();		//��ӡtoken
void memsettoken();		//��ʼ��token
void skip(int arg_num,...);		//����������...
//void errormessage(int error,int line,int no); //���������Ϣ
void getsym();			//��ȡ��һ������


//���������������������...
void skip(int arg_num,...){
	int i=0;
	int symbol_arr[50];	//�洢����
	va_list var_arg=NULL;      //����va_list����
	va_start(var_arg, arg_num);//׼�����ʿɱ����(��arg_num��ʼ)

	for(i=0;i<arg_num;i++){
		symbol_arr[i]=va_arg(var_arg,int);
	}
	va_end(var_arg);   //��ɿɱ�����Ĵ���

	while(true){
		for(i=0;i<arg_num;i++){//��������
			if(symid==symbol_arr[i] || c==EOF){
				if(c==EOF){
					exit(0);
				}

				return;
			}
		}
		getsym();
	}
}



int isreserve(){		//�ж�token�Ƿ��Ǳ�����,���Ƿ���-1,���򷵻��ڱ����ֱ������
	int i=0;
	for(i=0;i<nrw;i++){
		if(strcmp(token,resword[i])==0){//����resword
			return i;
		}
	}
	return -1;
}


char getch(){		//���ڶ�ȡһ���ַ�
	c=getc(fp);
	no++;					//��һ���ַ�,no+1
	return c;
}


void printtoken(){			//��ӡtoken
	printf("%d: %s %s\n",count,sym,token);
	count++;
}

void memsettoken(){			//token,sym,symid,tokenlen,num��ʼ��
	memset(token,0,tokensize);
	tokenlen=0;
	memset(sym,0,symsize);
	symid=0;
	num=0;
}


void getsym(){
	char c='\0';
	int i=0;
	while((c=getch())!=EOF){
		int error=0;
		//printf("%c",c);
		memsettoken();					//��ʼ��token,tokenlen,sym,symid,num

		if(istab(c) || isspace(c)){		//tab,space����
			continue;
		}
		else if(isnewline(c)){			//����,��line+1��no=0
			line++;
			no=0;
			//countinline=1;
			continue;
		}
		else if(isletter(c)){			//������ĸ
			while(isletter(c) || isdigit(c)){
				token[tokenlen]=c;
				tokenlen++;				//����token,��tokenlen+1
				c=getch();				
			}
			token[tokenlen]='\0';		//�Ѿ�����������ĸ������
			if(c!=EOF){
				fseek(fp,-1,1);			//û�ж����ļ���β���ʹӵ�ǰλ����һ�ַ�
				no--;					//���˺�no-1
			}

			if((i=isreserve())==-1){		//˵�����Ǳ�����
				strcpy(sym,"ID");
				symid=ID;
			}
			else{
				strcpy(sym,ressym[i]);		//�Ǳ����֣��򽫱��������Ƿ�����sym
				symid=i;
			}
		}
		else if(isdigit(c)){				//��������
			int temp=no;					//��¼���ֵ�һ���ַ�λ��
			while(isdigit(c)){
				token[tokenlen]=c;			//����token,��tokenlen+1
				tokenlen++;
				c=getch();
			}
			token[tokenlen]='\0';
			if(c!=EOF){
				fseek(fp,-1,1);				//û�ж����ļ���β���ʹӵ�ǰλ����һ�ַ�
				no--;						//no-1
			}
			num=atoi(token);				//�ַ���ת����
			if(token[0]=='0' && num!=0){	//��0��ͷ������,�Ҳ���0
				//error
				//Ӧ�ñ���
				//
				//
				error=ZEROSTARTNUM_ERROR;
				errormessage(error,line,temp);//����errormessage
			}
			strcpy(sym,"ANUMBER");
			symid=ANUMBER;
		}

		else if(issingq(c)){				//����������,Ӧ�����ַ�
			c=getch();						//�ٶ�һ���ַ�
			if(isplus(c) || issub(c) || ismul(c) || isdiv(c) || isletter(c) || isdigit(c)){	//�ַ�ֻ���� + - * / ��ĸ ����
				num=(int)c;					//���ַ�����num,token��
				token[tokenlen]=c;
				tokenlen++;
			}
			else{							//�Ƿ��ַ�
				//error
				//Ӧ�ñ���
				error=ILEGALCH_ERROR;
				errormessage(error,line,no);
			}
			c=getch();
			if(issingq(c)){					//�ֶ���������
				strcpy(sym,"ACHAR");
				symid=ACHAR;
			}
			else{							//���򱨴�,����
				//error
				//Ӧ�ñ���
				error=SINGLEQ_ERROR;
				errormessage(error,line,no);

				if(c!=EOF){
					fseek(fp,-1,1);				//û�ж����ļ���β���ʹӵ�ǰλ����һ�ַ�
					no--;
				}
			}
		}

		else if(isdoubq(c)){				//����˫����,Ӧ�����ַ���
			c=getch();
			while(isstrch(c)){				//�ַ����е��ַ�Ӧ�úϷ�,ʮ���Ʊ���Ϊ32,33,35-126��ASCII�ַ�,����Ϊ�մ�
				token[tokenlen]=c;
				tokenlen++;
				c=getch();
			}
			if(isdoubq(c)){					//����ĩβ��˫����
				strcpy(sym,"ASTRING");
				symid=ASTRING;
			}
			else{							//���򱨴�
				//error
				//Ӧ�ñ���
				error=DOUBLEQ_ERROR;
				errormessage(error,line,no);
				if(c!=EOF){
					fseek(fp,-1,1);				//û�ж����ļ���β���ʹӵ�ǰλ����һ�ַ�
					no--;
				}
			}
		}

		else if(issmall(c)){					//����С�ں�,����Ϊ<��<=
			token[tokenlen]=c;
			tokenlen++;
			c=getch();	
			if(isassi(c)){						//�ֶ������ں�,˵����<=
				token[tokenlen]=c;
				tokenlen++;
				strcpy(sym,"SMALLEQL");
				symid=SMALLEQL;
			}
			else{								//������<,���л���
				strcpy(sym,"SMALL");
				symid=SMALL;
				if(c!=EOF){
					fseek(fp,-1,1);				//û�ж����ļ���β���ʹӵ�ǰλ����һ�ַ�
					no--;
				}
			}
		}

		else if(isbig(c)){					//�������ں�,����Ϊ>��>=
			token[tokenlen]=c;
			tokenlen++;
			c=getch();	
			if(isassi(c)){						//�ֶ������ں�,˵����>=
				token[tokenlen]=c;
				tokenlen++;
				strcpy(sym,"BIGEQL");
				symid=BIGEQL;
			}
			else{								//������>,���л���
				strcpy(sym,"BIG");
				symid=BIG;
				if(c!=EOF){
					fseek(fp,-1,1);				//û�ж����ļ���β���ʹӵ�ǰλ����һ�ַ�
					no--;
				}
			}
		}

		else if(isassi(c)){					//���������ں�,����Ϊ=��==
			token[tokenlen]=c;
			tokenlen++;
			c=getch();	
			if(isassi(c)){						//�ֶ������ں�,˵����==
				token[tokenlen]=c;
				tokenlen++;
				strcpy(sym,"EQUAL");
				symid=EQUAL;
			}
			else{								//������=,���л���
				strcpy(sym,"ASSIGN");
				symid=ASSIGN;
				if(c!=EOF){
					fseek(fp,-1,1);				//û�ж����ļ���β���ʹӵ�ǰλ����һ�ַ�
					no--;
				}
			}
		}
		
		else if(isnot(c)){						//����!,Ӧ��Ϊ!=
			token[tokenlen]=c;
			tokenlen++;
			c=getch();	
			if(isassi(c)){						//�ֶ������ں�,˵����!=
				token[tokenlen]=c;
				tokenlen++;
				strcpy(sym,"NOTEQL");
				symid=NOTEQL;
			}
			else{								//���򱨴�,������
				//error
				//��������
				error=ASSIGN_ERROR;
				errormessage(error,line,no);
				if(c!=EOF){
					fseek(fp,-1,1);				//û�ж����ļ���β���ʹӵ�ǰλ����һ�ַ�
					no--;
				}
			}
		}

		else if(isplus(c)){						//+
			token[tokenlen++]=c;
			strcpy(sym,"PLUS");
			symid=PLUS;
		}
		else if(issub(c)){						//-
			token[tokenlen++]=c;
			strcpy(sym,"SUB");
			symid=SUB;
		}
		else if(ismul(c)){						//*
			token[tokenlen++]=c;
			strcpy(sym,"MUL");
			symid=MUL;
		}
		else if(isdiv(c)){						// /
			token[tokenlen++]=c;
			strcpy(sym,"DIV");
			symid=DIV;
		}
		else if(islpar(c)){						//(
			token[tokenlen++]=c;
			strcpy(sym,"LPAR");
			symid=LPAR;
		}
		else if(isrpar(c)){						//)
			token[tokenlen++]=c;
			strcpy(sym,"RPAR");
			symid=RPAR;
		}
		else if(islbt(c)){						//[
			token[tokenlen++]=c;
			strcpy(sym,"LBRACKET");
			symid=LBRACKET;
		}
		else if(isrbt(c)){						//]
			token[tokenlen++]=c;
			strcpy(sym,"RBRACKET");
			symid=RBRACKET;
		}
		else if(islbe(c)){						//{
			token[tokenlen++]=c;
			strcpy(sym,"LBRACE");
			symid=LBRACE;
		}
		else if(isrbe(c)){						//}
			token[tokenlen++]=c;
			strcpy(sym,"RBRACE");
			symid=RBRACE;
		}
		else if(issemi(c)){						//;
			token[tokenlen++]=c;
			strcpy(sym,"SEMICOLON");
			symid=SEMICOLON;
		}
		else if(iscolon(c)){					//:
			token[tokenlen++]=c;
			strcpy(sym,"COLON");
			symid=COLON;
		}
		else if(iscomma(c)){					//,
			token[tokenlen++]=c;
			strcpy(sym,"COMMA");
			symid=COMMA;
		}
		else{
			//error
			//����
			error=ILEGALCH_ERROR;
			errormessage(error,line,no);
		}

		if(error==0){
			//countinline++;
			//printtoken();
		}
		return;
	}
	//exit(0);	//��������ļ�ֱ�ӽ�������
}

/*
int main(void){
	int t;
	char* path=(char *)malloc(50*sizeof(char));
	printf("path of file:");			//������Գ���ľ���·��
	scanf("%s",path);
	fp=fopen(path,"r");
	if(fp==NULL){
		printf("can not open this file.");
	}
	for(t=0;t<594;t++){
		getsym();
	}
	
	fclose(fp);
	system("pause");
}

*/


#endif