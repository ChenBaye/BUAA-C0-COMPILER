/*
 * mips.c
 *
 *  Created on: 2018年1月8日
 *      Author: ChenShuwei
 */


#include"grammar.h"
#include"getsym.h"
#include"midcode.h"
#include"error.h"
#include<stdio.h> 
#include<ctype.h>
#include<string.h>
#include<stdlib.h>

#define functabsize 64
#define parasize 50
//一个函数的运行栈
//存储数据类型、id、值
struct data{
	char id[tokensize];		//id
	int addr;				//地址
	int type;				//类型,与符号表相同
};
struct funcstack{	
	struct data data_area[tabsize];	//存储函数中的变量(含临时变量)、常量
	int returnvalue;					//返回值								
	int returnaddr;						//返回地址							
	int fp;						//上一个函数运行栈的栈顶	
	
	int data_arealen;					//data数量	
};		
//函数的运行栈结构,数据向fp方向

//+
//+-------------	<------sp		低地址
//+	data area
//+		...		
//+		...		
//+		...
//+		...
//+-------------	<------fp-12
//+	返回值(fp-12)
//+-------------	<------fp-8
//+	返回地址(fp-8)
//+-------------	<------fp-4
//+	上一个函数的fp的值(fp-4)
//+-------------	<------fp（上一个函数的sp值）		高地址			sp=fp+16+sizeof(data_area)
// +	(fp)
//+

//存储每个函数的所有变量、常量
struct newtab{
	struct tabentry tab[tabsize];
	int tablen;
};

struct newtab functab[functabsize];		//每个函数的符号表
int funcsize[functabsize]={0};			//每个函数的size

struct data STRING[100];		//最多100个字符串 id为字符串值,下标为字符串编号
FILE* mipsfile;					//mips代码文件指针
int strnum=0;					//产生字符串的代号

struct tabentry PARA[parasize];		//存储参数type=0 数字，type=1 全局量，type=2 局部量
int paranum=0;					//记录有几个参数



//窥孔优化，删除mips代码中多余的lw语句
void deletelw(){
	char buffer1[1024]={'\0'};
	char buffer2[1024]={'\0'};

	fclose(mipsfile);
	mipsfile=fopen("mips.txt","r");
	if(mipsfile==NULL){
		printf("can not open this file.");
	}

	FILE *tempfile=fopen("temp.txt","w");
	if(mipsfile==NULL){
		printf("can not open this file.");
	}

	while (strcmp(buffer1,"Label_end:\n")!=0) 
	{ 
        fgets(buffer1,1024,mipsfile);  //读取一行
		//printf("%s\n",buffer1);
		fprintf(tempfile,"%s",buffer1);//拷贝到tempfile

		if(buffer1[1]=='s' && buffer1[2]=='w'){		//读到sw语句
			strcpy(buffer2,buffer1);				//保存sw语句
			if(!feof(mipsfile)){							//如果此时没到mips.txt文件结尾
				fgets(buffer1,1024,mipsfile);				//读取下一行，可能是lw
				char c=buffer1[1];
				buffer1[1]='s';						//把lw改成sw后如果前后字符串相等
				if(strcmp(buffer1,buffer2)==0){

				}
				else{
					buffer1[1]=c;
					fprintf(tempfile,"%s",buffer1);		//拷贝到tempfile
				}
			}
			
		}
	} 
	fclose(tempfile);
	fclose(mipsfile);
	



	mipsfile=fopen("mips.txt","w");
	if(mipsfile==NULL){
		printf("can not open this file.");
	}
	tempfile=fopen("temp.txt","r");
	if(mipsfile==NULL){
		printf("can not open this file.");
	}

	//将temp.txt拷贝到mips.txt
	memset(buffer1,0,1024);
	while (strcmp(buffer1,"Label_end:\n")!=0) 
	{ 
        fgets(buffer1,1024,tempfile);  //读取一行
		fprintf(mipsfile,"%s",buffer1);
	}

	fclose(tempfile);
	remove("temp.txt");

}



//向函数符号表中插入中间变量
void addnewsym(char* id,int funcnum){
	int symnum=0;
	int i=0;
	for(i=0;i<functab[funcnum].tablen;i++){
		if(strcmp(id,functab[funcnum].tab[i].id)==0){	//中间变量已经填表
			return;
		}
	}
	//没找到，在末尾添加中间变量的表项
	//表满了
	if(functab[funcnum].tablen>=tabsize){
		errormessage(OUTOFTAB_ERROR,line,no);
		return;
	}
	
	symnum=functab[funcnum].tablen;
	strcpy(functab[funcnum].tab[symnum].id,id);
	functab[funcnum].tab[symnum].len=0;
	functab[funcnum].tab[symnum].lev=0;
	functab[funcnum].tab[symnum].size=4;
	functab[funcnum].tab[symnum].type=0;
	functab[funcnum].tab[symnum].addr=funcsize[funcnum];
	funcsize[funcnum]+=4;				//函数size+4
	functab[funcnum].tablen++;
	return;
}

//打印函数符号表的内容
void printfunctab(){
	int i=0;
	int j=0;
	//打印数据区
	printf("****************************FUNC TAB****************************\n");
	for(i=0;i<funclen;i++){
		printf("****************************%s\n",tab[funcindex[i]].id);
		printf("tablen=%d\n",functab[i].tablen);
		if(i==12){
			printf("1\n");
		}

		for(j=0;j<functab[i].tablen;j++){
			printf("id=%s addr=%d\n",functab[i].tab[j].id,functab[i].tab[j].addr);
		}
	}
}


//产生字符串标号
void getstrname(char* strname){
	if(strnum==100){
		printf("******************ERROR******************\n");
		printf("too much string!!!!!!!!!!!\n");
		exit(0);
	}
	sprintf(strname,"$string_%d",strnum);
	strnum++;
}


