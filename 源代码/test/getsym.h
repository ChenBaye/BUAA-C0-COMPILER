/*
 * getsym.h
 *
 *  Created on: 2018年1月8日
 *      Author: ChenShuwei
 */

#ifndef _GETSYM_H_
#define _GETSYM_H_

#include "error.h"
#include<stdio.h> 
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
#include<stdarg.h>
//*******************************************
//保留字
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
//单个字符
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
//多个字符组成的操作符
//*******************************************
#define BIGEQL 32//		>=
#define SMALLEQL 33//	<=
#define EQUAL 34//		==
#define NOTEQL 35//		!=
//*******************************************
//类型
//*******************************************
#define ID 36//			identify
#define ANUMBER 37//	无符号整数
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
#define	isstrch(c)	(c==32 || c==33 || (c>=35 && c<=126))?1:0//字符串中的合法字符
#define isletter(c)	(c=='_' || isalpha(c))?1:0				//字母:'_','a'->'z','A'->'Z'

#define tokensize 200				//token数组大小
#define symsize 20					//sym数组大小



//保留字数组
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
//保留字对应助记符
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

char token[tokensize]={'\0'};		//储存读到的字符串
int tokenlen=0;						//token中字符串长度
int num=0;							//储存读取的数字
int line=1;							//当前行数
int	no=0;							//当前字符为本行中第no个字符
char sym[symsize]={'\0'};			//储存token的符号助记符
int symid=0;						//储存token的符号对应编码
char c;								//储存临时读到的字符
FILE *fp=NULL;						//源代码的文件指针
FILE *grammarfile=NULL;				//语法分析的输出结果
int count=1;						//记录这是第几个符号
//int countinline=1;					//记录这是本行第几个符号



//函数声明
int isreserve();		//是否为保留字
char getch();			//读一个字符
void printtoken();		//打印token
void memsettoken();		//初始化token
void skip(int arg_num,...);		//跳读到符号...
//void errormessage(int error,int line,int no); //输出错误信息
void getsym();			//读取下一个符号


