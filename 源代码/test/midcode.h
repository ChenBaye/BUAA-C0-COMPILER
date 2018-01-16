#ifndef _MIDCODE_H_
#define _MIDCODE_H_
#include<stdio.h> 
#include<ctype.h>
#include<string.h>
#include<stdlib.h>

#define ADDOP			1	//加操作*			
#define SUBOP			2	//减操作*			
#define MULOP			3	//乘操作*
#define DIVOP			4	//除操作*
#define INTFUNCDEF		5	//int函数定义*
#define CHARFUNCDEF		6	//char函数定义*
#define VOIDFUNCDEF		7	//void函数定义*
	
#define	FUNCCALL		8	//函数调用*
#define PARAOP			9	//函数传递参数*
#define	FUNCRET			10	//函数返回值*

#define CONSTINTDEF		11	//int常量定义*
#define CONSTCHARDEF	12	//char常量定义*
#define VARINTDEF		13	//int变量定义*
#define VARCHARDEF		14	//char变量定义*
#define	INTARRDEF		15	//int数组定义*
#define CHARARRDEF		16	//char数组定义*
#define	VARASSIGN		17	//给变量赋值*
#define	ARRASSIGN		18	//给数组元素赋值*
#define ARRUSE			19	//访问数组元素的值*


#define PRINTSTROP		20	//printf 字符串*
#define PRINTINTOP		21	//printf integer*
#define PRINTCHAROP		22	//printf char*
#define	SCANINTOP		23	//scanf int
#define SCANCHAROP		24	//scanf char
#define RETEXPOP		25	//return 表达式*
#define RETNULLOP		26	//return 空*
#define	SETLABEL		27	//设置标签*
#define BIGEQLOP		28	//		>=*
#define SMALLEQLOP		29	//		<=*
#define EQUALOP			30	//		==*
#define NOTEQLOP		31	//		!=*
#define BIGOP			32	//		>*
#define SMALLOP			33	//		<*

#define GOTOOP			34	//无条件跳转*
#define BNZOP			35	//满足条件跳转 not zero*
#define BZOP			36	//不满足条件跳转 zero*
#define INTPARA			37	//int形参*
#define CHARPARA		38	//char形参*
#define PRINTLINE		39	//换行

#define opsize			200
struct MIDCODE{
	int type;
	char op1[opsize];
	char op2[opsize];
	char result[opsize];
};



struct MIDCODE mid[2048];	//用于存储中间代码
struct MIDCODE com[2048];	//用于优化中间代码
int v[2048]={0};			//标记删除的中间代码(1代表删除)
int midlen=0;				//存储的中间代码的数量(常量、变量定义，函数参数不储存）				



int tempmid_len=0;			//中间代码长度
FILE* midfile;			//保存中间代码的文件指针
int tempvar_count=0;	//记录临时变量个数
int label_count=0;		//记录label个数


//数组是否是数字
int isrealnum(char *s){
	if(isdigit(s[0]) || s[0]=='+' || s[0]=='-'){
		return 1;
	}
	return 0;
}

//常数合并优化，针对code一条中间代码
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