//初始化全局变量、常量、字符串，以及mips代码的.data段
//计算每个函数中出现的中间变量，填入各个函数的符号表
void mips_init(){				
	int i=0;
	int j=0;
	int k=0;
	int index=0;
	int nextindex=0;
	int funcnum=0;
	fprintf(mipsfile,".data\n");

	//遍历符号表，将全局变量(没有常量)生成.data段,char、int均为为4字节
	for(i=0;i<globallen;i++){
		if(tab[i].type==0){		//int
			fprintf(mipsfile,"\t%s:.word %d\n",tab[i].id,tab[i].len);		//id:.word len
		}
		else if(tab[i].type==1){	//char
			fprintf(mipsfile,"\t%s:.word %d\n",tab[i].id,tab[i].len);		//id:.word len
		}
		else if(tab[i].type==4){	//int数组
			fprintf(mipsfile,"\t%s:.word 0:%d\n",tab[i].id,tab[i].len);		//id:.word 0:len
		}
		else if(tab[i].type==5){						//char数组
			fprintf(mipsfile,"\t%s:.word 0:%d\n",tab[i].id,tab[i].len);		//id:.word 0:len
		}
	}

	//遍历中间代码，
	//1.为字符串申请空间
	//2.计算每个函数的size，为所有中间变量申请空间
	for(i=0;i<midlen;i++){
		if(mid[i].type==PRINTSTROP){			//print string
			//将字符串及标号存入string,数组下标即为其标号
			strcpy(STRING[strnum].id,mid[i].op1);
			char strname[tokensize]={'\0'};
			getstrname(strname);	//strnum++
			fprintf(mipsfile,"\t%s:.asciiz \"%s\"\n",strname,mid[i].op1);	//str:.asciiz ""
			//共有strnum个字符串
		}
		else if(mid[i].type==INTFUNCDEF || mid[i].type==CHARFUNCDEF || mid[i].type==VOIDFUNCDEF){//函数定义
			//找到函数在符号表的索引
			for(j=0;j<funclen;j++){
				if(strcmp(tab[funcindex[j]].id,mid[i].op1)==0){
					index=funcindex[j];
					funcnum=j;							//第j个函数
					if(j>=functabsize){
						//符号表溢出
						errormessage(OUTOFTAB_ERROR,line,no);
						return;
					}

					if(j+1<funclen){
						nextindex=funcindex[j+1];		//后面还有函数
					}
					else{
						nextindex=tablen;				//是最后一个函数 即main
					}
					break;
				}
			}

			functab[funcnum].tablen=0;
			for(j=index+1,k=0;j<nextindex;j++,k++){
				//拷贝已有符号
				functab[funcnum].tab[k].type=tab[j].type;
				functab[funcnum].tab[k].addr=tab[j].addr;
				functab[funcnum].tab[k].len=tab[j].len;
				functab[funcnum].tab[k].lev=tab[j].lev;
				functab[funcnum].tab[k].size=tab[j].size;
				strcpy(functab[funcnum].tab[k].id,tab[j].id);
				funcsize[funcnum]=tab[index].size;				//拷贝函数size
				functab[funcnum].tablen++;
			}

		}
		//以下几种中间代码类型，为中间变量第一次出现处
		else if(mid[i].type==VARASSIGN || 
			   (mid[i].type>=1 && mid[i].type<=4) ||
				mid[i].type==ARRUSE ||
				mid[i].type==FUNCRET){
			
			if(mid[i].op1!=NULL && mid[i].op1[0]=='$'){			//中间变量
				addnewsym(mid[i].op1,funcnum);					//向函数符号表中插入中间变量
			}

			if(mid[i].op2!=NULL && mid[i].op2[0]=='$'){
				addnewsym(mid[i].op2,funcnum);					//向函数符号表中插入中间变量
			}

			if(mid[i].result!=NULL && mid[i].result[0]=='$'){
				addnewsym(mid[i].result,funcnum);					//向函数符号表中插入中间变量
			}
		}
	}
	fprintf(mipsfile,".text\n");								//.text
	fprintf(mipsfile,"\tadd $fp $0 $sp\n");						//add $fp $0 $sp
	fprintf(mipsfile,"\tsubi $sp $sp 12\n");					//subi $sp $sp 12
	fprintf(mipsfile,"\tsubi $sp $sp %d\n",funcsize[funclen-1]);//subi $sp $sp sizeof(main)
	fprintf(mipsfile,"\tj main\n");								//j main
}


//返回0，局部变量；返回1，全局变量
int getaddr(char* id,int isarr,int funcnum,char* addr){
	int i=0;
	//先遍历当前函数的数据区, 当前函数的符号表为functab[funcnum]
	for(i=0;i<functab[funcnum].tablen;i++){
		if(strcmp(id,functab[funcnum].tab[i].id)==0){		//读到相同变量名
			if((isarr==1 && (functab[funcnum].tab[i].type==4 || functab[funcnum].tab[i].type==5)) 
				|| (isarr==0 && functab[funcnum].tab[i].type!=4 && functab[funcnum].tab[i].type!=5)){
				//是局部变量，地址先转成字符串
				//addr只是相对函数地址
				sprintf(addr,"%d",functab[funcnum].tab[i].addr);
				return 0;
			}
		}
	}

	//再遍历全局栈区(符号表的全局量)
	for(i=0;i<globallen;i++){
		if(strcmp(tab[i].id,id)==0){
			//标识符相同且类型相同
			if((isarr==1 && (tab[i].type==4 || tab[i].type==5))
				|| (isarr==0 && tab[i].type!=4 && tab[i].type!=5)){
				sprintf(addr,"%s",id);
				return 1;
			}
		}
	}
	
	//到这里时前面已经报错 
	printf("************************************ERROR************************************\n");
	printf("in mips.c:the ID is not defined\n %s",id);
	
	sprintf(addr,"%s",id);
	return 0;
}



//为局部常量在栈上赋初值 const int/char = x
/*
void mips_localconst(int index,int funcnum){
	if(mid[index].type==CONSTINTDEF){
		//fprintf(mipsfile,"#const int %s = %s\n",mid[index].op1,mid[index].op2);
	}
	else{
		//fprintf(mipsfile,"#const char %s = %s\n",mid[index].op1,mid[index].op2);
	}
	
	int i=0;
	char addr[200]={'\0'};
	int temp=getaddr(mid[index].op1,0,funcnum,addr);
	//查找局部常量的值
	for(i=0;i<functab[funcnum].tablen;i++){
		if(strcmp(mid[index].op1,functab[funcnum].tab[i].id)==0){
			fprintf(mipsfile,"\tli $t0 %d\n",functab[funcnum].tab[i].len);
			fprintf(mipsfile,"\tsw $t0 %s($sp)\n",addr);
			break;
		}
	}


}
*/


