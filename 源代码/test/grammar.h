/*
 * grammar.h
 *
 *  Created on: 2018年1月8日
 *      Author: ChenShuwei
 */


#ifndef _GRAMMAR_H_
#define	_GRAMMAR_H_

#include"getsym.h"
#include"midcode.h"
#include"error.h"
#include<stdio.h> 
#include<ctype.h>
#include<string.h>
#include <stdlib.h>


//符号表表项
#define tabsize 256
struct tabentry{
	char id[tokensize];//取前tokensize个字符
	int type;	//0-int, 1-char ,2-const int, 3-const char,4-int数组,5-char数组,
				//6-返回int的函数,7-返回char的函数,8-返回void的函数,9-int型参数,10-char型参数
	int len;	//数组长度或函数参数个数或常量的值
	int lev;	//只可能为0(全局标识符:函数、全局常量、全局变量)或1(局部标识符:局部常量，局部变量)
	int size;	//常变量所占字节大小或函数中参数及常变量所占字节大小之和
	int addr;	//标识符存储地址偏移(相对于本函数)
};

								//用于临时储存表项的各种信息
char id[tokensize]={'\0'};
int type=0;
int len=0;
int addr=0;
int lev=0;
int size=0;

struct tabentry tab[tabsize];		//符号表
int tablen=0;						//已经存了tablen个标识符
//int globalindex[tabsize]={0};		//各个全局变量、常量在tab的索引
int globallen=0;					//已经存了globallen个全局量,全局变量一定是在符号表的前globallen个
int funcindex[tabsize]={0};			//各个函数在tab的索引
int	funclen=0;						//已经存了funclen个函数

char token1[tokensize]={'\0'};	//储存预读前的信息
int symid1=0;
char token2[tokensize]={'\0'};	//储存预读前的信息
int symid2=0;

//*********************************
//函数声明
//*********************************
int sizecalcu(int type,int len);							//计算标识符对应类型占内存大小
void inserttab(char* id,int type,int len,int lev,int size,int addr);	//填符号表(用于声明阶段)
int searchtab(char* id,int whichtype);						//查符号表(用于使用阶段)
void constdef();			//处理常量定义
void conststa();			//处理常量说明
void variabledef();			//处理变量定义
void compoundsta();			//处理复合语句
void hasretfunc();			//处理有返回值函数
void noretfunc();			//处理无返回值函数
void mainfunction();		//处理主函数
int  integer();				//处理计算整数
void program();				//处理语法分析
int  paralist();			//处理参数表
int	 expression(char* temp);//处理表达式(返回值表示类型0-int ， 1-char)
void stalist();				//处理语句列
void sta();					//处理语句
int  term(char* temp);		//处理项(返回值表示类型0-int ， 1-char)
int  factor(char* temp);	//处理因子(返回值表示类型0-int ， 1-char)
void valueofpara(int index);//处理值参数表(参数为函数在符号表的索引)
void returnsta();			//处理返回语句
void printsta();			//处理写语句
void scansta();				//处理读语句
void ifsta();				//处理if-else
void switchsta();			//处理switch-case
void forsta();				//处理for循环
void assignsta();			//处理赋值语句
void callretfunc(int index);	//处理有返回值函数的调用(参数为函数在符号表的索引)
void callnoretfunc(int index);	//处理无返回值函数调用(参数为函数在符号表的索引)
int	 isstahead(int symid);	//语句可能的第一个符号
void condition();			//处理条件
void casesta(char* temp_exp,char* label,char* default_label,int switch_type);//情况表处理程序
void defaultsta(char* default_label);			//缺省处理程序
void casesubsta(char* temp_exp,char* mylabel,char* nextlabel,int switch_type);			//情况表子语句


//计算符号所需字节数,为便于处理，int和char都为4字节
int sizecalcu(int type,int len){
	if(type==1 || type==3 || type==10){				//const char,char,char型参数
		return sizeof(int);
	}
	else if(type==0 || type==2 || type==9){			//const int,int,int型参数
		return sizeof(int);
	}
	else if(type==4){								//int数组
		return len*sizeof(int);
	}
	else{											//char数组
		return len*sizeof(int);
	}
}




//填符号表
void inserttab(char* id,int type,int len,int lev,int size,int addr){
	if(tablen>=tabsize){
		//error:tab满了
		errormessage(OUTOFTAB_ERROR,line,no);
		return;
	}
	//1.全局量、函数与全局量、函数不可重名
	//2.同一函数的局部变量不可重名
	else{														//检查是否重名
		if(lev==0 || type==6 || type==7 || type==8){			//标识符是全局量或函数名
			for(int i=0;i<tablen;i++){
				if(tab[i].lev!=0){
					continue;
				}
				if(strcmp(tab[i].id,id)==0){						//错误，全局量、函数和（函数，全局量)不可重名
					//error:全局量、函数与全局量、函数不可重名
					errormessage(GLOBALNAME_ERROR,line,no);
					return;
				}
			}
		}
		else{													//标识符是局部常、变量,说明此时还在一个函数中
			for(int i=funcindex[funclen-1]+1;i<tablen;i++){		//funcindex存的是函数标识符索引，故要加1
				if(strcmp(tab[i].id,id)==0){
					//error:局部常、变量重名							//错误,同一函数的局部常变、量不可重名
					errormessage(LOCALNAME_ERROR,line,no);
					return;
				}
			}
		}

	}
	//没有重名才填符号表
	tab[tablen].addr=addr;		//addr
	strcpy(tab[tablen].id,id);	//id
	tab[tablen].len=len;		//len
	tab[tablen].lev=lev;		//lev
	tab[tablen].type=type;		//type
	tab[tablen].size=size;		//size
	//接着分别填funcindex和globalindex
	if(lev==0 && type>=0 && type<=5){		//全局常、变量,可能为const int或const char或int或char或int数组或char数组
		//globalindex[globallen]=tablen;		//type =0 1 2 3 4 5之一，则为全局常、变量
		globallen++;
	}
	else if(type==6 || type==7 || type==8){	//三种函数
		funcindex[funclen]=tablen;
		funclen++;
	}

	tablen++;								//tablen加1
	return;
}