//变参数函数，跳读到符号...
void skip(int arg_num,...){
	int i=0;
	int symbol_arr[50];	//存储参数
	va_list var_arg=NULL;      //声明va_list变量
	va_start(var_arg, arg_num);//准备访问可变参数(从arg_num开始)

	for(i=0;i<arg_num;i++){
		symbol_arr[i]=va_arg(var_arg,int);
	}
	va_end(var_arg);   //完成可变参数的处理

	while(true){
		for(i=0;i<arg_num;i++){//遍历参数
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



int isreserve(){		//判断token是否是保留字,不是返回-1,是则返回在保留字表的索引
	int i=0;
	for(i=0;i<nrw;i++){
		if(strcmp(token,resword[i])==0){//遍历resword
			return i;
		}
	}
	return -1;
}


char getch(){		//用于读取一个字符
	c=getc(fp);
	no++;					//读一个字符,no+1
	return c;
}


void printtoken(){			//打印token
	printf("%d: %s %s\n",count,sym,token);
	count++;
}

void memsettoken(){			//token,sym,symid,tokenlen,num初始化
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
		memsettoken();					//初始化token,tokenlen,sym,symid,num

		if(istab(c) || isspace(c)){		//tab,space跳过
			continue;
		}
		else if(isnewline(c)){			//换行,则line+1及no=0
			line++;
			no=0;
			//countinline=1;
			continue;
		}
		else if(isletter(c)){			//读到字母
			while(isletter(c) || isdigit(c)){
				token[tokenlen]=c;
				tokenlen++;				//存入token,且tokenlen+1
				c=getch();				
			}
			token[tokenlen]='\0';		//已经读到不是字母或数字
			if(c!=EOF){
				fseek(fp,-1,1);			//没有读到文件结尾，就从当前位置退一字符
				no--;					//回退后no-1
			}

			if((i=isreserve())==-1){		//说明不是保留字
				strcpy(sym,"ID");
				symid=ID;
			}
			else{
				strcpy(sym,ressym[i]);		//是保留字，则将保留字助记符拷到sym
				symid=i;
			}
		}
		else if(isdigit(c)){				//读到数字
			int temp=no;					//记录数字第一个字符位置
			while(isdigit(c)){
				token[tokenlen]=c;			//存入token,且tokenlen+1
				tokenlen++;
				c=getch();
			}
			token[tokenlen]='\0';
			if(c!=EOF){
				fseek(fp,-1,1);				//没有读到文件结尾，就从当前位置退一字符
				no--;						//no-1
			}
			num=atoi(token);				//字符串转整数
			if(token[0]=='0' && num!=0){	//以0开头的数字,且不是0
				//error
				//应该报错
				//
				//
				error=ZEROSTARTNUM_ERROR;
				errormessage(error,line,temp);//调用errormessage
			}
			strcpy(sym,"ANUMBER");
			symid=ANUMBER;
		}

		else if(issingq(c)){				//读到单引号,应该是字符
			c=getch();						//再读一个字符
			if(isplus(c) || issub(c) || ismul(c) || isdiv(c) || isletter(c) || isdigit(c)){	//字符只能是 + - * / 字母 数字
				num=(int)c;					//将字符存在num,token中
				token[tokenlen]=c;
				tokenlen++;
			}
			else{							//非法字符
				//error
				//应该报错
				error=ILEGALCH_ERROR;
				errormessage(error,line,no);
			}
			c=getch();
			if(issingq(c)){					//又读到单引号
				strcpy(sym,"ACHAR");
				symid=ACHAR;
			}
			else{							//否则报错,回退
				//error
				//应该报错
				error=SINGLEQ_ERROR;
				errormessage(error,line,no);

				if(c!=EOF){
					fseek(fp,-1,1);				//没有读到文件结尾，就从当前位置退一字符
					no--;
				}
			}
		}

		else if(isdoubq(c)){				//读到双引号,应该是字符串
			c=getch();
			while(isstrch(c)){				//字符串中的字符应该合法,十进制编码为32,33,35-126的ASCII字符,可能为空串
				token[tokenlen]=c;
				tokenlen++;
				c=getch();
			}
			if(isdoubq(c)){					//读到末尾是双引号
				strcpy(sym,"ASTRING");
				symid=ASTRING;
			}
			else{							//否则报错
				//error
				//应该报错
				error=DOUBLEQ_ERROR;
				errormessage(error,line,no);
				if(c!=EOF){
					fseek(fp,-1,1);				//没有读到文件结尾，就从当前位置退一字符
					no--;
				}
			}
		}

		else if(issmall(c)){					//读到小于号,可能为<或<=
			token[tokenlen]=c;
			tokenlen++;
			c=getch();	
			if(isassi(c)){						//又读到等于号,说明是<=
				token[tokenlen]=c;
				tokenlen++;
				strcpy(sym,"SMALLEQL");
				symid=SMALLEQL;
			}
			else{								//否则是<,进行回退
				strcpy(sym,"SMALL");
				symid=SMALL;
				if(c!=EOF){
					fseek(fp,-1,1);				//没有读到文件结尾，就从当前位置退一字符
					no--;
				}
			}
		}

		else if(isbig(c)){					//读到大于号,可能为>或>=
			token[tokenlen]=c;
			tokenlen++;
			c=getch();	
			if(isassi(c)){						//又读到等于号,说明是>=
				token[tokenlen]=c;
				tokenlen++;
				strcpy(sym,"BIGEQL");
				symid=BIGEQL;
			}
			else{								//否则是>,进行回退
				strcpy(sym,"BIG");
				symid=BIG;
				if(c!=EOF){
					fseek(fp,-1,1);				//没有读到文件结尾，就从当前位置退一字符
					no--;
				}
			}
		}

		else if(isassi(c)){					//读到等于于号,可能为=或==
			token[tokenlen]=c;
			tokenlen++;
			c=getch();	
			if(isassi(c)){						//又读到等于号,说明是==
				token[tokenlen]=c;
				tokenlen++;
				strcpy(sym,"EQUAL");
				symid=EQUAL;
			}
			else{								//否则是=,进行回退
				strcpy(sym,"ASSIGN");
				symid=ASSIGN;
				if(c!=EOF){
					fseek(fp,-1,1);				//没有读到文件结尾，就从当前位置退一字符
					no--;
				}
			}
		}
		
		else if(isnot(c)){						//读到!,应该为!=
			token[tokenlen]=c;
			tokenlen++;
			c=getch();	
			if(isassi(c)){						//又读到等于号,说明是!=
				token[tokenlen]=c;
				tokenlen++;
				strcpy(sym,"NOTEQL");
				symid=NOTEQL;
			}
			else{								//否则报错,并回退
				//error
				//报错并回退
				error=ASSIGN_ERROR;
				errormessage(error,line,no);
				if(c!=EOF){
					fseek(fp,-1,1);				//没有读到文件结尾，就从当前位置退一字符
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
			//报错
			error=ILEGALCH_ERROR;
			errormessage(error,line,no);
		}

		if(error==0){
			//countinline++;
			//printtoken();
		}
		return;
	}
	//exit(0);	//读完代码文件直接结束程序
}

/*
int main(void){
	int t;
	char* path=(char *)malloc(50*sizeof(char));
	printf("path of file:");			//输入测试程序的绝对路径
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