//产生加法指令三个操作数一定不是数组
void mips_add(int index,int funcnum){		// result=op1+op2,op1到$t0,op2到$t1,result到$t0
	char op1_addr[200]={'\0'};
	char op2_addr[200]={'\0'};
	char result_addr[200]={'\0'};
	int ret=0;
	//操作数1为数字或标识符
	if(isrealnum(mid[index].op1)){
		fprintf(mipsfile,"\tli $t0 %s\n",mid[index].op1);			//li $t0 op1
	}
	else{
		ret=getaddr(mid[index].op1,0,funcnum,op1_addr);						//查询地址
		if(ret==1){//全局量
			fprintf(mipsfile,"\tlw $t0 %s\n",op1_addr);				//lw $t0 op1_addr
		}
		else{		//局部量
			fprintf(mipsfile,"\tlw $t0 %s($sp)\n",op1_addr);		//lw $t0 op1_addr($sp)
		}
	}

	//操作数2为数字或标识符
	if(isrealnum(mid[index].op2)){
		fprintf(mipsfile,"\tli $t1 %s\n",mid[index].op2);		//li $t1 op2
	}
	else{
		ret=getaddr(mid[index].op2,0,funcnum,op2_addr);
		if(ret==1){//全局量
			fprintf(mipsfile,"\tlw $t1 %s\n",op2_addr);				//lw $t1 op2_addr
		}
		else{		//局部量
			fprintf(mipsfile,"\tlw $t1 %s($sp)\n",op2_addr);		//lw $t1 op2_addr($sp)
		}
	}
	
	fprintf(mipsfile,"\tadd $t0 $t0 $t1\n");						//add $t0 $t0 $t1
	ret=getaddr(mid[index].result,0,funcnum,result_addr);

	if(ret==1){//全局量
		fprintf(mipsfile,"\tsw $t0 %s\n",result_addr);				//sw $t0 result_addr
	}
	else{		//局部量
		fprintf(mipsfile,"\tsw $t0 %s($sp)\n",result_addr);			//sw $t0 result_addr($sp)
	}
}

//产生减法指令三个操作数一定不是数组
void mips_sub(int index,int funcnum){		// result=op1-op2,op1到$t0,op2到$t1,result到$t0
	char op1_addr[200]={'\0'};
	char op2_addr[200]={'\0'};
	char result_addr[200]={'\0'};
	int ret=0;
	//操作数1为数字或标识符
	if(isrealnum(mid[index].op1)){
		fprintf(mipsfile,"\tli $t0 %s\n",mid[index].op1);			//li $t0 op1
	}
	else{
		ret=getaddr(mid[index].op1,0,funcnum,op1_addr);						//查询地址
		if(ret==1){//全局量
			fprintf(mipsfile,"\tlw $t0 %s\n",op1_addr);				//lw $t0 op1_addr
		}
		else{		//局部量
			fprintf(mipsfile,"\tlw $t0 %s($sp)\n",op1_addr);		//lw $t0 op1_addr($sp)
		}
		
	}

	//操作数2为数字或标识符
	if(isrealnum(mid[index].op2)){
		fprintf(mipsfile,"\tli $t1 %s\n",mid[index].op2);		//li $t1 op2
	}
	else{
		ret=getaddr(mid[index].op2,0,funcnum,op2_addr);
		if(ret==1){//全局量
			fprintf(mipsfile,"\tlw $t1 %s\n",op2_addr);				//lw $t1 op2_addr
		}
		else{		//局部量
			fprintf(mipsfile,"\tlw $t1 %s($sp)\n",op2_addr);		//lw $t1 op2_addr($sp)
		}
	}
	
	fprintf(mipsfile,"\tsub $t0 $t0 $t1\n");						//sub $t0 $t0 $t1
	ret=getaddr(mid[index].result,0,funcnum,result_addr);

	if(ret==1){//全局量
		fprintf(mipsfile,"\tsw $t0 %s\n",result_addr);				//sw $t0 result_addr
	}
	else{		//局部量
		fprintf(mipsfile,"\tsw $t0 %s($sp)\n",result_addr);			//sw $t0 result_addr($sp)
	}
}

//产生乘法指令三个操作数一定不是数组
void mips_mul(int index,int funcnum){		// result=op1*op2,op1到$t0,op2到$t1,result到$t0
	char op1_addr[200]={'\0'};
	char op2_addr[200]={'\0'};
	char result_addr[200]={'\0'};
	int ret=0;
	//操作数1为数字或标识符
	if(isrealnum(mid[index].op1)){
		fprintf(mipsfile,"\tli $t0 %s\n",mid[index].op1);			//li $t0 op1
	}
	else{
		ret=getaddr(mid[index].op1,0,funcnum,op1_addr);						//查询地址
		if(ret==1){//全局量
			fprintf(mipsfile,"\tlw $t0 %s\n",op1_addr);				//lw $t0 op1_addr
		}
		else{		//局部量
			fprintf(mipsfile,"\tlw $t0 %s($sp)\n",op1_addr);		//lw $t0 op1_addr($sp)
		}
		
	}

	//操作数2为数字或标识符
	if(isrealnum(mid[index].op2)){
		fprintf(mipsfile,"\tli $t1 %s\n",mid[index].op2);			//li $t1 op2
	}
	else{
		ret=getaddr(mid[index].op2,0,funcnum,op2_addr);
		if(ret==1){//全局量
			fprintf(mipsfile,"\tlw $t1 %s\n",op2_addr);				//lw $t1 op2_addr
		}
		else{		//局部量
			fprintf(mipsfile,"\tlw $t1 %s($sp)\n",op2_addr);		//lw $t1 op2_addr($sp)
		}
	}
	
	fprintf(mipsfile,"\tmult $t0 $t1\n");							//mult $t0 $t1
	fprintf(mipsfile,"\tmflo $t0\n");								//mflo $t0
	ret=getaddr(mid[index].result,0,funcnum,result_addr);

	if(ret==1){//全局量
		fprintf(mipsfile,"\tsw $t0 %s\n",result_addr);				//sw $t0 result_addr
	}
	else{		//局部量
		fprintf(mipsfile,"\tsw $t0 %s($sp)\n",result_addr);			//sw $t0 result_addr($sp)
	}
	
}