//复制传播优化 若x=y,则将x用y代替
void copyconv(){
	int i=0;
	int j=0;
	for(i=0;i<midlen;i++){
		char before[200]={'\0'};
		char now[200]={'\0'};


		//先调用常数合并优化函数
		constcombine(&com[i]);

		if(com[i].type==VARASSIGN){	//遇到赋值
			strcpy(before,com[i].op1);
			strcpy(now,com[i].op2);
			for(j=i+1;j<midlen;j++){
				if((com[j].type>=INTFUNCDEF && com[j].type<=VOIDFUNCDEF) || j==midlen-1 || com[j].type==FUNCCALL || (com[j].type>=GOTOOP  && com[j].type<=BZOP) || com[j].type==SETLABEL){
					//遇到label 或者 跳转语句 或者 函数调用 或者 函数开头 或者 中间代码的结尾
					//复制传播就结束了
					//if(com[i].op1[0]=='$'){	//如果赋值的左部是中间变量,则可删除代码
					//	v[i]=1;
					//}
					break;
				}
				else if(com[j].type==VARASSIGN){								//遇到变量赋值，检查before或now是否被重新赋值
					if(strcmp(com[j].op2,before)==0){
						memset(com[j].op2,0,200);
						strcpy(com[j].op2,now);
						if(com[i].op1[0]=='$'){									//如果赋值的左部是中间变量,则可删除代码
							v[i]=1;
						}
					}
					if(strcmp(com[j].op1,before)==0 || strcmp(com[j].op1,now)==0){//before、now被重新赋值
						break;
					}
				}
				else if(com[j].type<=DIVOP && com[j].type>=ADDOP){			//遇到+ - * /,进行替代
					if(strcmp(com[j].op1,before)==0){					//op1是before
						memset(com[j].op1,0,200);
						strcpy(com[j].op1,now);
						if(com[i].op1[0]=='$'){	//如果赋值的左部是中间变量,则可删除代码
							v[i]=1;
						}
					}
					if(strcmp(com[j].op2,before)==0){					//op2是before
						memset(com[j].op2,0,200);
						strcpy(com[j].op2,now);
						if(com[i].op1[0]=='$'){	//如果赋值的左部是中间变量,则可删除代码
							v[i]=1;
						}
					}
					if(strcmp(com[j].result,before)==0 || strcmp(com[j].result,now)==0){//如果before或now被重新赋值
						break;
					}
				}
				else if(com[j].type==ARRASSIGN){				//遇到给数组元素赋值 op1[op2] = result
					if(strcmp(com[j].result,before)==0){					//result是before
						memset(com[j].result,0,200);
						strcpy(com[j].result,now);
						if(com[i].op1[0]=='$'){	//如果赋值的左部是中间变量,则可删除代码
							v[i]=1;
						}
					}
					if(strcmp(com[j].op2,before)==0){					//op2是before
						memset(com[j].op2,0,200);
						strcpy(com[j].op2,now);
						if(com[i].op1[0]=='$'){	//如果赋值的左部是中间变量,则可删除代码
							v[i]=1;
						}
					}
				}

				else if(com[j].type>=BIGEQLOP && com[j].type<=SMALLOP){			//遇到>= == <= != > <
					if(strcmp(com[j].op1,before)==0){					//op1是before
						memset(com[j].op1,0,200);
						strcpy(com[j].op1,now);
						if(com[i].op1[0]=='$'){	//如果赋值的左部是中间变量,则可删除代码
							v[i]=1;
						}
					}
					if(strcmp(com[j].op2,before)==0){					//op2是before
						memset(com[j].op2,0,200);
						strcpy(com[j].op2,now);
						if(com[i].op1[0]=='$'){	//如果赋值的左部是中间变量,则可删除代码
							v[i]=1;
						}
					}
				}
				else if(com[j].type==PARAOP || com[j].type==PRINTINTOP || com[j].type==PRINTCHAROP || com[j].type==RETEXPOP){
					//printint, printchar ,push para,return value
					if(strcmp(com[j].op1,before)==0){					//op1是before
						memset(com[j].op1,0,200);
						strcpy(com[j].op1,now);
						if(com[i].op1[0]=='$'){	//如果赋值的左部是中间变量,则可删除代码
							v[i]=1;
						}
					}
				}

				else if(com[j].type==ARRUSE){							//result = op1[op2]
					if(strcmp(com[j].op2,before)==0){					//op2是before
						memset(com[j].op2,0,200);
						strcpy(com[j].op2,now);
						if(com[i].op1[0]=='$'){							//如果赋值的左部是中间变量,则可删除代码
							v[i]=1;
						}
					}
				}

			}
		}
	}
}


//获得临时变量对应字符串
void gettemp(char* str){		
	tempvar_count++;
	memset(str,0,200);
	sprintf(str,"$t_%d",tempvar_count);	//将字符串写入str
}


//获得标签对应字符串
void getlabel(char * str){		
	memset(str,0,200);
	label_count++;
	sprintf(str,"Label_%d",label_count);	//将字符串写入str
}


//生成中间代码
void midcode(int type,char* op1,char* op2,char* result){
	//不保存变量\常量\参数声明定义
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


//将com中优化的中间代码拷到mid中并写入midcode.txt文件
void writemidcode(){
	int i=0;
	int max=midlen;
	//midlen清零
	midlen=0;
	for(i=0;i<max;i++){
		if(v[i]==1){			//中间代码被删除
			continue;
		}
		//不保存变量\常量\参数声明定义
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
					fprintf(midfile,"push %s\n",com[i].op1);				//push op1(op1可能是一个数字而非id)
					break;
				case FUNCRET:
					fprintf(midfile,"%s = RET\n",com[i].op1);				//op1=RET(op1接受一个函数的返回值)
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
					fprintf(midfile,"para int %s\n",com[i].op1);				//para int op1参数定义
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