//在使用符号时查询符号表,返回索引   查询已知id可能为 函数-1, 常量、变量标识符(非数组)-2,数组-3
int searchtab(char* id,int whichtype){		
	if(whichtype==1){				//是函数
		for(int i=0;i<funclen;i++){
			if(strcmp(tab[funcindex[i]].id,id)==0){
				return funcindex[i];
			}
		}
	}
	else if(whichtype==2){						//是常量、变量标识符
												//先遍历同一函数(如果有)下的局部变量(需要排除数组)
		if(funclen!=0){							//不存在任何函数（funclen==0）这种情况应该早就已经报错
			for(int i=funcindex[funclen-1]+1;i<tablen;i++){
				if(strcmp(tab[i].id,id)==0 && tab[i].type!=4 && tab[i].type!=5){
					return i;
				}
			}
		}
		else{
			//error:对变量的使用不在函数中，程序不可能运行到这里
		}
												//如果没找到，再遍历全局常、变量(需要排除数组)
		for(int i=0;i<globallen;i++){
			if(strcmp(tab[i].id,id)==0 && tab[i].type!=4 && tab[i].type!=5){
				return i;
			}
		}
	}
	else{										//是数组,方法与上面相同
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
	//error:没有找到标识符,未定义的标识符，在外层函数报错
	return -1;

}


//打印符号表
void printtab(){
	for(int i=0;i<tablen;i++){
		printf("%d:id=%s type=%d len=%d lev=%d size=%d addr=%d\n",i,tab[i].id,tab[i].type,tab[i].len,tab[i].lev,tab[i].size,tab[i].addr);
	}
}





//语句的头部可能是 if for switch { id scanf printf return ;
int isstahead(int symid){
	if(symid==IFSYM || symid==FORSYM || symid==SWITCHSYM || symid==LBRACE || symid==ID || symid==SCANFSYM || symid==PRINTFSYM || symid==RETURNSYM || symid==SEMICOLON){
		return 1;
	}
	else{
		return 0;
	}
}


//＜返回语句＞   ::=  return[‘(’＜表达式＞‘)’]  
//处理返回语句
void returnsta(){
	//进入函数时，已读到return
	char temp[tokensize];			//存储临时变量

	getsym();
	if(symid==LPAR){				//读到(		可能为return(表达式);
		getsym();				//再读一个
		expression(temp);		//调用处理表达式程序
		if(symid==RPAR){
			midcode(RETEXPOP,temp,NULL,NULL);			//生成中间代码
			getsym();
		}
		else{
			//error:应有)
			errormessage(RPAR_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//跳到id if for switch scanf return printf ; { }
			return;
		}
	}
	else{					//可能为		return;
		midcode(RETNULLOP,NULL,NULL,NULL);		//产生中间代码 return null	
	}

	//printf("this is a return statement\n");
	fprintf(grammarfile,"%s","this is a return statement\n");
}


//＜写语句＞    ::= printf ‘(’ ＜字符串＞,＜表达式＞ ‘)’| printf ‘(’＜字符串＞ ‘)’| printf ‘(’＜表达式＞‘)’
//处理写语句
void printsta(){
	//进入程序时已经读到printf
	int exp_type=0;				//表达式的类型
	char str[tokensize]={'\0'};
	char temp[tokensize]={'\0'};		//存储临时变量
	getsym();
	if(symid==LPAR){
		getsym();
		if(symid==ASTRING){		//读到＜字符串＞
			strcpy(str,token);	//temp只保存字符串的值
			midcode(PRINTSTROP,str,NULL,NULL);
			getsym();
			if(symid==COMMA){	//读到,						
				getsym();
				exp_type=expression(temp);	//调用表达式处理程序
				if(symid==RPAR){//读到)
					if(exp_type==1){		//表达式是char类型
						midcode(PRINTCHAROP,temp,NULL,NULL);			//产生中间代码
					}
					else{
						midcode(PRINTINTOP,temp,NULL,NULL);			//产生中间代码
					}
					
					getsym();
				}
				else{
					//error:应有)
					errormessage(RPAR_ERROR,line,no);
					skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//跳到id if for switch scanf return printf ; { }
					return;
				}
			}
			else if(symid==RPAR){	//读到)
				getsym();
			}
			else{
				//error:应有)
				errormessage(RPAR_ERROR,line,no);
				skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//跳到id if for switch scanf return printf ; { }
				return;
			}
		}
		else{					//可能是表达式
			exp_type=expression(temp);
			if(symid==RPAR){	//读到)
				if(exp_type==1){		//表达式是char类型
					midcode(PRINTCHAROP,temp,NULL,NULL);			//产生中间代码
				}
				else{
					midcode(PRINTINTOP,temp,NULL,NULL);			//产生中间代码
				}
				getsym();
			}
			else{
				//error:应有)
				errormessage(RPAR_ERROR,line,no);
				skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//跳到id if for switch scanf return printf ; { }
				return;
			}
		}
	}
	else{
		//error:应有(
		errormessage(LPAR_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到if for switch scanf return printf ; { }
		return;
	}

	midcode(PRINTLINE,NULL,NULL,NULL);	//换行
	//printf("this is a print statement\n");
	fprintf(grammarfile,"%s","this is a print statement\n");
}

//＜读语句＞    ::=  scanf ‘(’＜标识符＞{,＜标识符＞}‘)’
//读语句
void scansta(){
	//进入函数时，已经读到scanf
	getsym();
	if(symid==LPAR){				//读到(
		do{
			getsym();
			if(symid==ID){
				int index=searchtab(token,2);	//查符号表
				if(index==-1){
					//error:未定义标识符
					errormessage(UNDEFID_ERROR,line,no);
					skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//跳到 if for switch scanf return printf ; { }
					return;
				}
				else if(tab[index].type==0 || tab[index].type==9){
					midcode(SCANINTOP,token,NULL,NULL);				//生成中间代码
					//int 变量或参数
					getsym();
				}
				else if(tab[index].type==1 || tab[index].type==10){
					midcode(SCANCHAROP,token,NULL,NULL);			//生成中间代码
					//int char变量或参数
					getsym();
				}
				else{
					//error:变量类型错误，应为int char 变量
					errormessage(VARIABLE_ERROR,line,no);
					skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//跳到 if for switch scanf return printf ; { }
					return;
				}
			
			}
			else{
				//error:应有id
				errormessage(ID_ERROR,line,no);
				return;
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//跳到 if for switch scanf return printf ; { }
			}
		}while(symid==COMMA);			//必须有至少一个标识符

		if(symid==RPAR){				//读到)
			getsym();
		}
		else{
			//error:应有)
			errormessage(RPAR_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//跳到id if for switch scanf return printf ; { }
			return;
		}
	}
	else{
		//error:应有(
		errormessage(LPAR_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 if for switch scanf return printf ; { }
		return;
	}

	//printf("this is a scan statement\n");
	fprintf(grammarfile,"%s","this is a scan statement\n");
}



//＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞‘[’＜表达式＞‘]’=＜表达式＞
//处理赋值语句
void assignsta(){
	//进入函数时已经读到了= 或[
	char op1[tokensize]={'\0'};
	char op2[tokensize]={'\0'};
	char result[tokensize]={'\0'};
	strcpy(id,token1);				//储存变量名
	strcpy(op1,token1);
	//printtab();
	//printf("%s\n",id);

	if(symid==ASSIGN){				//= int或char变量
		int index=searchtab(id,2);	//查符号表

		if(index==-1){
			//error:未声明变量
			errormessage(UNDEFID_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//跳到 if for switch scanf return printf ; { }
			return;
		}
		else if(tab[index].type==0 || tab[index].type==1 || tab[index].type==9 || tab[index].type==10){	//= int或char变量
			getsym();
			expression(op2);				//调用表达式处理程序
			midcode(VARASSIGN,op1,op2,NULL);		//产生中间代码
		}
		else{
			//error:不是变量
			errormessage(VARIABLE_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//跳到 if for switch scanf return printf ; { }
			return;
		}
	}
	else{							//[ int或char数组
		int index=searchtab(id,3);	//查符号表
		if(index==-1){
			//error:未声明变量
			errormessage(UNDEFID_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//跳到 if for switch scanf return printf ; { }
			return;
		}
		else if(tab[index].type==4 || tab[index].type==5){	//查表后发现是int或char数组
			getsym();
			expression(op2);			//调用表达式处理程序
			if(symid==RBRACKET){		//读到]
				getsym();
				if(symid==ASSIGN){		//读到=
					getsym();
					expression(result);		//调用表达式处理程序
					midcode(ARRASSIGN,op1,op2,result);		//产生中间代码
				}
				else{
					//error:应有=
					errormessage(ASSIGN_ERROR,line,no);
					skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//跳到if for switch scanf return printf ; { }
					return;
				}
			}
			else{
				//error:应有]
				errormessage(RBRACKET_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//跳到id if for switch scanf return printf ; { }
				return;
			}
		}
		else{
			//error:不是数组
			errormessage(NOTARR_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//跳到id if for switch scanf return printf ; { }
			return;
		}
			
	}

	//printf("this is a assign statement\n");
	fprintf(grammarfile,"%s","this is a assign statement\n");
}

//＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞ //表达式为0条件为假，否则为真
//处理条件
void condition(){
	//进入函数时，已经读取了首符号
	char op1[tokensize]={'\0'};
	char op2[tokensize]={'\0'};
	int temp_type=0;
	expression(op1);			//调用表达式处理程序
	//读到关系运算符
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
		expression(op2);//调用表达式处理程序
		midcode(temp_type,op1,op2,NULL);			//产生中间代码
	}
	else{
		//表达式即是条件
	}

		
	//printf("this is a condition\n");
	fprintf(grammarfile,"%s","this is a condition\n");
}


//＜条件语句＞  ::=  if ‘(’＜条件＞‘)’＜语句＞［else＜语句＞］
//处理if-else语句
void ifsta(){
	//进入函数时已经读到if
	char label1[tokensize]={'\0'};
	char label2[tokensize]={'\0'};
	getsym();
	if(symid==LPAR){				//读到(
		getsym();			
		condition();				//调用条件处理程序
									
															//生成中间代码											//产生一个label序号
		getlabel(label1);						//产生label字符串,存在label1中
		getlabel(label2);
		midcode(BZOP,label1,NULL,NULL);			//条件为假时跳转到label1（若有else则为else的开头，否则为if-else语句的结尾）


		if(symid==RPAR){			//读到)
			getsym();
			sta();					//调用语句处理程序


			midcode(GOTOOP,label2,NULL,NULL);			//能运行到此处说明条件为真，跳过else
			midcode(SETLABEL,label1,NULL,NULL);			//设置label1		

			if(symid==ELSESYM){		//读到else
				getsym();
				sta();
			}
			midcode(SETLABEL,label2,NULL,NULL);			//设置label2
		}
		else{
			//error:应有)
			errormessage(RPAR_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//跳到 if for switch scanf return printf ; { }
			return;
		}
	}
	else{
		//error:读到(
		errormessage(LPAR_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到id if for switch scanf return printf ; { }
		return;
	}

	//printf("this is an if-else \n");
	fprintf(grammarfile,"%s","this is an if-else \n");
}


//＜循环语句＞   ::=  for‘(’＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞‘)’＜语句＞
//处理for循环 先处理 ＜标识符＞＝＜表达式＞ ，再处理 ＜标识符＞＝＜标识符＞(+|-)＜步长＞，再处理<条件>
void forsta(){
	//进入函数时，已经读到了for
	struct MIDCODE temp_code2[2];			//暂存加减步长产生的中间代码
	char temp_num[tokensize]={'\0'};				//i=0
	int temp_type=0;								//label1:
	int index1=0;									//循环体									
	int index2=0;									//i++								
	char temp1[tokensize]={'\0'};					//条件					
	char temp2[tokensize]={'\0'};					//BNZ label1			
	char label1[tokensize]={'\0'};	
	char label2[tokensize]={'\0'};
	char label3[tokensize]={'\0'};
											
	getlabel(label1);				//获取1个label		
	getlabel(label2);				//获取1个label	
	getlabel(label3);				//获取1个label	
	gettemp(temp2);														

	getsym();															
	if(symid!=LPAR){				//读到(	
		//error:应有(
		errormessage(LPAR_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 if for switch scanf return printf ; { }
		return;
	}
	getsym();														
	if(symid!=ID){					//读到标识符	
		//error:应有id
		errormessage(ID_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 if for switch scanf return printf ; { }
		return;
	}
	index1=searchtab(token,2);
	if(index1==-1){
		//error:未定义变量及参数
		errormessage(UNDEFID_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 if for switch scanf return printf ; { }
		return;
	}
	else if(tab[index1].type!=0 && tab[index1].type!=1 && tab[index1].type!=9 && tab[index1].type!=10 ){
		//error:不是int char变量及参数
		errormessage(VARIABLE_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 if for switch scanf return printf ; { }
		return;
	}

	getsym();
	if(symid!=ASSIGN){		//读到=
		//error:应有=
		errormessage(ASSIGN_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 if for switch scanf return printf ; { }
		return;
	}
	getsym();
	expression(temp1);		//调用表达式处理程序
	midcode(VARASSIGN,tab[index1].id,temp1,NULL);		//生成中间代码，i=temp1
	midcode(GOTOOP,label2,NULL,NULL);					//生成中间代码，goto label2
	midcode(SETLABEL,label1,NULL,NULL);					//生成中间代码，set label1
	if(symid!=SEMICOLON){		//读到;
		//error:应有;
		errormessage(SEMICOLON_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 if for switch scanf return printf ; { }
		return;
	}

	getsym();
	condition();			//调用条件处理程序

	midcode(BZOP,label3,NULL,NULL);					//生成中间代码，BZ label3


	if(symid!=SEMICOLON){	//读到;
		//error:应有;
		errormessage(SEMICOLON_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 if for switch scanf return printf ; { }
		return;
	}

	
	getsym();
	if(symid!=ID){		//读到id
		//error:应有id;
		errormessage(ID_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到id if for switch scanf return printf ; { }
		return;

	}
		
	if(strcmp(token,tab[index1].id)!=0){
		//error:for循环出现其他标识符
		errormessage(DIFFVAR_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到id if for switch scanf return printf ; { }
		return;
	}
							
	getsym();
	if(symid!=ASSIGN){	//读到=
		//error:应有=
		errormessage(ASSIGN_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 if for switch scanf return printf ; { }
		return;
	}
	getsym();
	if(symid!=ID){	//读到id
		//error:应有id
		errormessage(ID_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 if for switch scanf return printf ; { }
		return;
	}
									
	if(strcmp(token,tab[index1].id)!=0){
		//error:for循环出现其他标识符
		errormessage(DIFFVAR_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 if for switch scanf return printf ; { }
		return;
	}
	getsym();
	if(symid!=PLUS && symid!=SUB){		//读到+或-
		//error:缺少+ -
		errormessage(ADDSUB_ERROR,line,no);
		skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到id if for switch scanf return printf ; { }
		return;
	}
	temp_type=(symid==PLUS)?ADDOP:SUBOP;	
	getsym();
	if(symid!=ANUMBER || num==0){	//读到 无符号非零整数
		//error:步长错误，应为无符号非零整数
		errormessage(STEP_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 if for switch scanf return printf ; { }
		return;
	}
	sprintf(temp_num,"%d",num);		//数字转数组

//使用temp_code2暂时储存步长加减产生的两个中间代码
	temp_code2[0].type=temp_type;
	strcpy(temp_code2[0].op1,tab[index1].id);				//生成中间代码，temp2=i+/-num
	strcpy(temp_code2[0].op2,temp_num);
	strcpy(temp_code2[0].result,temp2);
	
	temp_code2[1].type=VARASSIGN;
	strcpy(temp_code2[1].op1,tab[index1].id);		//生成中间代码，i=temp2
	strcpy(temp_code2[1].op2,temp2);

	getsym();
	if(symid!=RPAR){
		//error:缺少)
		errormessage(RPAR_ERROR,line,no);
		skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到id if for switch scanf return printf ; { }
		return;
	}

	midcode(SETLABEL,label2,NULL,NULL);					//生成中间代码，set label2
	getsym();
	sta();
														
	
	//将暂存的代码写入文件
	midcode(temp_code2[0].type,temp_code2[0].op1,temp_code2[0].op2,temp_code2[0].result);
	midcode(temp_code2[1].type,temp_code2[1].op1,temp_code2[1].op2,temp_code2[1].result);

	midcode(GOTOOP,label1,NULL,NULL);					//生成中间代码，goto label1
	midcode(SETLABEL,label3,NULL,NULL);					//生成中间代码，set label3

	//printf("this is a for-loop\n");
	fprintf(grammarfile,"%s","this is a for-loop\n");
}


//＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’
//处理switch-case
void switchsta(){
	//进入函数时已经读到switch
	int switch_type=0;				//switch的表达式类型
	char label[tokensize]={'\0'};
	char default_label[tokensize]={'\0'};		//default_label由case子语句赋值
	char temp_exp[tokensize]={'\0'};
	gettemp(temp_exp);		//产生接受表达式值的临时变量
	getlabel(label);
	getsym();
	if(symid==LPAR){		//读到(
		getsym();
		switch_type=expression(temp_exp);	//调用表达式处理程序
		if(symid==RPAR){
			getsym();
			if(symid==LBRACE){		//读到{
				getsym();
				if(symid==CASESYM){		//读到case
					casesta(temp_exp,label,default_label,switch_type);		//调用情况表处理程序
					if(symid==DEFAULTSYM){		//读到default
						defaultsta(default_label);			//调用缺省处理程序
						if(symid==RBRACE){		//读到}
							midcode(SETLABEL,label,NULL,NULL);		//设置label
							getsym();
						}
						else{
							//error:应为}
							errormessage(RBRACE_ERROR,line,no);
							skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
							//跳到 case default if for switch scanf return printf ; { }
							return;
						}
					}
					else{
						//error:缺失default
						errormessage(DEFAULT_ERROR,line,no);
						skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
						//跳到 case default if for switch scanf return printf ; { }
						return;
					}
				}
				else{
					//error:缺失case
					errormessage(CASE_ERROR,line,no);
					skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//跳到 case default if for switch scanf return printf ; { }
					return;
				}
			}
			else{
				//error:应为{
				errormessage(LBRACE_ERROR,line,no);
				skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//跳到 case default if for switch scanf return printf ; { }
				return;
			}
		}
		else{
			//error:应为)
			errormessage(RPAR_ERROR,line,no);
			skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//跳到 case default if for switch scanf return printf ; { }
			return;
		}
	}
	else{
		//error:应有(
		errormessage(LPAR_ERROR,line,no);
		skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 case default if for switch scanf return printf ; { }
		return;
	}

	//printf("this is a switch-case statement\n");
	fprintf(grammarfile,"%s","this is a switch-case statement\n");
}


//＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}
//处理情况表
void casesta(char* temp_exp,char* label,char* default_label,int switch_type){
	//进入函数时已读取到case
	char mylabel[tokensize]={'\0'};
	char nextlabel[tokensize]={'\0'};
	getlabel(mylabel);			//先获得第一个case的label
	do{
		casesubsta(temp_exp,mylabel,nextlabel,switch_type);
		midcode(GOTOOP,label,NULL,NULL);			//case结束直接跳到结尾

		memset(mylabel,0,tokensize);				//以下将nextlabel的值赋给mylabel，并将nextlabel初始化
		strcpy(mylabel,nextlabel);
		
		memset(default_label,0,tokensize);			//无预料何时读到default，default_label必须是最后一个nextlabel
		strcpy(default_label,nextlabel);

		memset(nextlabel,0,tokensize);
	}while(symid==CASESYM);		//至少有一个case语句

	//printf("this is a case statement\n");
	fprintf(grammarfile,"%s","this is a case statement\n");
}

//＜情况子语句＞  ::=  case＜常量＞：＜语句＞
//处理情况子语句,mylabel已由上一个case（或函数开头申请），nextlabel未申请,temp_exp为表达式的临时变量
void casesubsta(char* temp_exp,char* mylabel,char* nextlabel,int switch_type){
	//进入函数已读到case
	int number=0;
	char temp_num[tokensize]={'\0'};
	midcode(SETLABEL,mylabel,NULL,NULL);		//先设置本case的label
	getsym();
	if(symid==PLUS || symid==SUB || symid==ANUMBER){		//读到+ - 无前零数字
		number=integer();							//integer不预读一个符号
		if(switch_type==1){						//switch为char case为int
			//类型冲突
			errormessage(TYPECONFL_ERROR,line,no);
		}
	}
	else if(symid==ACHAR){
		number=num;
		if(switch_type==0){
			//类型冲突
			errormessage(TYPECONFL_ERROR,line,no);
		}
	}
	else{
		//error:应为常量
		errormessage(CONSTVALUE_ERROR,line,no);
		skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 case default if for switch scanf return printf ; { }
		return;
	}
	sprintf(temp_num,"%d",number);
	midcode(EQUALOP,temp_exp,temp_num,NULL);		//产生关系运算表达式
	getlabel(nextlabel);
	midcode(BZOP,nextlabel,NULL,NULL);

	getsym();
	if(symid==COLON){		//读到:
		getsym();
		sta();				//调用语句处理程序
	}
	else{
		//error:应有:
		errormessage(COLON_ERROR,line,no);
		skip(11,CASESYM,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 case default if for switch scanf return printf ; { }
		return;
	}


	//printf("this is a case substatement\n");
	fprintf(grammarfile,"%s","this is a case substatement\n");
}

//＜缺省＞   ::=  default : ＜语句＞
//处理缺省语句
void defaultsta(char* default_label){
	//进入函数时已经读取到了default
	midcode(SETLABEL,default_label,NULL,NULL);		//设置default label
	getsym();
	if(symid==COLON){
		getsym();
		sta();
	}
	else{
		//error:应有:
		errormessage(COLON_ERROR,line,no);
		skip(10,DEFAULTSYM,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到default if for switch scanf return printf ; { }
		return;
	}

	//printf("this is a default statement\n");
	fprintf(grammarfile,"%s","this is a default statement\n");
}

//＜有返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’
//处理有返回值函数的调用
void callretfunc(int index){
	char op1[tokensize]={'\0'};
	//进入函数时，已经读到标识符 或 (
	if(symid==ID){					//保留当前读到的符号信息,因为程序可能被 语句列处理程序 或 因子处理程序 调用
		strcpy(token1,token);		//由因子处理程序调用时，已读到( 且函数名标识符已保存在token1，symid1
		symid1=symid;
		getsym();
		if(symid==LPAR){			//读到(

		}
		else{
			//error:缺少(
			errormessage(LPAR_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//跳到 if for switch scanf return printf ; { }
			return;
		}
	}


	getsym();
	valueofpara(index);					//调用值参数列处理程序

	midcode(FUNCCALL,tab[index].id,NULL,NULL);	//call function
	if(symid==RPAR){				//读取到)
		getsym();
	}
	else{
		//error:应有)
		errormessage(RPAR_ERROR,line,no);
		skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 if for switch scanf return printf ; { }
		return;
	}

	//printf("this is a call of function\n");
	fprintf(grammarfile,"%s","this is a call of function\n");
}		

//＜无返回值函数调用语句＞ ::= ＜标识符＞‘(’＜值参数表＞‘)’
//处理无返回值函数的调用,只可能被语句列调用
void callnoretfunc(int index){
	//进入函数时，已经读到(
	getsym();
	valueofpara(index);				//调用值参数列处理程序
	midcode(FUNCCALL,tab[index].id,NULL,NULL);	//call function
	if(symid==RPAR){				//读到)
		getsym();
	}
	else{
		//error:应有)
		errormessage(RPAR_ERROR,line,no);
		skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 if for switch scanf return printf ; { }
		return;
	}

	//printf("this is a call of function\n");
	fprintf(grammarfile,"%s","this is a call of function\n");
}		


//＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
//处理常量说明
void conststa(){		
	//进入函数时已经读到const
	do{
		getsym();
		if(symid==INTSYM || symid==CHARSYM){	//读到类型标识符
			constdef();							//调用处理常量定义程序
			if(symid==SEMICOLON){				//读到分号
				getsym();						//再向下读一个符号,如果是const，继续循环
			}
			else{
				//error:应该有分号
				errormessage(SEMICOLON_ERROR,line,no);
				skip(5,SEMICOLON,CONSTSYM,INTSYM,CHARSYM,VOIDSYM);
				//跳到 ; const int char void 
			}
		}
		else{
			//error:应有int 或 char
			errormessage(TYPE_ERROR,line,no);
			skip(5,SEMICOLON,CONSTSYM,INTSYM,CHARSYM,VOIDSYM);
			//跳到 ; const int char void 
		}
	}while(symid==CONSTSYM);					//至少有一个const＜常量定义＞;
	//printf("this is a conststatement\n");
	fprintf(grammarfile,"%s","this is a conststatement\n");
}


//＜常量定义＞ ::=int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}
//              | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
//处理常量定义
void constdef(){	
	//进入函数时已经读到int或char
	char temp_num[tokensize]={'\0'};
	switch(symid){
		case INTSYM:	type=2;break;		//const int
		case CHARSYM:	type=3;break;		//const char
	}
	getsym();								//再读一个,应该是标识符

	while(true){
		if(symid==ID){
			strcpy(id,token);					//在读=之前，保存标识符
			getsym();
			if(symid==ASSIGN){					//再读一个,应该是=
				if(type==2){					//const int
					getsym();	
					len=integer();						//调用处理整数程序
					inserttab(id,type,len,lev,4,addr);		//填符号表
					addr+=4;									//daddr
					memset(temp_num,0,tokensize);
					sprintf(temp_num,"%d",len);
					midcode(CONSTINTDEF,id,temp_num,NULL);						//产生中间代码
				}
				else{									//const char
					getsym();	
					if(symid==ACHAR){
						len=num;						//储存字符
						inserttab(id,type,len,lev,4,addr);	//填符号表
						addr+=4;							//地址加4，字符也占据4字节
						memset(temp_num,0,tokensize);
						sprintf(temp_num,"%d",len);
						midcode(CONSTCHARDEF,id,temp_num,NULL);					//产生中间代码	
					}
					else{
						//error:应为一个字符
						errormessage(CHAR_ERROR,line,no);
						skip(5,SEMICOLON,CONSTSYM,INTSYM,CHARSYM,VOIDSYM);
						//跳到 ; const int char void 
						break;
					}
				}
			}
			else{
				//error:应该是=
				errormessage(ASSIGN_ERROR,line,no);
				skip(5,SEMICOLON,CONSTSYM,INTSYM,CHARSYM,VOIDSYM);
				//跳到 ; const int char void 
				break;
			}
			getsym();							//再读一个符号
			if(symid==COMMA){					//读到逗号则后面还有
				getsym();
			}
			else{								//否则退出函数
				break;
			}
		}
		else{
			//error:应该是标识符
			errormessage(ID_ERROR,line,no);
			skip(5,SEMICOLON,CONSTSYM,INTSYM,CHARSYM,VOIDSYM);
			//跳到 ; const int char void 
			break;
		}
	}
	//printf("this is a constdefine \n");
	fprintf(grammarfile,"%s","this is a constdefine \n");
}

//＜整数＞        ::= ［＋｜－］＜无符号整数＞｜０
//处理整数,和其他处理子程序不同，该程序在结尾没有预读一个符号
int integer(){
	//进入函数时，已经读取了第一个符号
	int head=0;				//+----head=1 ------head=-1
	if(symid==PLUS || symid==SUB){		//读到+,-
		head=(symid==PLUS)?1:-1;
		getsym();
		if(symid==ANUMBER){
			if(head!=0 && num==0){
				//error:0有符号
				errormessage(PLUSZERO_ERROR,line,no);
				return 0;
			}
			return head*num;
		}
		else{
			//error:缺少数字
			errormessage(INTEGER_ERROR,line,no);
			return 0;
		}
	}	
	else if(symid==ANUMBER){
		return num;
	}
	else{
		//error:缺少整数
		errormessage(INTEGER_ERROR,line,no);
		return 0;
	}

	//printf("this is an integer\n");
}


//＜变量定义＞  ::= (int|char)(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’)
//							 {,(＜标识符＞|＜标识符＞‘[’＜无符号整数＞‘]’)};
//处理变量定义
void variabledef(){					
	//调用函数时已读到[ , ;
	strcpy(id,token2);							//读取临时保存的标识符
	int temp_type=(symid1==INTSYM)?0:1;			//读取临时保存的类型 ，暂时认为不是数组int--type=0，char--type=1，还可能是数组
	len=0;
	type=temp_type;
	while(true){
		char temp_num[tokensize]={'\0'};
		if(symid==COMMA){							//读到, 则还有变量定义
			len=(type==0 || type==1)?0:len;			//int char变量的len为0
			size=sizecalcu(type,len);
			inserttab(id,type,len,lev,size,addr);		//填符号表
			addr+=size;
			sprintf(temp_num,"%d",len);
			midcode((type==0)?VARINTDEF:(type==1)?VARCHARDEF:(type==4)?INTARRDEF:CHARARRDEF,id,temp_num,NULL);//生成中间代码
			
			type=temp_type;							//填完表，type变回temp_type
			getsym();								//应该读到 标识符
			if(symid==ID){
				strcpy(id,token);					//保存标识符
				getsym();
				if(symid==LBRACKET || symid==COMMA || symid==SEMICOLON){//又读到[ , ;
					continue;
				}
				else{
					//error:缺少[ , ;
					errormessage(SEMICOLON_ERROR,line,no);
					skip(4,SEMICOLON,INTSYM,CHARSYM,VOIDSYM);
					//跳到 ; int char void 
					return;
				}
			}
			else{
				//error:应有id
				errormessage(ID_ERROR,line,no);
				skip(4,SEMICOLON,INTSYM,CHARSYM,VOIDSYM);
				//跳到 ; int char void 
				return;
			}
		}
		else if(symid==LBRACKET){					//读到[ 说明是数组
			getsym();
			if(symid==ANUMBER && num!=0){			//数组的长度是无符号非零整数
				len=num;							//记录数组长度
				type=(temp_type==0)?4:5;			//int数组-4  char数组-5
				size=sizecalcu(type,len);			//记录数组字节数
				getsym();
				if(symid==RBRACKET){				//]
					getsym();
					if(symid==COMMA || symid==SEMICOLON){//又读到 , ;
						//此处先不填符号表，留到, ;再填
						continue;
					}
					else{
						//error:缺少, ;
						errormessage(SEMICOLON_ERROR,line,no);
						skip(4,SEMICOLON,INTSYM,CHARSYM,VOIDSYM);
						//跳到 ; int char void 
						return;
					}
				}
				else{
					//error:应有]
					errormessage(RBRACKET_ERROR,line,no);
					skip(4,SEMICOLON,INTSYM,CHARSYM,VOIDSYM);
					//跳到 ; int char void 
					return;
				}
			}
			else{
				//error://数组的长度是无符号非零整数
				errormessage(ARRLEN_ERROR,line,no);
				skip(4,SEMICOLON,INTSYM,CHARSYM,VOIDSYM);
				//跳到 ; int char void 
				return;
			}
		}
		else if(symid==SEMICOLON){					//读到; 结束
			len=(type==0 || type==1)?0:len;			//int char变量的len为0

			size=sizecalcu(type,len);
			inserttab(id,type,len,lev,size,addr);		//填符号表
			addr+=size;
			sprintf(temp_num,"%d",len);			//数字转数组
			midcode((type==0)?VARINTDEF:(type==1)?VARCHARDEF:(type==4)?INTARRDEF:CHARARRDEF,id,temp_num,NULL);//生成中间代码
			
			type=temp_type;							//填完表，type变回temp_type
			getsym();								//预读一个符号
			break;
		}
		else{
			//error:应有;
			errormessage(SEMICOLON_ERROR,line,no);
			skip(4,SEMICOLON,INTSYM,CHARSYM,VOIDSYM);
			//跳到 ; int char void 
			return;
		}
	}

	//printf("this is a variabledefine\n");
	fprintf(grammarfile,"%s","this is a variabledefine\n");
}

//＜表达式＞    ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
//处理表达式
int expression(char* temp){
	//char temp1[tokensize]={'\0'};
	char temp2[tokensize]={'\0'};
	char temp_num[tokensize]={'\0'};
	int temp_type=0;
	int exp_type=0;
	int term_sum=0;
	//gettemp(temp);

	//进入表达式时，已读取第一个字符
	if(symid==PLUS || symid==SUB){		//读到+ -
		temp_type=(symid==PLUS)?ADDOP:SUBOP;
		getsym();
		exp_type=term(temp);							//调用项处理程序
		sprintf(temp_num,"%d",0);
		midcode(temp_type,temp_num,temp,temp);
		term_sum++;
	}
	else{
		exp_type=term(temp);							//直接调用项处理程序
	}



	while(symid==PLUS || symid==SUB){	//又读到+ -
		temp_type=(symid==PLUS)?ADDOP:SUBOP;
		getsym();						//后面应该是表达式
		term(temp2);
		midcode(temp_type,temp,temp2,temp);				//产生中间代码 temp1=temp1+/-temp2
		term_sum++;
	}

	//printf("this is a expression\n");
	fprintf(grammarfile,"%s","this is a expression\n");
	return (exp_type==1 && term_sum==0)?1:0;			//只有一个项且为char且无符号，才返回1
}

//＜项＞     ::= ＜因子＞{＜乘法运算符＞＜因子＞}
//处理项(返回值表示类型0-int ， 1-char)
int term(char* temp){
	//char temp1[tokensize]={'\0'};
	char temp2[tokensize]={'\0'};
	int temp_type=0;
	int term_type=0;
	int factor_sum=0;
	//gettemp(temp);
	//进入程序时已经读取了第一个符号
	term_type=factor(temp);						//调用因子处理程序
	while(symid==MUL || symid==DIV){				//读到* /
		temp_type=(symid==MUL)?MULOP:DIVOP;
		getsym();									//再读一个
		factor(temp2);								//调用因子处理程序
		midcode(temp_type,temp,temp2,temp);				//产生中间代码 temp1=temp1*//temp2
		factor_sum++;
	}

	//midcode(VARASSIGN,temp,temp1,NULL);						//产生中间代码 temp=temp1
	//printf("this is a term\n");
	fprintf(grammarfile,"%s","this is a term\n");
	return (term_type==1 && factor_sum==0)?1:0;			//如果只有一个因子，且因子为char，才返回1
}

//＜因子＞   nn ::= ＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞|‘(’＜表达式＞‘)’   
//处理因子(返回值表示类型0-int ， 1-char)
int factor(char* temp){
	char temp_num[tokensize]={'\0'};
	char op1[tokensize]={'\0'};
	char op2[tokensize]={'\0'};
	int factor_type=0;
	int value=0;
	//进入函数时，已经读取了第一个符号
	int index=0;
	gettemp(temp);
	if(symid==ID){		//因子可能为＜标识符＞｜＜标识符＞‘[’＜表达式＞‘]’|＜有返回值函数调用语句＞
		strcpy(token1,token);			//保留当前读到的符号信息
		strcpy(op1,token);
		symid1=symid;
		getsym();
		if(symid==LBRACKET){		//读到[，因子可能为＜标识符＞‘[’＜表达式＞‘]’

		//查表 中的数组，是否有这个数组
			index=searchtab(token1,3);	
			if(index==-1){
				//error:未定义数组
				errormessage(UNDEFID_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//跳到 if for switch scanf return printf ; { }
				return factor_type;
			}
			else if(tab[index].type!=4 && tab[index].type!=5){
				//error:不是数组
				errormessage(NOTARR_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//跳到 if for switch scanf return printf ; { }
				return factor_type;
			}

			if(tab[index].type==5){		//char数组
				factor_type=1;			//将因子定为char类型
			}

			getsym();
			expression(op2);		//调用表达式分析程序
			if(symid==RBRACKET){//读到]
				midcode(ARRUSE,op1,op2,temp);			//产生中间代码 temp=op1[op2]
				getsym();
			}
			else{
				//error:应有]
				errormessage(RBRACKET_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//跳到 if for switch scanf return printf ; { }
				return factor_type;
			}
		}
		else if(symid==LPAR){		//读到(，/因子可能为＜有返回值函数调用语句＞,还需查表



			index=searchtab(token1,1);	//查表
			if(index==-1){
				//error:未定义有返回值函数
				errormessage(UNDEFID_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//跳到 if for switch scanf return printf ; { }
				return factor_type;
			}
			else if(tab[index].type!=6 && tab[index].type!=7){
				//error:不是有返回值函数
				errormessage(HASRETFUNC_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//跳到 if for switch scanf return printf ; { }
				return factor_type;
			}

			if(tab[index].type==7){			//char型函数,factor_type=1
				factor_type=1;
			}


			callretfunc(index);			//调用有返回值函数调用处理程序
			midcode(FUNCRET,temp,NULL,NULL);		//产生中间代码，temp=RET
		}
		else{						//因子可能为＜标识符＞



			index=searchtab(token1,2);	//查表
			if(index==-1){
				//error:未定义常变量、函数参数
				errormessage(UNDEFID_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//跳到 if for switch scanf return printf ; { }
				return factor_type;
			}
			else if(tab[index].type!=0 && tab[index].type!=1 && tab[index].type!=2 && tab[index].type!=3 && tab[index].type!=9 && tab[index].type!=10){
				//error:不是常变量及参数
				errormessage(FACTOR_ERROR,line,no);
			}

			//char型变量常量量
			if(tab[index].type==1 || tab[index].type==3 || tab[index].type==10){
				factor_type=1;
			}

			//如果是常量,直接用常量的值
			if(tab[index].type==2 || tab[index].type==3){
				sprintf(temp_num,"%d",tab[index].len);
				midcode(VARASSIGN,temp,temp_num,NULL);			//产生中间代码temp=temp_num
			}
			//如果不是常量
			else{
				midcode(VARASSIGN,temp,op1,NULL);			//产生中间代码temp=op1
			}
			//之前已经预读过，不要再预读
		}
	}
	else if(symid==PLUS || symid==SUB || symid==ANUMBER){	//因子可能为整数
		value=integer();					//调用整数分析程序
		getsym();
		sprintf(temp_num,"%d",value);
		midcode(VARASSIGN,temp,temp_num,NULL);			//生成中间代码 temp=num
	}
	else if(symid==ACHAR){			//读到字符,因子为字符
		sprintf(temp_num,"%d",num);		//字符化成ascii码
		midcode(VARASSIGN,temp,temp_num,NULL);			//生成中间代码 temp=num
		getsym();
		factor_type=1;
	}
	else if(symid==LPAR){			//读到(,因子为‘(’＜表达式＞‘)’   
		getsym();
		factor_type=expression(op1);				//调用表达式分析程序
		if(symid==RPAR){			//读到)
			midcode(VARASSIGN,temp,op1,NULL);				//产生中间代码,temp=op1
			getsym();
		}
		else{
			//error:应有)
			errormessage(RPAR_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//跳到 if for switch scanf return printf ; { }
			return factor_type;
		}
	}
	else{
		//error:因子错误
		errormessage(FACTOR_ERROR,line,no);
		skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到 if for switch scanf return printf ; { }
		return factor_type;
	}

	
	//printf("this is a factor\n");
	fprintf(grammarfile,"%s","this is a factor\n");
	return factor_type;
}


//＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞
//值参数表
void valueofpara(int index){
	//进入函数时已经读取了第一个符号
	char temp[tokensize]={'\0'};
	int type=0;
	int k=1;					//当前为第k个参数
	if(symid==RPAR){			//读到)，说明没有参数
		if(tab[index].len==0){
			//无事
		}
		else{
			//error:参数数目错误
			errormessage(PARACOUNT_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//跳到id if for switch scanf return printf ; { }
			return;
		}
	}
	else{
		//先分析第一个表达式
		type=expression(temp);			//调用表达式分析程序 0-int 1-char
		if((type==0 && tab[index+k].type!=9) || (type==1 && tab[index+k].type!=10)){
			//error:类型不一致
			errormessage(TYPECONFL_ERROR,line,no);
		}						
		midcode(PARAOP,temp,NULL,NULL);			//产生中间代码 push temp


		while(symid==COMMA){			//读到,
			k++;							//当前为k+1个参数
			getsym();
			type=expression(temp);			//调用表达式分析程序 0-int 1-char
			if((type==0 && tab[index+k].type!=9) || (type==1 && tab[index+k].type!=10)){
				//error:类型不一致
				errormessage(TYPECONFL_ERROR,line,no);
			}
			midcode(PARAOP,temp,NULL,NULL);			//产生中间代码 push temp
			//此处要判断exp类型与tab[index+k]的类型是否一致
		}			

		if(k!=tab[index].len){
			//error:参数数目错误
			errormessage(PARACOUNT_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//跳到 if for switch scanf return printf ; { }
			return;
		}
	}
	//printf("this is a value of paralist\n");
	fprintf(grammarfile,"%s","this is a value of paralist\n");

}

//＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞
//处理复合语句
void compoundsta(){			
	//进入函数时，已经读取了第一个字符
	if(symid==CONSTSYM){		//读到const
		conststa();				//调用变量声明分析程序
	}
	while(true){				//接下来分析变量说明
		if(symid==INTSYM || symid==CHARSYM){
			strcpy(token1,token);			//保留当前读到的符号信息
			symid1=symid;
			getsym();
			if(symid==ID){					//又读到标识符
				strcpy(token2,token);		//保留当前读到的符号信息
				symid2=symid;
				getsym();
				if(symid==LBRACKET || symid==SEMICOLON || symid==COMMA){
					variabledef();				//调用变量定义处理程序,出来时读到;
				}
				else{
					//error:应有[ , ;
					errormessage(SEMICOLON_ERROR,line,no);
					skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//跳到if for switch scanf return printf ; { }
					return;
				}
			}
			else{
				//error:应有id
				errormessage(ID_ERROR,line,no);
				skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//跳到if for switch scanf return printf ; { }
				return;
			}
		}
		else{
			break;
		}
	}

	stalist();			//调用处理语句列程序
	//printf("this is a compound statement\n");
	fprintf(grammarfile,"%s","this is a compound statement\n");
}



//＜语句＞    ::= ＜条件语句＞｜＜循环语句＞| ‘{’＜语句列＞‘}’｜＜有返回值函数调用语句＞; 
//| ＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;|＜情况语句＞｜＜返回语句＞;
//处理语句
void sta(){
	//进入时已经读取首符号
	int index=0;
	if(symid==IFSYM){			//读到if，为条件语句
		ifsta();
	}
	else if(symid==FORSYM){		//读到for，为循环语句
		forsta();
	}
	else if(symid==SWITCHSYM){	//读到switch，为情况语句
		switchsta();
	}
	else if(symid==LBRACE){						//读到{，为语句列
		getsym();
		stalist();								//调用语句列处理程序
		if(symid==RBRACE){						//读到}
			getsym();
		}
		else{
			//error:应有 }
			errormessage(RBRACE_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//跳到id if for switch scanf return printf ; { }
			return;
		}
	}
	else if(symid==ID){							//读到id,为有返回值函数调用语句 或 无返回值函数调用语句 或 赋值语句
		strcpy(token1,token);					//保留当前读到的符号信息
		symid1=symid;
		getsym();
		if(symid==ASSIGN || symid==LBRACKET){	//读到[或赋值号=,可能为赋值语句
			assignsta();						//调用赋值语句处理程序
		}
		else if(symid==LPAR){					//读到(,为函数调用
			//此处应查符号表判断有无返回值,暂且认为有返回值
			index=searchtab(token1,1);

			if(index==-1){
				//error:未定义函数
				errormessage(UNDEFID_ERROR,line,no);
				getsym();
				skip(10,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
				//跳到id if for switch scanf return printf ; { }
				return;
			}
			else if(tab[index].type!=6 && tab[index].type!=7 && tab[index].type!=8){//不是三种函数
				//error:不是函数(不是语句)
				errormessage(STA_ERROR,line,no);
				getsym();
				skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//跳到id if for switch scanf return printf ; { }
				return;
			}


			if(tab[index].type==6 || tab[index].type==7){
				callretfunc(index);					//调用有返回值函数处理程序
			}
			else if(tab[index].type==8){
				callnoretfunc(index);				//调用无返回值函数处理程序
			}
		}
		else{
			//error:缺少=或（或[
			errormessage(ASSIGN_ERROR,line,no);
			skip(9,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//跳到if for switch scanf return printf ; { }
			return;
		}

		if(symid==SEMICOLON){	//以上三语句后接分号
			getsym();
		}
		else{
			//error:应有;
			errormessage(SEMICOLON_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//跳到id if for switch scanf return printf ; { }
			return;
		}
	}
	else if(symid==SCANFSYM){	//读到scanf,为读语句
		scansta();
		if(symid==SEMICOLON){	//读语句后接分号
			getsym();
		}
		else{
			//error:应有;
			errormessage(SEMICOLON_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
			//跳到id if for switch scanf return printf ; { }
			return;
		}
	}
	else if(symid==PRINTFSYM){	//读到printf,为写语句
		printsta();
		if(symid==SEMICOLON){	//写语句后接分号
			getsym();
		}
		else{
			//error:应有;
			errormessage(SEMICOLON_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//跳到id if for switch scanf return printf ; { }
			return;
		}
	}
	else if(symid==RETURNSYM){	//读到return，为返回语句
		returnsta();
		if(symid==SEMICOLON){	//返回语句后接分号
			getsym();
		}
		else{
			//error:应有;
			errormessage(SEMICOLON_ERROR,line,no);
			skip(10,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
					//跳到id if for switch scanf return printf ; { }
			return;
		}
	}
	else if(symid==SEMICOLON){	//空语句，仅含有;
		getsym();
	}
	else{
		//error:语句头部错误
		errormessage(STA_ERROR,line,no);
		return;
	}

	//printf("this is a statement\n");
	fprintf(grammarfile,"%s","this is a statement\n");
}



//＜语句列＞   ::= ｛＜语句＞｝
//处理语句列程序
void stalist(){
	//进入时已经读取了首符号
	while(isstahead(symid)==1){
		sta();
	};
	//printf("this is a statement list\n");
	fprintf(grammarfile,"%s","this is a statement list\n");
}


//＜有返回值函数定义＞  ::= (int|char)＜标识符＞‘(’＜参数＞‘)’ ‘{’＜复合语句＞‘}’
//处理有返回值函数定义
void hasretfunc(){				
	//进入函数时,已经读到了(
	char temp_num[tokensize]={'\0'};
	int midtype;
	strcpy(id,token2);				//保存函数名
	type=(symid1==INTSYM)?6:7;		//保存函数类型
	inserttab(id,type,0,0,0,0);		//先填表，后修改len(参数个数) size(函数所占字节)
	addr=0;							//进入函数addr=0
	midtype=(type==6)?INTFUNCDEF:CHARFUNCDEF;			//生成中间代码
	midcode(midtype,id,NULL,NULL);

	lev=1;							//在函数中lev=1
	getsym();	
	len=paralist();						//调用参数表处理程序
	tab[funcindex[funclen-1]].len=len;

	if(symid!=RPAR){					//读到)
		//error:应有}
		errormessage(RPAR_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到const int char id if for switch scanf return printf semicolon { }
	}
	else{
		getsym();
	}
	
	if(symid!=LBRACE){				//读到{
		//error:缺少{
		errormessage(LBRACE_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到const int char id if for switch scanf return printf semicolon { }
	}
	else{
		getsym();
	}
	
	compoundsta();		//调用复合语句分析
	if(symid!=RBRACE){			//读到}
		//error:应有}
		errormessage(RBRACE_ERROR,line,no);
		skip(3,INTSYM,VOIDSYM,CHARSYM);
		lev=0;
		addr=0;
		return;
	}
	//重新计算函数的size
	size=0;
	for(int i=funcindex[funclen-1]+1;i<tablen;i++){	//遍历函数的参数、变量、常量，统计总的size大小
		size+=tab[i].size;
	}
	tab[funcindex[funclen-1]].size=size;				//将总的大小赋值给函数的表项
	lev=0;
	addr=0;
	getsym();
			
		

	//默认所有函数结尾都加上return 0
	sprintf(temp_num,"%d",0);
	midcode(RETEXPOP,temp_num,NULL,NULL);
	//printf("this is a function with return value\n");
	fprintf(grammarfile,"%s","this is a function with return value\n");
}


//＜参数表＞    ::=  ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}|＜空＞
//处理参数表的程序,返回参数的个数
int paralist(){
	//进入函数时已经读取了＜类型标识符＞ 或 )
	int k=0;//记录有多少个参数
	int midtype=0;
	if(symid==RPAR){	//无参数
		//printf("this is a paralist\n");
		fprintf(grammarfile,"%s","this is a paralist\n");
		return k;
	}

	while(true){
		if(symid==INTSYM || symid==CHARSYM){			//读到int或char
			type=(symid==INTSYM)?9:10;					//int型参数对应type=9，char型参数对应type=10
			len=0;
			size=sizecalcu(type,len);					//计算所占字节
			getsym();
			if(symid==ID){			//读到标识符
				strcpy(id,token);
				inserttab(id,type,0,lev,size,addr);
				addr+=size;
				k++;

				midtype=(type==9)?INTPARA:CHARPARA;		//生成中间代码
				midcode(midtype,id,NULL,NULL);

				getsym();
				if(symid==COMMA){	//读到, 说明还有参数
					getsym();
					continue;
				}
				else{				//没有,直接跳出
					break;
				}
			}
			else{
				//error:应有标识符
				errormessage(ID_ERROR,line,no);
				skip(2,RPAR,LBRACE);
				//跳到{ )
				return k;
			}
		}
		else{
			//error:缺少类型标识符int,char

			errormessage(TYPE_ERROR,line,no);
			skip(2,RPAR,LBRACE);
			//跳到{ )
			return k;
		}
	}

	//printf("this is a paralist\n");
	fprintf(grammarfile,"%s","this is a paralist\n");
	return k;
}

//＜无返回值函数定义＞  ::=void＜标识符＞‘(’＜参数＞‘)’‘{’＜复合语句＞‘}’
//处理无返回值函数定义
void noretfunc(){				
	//进入函数时已经读取了＜标识符＞
	char temp_num[tokensize]={'\0'};
	strcpy(id,token);				//保存函数名
	type=8;		//保存函数类型
	inserttab(id,type,0,0,0,0);		//先填表，后修改len(参数个数) size(函数所占字节)
	midcode(VOIDFUNCDEF,id,NULL,NULL);			//产生中间代码，void id();
	addr=0;							//将距函数偏移值为0
	lev=1;							//在函数中lev=1
	getsym();	

	if(symid!=LPAR){	//再读（
		//error:应有(
		errormessage(LPAR_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到const int char id if for switch scanf return printf semicolon { }
	}
	getsym();
	len=paralist();
	tab[funcindex[funclen-1]].len=len;	//记录参数个数

	if(symid!=RPAR){    //读到 )
		//error:应有)
		errormessage(RPAR_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到const int char id if for switch scanf return printf semicolon { }
	}
	else{
		getsym();
	}

	
	if(symid!=LBRACE){
		//error:应有{
		errormessage(LBRACE_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到const int char id if for switch scanf return printf ; { }
	}
	else{
		getsym();
	}
	compoundsta();		//调用复合语句分析
	if(symid!=RBRACE){			//读到}
		//error:应有}
		errormessage(RBRACE_ERROR,line,no);
		skip(3,INTSYM,VOIDSYM,CHARSYM);
		//跳到int char void
		addr=0;							//将距函数偏移值为0
		lev=0;
	}
	size=0;
	for(int i=funcindex[funclen-1]+1;i<tablen;i++){	//遍历函数的参数、变量、常量，统计总的size大小
		size+=tab[i].size;
	}
	tab[funcindex[funclen-1]].size=size;				//将总的大小赋值给函数的表项
	addr=0;							//将距函数偏移值为0
	lev=0;
	getsym();

				

	//默认所有函数结尾都加上return 0
	sprintf(temp_num,"%d",0);
	midcode(RETEXPOP,temp_num,NULL,NULL);
	//printf("this is a function without return value\n");
	fprintf(grammarfile,"%s","this is a function without return value\n");

}

//＜主函数＞    ::= void main‘(’‘)’ ‘{’＜复合语句＞‘}’
//处理主函数
void mainfunction(){			
	//进入函数时已读到main
	midcode(VOIDFUNCDEF,"main",NULL,NULL);			//产生中间代码，void id();
	inserttab(token,8,0,0,0,0);		//填符号表
	addr=0;
	lev=1;

	getsym();
	if(symid!=LPAR){				//读取(
		//error:缺少(
		errormessage(LPAR_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到const int char id if for switch scanf return printf semicolon { }
	}
	else{
		getsym();
	}	
	
	if(symid!=RPAR){			//读取)
		//error:缺少)
		errormessage(RPAR_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到const int char id if for switch scanf return printf semicolon { }
	}
	else{
		getsym();
	}
	
	if(symid!=LBRACE){		//读取{
		//error:缺少{
		errormessage(LBRACE_ERROR,line,no);
		skip(13,CONSTSYM,INTSYM,CHARSYM,ID,IFSYM,FORSYM,SWITCHSYM,SCANFSYM,RETURNSYM,PRINTFSYM,SEMICOLON,LBRACE,RBRACE);
		//跳到const int char id if for switch scanf return printf semicolon { }
	}
	else{
		getsym();
	}
	compoundsta();		//调用复合语句分析

	if(symid!=RBRACE){	//读到}
		errormessage(RBRACE_ERROR,line,no);
		addr=0;
		lev=0;
		return;
	}
	size=0;
	for(int i=funcindex[funclen-1]+1;i<tablen;i++){	//遍历函数的参数、变量、常量，统计总的size大小
		size+=tab[i].size;
	}
	tab[funcindex[funclen-1]].size=size;				//将总的大小赋值给函数的表项
	addr=0;
	lev=0;
	midcode(SETLABEL,"Label_end",NULL,NULL);			//设置程序结束的label
	
		
	
	//printf("this is a main function\n");
	fprintf(grammarfile,"%s","this is a main function\n");
}


//语法分析程序
void program(){
	getsym();
	if(symid==CONSTSYM){				//读到const
		conststa();						//调用常量说明处理程序
	}
	int flag=0;							//flag=0说明还在变量说明阶段,flag=1说明不在变量说明阶段
	while(true){						//循环处理所有变量说明、以及第一个 有返回值函数定义
		if(symid==INTSYM || symid==CHARSYM){//可能为变量说明或有返回值函数定义,需要再往后读2个字符	
			strcpy(token1,token);			//保留当前读到的符号信息
			symid1=symid;
			getsym();
			if(symid==ID){					//又读到标识符
				strcpy(token2,token);		//保留当前读到的符号信息
				symid2=symid;
				getsym();
				if(symid==LBRACKET || symid==SEMICOLON || symid==COMMA){	//读到[ , ; 说明是变量说明
					if(flag!=0){
						//error:函数定义阶段中混有变量说明
						errormessage(VARDEF_ERROR,line,no);
					}
					variabledef();				//调用变量定义处理程序
					continue;
				}
				else{	

					if(symid!=LPAR){
						//error:缺少(
						errormessage(LPAR_ERROR,line,no);
						skip(3,LPAR,ID,RPAR);	//跳到( ,ID ,)
					}

					flag=1;
					hasretfunc();			//没有读到[ , ;,说明是有返回值函数定义
											//既然已经读到 有返回值函数定义 ,那么变量说明已经结束,
											//接下来可能是有返回值函数定义、无返回值函数定义、void main
				}
			}
			else{
				//error:缺少标识符
				errormessage(ID_ERROR,line,no);
				skip(2,ID,MAINSYM);
			}
		}
		else if(symid==VOIDSYM){				//读到void
			getsym();
			if(symid==MAINSYM){					//读到main
				mainfunction();	
				break;							//main函数处理后不应再有任何符号
			}
			else if(symid==ID){		
				noretfunc();					//调用无返回值函数处理程序
			}
			else{
				//error: 应有id或main
				errormessage(ID_ERROR,line,no);
				skip(2,ID,MAINSYM);
			}
		}
		else{
			//error:应有char int 或void
			errormessage(FUNCTYPE_ERROR,line,no);
			skip(3,VOIDSYM,INTSYM,CHARSYM);
			//return;
		}
	}
	//printf("this is a program\n");
	fprintf(grammarfile,"%s","this is a program\n");
}










#endif