//产生除法指令三个操作数一定不是数组
void mips_div(int index,int funcnum){		// result=op1、op2,op1到$t0,op2到$t1,result到$t0
	char op1_addr[200]={'\0'};
	char op2_addr[200]={'\0'};
	char result_addr[200]={'\0'};
	int ret=0;
	//操作数1为数字或标识符
	if(isrealnum(mid[index].op1)){
		fprintf(mipsfile,"\tli $t0 %s\n",mid[index].op1);			//li $t0 op1
	}
	else{
		ret=getaddr(mid[index].op1,0,funcnum,op1_addr);						//查询地址
		if(ret==1){//全局量
			fprintf(mipsfile,"\tlw $t0 %s\n",op1_addr);				//lw $t0 op1_addr
		}
		else{		//局部量
			fprintf(mipsfile,"\tlw $t0 %s($sp)\n",op1_addr);		//lw $t0 op1_addr($sp)
		}
		
	}

	//操作数2为数字或标识符
	if(isrealnum(mid[index].op2)){
		fprintf(mipsfile,"\tli $t1 %s\n",mid[index].op2);			//li $t1 op2
	}
	else{
		ret=getaddr(mid[index].op2,0,funcnum,op2_addr);
		if(ret==1){//全局量
			fprintf(mipsfile,"\tlw $t1 %s\n",op2_addr);				//lw $t1 op2_addr
		}
		else{		//局部量
			fprintf(mipsfile,"\tlw $t1 %s($sp)\n",op2_addr);		//lw $t1 op2_addr($sp)
		}
	}
	
	fprintf(mipsfile,"\tdiv $t0 $t1\n");							//div $t0 $t1
	fprintf(mipsfile,"\tmflo $t0\n");								//mflo $t0
	ret=getaddr(mid[index].result,0,funcnum,result_addr);

	if(ret==1){//全局量
		fprintf(mipsfile,"\tsw $t0 %s\n",result_addr);				//sw $t0 result_addr
	}
	else{		//局部量
		fprintf(mipsfile,"\tsw $t0 %s($sp)\n",result_addr);			//sw $t0 result_addr($sp)
	}
}


//函数定义
void mips_funcdef(int index,int funcnum){				//设置函数label
	fprintf(mipsfile,"%s:\n",mid[index].op1);			//function:
	if(strcmp(mid[index].op1,"main")==0){				//main函数不要保存返回地址
		return;
	}
	fprintf(mipsfile,"\tsw $ra -8($fp)\n");			//sw $ra -8($fp)保存返回地址
	fprintf(mipsfile,"\tsw $0 -12($fp)\n");			//sw 0 -12($fp)将返回值置为0

}														


//调用函数funcnum
void mips_funccall(int index,int funcnum){
	int i=0;
	int j=0;
	fprintf(mipsfile,"\tsw $fp -4($sp)\n");	//sw $fp -4($sp)保存上一个函数的$fp

	//将fp置于新的栈顶，new_fp=old_sp
	fprintf(mipsfile,"\tmove $fp $sp\n");	//move $fp $sp

	//分两步将$sp置于新的栈底 new_sp=old_sp-12-sizeof(func)
	fprintf(mipsfile,"\tsubi $sp $sp 12\n");					//subi $sp $sp 12
	fprintf(mipsfile,"\tsubi $sp $sp %d\n",funcsize[funcnum]);	//subi $sp $sp sizeof(func)


	//处理参数
	//fprintf(mipsfile,"#function para\n");
	//从倒数第len个开始
	for(i=paranum-tab[funcindex[funcnum]].len,j=0;i<paranum && j<tab[funcindex[funcnum]].len;i++,j++){
		if(PARA[i].type==0){			//参数是数字
			fprintf(mipsfile,"\tli $t0 %s\n",PARA[i].id);	//li $t0 para
		}
		else if(PARA[i].type==1){		//是全局量
			fprintf(mipsfile,"\tlw $t0 %s\n",PARA[i].id);	//lw $t0 addr
		}
		else{
			fprintf(mipsfile,"\tlw $t0 %s($fp)\n",PARA[i].id);//lw $t0 addr($fp)现在的fp是当时的sp
		}
		fprintf(mipsfile,"\tsw $t0 %d($sp)\n",j*4);			//sw $t0 j*4($sp)
		memset(PARA[i].id,0,tokensize);						//出栈
	}
	paranum=paranum-tab[funcindex[funcnum]].len;
	//fprintf(mipsfile,"#function para end\n");
	fprintf(mipsfile,"\tjal %s\n",mid[index].op1);				//jal function
}


//函数传参(push para)
void mips_pushpara(int index,int funcnum){
	int  temp=0;
	char addr[200]={'\0'};
	memset(addr,0,200);

	if(paranum>=parasize){
		printf("*******************************too much para*******************************");
		exit(0);
	}

	//如果参数是个数字,直接入栈
	if(isrealnum(mid[index].op1)){
		PARA[paranum].type=0;					//是数字
		strcpy(PARA[paranum].id,mid[index].op1);//拷贝数字
		paranum++;
	}
	//否则查找地址,再入栈,参数不可能是数组成员
	else{
		temp=getaddr(mid[index].op1,0,funcnum,addr);
		if(temp==0){		//局部变量,参数位于函数符号表的开头
							//在调用函数后，处理参数，因此被调用函数的fp是之前的sp
			PARA[paranum].type=2;
		}
		else{				//全局变量，名字即是地址
			PARA[paranum].type=1;
		}
		strcpy(PARA[paranum].id,addr);		//拷贝地址
		paranum++;
	}
	
}


//return value或return value
void mips_retvalue(int index,int funcnum){
	int temp=0;
	char addr[200]={'\0'};
	if(mid[index].type==RETNULLOP){			//返回NULL

	}
	else{									//返回value	
		//返回值是一个数字
		if(isrealnum(mid[index].op1)){
			fprintf(mipsfile,"\tli $t0 %s\n",mid[index].op1);		//li $t0 op1
			fprintf(mipsfile,"\tsw $t0 -12($fp)\n");			//sw $t0 -12($fp) 返回值地址为fp-12
		}
		else{
			temp=getaddr(mid[index].op1,0,funcnum,addr);
			if(temp==1){//全局量，标识符即为地址
				fprintf(mipsfile,"\tlw $t0 %s\n",addr);		//lw $t0 addr
				fprintf(mipsfile,"\tsw $t0 -12($fp)\n");	//sw $t0 -12($fp)
			}
			else{		//局部量,需要加上sp寄存器的值
				fprintf(mipsfile,"\tlw $t0 %s($sp)\n",addr);//lw $t0 addr($sp)
				fprintf(mipsfile,"\tsw $t0 -12($fp)\n");	//sw $t0 -12($fp)
			}
		}
	}

	//如果此时是main函数
	if(funcnum==funclen-1){
		fprintf(mipsfile,"\tj Label_end\n");		//直接跳到函数结尾的label
	}
	else{
		fprintf(mipsfile,"\tmove $sp $fp\n");		//move $sp $fp		sp=fp
		fprintf(mipsfile,"\tlw $ra -8($fp)\n");		//lw $ra -8($fp)	从栈上取返回地址到$ra
		fprintf(mipsfile,"\tlw $fp -4($fp)\n");		//lw $fp -4($fp)	从栈上取上一个函数的fp
		fprintf(mipsfile,"\tjr $ra\n");				//jr $ra			跳转到返回地址
	}
}


//变量赋值语句op1=op2
void mips_varassign(int index,int funcnum){
	char addr1[200]={'\0'};
	char addr2[200]={'\0'};
	int temp=0;
	if(isrealnum(mid[index].op2)){		//op2是数字
		fprintf(mipsfile,"\tli $t0 %s\n",mid[index].op2);	//li $t0 op2
	}
	else{
		temp=getaddr(mid[index].op2,0,funcnum,addr1);
		if(temp==0){		//op2局部变量
			fprintf(mipsfile,"\tlw $t0 %s($sp)\n",addr1);	//lw $t0 addr1($sp)
		}
		else{				//op2全局变量
			fprintf(mipsfile,"\tlw $t0 %s\n",addr1);		//lw $t0 addr1
		}
	}

	temp=getaddr(mid[index].op1,0,funcnum,addr2);
	if(temp==0){//op1局部变量
		fprintf(mipsfile,"\tsw $t0 %s($sp)\n",addr2);	//sw $t0 addr2($sp)
	}
	else{		//op1全局变量
		fprintf(mipsfile,"\tsw $t0 %s\n",addr2);		//sw $t0 addr2
	}
}


//为数组赋值op1[op2] = result
void mips_arrassign(int index,int funcnum){
	char addr1[200]={'\0'};
	char addr2[200]={'\0'};
	char res_addr[200]={'\0'};
	int temp=0;
	//op1->$t0
	//注意，此处用la指令，因为需要的是op1的地址,而非op1地址中的值
	temp=getaddr(mid[index].op1,1,funcnum,addr1);
	if(temp==0){//局部变量
		fprintf(mipsfile,"\tla $t0 %s($sp)\n",addr1);		//la $t0 addr1($sp)
	}
	else{		//全局变量
		fprintf(mipsfile,"\tla $t0 %s\n",addr1);			//la $t0 addr1
	}

	//op2->$t1
	if(isrealnum(mid[index].op2)){		//op2是数字
		fprintf(mipsfile,"\tli $t1 %s\n",mid[index].op2);	//li $t1 op2
	}
	else{
		temp=getaddr(mid[index].op2,0,funcnum,addr2);
		if(temp==0){		//op2局部变量
			fprintf(mipsfile,"\tlw $t1 %s($sp)\n",addr2);	//lw $t1 addr2($sp)
		}
		else{				//op2全局变量
			fprintf(mipsfile,"\tlw $t1 %s\n",addr2);		//lw $t1 addr2
		}
	}
	//计算 op2*4
	fprintf(mipsfile,"\tli $t2 4\n");				//li $t2 4
	fprintf(mipsfile,"\tmul $t1 $t1 $t2\n");		//mul $t1 $t1 $t2

	//result->$t2
	if(isrealnum(mid[index].result)){		//result是数字
		fprintf(mipsfile,"\tli $t2 %s\n",mid[index].result);	//li $t2 result
	}
	else{
		temp=getaddr(mid[index].result,0,funcnum,res_addr);
		if(temp==0){		//result局部变量
			fprintf(mipsfile,"\tlw $t2 %s($sp)\n",res_addr);	//lw $t2 res_addr($sp)
		}
		else{				//result全局变量
			fprintf(mipsfile,"\tlw $t2 %s\n",res_addr);			//lw $t2 res_addr
		}
	}

	//op1[op2]=result	
	//先计算op1[op2]地址=op1+4*op2
	//					=$t0 + $t1
	fprintf(mipsfile,"\tadd $t1 $t1 $t0\n");			//add $t1 $t1 $t0
	fprintf(mipsfile,"\tsw $t2 0($t1)\n");				//sw $t2 0($t1)

}


//数组值赋给非数组变量result = op1[op2]
void mips_arruse(int index,int funcnum){
	char addr1[200]={'\0'};
	char addr2[200]={'\0'};
	char res_addr[200]={'\0'};
	int temp=0;
	//op1->$t0
	//注意，此处用la指令，因为需要的是op1的地址,而非op1地址中的值
	temp=getaddr(mid[index].op1,1,funcnum,addr1);
	if(temp==0){//局部变量
		fprintf(mipsfile,"\tla $t0 %s($sp)\n",addr1);		//la $t0 addr1($sp)
	}
	else{		//全局变量
		fprintf(mipsfile,"\tla $t0 %s\n",addr1);			//la $t0 addr1
	}

	//op2->$t1
	if(isrealnum(mid[index].op2)){		//op2是数字
		fprintf(mipsfile,"\tli $t1 %s\n",mid[index].op2);	//li $t1 op2
	}
	else{
		temp=getaddr(mid[index].op2,0,funcnum,addr2);
		if(temp==0){		//op2局部变量
			fprintf(mipsfile,"\tlw $t1 %s($sp)\n",addr2);	//lw $t1 addr2($sp)
		}
		else{				//op2全局变量
			fprintf(mipsfile,"\tlw $t1 %s\n",addr2);		//lw $t1 addr2
		}
	}
	//计算 op2*4
	fprintf(mipsfile,"\tli $t2 4\n");				//li $t2 4
	fprintf(mipsfile,"\tmul $t1 $t1 $t2\n");		//mul $t1 $t1 $t2
	

	//result=op1[op2]	
	//先计算op1[op2]地址存在$t1
	fprintf(mipsfile,"\tadd $t1 $t1 $t0\n");			//add $t1 $t1 $t0
	//再取op1[op2]的值,存在$t0
	fprintf(mipsfile,"\tlw $t0 0($t1)\n");				//lw $t0 0($t1)

	
	temp=getaddr(mid[index].result,0,funcnum,res_addr);
	if(temp==0){		//result局部变量
		fprintf(mipsfile,"\tsw $t0 %s($sp)\n",res_addr);	//sw $t0 res_addr($sp)
	}
	else{				//result全局变量
		fprintf(mipsfile,"\tsw $t0 %s\n",res_addr);			//sw $t0 res_addr
	}

}


//打印字符串 print str
void mips_printstr(int index){
	int i=0;
	char strname[200]={'\0'};	//.data段的字符串名字
	for(i=0;i<strnum;i++){
		if(strcmp(STRING[i].id,mid[index].op1)==0){
			fprintf(mipsfile,"\tli $v0 4\n");			//置$v0=4
			fprintf(mipsfile,"\tla $a0 $string_%d\n",i);//置$a0=$string_i
			fprintf(mipsfile,"\tsyscall\n");			//syscall
			break;
		}
	}
}


//打印整数 print integer
void mips_printint(int index,int funcnum){
	int temp=0;
	char addr[200]={'\0'};
	fprintf(mipsfile,"\tli $v0 1\n");			//置$v0=1
	
	if(isrealnum(mid[index].op1)){				//op1是数字
		fprintf(mipsfile,"\tli $a0 %s\n",mid[index].op1);		//li $a0 op1
	}
	else{
		temp=getaddr(mid[index].op1,0,funcnum,addr);				//查询地址
		if(temp==1){//全局量
			fprintf(mipsfile,"\tlw $a0 %s\n",addr);				//lw $a0 addr
		}
		else{		//局部量
			fprintf(mipsfile,"\tlw $a0 %s($sp)\n",addr);		//lw $a0 addr($sp)
		}
	}
	fprintf(mipsfile,"\tsyscall\n");			//syscall
}


//打印字符 print char
void mips_printchar(int index,int funcnum){
	int temp=0;
	char addr[200]={'\0'};
	fprintf(mipsfile,"\tli $v0 11\n");			//置$v0=11
	
	if(isrealnum(mid[index].op1)){				//op1是数字
		fprintf(mipsfile,"\tli $a0 %s\n",mid[index].op1);		//li $a0 op1
	}
	else{
		temp=getaddr(mid[index].op1,0,funcnum,addr);				//查询地址
		if(temp==1){//全局量
			fprintf(mipsfile,"\tlw $a0 %s\n",addr);				//lw $a0 addr
		}
		else{		//局部量
			fprintf(mipsfile,"\tlw $a0 %s($sp)\n",addr);		//lw $a0 addr($sp)
		}
	}
	fprintf(mipsfile,"\tsyscall\n");							//syscall
}


//读取 scan a(a不是数组) 
//a为 int
void mips_scanint(int index,int funcnum){
	int temp=0;
	char addr[200]={'\0'};
	fprintf(mipsfile,"\tli $v0 5\n");			//置$v0=5(read integer) 
	fprintf(mipsfile,"\tsyscall\n");			//syscall
	
	temp=getaddr(mid[index].op1,0,funcnum,addr);				//查询地址
	if(temp==1){//全局量
		fprintf(mipsfile,"\tsw $v0 %s\n",addr);				//sw $v0 addr
	}
	else{		//局部量
		fprintf(mipsfile,"\tsw $v0 %s($sp)\n",addr);		//sw $v0 addr($sp)
	}
}



//读取 scan a(a不是数组) 
//a为 char
void mips_scanchar(int index,int funcnum){
	int temp=0;
	char addr[200]={'\0'};
	fprintf(mipsfile,"\tli $v0 12\n");			//置$v0=12(read character)
	fprintf(mipsfile,"\tsyscall\n");			//syscall
	
	temp=getaddr(mid[index].op1,0,funcnum,addr);				//查询地址
	if(temp==1){//全局量
		fprintf(mipsfile,"\tsw $v0 %s\n",addr);				//sw $v0 addr
	}
	else{		//局部量
		fprintf(mipsfile,"\tsw $v0 %s($sp)\n",addr);		//sw $v0 addr($sp)
	}
}


//op1=RET 将被调用函数的返回值赋给变量
void mips_funcret(int index,int funcnum){
	int temp=0;
	char addr[200]={'\0'};
	fprintf(mipsfile,"\tlw $t0 -12($sp)\n");			//lw $t0 -12($sp)	当前sp即为被调用函数的fp
	temp=getaddr(mid[index].op1,0,funcnum,addr);				//查询地址
	if(temp==1){//全局量
		fprintf(mipsfile,"\tsw $t0 %s\n",addr);				//sw $t0 addr
	}
	else{		//局部量
		fprintf(mipsfile,"\tsw $t0 %s($sp)\n",addr);		//sw $t0 addr($sp)
	}
}



//上一个表达式为0(false)，跳转
void mips_bz(int index,int funcnum){
	char op1_addr[200]={'\0'};
	char op2_addr[200]={'\0'};
	char result_addr[200]={'\0'};
	int ret=0;
	//对上一个中间代码处理


	//是 访问数组 + - * /
	if(mid[index-1].type==ARRUSE || (mid[index-1].type>=1 && mid[index-1].type<=4)){
		ret=getaddr(mid[index-1].result,0,funcnum,result_addr);
		if(ret==1){//全局量
			fprintf(mipsfile,"\tlw $t0 %s\n",result_addr);				//lw $t0 result_addr
		}
		else{		//局部量
			fprintf(mipsfile,"\tlw $t0 %s($sp)\n",result_addr);			//lw $t0 result_addr($sp)
		}
	}
	//是函数返回值 op1=RET
	else if(mid[index-1].type==FUNCRET){
		ret=getaddr(mid[index-1].op1,0,funcnum,op1_addr);			//查询地址
		if(ret==1){//全局量
			fprintf(mipsfile,"\tlw $t0 %s\n",op1_addr);				//lw $t0 op1_addr
		}
		else{		//局部量
			fprintf(mipsfile,"\tlw $t0 %s($sp)\n",op1_addr);		//lw $t0 op1_addr($sp)
		}
	}
	// > < == != >= <=
	else{
		//操作数1为数字或标识符
		if(isrealnum(mid[index-1].op1)){
			fprintf(mipsfile,"\tli $t0 %s\n",mid[index-1].op1);			//li $t0 op1
		}
		else{
			ret=getaddr(mid[index-1].op1,0,funcnum,op1_addr);			//查询地址
			if(ret==1){//全局量
				fprintf(mipsfile,"\tlw $t0 %s\n",op1_addr);				//lw $t0 op1_addr
			}
			else{		//局部量
				fprintf(mipsfile,"\tlw $t0 %s($sp)\n",op1_addr);		//lw $t0 op1_addr($sp)
			}
		}

		//操作数2为数字或标识符,
		if(isrealnum(mid[index-1].op2)){
			fprintf(mipsfile,"\tli $t1 %s\n",mid[index-1].op2);		//li $t1 op2
		}
		else{
			ret=getaddr(mid[index-1].op2,0,funcnum,op2_addr);
			if(ret==1){//全局量
				fprintf(mipsfile,"\tlw $t1 %s\n",op2_addr);				//lw $t1 op2_addr
			}
			else{		//局部量
				fprintf(mipsfile,"\tlw $t1 %s($sp)\n",op2_addr);		//lw $t1 op2_addr($sp)
			}
		}
	}
	

	//上一个式子是函数返回值 op1=RET
	if(mid[index-1].type==FUNCRET){
		fprintf(mipsfile,"\tbeqz $t0 %s\n",mid[index].op1);						//beqz op1	
	}
	//上一个式子是+ - * /
	else if(mid[index-1].type>=1 && mid[index-1].type<=4){
		fprintf(mipsfile,"\tbeqz $t0 %s\n",mid[index].op1);						//beqz op1	
	}
	//上一个式子是变量赋值 op1=op2
	else if(mid[index-1].type==VARASSIGN){								//branch if equal zero
		fprintf(mipsfile,"\tbeqz $t0 %s\n",mid[index].op1);						//beqz op1		
	}
	//上一个式子是数组使用 result = op1[op2]
	else if(mid[index-1].type==ARRUSE){
		fprintf(mipsfile,"\tbeqz $t0 %s\n",mid[index].op1);						//beqz op1
	}
	//上一个式子是>= op1>=op2
	else if(mid[index-1].type==BIGEQLOP){							//branch if less than
		fprintf(mipsfile,"\tblt $t0 $t1 %s\n",mid[index].op1);				//blt $t0 $t1 op1
	}
	//上一个式子是<= op1<=op2
	else if(mid[index-1].type==SMALLEQLOP){							//branch if greater than			
		fprintf(mipsfile,"\tbgt $t0 $t1 %s\n",mid[index].op1);				//bgt $t0 $t1 op1
	}
	//上一个式子是== op1==op2
	else if(mid[index-1].type==EQUALOP){							//branch if no equal
		fprintf(mipsfile,"\tbne $t0 $t1 %s\n",mid[index].op1);				//bne $t0 $t1 op1
	}
	//上一个式子是!= op1!=op2
	else if(mid[index-1].type==NOTEQLOP){							//branch if equal
		fprintf(mipsfile,"\tbeq $t0 $t1 %s\n",mid[index].op1);				//beq $t0 $t1 op1
	}
	//上一个式子是> op1>op2
	else if(mid[index-1].type==BIGOP){								//branch if less of equal
		fprintf(mipsfile,"\tble $t0 $t1 %s\n",mid[index].op1);				//ble $t0 $t1 op1
	}
	//上一个式子是< op1<op2
	else if(mid[index-1].type==SMALLOP){							//branch if greater of equal
		fprintf(mipsfile,"\tbge $t0 $t1 %s\n",mid[index].op1);				//bge $t0 $t1 op1
	}
	else{
		//不可能到这里
		printf("bz error\n %d",index);
	}
}


void mips(){
	int i=0;
	int funcnum=-1;			

	mips_init();			//设置.data区,填充函数符号表

	//printfunctab();			//打印符号表
	for(i=0;i<midlen;i++){
		if(mid[i].type==ADDOP){			//+
			//fprintf(mipsfile,"#%s = %s + %s\n",mid[i].result,mid[i].op1,mid[i].op2);
			mips_add(i,funcnum);
		}
		else if(mid[i].type==SUBOP){	//-
			//fprintf(mipsfile,"#%s = %s - %s\n",mid[i].result,mid[i].op1,mid[i].op2);
			mips_sub(i,funcnum);
		}
		else if(mid[i].type==MULOP){	//*
			//fprintf(mipsfile,"#%s = %s * %s\n",mid[i].result,mid[i].op1,mid[i].op2);
			mips_mul(i,funcnum);			
		}
		else if(mid[i].type==DIVOP){	//div
			//fprintf(mipsfile,"#%s = %s / %s\n",mid[i].result,mid[i].op1,mid[i].op2);
			mips_div(i,funcnum);
		}
		else if((mid[i].type==CONSTINTDEF || mid[i].type==CONSTCHARDEF) && funcnum!=-1){
			//仅仅针对局部常量，因为全局常、变量已经在.data段分配了空间
			//局部常量初始化
			//mips_localconst(i,funcnum);
		}
		else if(mid[i].type==INTFUNCDEF //函数定义
			|| mid[i].type==CHARFUNCDEF 
			|| mid[i].type==VOIDFUNCDEF){
			funcnum++;
			mips_funcdef(i,funcnum);
		}
		else if(mid[i].type==FUNCCALL){	//函数调用
			//fprintf(mipsfile,"#call %s\n",mid[i].op1);
			int temp=0;
			int j=0;
			for(j=0;j<funclen;j++){
				if(strcmp(mid[i].op1,tab[funcindex[j]].id)==0){
					temp=j;
					break;
				}
			}
			//temp为被调用函数的索引
			mips_funccall(i,temp);
		}
		else if(mid[i].type==PARAOP){	//函数传参
			//fprintf(mipsfile,"#push %s\n",mid[i].op1);
			mips_pushpara(i,funcnum);
		}
		else if(mid[i].type==RETEXPOP || mid[i].type==RETNULLOP){	//return (a); 或 return NULL
			//fprintf(mipsfile,"#return value\n");
			mips_retvalue(i,funcnum);
		}
		else if(mid[i].type==VARASSIGN){							//为变量赋值op1 = op2
			//fprintf(mipsfile,"#%s = %s\n",mid[i].op1,mid[i].op2);
			mips_varassign(i,funcnum);
		}
		else if(mid[i].type==ARRASSIGN){							//为数组赋值op1[op2] = result
			//fprintf(mipsfile,"#%s[%s] = %s\n",mid[i].op1,mid[i].op2,mid[i].result);	
			mips_arrassign(i,funcnum);
		}
		else if(mid[i].type==ARRUSE){								//数组赋给非数组变量result = op1[op2]
			//fprintf(mipsfile,"#%s = %s[%s]\n",mid[i].result,mid[i].op1,mid[i].op2);
			mips_arruse(i,funcnum);
		}
		else if(mid[i].type==PRINTSTROP){							//打印字符串 print string
			//fprintf(mipsfile,"#print %s\n",mid[i].op1);	
			mips_printstr(i);
			//后三条为换行
			//fprintf(mipsfile,"\tli $v0 11\n");
			//fprintf(mipsfile,"\tli $a0 10\n");
			//fprintf(mipsfile,"\tsyscall\n");
		}
		else if(mid[i].type==PRINTINTOP){							//打印整数 print integer
			//fprintf(mipsfile,"#print %s\n",mid[i].op1);
			mips_printint(i,funcnum);
			//后三条为换行
			//fprintf(mipsfile,"\tli $v0 11\n");
			//fprintf(mipsfile,"\tli $a0 10\n");
			//fprintf(mipsfile,"\tsyscall\n");
		}
		else if(mid[i].type==PRINTCHAROP){							//打印字符 print char
			//fprintf(mipsfile,"#print %s\n",mid[i].op1);
			mips_printchar(i,funcnum);
			//后三条为换行
			//fprintf(mipsfile,"\tli $v0 11\n");
			//fprintf(mipsfile,"\tli $a0 10\n");
			//fprintf(mipsfile,"\tsyscall\n");
		}
		else if(mid[i].type==PRINTLINE){							//换行
			fprintf(mipsfile,"\tli $v0 11\n");
			fprintf(mipsfile,"\tli $a0 10\n");
			fprintf(mipsfile,"\tsyscall\n");
		}
		else if(mid[i].type==SCANINTOP){							//读取 scan a(int)
			//fprintf(mipsfile,"#scan int %s\n",mid[i].op1);
			mips_scanint(i,funcnum);
		}
		else if(mid[i].type==SCANCHAROP){							//读取 scan a(char)
			//fprintf(mipsfile,"#scan char %s\n",mid[i].op1);
			mips_scanchar(i,funcnum);
		}
		else if(mid[i].type==FUNCRET){
			//fprintf(mipsfile,"#%s = RET\n",mid[i].op1);
			mips_funcret(i,funcnum);								//t1=RET
		}
		else if(mid[i].type==SETLABEL){								//设置label
			fprintf(mipsfile,"%s:\n",mid[i].op1);					//label:
		}
		else if(mid[i].type==GOTOOP){
			fprintf(mipsfile,"\tj %s\n",mid[i].op1);				//j op1
		}
		else if(mid[i].type==BZOP){
			//fprintf(mipsfile,"#BZ %s\n",mid[i].op1);
			mips_bz(i,funcnum);										//为0，则跳转
		}
		else{
			continue;
		}
	}


}






int main(void){
	char* path=(char *)malloc(50*sizeof(char));
	int isoptimalize=0;
	printf("path of file:");			//输入测试程序的绝对路径
	scanf("%s",path);
	printf("is optimalize?(1/0)");
	scanf("%d",&isoptimalize);
	fp=fopen(path,"r");
	grammarfile=fopen("grammar.txt","w");		//语法分析结果输出在工程目录下grammar.txt
	if(fp==NULL){
		printf("can not open this file.");
	}
	midfile=fopen("midcode.txt","w");			//中间代码输出在工程目录下的midcode.txt
	if(midfile==NULL){
		printf("can not open this file.");
	}
	mipsfile=fopen("mips.txt","w");				//mips代码输出在工程目录下的mips.txt
	if(mipsfile==NULL){
		printf("can not open this file.");
	}
	errorfile=fopen("error.txt","w");			//错误信息输出在工程目录下的error.txt
	if(errorfile==NULL){
		printf("can not open this file.");
	}

	program();				//进行语法分析并生成中间代码
	if(!iserror){			//如果没有错误
		//printtab();			//打印最终的符号表
		if(isoptimalize){
			copyconv();			//复制传播、常数合并
		}
		writemidcode();			//将中间代码写入文件midcode.txt
		mips();					//产生mips代码
		if(isoptimalize){
			//窥孔优化,删除多余的lw
			deletelw();
		}
	}


	fclose(fp);
	fclose(grammarfile);
	fclose(midfile);
	fclose(mipsfile);
	fclose(errorfile);
	system("pause");
}
