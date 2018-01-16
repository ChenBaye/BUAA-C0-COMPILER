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
//һ������������ջ
//�洢�������͡�id��ֵ
struct data{
	char id[tokensize];		//id
	int addr;				//��ַ
	int type;				//����,����ű���ͬ
};
struct funcstack{	
	struct data data_area[tabsize];	//�洢�����еı���(����ʱ����)������
	int returnvalue;					//����ֵ								
	int returnaddr;						//���ص�ַ							
	int fp;						//��һ����������ջ��ջ��	
	
	int data_arealen;					//data����	
};		
//����������ջ�ṹ,������fp����

//+
//+-------------	<------sp		�͵�ַ
//+	data area
//+		...		
//+		...		
//+		...
//+		...
//+-------------	<------fp-12
//+	����ֵ(fp-12)
//+-------------	<------fp-8
//+	���ص�ַ(fp-8)
//+-------------	<------fp-4
//+	��һ��������fp��ֵ(fp-4)
//+-------------	<------fp����һ��������spֵ��		�ߵ�ַ			sp=fp+16+sizeof(data_area)
// +	(fp)
//+

//�洢ÿ�����������б���������
struct newtab{
	struct tabentry tab[tabsize];
	int tablen;
};

struct newtab functab[functabsize];		//ÿ�������ķ��ű�
int funcsize[functabsize]={0};			//ÿ��������size

struct data STRING[100];		//���100���ַ��� idΪ�ַ���ֵ,�±�Ϊ�ַ������
FILE* mipsfile;					//mips�����ļ�ָ��
int strnum=0;					//�����ַ����Ĵ���

struct tabentry PARA[parasize];		//�洢����type=0 ���֣�type=1 ȫ������type=2 �ֲ���
int paranum=0;					//��¼�м�������



//�����Ż���ɾ��mips�����ж����lw���
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
        fgets(buffer1,1024,mipsfile);  //��ȡһ��
		//printf("%s\n",buffer1);
		fprintf(tempfile,"%s",buffer1);//������tempfile

		if(buffer1[1]=='s' && buffer1[2]=='w'){		//����sw���
			strcpy(buffer2,buffer1);				//����sw���
			if(!feof(mipsfile)){							//�����ʱû��mips.txt�ļ���β
				fgets(buffer1,1024,mipsfile);				//��ȡ��һ�У�������lw
				char c=buffer1[1];
				buffer1[1]='s';						//��lw�ĳ�sw�����ǰ���ַ������
				if(strcmp(buffer1,buffer2)==0){

				}
				else{
					buffer1[1]=c;
					fprintf(tempfile,"%s",buffer1);		//������tempfile
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

	//��temp.txt������mips.txt
	memset(buffer1,0,1024);
	while (strcmp(buffer1,"Label_end:\n")!=0) 
	{ 
        fgets(buffer1,1024,tempfile);  //��ȡһ��
		fprintf(mipsfile,"%s",buffer1);
	}

	fclose(tempfile);
	remove("temp.txt");

}



//�������ű��в����м����
void addnewsym(char* id,int funcnum){
	int symnum=0;
	int i=0;
	for(i=0;i<functab[funcnum].tablen;i++){
		if(strcmp(id,functab[funcnum].tab[i].id)==0){	//�м�����Ѿ����
			return;
		}
	}
	//û�ҵ�����ĩβ����м�����ı���
	//������
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
	funcsize[funcnum]+=4;				//����size+4
	functab[funcnum].tablen++;
	return;
}

//��ӡ�������ű������
void printfunctab(){
	int i=0;
	int j=0;
	//��ӡ������
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


//�����ַ������
void getstrname(char* strname){
	if(strnum==100){
		printf("******************ERROR******************\n");
		printf("too much string!!!!!!!!!!!\n");
		exit(0);
	}
	sprintf(strname,"$string_%d",strnum);
	strnum++;
}


//��ʼ��ȫ�ֱ������������ַ������Լ�mips�����.data��
//����ÿ�������г��ֵ��м������������������ķ��ű�
void mips_init(){				
	int i=0;
	int j=0;
	int k=0;
	int index=0;
	int nextindex=0;
	int funcnum=0;
	fprintf(mipsfile,".data\n");

	//�������ű���ȫ�ֱ���(û�г���)����.data��,char��int��ΪΪ4�ֽ�
	for(i=0;i<globallen;i++){
		if(tab[i].type==0){		//int
			fprintf(mipsfile,"\t%s:.word %d\n",tab[i].id,tab[i].len);		//id:.word len
		}
		else if(tab[i].type==1){	//char
			fprintf(mipsfile,"\t%s:.word %d\n",tab[i].id,tab[i].len);		//id:.word len
		}
		else if(tab[i].type==4){	//int����
			fprintf(mipsfile,"\t%s:.word 0:%d\n",tab[i].id,tab[i].len);		//id:.word 0:len
		}
		else if(tab[i].type==5){						//char����
			fprintf(mipsfile,"\t%s:.word 0:%d\n",tab[i].id,tab[i].len);		//id:.word 0:len
		}
	}

	//�����м���룬
	//1.Ϊ�ַ�������ռ�
	//2.����ÿ��������size��Ϊ�����м��������ռ�
	for(i=0;i<midlen;i++){
		if(mid[i].type==PRINTSTROP){			//print string
			//���ַ�������Ŵ���string,�����±꼴Ϊ����
			strcpy(STRING[strnum].id,mid[i].op1);
			char strname[tokensize]={'\0'};
			getstrname(strname);	//strnum++
			fprintf(mipsfile,"\t%s:.asciiz \"%s\"\n",strname,mid[i].op1);	//str:.asciiz ""
			//����strnum���ַ���
		}
		else if(mid[i].type==INTFUNCDEF || mid[i].type==CHARFUNCDEF || mid[i].type==VOIDFUNCDEF){//��������
			//�ҵ������ڷ��ű������
			for(j=0;j<funclen;j++){
				if(strcmp(tab[funcindex[j]].id,mid[i].op1)==0){
					index=funcindex[j];
					funcnum=j;							//��j������
					if(j>=functabsize){
						//���ű����
						errormessage(OUTOFTAB_ERROR,line,no);
						return;
					}

					if(j+1<funclen){
						nextindex=funcindex[j+1];		//���滹�к���
					}
					else{
						nextindex=tablen;				//�����һ������ ��main
					}
					break;
				}
			}

			functab[funcnum].tablen=0;
			for(j=index+1,k=0;j<nextindex;j++,k++){
				//�������з���
				functab[funcnum].tab[k].type=tab[j].type;
				functab[funcnum].tab[k].addr=tab[j].addr;
				functab[funcnum].tab[k].len=tab[j].len;
				functab[funcnum].tab[k].lev=tab[j].lev;
				functab[funcnum].tab[k].size=tab[j].size;
				strcpy(functab[funcnum].tab[k].id,tab[j].id);
				funcsize[funcnum]=tab[index].size;				//��������size
				functab[funcnum].tablen++;
			}

		}
		//���¼����м�������ͣ�Ϊ�м������һ�γ��ִ�
		else if(mid[i].type==VARASSIGN || 
			   (mid[i].type>=1 && mid[i].type<=4) ||
				mid[i].type==ARRUSE ||
				mid[i].type==FUNCRET){
			
			if(mid[i].op1!=NULL && mid[i].op1[0]=='$'){			//�м����
				addnewsym(mid[i].op1,funcnum);					//�������ű��в����м����
			}

			if(mid[i].op2!=NULL && mid[i].op2[0]=='$'){
				addnewsym(mid[i].op2,funcnum);					//�������ű��в����м����
			}

			if(mid[i].result!=NULL && mid[i].result[0]=='$'){
				addnewsym(mid[i].result,funcnum);					//�������ű��в����м����
			}
		}
	}
	fprintf(mipsfile,".text\n");								//.text
	fprintf(mipsfile,"\tadd $fp $0 $sp\n");						//add $fp $0 $sp
	fprintf(mipsfile,"\tsubi $sp $sp 12\n");					//subi $sp $sp 12
	fprintf(mipsfile,"\tsubi $sp $sp %d\n",funcsize[funclen-1]);//subi $sp $sp sizeof(main)
	fprintf(mipsfile,"\tj main\n");								//j main
}


//����0���ֲ�����������1��ȫ�ֱ���
int getaddr(char* id,int isarr,int funcnum,char* addr){
	int i=0;
	//�ȱ�����ǰ������������, ��ǰ�����ķ��ű�Ϊfunctab[funcnum]
	for(i=0;i<functab[funcnum].tablen;i++){
		if(strcmp(id,functab[funcnum].tab[i].id)==0){		//������ͬ������
			if((isarr==1 && (functab[funcnum].tab[i].type==4 || functab[funcnum].tab[i].type==5)) 
				|| (isarr==0 && functab[funcnum].tab[i].type!=4 && functab[funcnum].tab[i].type!=5)){
				//�Ǿֲ���������ַ��ת���ַ���
				//addrֻ����Ժ�����ַ
				sprintf(addr,"%d",functab[funcnum].tab[i].addr);
				return 0;
			}
		}
	}

	//�ٱ���ȫ��ջ��(���ű��ȫ����)
	for(i=0;i<globallen;i++){
		if(strcmp(tab[i].id,id)==0){
			//��ʶ����ͬ��������ͬ
			if((isarr==1 && (tab[i].type==4 || tab[i].type==5))
				|| (isarr==0 && tab[i].type!=4 && tab[i].type!=5)){
				sprintf(addr,"%s",id);
				return 1;
			}
		}
	}
	
	//������ʱǰ���Ѿ����� 
	printf("************************************ERROR************************************\n");
	printf("in mips.c:the ID is not defined\n %s",id);
	
	sprintf(addr,"%s",id);
	return 0;
}



//Ϊ�ֲ�������ջ�ϸ���ֵ const int/char = x
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
	//���Ҿֲ�������ֵ
	for(i=0;i<functab[funcnum].tablen;i++){
		if(strcmp(mid[index].op1,functab[funcnum].tab[i].id)==0){
			fprintf(mipsfile,"\tli $t0 %d\n",functab[funcnum].tab[i].len);
			fprintf(mipsfile,"\tsw $t0 %s($sp)\n",addr);
			break;
		}
	}


}
*/


//�����ӷ�ָ������������һ����������
void mips_add(int index,int funcnum){		// result=op1+op2,op1��$t0,op2��$t1,result��$t0
	char op1_addr[200]={'\0'};
	char op2_addr[200]={'\0'};
	char result_addr[200]={'\0'};
	int ret=0;
	//������1Ϊ���ֻ��ʶ��
	if(isrealnum(mid[index].op1)){
		fprintf(mipsfile,"\tli $t0 %s\n",mid[index].op1);			//li $t0 op1
	}
	else{
		ret=getaddr(mid[index].op1,0,funcnum,op1_addr);						//��ѯ��ַ
		if(ret==1){//ȫ����
			fprintf(mipsfile,"\tlw $t0 %s\n",op1_addr);				//lw $t0 op1_addr
		}
		else{		//�ֲ���
			fprintf(mipsfile,"\tlw $t0 %s($sp)\n",op1_addr);		//lw $t0 op1_addr($sp)
		}
	}

	//������2Ϊ���ֻ��ʶ��
	if(isrealnum(mid[index].op2)){
		fprintf(mipsfile,"\tli $t1 %s\n",mid[index].op2);		//li $t1 op2
	}
	else{
		ret=getaddr(mid[index].op2,0,funcnum,op2_addr);
		if(ret==1){//ȫ����
			fprintf(mipsfile,"\tlw $t1 %s\n",op2_addr);				//lw $t1 op2_addr
		}
		else{		//�ֲ���
			fprintf(mipsfile,"\tlw $t1 %s($sp)\n",op2_addr);		//lw $t1 op2_addr($sp)
		}
	}
	
	fprintf(mipsfile,"\tadd $t0 $t0 $t1\n");						//add $t0 $t0 $t1
	ret=getaddr(mid[index].result,0,funcnum,result_addr);

	if(ret==1){//ȫ����
		fprintf(mipsfile,"\tsw $t0 %s\n",result_addr);				//sw $t0 result_addr
	}
	else{		//�ֲ���
		fprintf(mipsfile,"\tsw $t0 %s($sp)\n",result_addr);			//sw $t0 result_addr($sp)
	}
}

//��������ָ������������һ����������
void mips_sub(int index,int funcnum){		// result=op1-op2,op1��$t0,op2��$t1,result��$t0
	char op1_addr[200]={'\0'};
	char op2_addr[200]={'\0'};
	char result_addr[200]={'\0'};
	int ret=0;
	//������1Ϊ���ֻ��ʶ��
	if(isrealnum(mid[index].op1)){
		fprintf(mipsfile,"\tli $t0 %s\n",mid[index].op1);			//li $t0 op1
	}
	else{
		ret=getaddr(mid[index].op1,0,funcnum,op1_addr);						//��ѯ��ַ
		if(ret==1){//ȫ����
			fprintf(mipsfile,"\tlw $t0 %s\n",op1_addr);				//lw $t0 op1_addr
		}
		else{		//�ֲ���
			fprintf(mipsfile,"\tlw $t0 %s($sp)\n",op1_addr);		//lw $t0 op1_addr($sp)
		}
		
	}

	//������2Ϊ���ֻ��ʶ��
	if(isrealnum(mid[index].op2)){
		fprintf(mipsfile,"\tli $t1 %s\n",mid[index].op2);		//li $t1 op2
	}
	else{
		ret=getaddr(mid[index].op2,0,funcnum,op2_addr);
		if(ret==1){//ȫ����
			fprintf(mipsfile,"\tlw $t1 %s\n",op2_addr);				//lw $t1 op2_addr
		}
		else{		//�ֲ���
			fprintf(mipsfile,"\tlw $t1 %s($sp)\n",op2_addr);		//lw $t1 op2_addr($sp)
		}
	}
	
	fprintf(mipsfile,"\tsub $t0 $t0 $t1\n");						//sub $t0 $t0 $t1
	ret=getaddr(mid[index].result,0,funcnum,result_addr);

	if(ret==1){//ȫ����
		fprintf(mipsfile,"\tsw $t0 %s\n",result_addr);				//sw $t0 result_addr
	}
	else{		//�ֲ���
		fprintf(mipsfile,"\tsw $t0 %s($sp)\n",result_addr);			//sw $t0 result_addr($sp)
	}
}

//�����˷�ָ������������һ����������
void mips_mul(int index,int funcnum){		// result=op1*op2,op1��$t0,op2��$t1,result��$t0
	char op1_addr[200]={'\0'};
	char op2_addr[200]={'\0'};
	char result_addr[200]={'\0'};
	int ret=0;
	//������1Ϊ���ֻ��ʶ��
	if(isrealnum(mid[index].op1)){
		fprintf(mipsfile,"\tli $t0 %s\n",mid[index].op1);			//li $t0 op1
	}
	else{
		ret=getaddr(mid[index].op1,0,funcnum,op1_addr);						//��ѯ��ַ
		if(ret==1){//ȫ����
			fprintf(mipsfile,"\tlw $t0 %s\n",op1_addr);				//lw $t0 op1_addr
		}
		else{		//�ֲ���
			fprintf(mipsfile,"\tlw $t0 %s($sp)\n",op1_addr);		//lw $t0 op1_addr($sp)
		}
		
	}

	//������2Ϊ���ֻ��ʶ��
	if(isrealnum(mid[index].op2)){
		fprintf(mipsfile,"\tli $t1 %s\n",mid[index].op2);			//li $t1 op2
	}
	else{
		ret=getaddr(mid[index].op2,0,funcnum,op2_addr);
		if(ret==1){//ȫ����
			fprintf(mipsfile,"\tlw $t1 %s\n",op2_addr);				//lw $t1 op2_addr
		}
		else{		//�ֲ���
			fprintf(mipsfile,"\tlw $t1 %s($sp)\n",op2_addr);		//lw $t1 op2_addr($sp)
		}
	}
	
	fprintf(mipsfile,"\tmult $t0 $t1\n");							//mult $t0 $t1
	fprintf(mipsfile,"\tmflo $t0\n");								//mflo $t0
	ret=getaddr(mid[index].result,0,funcnum,result_addr);

	if(ret==1){//ȫ����
		fprintf(mipsfile,"\tsw $t0 %s\n",result_addr);				//sw $t0 result_addr
	}
	else{		//�ֲ���
		fprintf(mipsfile,"\tsw $t0 %s($sp)\n",result_addr);			//sw $t0 result_addr($sp)
	}
	
}

//��������ָ������������һ����������
void mips_div(int index,int funcnum){		// result=op1��op2,op1��$t0,op2��$t1,result��$t0
	char op1_addr[200]={'\0'};
	char op2_addr[200]={'\0'};
	char result_addr[200]={'\0'};
	int ret=0;
	//������1Ϊ���ֻ��ʶ��
	if(isrealnum(mid[index].op1)){
		fprintf(mipsfile,"\tli $t0 %s\n",mid[index].op1);			//li $t0 op1
	}
	else{
		ret=getaddr(mid[index].op1,0,funcnum,op1_addr);						//��ѯ��ַ
		if(ret==1){//ȫ����
			fprintf(mipsfile,"\tlw $t0 %s\n",op1_addr);				//lw $t0 op1_addr
		}
		else{		//�ֲ���
			fprintf(mipsfile,"\tlw $t0 %s($sp)\n",op1_addr);		//lw $t0 op1_addr($sp)
		}
		
	}

	//������2Ϊ���ֻ��ʶ��
	if(isrealnum(mid[index].op2)){
		fprintf(mipsfile,"\tli $t1 %s\n",mid[index].op2);			//li $t1 op2
	}
	else{
		ret=getaddr(mid[index].op2,0,funcnum,op2_addr);
		if(ret==1){//ȫ����
			fprintf(mipsfile,"\tlw $t1 %s\n",op2_addr);				//lw $t1 op2_addr
		}
		else{		//�ֲ���
			fprintf(mipsfile,"\tlw $t1 %s($sp)\n",op2_addr);		//lw $t1 op2_addr($sp)
		}
	}
	
	fprintf(mipsfile,"\tdiv $t0 $t1\n");							//div $t0 $t1
	fprintf(mipsfile,"\tmflo $t0\n");								//mflo $t0
	ret=getaddr(mid[index].result,0,funcnum,result_addr);

	if(ret==1){//ȫ����
		fprintf(mipsfile,"\tsw $t0 %s\n",result_addr);				//sw $t0 result_addr
	}
	else{		//�ֲ���
		fprintf(mipsfile,"\tsw $t0 %s($sp)\n",result_addr);			//sw $t0 result_addr($sp)
	}
}


//��������
void mips_funcdef(int index,int funcnum){				//���ú���label
	fprintf(mipsfile,"%s:\n",mid[index].op1);			//function:
	if(strcmp(mid[index].op1,"main")==0){				//main������Ҫ���淵�ص�ַ
		return;
	}
	fprintf(mipsfile,"\tsw $ra -8($fp)\n");			//sw $ra -8($fp)���淵�ص�ַ
	fprintf(mipsfile,"\tsw $0 -12($fp)\n");			//sw 0 -12($fp)������ֵ��Ϊ0

}														


//���ú���funcnum
void mips_funccall(int index,int funcnum){
	int i=0;
	int j=0;
	fprintf(mipsfile,"\tsw $fp -4($sp)\n");	//sw $fp -4($sp)������һ��������$fp

	//��fp�����µ�ջ����new_fp=old_sp
	fprintf(mipsfile,"\tmove $fp $sp\n");	//move $fp $sp

	//��������$sp�����µ�ջ�� new_sp=old_sp-12-sizeof(func)
	fprintf(mipsfile,"\tsubi $sp $sp 12\n");					//subi $sp $sp 12
	fprintf(mipsfile,"\tsubi $sp $sp %d\n",funcsize[funcnum]);	//subi $sp $sp sizeof(func)


	//�������
	//fprintf(mipsfile,"#function para\n");
	//�ӵ�����len����ʼ
	for(i=paranum-tab[funcindex[funcnum]].len,j=0;i<paranum && j<tab[funcindex[funcnum]].len;i++,j++){
		if(PARA[i].type==0){			//����������
			fprintf(mipsfile,"\tli $t0 %s\n",PARA[i].id);	//li $t0 para
		}
		else if(PARA[i].type==1){		//��ȫ����
			fprintf(mipsfile,"\tlw $t0 %s\n",PARA[i].id);	//lw $t0 addr
		}
		else{
			fprintf(mipsfile,"\tlw $t0 %s($fp)\n",PARA[i].id);//lw $t0 addr($fp)���ڵ�fp�ǵ�ʱ��sp
		}
		fprintf(mipsfile,"\tsw $t0 %d($sp)\n",j*4);			//sw $t0 j*4($sp)
		memset(PARA[i].id,0,tokensize);						//��ջ
	}
	paranum=paranum-tab[funcindex[funcnum]].len;
	//fprintf(mipsfile,"#function para end\n");
	fprintf(mipsfile,"\tjal %s\n",mid[index].op1);				//jal function
}


//��������(push para)
void mips_pushpara(int index,int funcnum){
	int  temp=0;
	char addr[200]={'\0'};
	memset(addr,0,200);

	if(paranum>=parasize){
		printf("*******************************too much para*******************************");
		exit(0);
	}

	//��������Ǹ�����,ֱ����ջ
	if(isrealnum(mid[index].op1)){
		PARA[paranum].type=0;					//������
		strcpy(PARA[paranum].id,mid[index].op1);//��������
		paranum++;
	}
	//������ҵ�ַ,����ջ,�����������������Ա
	else{
		temp=getaddr(mid[index].op1,0,funcnum,addr);
		if(temp==0){		//�ֲ�����,����λ�ں������ű�Ŀ�ͷ
							//�ڵ��ú����󣬴����������˱����ú�����fp��֮ǰ��sp
			PARA[paranum].type=2;
		}
		else{				//ȫ�ֱ��������ּ��ǵ�ַ
			PARA[paranum].type=1;
		}
		strcpy(PARA[paranum].id,addr);		//������ַ
		paranum++;
	}
	
}


//return value��return value
void mips_retvalue(int index,int funcnum){
	int temp=0;
	char addr[200]={'\0'};
	if(mid[index].type==RETNULLOP){			//����NULL

	}
	else{									//����value	
		//����ֵ��һ������
		if(isrealnum(mid[index].op1)){
			fprintf(mipsfile,"\tli $t0 %s\n",mid[index].op1);		//li $t0 op1
			fprintf(mipsfile,"\tsw $t0 -12($fp)\n");			//sw $t0 -12($fp) ����ֵ��ַΪfp-12
		}
		else{
			temp=getaddr(mid[index].op1,0,funcnum,addr);
			if(temp==1){//ȫ��������ʶ����Ϊ��ַ
				fprintf(mipsfile,"\tlw $t0 %s\n",addr);		//lw $t0 addr
				fprintf(mipsfile,"\tsw $t0 -12($fp)\n");	//sw $t0 -12($fp)
			}
			else{		//�ֲ���,��Ҫ����sp�Ĵ�����ֵ
				fprintf(mipsfile,"\tlw $t0 %s($sp)\n",addr);//lw $t0 addr($sp)
				fprintf(mipsfile,"\tsw $t0 -12($fp)\n");	//sw $t0 -12($fp)
			}
		}
	}

	//�����ʱ��main����
	if(funcnum==funclen-1){
		fprintf(mipsfile,"\tj Label_end\n");		//ֱ������������β��label
	}
	else{
		fprintf(mipsfile,"\tmove $sp $fp\n");		//move $sp $fp		sp=fp
		fprintf(mipsfile,"\tlw $ra -8($fp)\n");		//lw $ra -8($fp)	��ջ��ȡ���ص�ַ��$ra
		fprintf(mipsfile,"\tlw $fp -4($fp)\n");		//lw $fp -4($fp)	��ջ��ȡ��һ��������fp
		fprintf(mipsfile,"\tjr $ra\n");				//jr $ra			��ת�����ص�ַ
	}
}


//������ֵ���op1=op2
void mips_varassign(int index,int funcnum){
	char addr1[200]={'\0'};
	char addr2[200]={'\0'};
	int temp=0;
	if(isrealnum(mid[index].op2)){		//op2������
		fprintf(mipsfile,"\tli $t0 %s\n",mid[index].op2);	//li $t0 op2
	}
	else{
		temp=getaddr(mid[index].op2,0,funcnum,addr1);
		if(temp==0){		//op2�ֲ�����
			fprintf(mipsfile,"\tlw $t0 %s($sp)\n",addr1);	//lw $t0 addr1($sp)
		}
		else{				//op2ȫ�ֱ���
			fprintf(mipsfile,"\tlw $t0 %s\n",addr1);		//lw $t0 addr1
		}
	}

	temp=getaddr(mid[index].op1,0,funcnum,addr2);
	if(temp==0){//op1�ֲ�����
		fprintf(mipsfile,"\tsw $t0 %s($sp)\n",addr2);	//sw $t0 addr2($sp)
	}
	else{		//op1ȫ�ֱ���
		fprintf(mipsfile,"\tsw $t0 %s\n",addr2);		//sw $t0 addr2
	}
}


//Ϊ���鸳ֵop1[op2] = result
void mips_arrassign(int index,int funcnum){
	char addr1[200]={'\0'};
	char addr2[200]={'\0'};
	char res_addr[200]={'\0'};
	int temp=0;
	//op1->$t0
	//ע�⣬�˴���laָ���Ϊ��Ҫ����op1�ĵ�ַ,����op1��ַ�е�ֵ
	temp=getaddr(mid[index].op1,1,funcnum,addr1);
	if(temp==0){//�ֲ�����
		fprintf(mipsfile,"\tla $t0 %s($sp)\n",addr1);		//la $t0 addr1($sp)
	}
	else{		//ȫ�ֱ���
		fprintf(mipsfile,"\tla $t0 %s\n",addr1);			//la $t0 addr1
	}

	//op2->$t1
	if(isrealnum(mid[index].op2)){		//op2������
		fprintf(mipsfile,"\tli $t1 %s\n",mid[index].op2);	//li $t1 op2
	}
	else{
		temp=getaddr(mid[index].op2,0,funcnum,addr2);
		if(temp==0){		//op2�ֲ�����
			fprintf(mipsfile,"\tlw $t1 %s($sp)\n",addr2);	//lw $t1 addr2($sp)
		}
		else{				//op2ȫ�ֱ���
			fprintf(mipsfile,"\tlw $t1 %s\n",addr2);		//lw $t1 addr2
		}
	}
	//���� op2*4
	fprintf(mipsfile,"\tli $t2 4\n");				//li $t2 4
	fprintf(mipsfile,"\tmul $t1 $t1 $t2\n");		//mul $t1 $t1 $t2

	//result->$t2
	if(isrealnum(mid[index].result)){		//result������
		fprintf(mipsfile,"\tli $t2 %s\n",mid[index].result);	//li $t2 result
	}
	else{
		temp=getaddr(mid[index].result,0,funcnum,res_addr);
		if(temp==0){		//result�ֲ�����
			fprintf(mipsfile,"\tlw $t2 %s($sp)\n",res_addr);	//lw $t2 res_addr($sp)
		}
		else{				//resultȫ�ֱ���
			fprintf(mipsfile,"\tlw $t2 %s\n",res_addr);			//lw $t2 res_addr
		}
	}

	//op1[op2]=result	
	//�ȼ���op1[op2]��ַ=op1+4*op2
	//					=$t0 + $t1
	fprintf(mipsfile,"\tadd $t1 $t1 $t0\n");			//add $t1 $t1 $t0
	fprintf(mipsfile,"\tsw $t2 0($t1)\n");				//sw $t2 0($t1)

}


//����ֵ�������������result = op1[op2]
void mips_arruse(int index,int funcnum){
	char addr1[200]={'\0'};
	char addr2[200]={'\0'};
	char res_addr[200]={'\0'};
	int temp=0;
	//op1->$t0
	//ע�⣬�˴���laָ���Ϊ��Ҫ����op1�ĵ�ַ,����op1��ַ�е�ֵ
	temp=getaddr(mid[index].op1,1,funcnum,addr1);
	if(temp==0){//�ֲ�����
		fprintf(mipsfile,"\tla $t0 %s($sp)\n",addr1);		//la $t0 addr1($sp)
	}
	else{		//ȫ�ֱ���
		fprintf(mipsfile,"\tla $t0 %s\n",addr1);			//la $t0 addr1
	}

	//op2->$t1
	if(isrealnum(mid[index].op2)){		//op2������
		fprintf(mipsfile,"\tli $t1 %s\n",mid[index].op2);	//li $t1 op2
	}
	else{
		temp=getaddr(mid[index].op2,0,funcnum,addr2);
		if(temp==0){		//op2�ֲ�����
			fprintf(mipsfile,"\tlw $t1 %s($sp)\n",addr2);	//lw $t1 addr2($sp)
		}
		else{				//op2ȫ�ֱ���
			fprintf(mipsfile,"\tlw $t1 %s\n",addr2);		//lw $t1 addr2
		}
	}
	//���� op2*4
	fprintf(mipsfile,"\tli $t2 4\n");				//li $t2 4
	fprintf(mipsfile,"\tmul $t1 $t1 $t2\n");		//mul $t1 $t1 $t2
	

	//result=op1[op2]	
	//�ȼ���op1[op2]��ַ����$t1
	fprintf(mipsfile,"\tadd $t1 $t1 $t0\n");			//add $t1 $t1 $t0
	//��ȡop1[op2]��ֵ,����$t0
	fprintf(mipsfile,"\tlw $t0 0($t1)\n");				//lw $t0 0($t1)

	
	temp=getaddr(mid[index].result,0,funcnum,res_addr);
	if(temp==0){		//result�ֲ�����
		fprintf(mipsfile,"\tsw $t0 %s($sp)\n",res_addr);	//sw $t0 res_addr($sp)
	}
	else{				//resultȫ�ֱ���
		fprintf(mipsfile,"\tsw $t0 %s\n",res_addr);			//sw $t0 res_addr
	}

}


//��ӡ�ַ��� print str
void mips_printstr(int index){
	int i=0;
	char strname[200]={'\0'};	//.data�ε��ַ�������
	for(i=0;i<strnum;i++){
		if(strcmp(STRING[i].id,mid[index].op1)==0){
			fprintf(mipsfile,"\tli $v0 4\n");			//��$v0=4
			fprintf(mipsfile,"\tla $a0 $string_%d\n",i);//��$a0=$string_i
			fprintf(mipsfile,"\tsyscall\n");			//syscall
			break;
		}
	}
}


//��ӡ���� print integer
void mips_printint(int index,int funcnum){
	int temp=0;
	char addr[200]={'\0'};
	fprintf(mipsfile,"\tli $v0 1\n");			//��$v0=1
	
	if(isrealnum(mid[index].op1)){				//op1������
		fprintf(mipsfile,"\tli $a0 %s\n",mid[index].op1);		//li $a0 op1
	}
	else{
		temp=getaddr(mid[index].op1,0,funcnum,addr);				//��ѯ��ַ
		if(temp==1){//ȫ����
			fprintf(mipsfile,"\tlw $a0 %s\n",addr);				//lw $a0 addr
		}
		else{		//�ֲ���
			fprintf(mipsfile,"\tlw $a0 %s($sp)\n",addr);		//lw $a0 addr($sp)
		}
	}
	fprintf(mipsfile,"\tsyscall\n");			//syscall
}


//��ӡ�ַ� print char
void mips_printchar(int index,int funcnum){
	int temp=0;
	char addr[200]={'\0'};
	fprintf(mipsfile,"\tli $v0 11\n");			//��$v0=11
	
	if(isrealnum(mid[index].op1)){				//op1������
		fprintf(mipsfile,"\tli $a0 %s\n",mid[index].op1);		//li $a0 op1
	}
	else{
		temp=getaddr(mid[index].op1,0,funcnum,addr);				//��ѯ��ַ
		if(temp==1){//ȫ����
			fprintf(mipsfile,"\tlw $a0 %s\n",addr);				//lw $a0 addr
		}
		else{		//�ֲ���
			fprintf(mipsfile,"\tlw $a0 %s($sp)\n",addr);		//lw $a0 addr($sp)
		}
	}
	fprintf(mipsfile,"\tsyscall\n");							//syscall
}


//��ȡ scan a(a��������) 
//aΪ int
void mips_scanint(int index,int funcnum){
	int temp=0;
	char addr[200]={'\0'};
	fprintf(mipsfile,"\tli $v0 5\n");			//��$v0=5(read integer) 
	fprintf(mipsfile,"\tsyscall\n");			//syscall
	
	temp=getaddr(mid[index].op1,0,funcnum,addr);				//��ѯ��ַ
	if(temp==1){//ȫ����
		fprintf(mipsfile,"\tsw $v0 %s\n",addr);				//sw $v0 addr
	}
	else{		//�ֲ���
		fprintf(mipsfile,"\tsw $v0 %s($sp)\n",addr);		//sw $v0 addr($sp)
	}
}



//��ȡ scan a(a��������) 
//aΪ char
void mips_scanchar(int index,int funcnum){
	int temp=0;
	char addr[200]={'\0'};
	fprintf(mipsfile,"\tli $v0 12\n");			//��$v0=12(read character)
	fprintf(mipsfile,"\tsyscall\n");			//syscall
	
	temp=getaddr(mid[index].op1,0,funcnum,addr);				//��ѯ��ַ
	if(temp==1){//ȫ����
		fprintf(mipsfile,"\tsw $v0 %s\n",addr);				//sw $v0 addr
	}
	else{		//�ֲ���
		fprintf(mipsfile,"\tsw $v0 %s($sp)\n",addr);		//sw $v0 addr($sp)
	}
}


//op1=RET �������ú����ķ���ֵ��������
void mips_funcret(int index,int funcnum){
	int temp=0;
	char addr[200]={'\0'};
	fprintf(mipsfile,"\tlw $t0 -12($sp)\n");			//lw $t0 -12($sp)	��ǰsp��Ϊ�����ú�����fp
	temp=getaddr(mid[index].op1,0,funcnum,addr);				//��ѯ��ַ
	if(temp==1){//ȫ����
		fprintf(mipsfile,"\tsw $t0 %s\n",addr);				//sw $t0 addr
	}
	else{		//�ֲ���
		fprintf(mipsfile,"\tsw $t0 %s($sp)\n",addr);		//sw $t0 addr($sp)
	}
}



//��һ�����ʽΪ0(false)����ת
void mips_bz(int index,int funcnum){
	char op1_addr[200]={'\0'};
	char op2_addr[200]={'\0'};
	char result_addr[200]={'\0'};
	int ret=0;
	//����һ���м���봦��


	//�� �������� + - * /
	if(mid[index-1].type==ARRUSE || (mid[index-1].type>=1 && mid[index-1].type<=4)){
		ret=getaddr(mid[index-1].result,0,funcnum,result_addr);
		if(ret==1){//ȫ����
			fprintf(mipsfile,"\tlw $t0 %s\n",result_addr);				//lw $t0 result_addr
		}
		else{		//�ֲ���
			fprintf(mipsfile,"\tlw $t0 %s($sp)\n",result_addr);			//lw $t0 result_addr($sp)
		}
	}
	//�Ǻ�������ֵ op1=RET
	else if(mid[index-1].type==FUNCRET){
		ret=getaddr(mid[index-1].op1,0,funcnum,op1_addr);			//��ѯ��ַ
		if(ret==1){//ȫ����
			fprintf(mipsfile,"\tlw $t0 %s\n",op1_addr);				//lw $t0 op1_addr
		}
		else{		//�ֲ���
			fprintf(mipsfile,"\tlw $t0 %s($sp)\n",op1_addr);		//lw $t0 op1_addr($sp)
		}
	}
	// > < == != >= <=
	else{
		//������1Ϊ���ֻ��ʶ��
		if(isrealnum(mid[index-1].op1)){
			fprintf(mipsfile,"\tli $t0 %s\n",mid[index-1].op1);			//li $t0 op1
		}
		else{
			ret=getaddr(mid[index-1].op1,0,funcnum,op1_addr);			//��ѯ��ַ
			if(ret==1){//ȫ����
				fprintf(mipsfile,"\tlw $t0 %s\n",op1_addr);				//lw $t0 op1_addr
			}
			else{		//�ֲ���
				fprintf(mipsfile,"\tlw $t0 %s($sp)\n",op1_addr);		//lw $t0 op1_addr($sp)
			}
		}

		//������2Ϊ���ֻ��ʶ��,
		if(isrealnum(mid[index-1].op2)){
			fprintf(mipsfile,"\tli $t1 %s\n",mid[index-1].op2);		//li $t1 op2
		}
		else{
			ret=getaddr(mid[index-1].op2,0,funcnum,op2_addr);
			if(ret==1){//ȫ����
				fprintf(mipsfile,"\tlw $t1 %s\n",op2_addr);				//lw $t1 op2_addr
			}
			else{		//�ֲ���
				fprintf(mipsfile,"\tlw $t1 %s($sp)\n",op2_addr);		//lw $t1 op2_addr($sp)
			}
		}
	}
	

	//��һ��ʽ���Ǻ�������ֵ op1=RET
	if(mid[index-1].type==FUNCRET){
		fprintf(mipsfile,"\tbeqz $t0 %s\n",mid[index].op1);						//beqz op1	
	}
	//��һ��ʽ����+ - * /
	else if(mid[index-1].type>=1 && mid[index-1].type<=4){
		fprintf(mipsfile,"\tbeqz $t0 %s\n",mid[index].op1);						//beqz op1	
	}
	//��һ��ʽ���Ǳ�����ֵ op1=op2
	else if(mid[index-1].type==VARASSIGN){								//branch if equal zero
		fprintf(mipsfile,"\tbeqz $t0 %s\n",mid[index].op1);						//beqz op1		
	}
	//��һ��ʽ��������ʹ�� result = op1[op2]
	else if(mid[index-1].type==ARRUSE){
		fprintf(mipsfile,"\tbeqz $t0 %s\n",mid[index].op1);						//beqz op1
	}
	//��һ��ʽ����>= op1>=op2
	else if(mid[index-1].type==BIGEQLOP){							//branch if less than
		fprintf(mipsfile,"\tblt $t0 $t1 %s\n",mid[index].op1);				//blt $t0 $t1 op1
	}
	//��һ��ʽ����<= op1<=op2
	else if(mid[index-1].type==SMALLEQLOP){							//branch if greater than			
		fprintf(mipsfile,"\tbgt $t0 $t1 %s\n",mid[index].op1);				//bgt $t0 $t1 op1
	}
	//��һ��ʽ����== op1==op2
	else if(mid[index-1].type==EQUALOP){							//branch if no equal
		fprintf(mipsfile,"\tbne $t0 $t1 %s\n",mid[index].op1);				//bne $t0 $t1 op1
	}
	//��һ��ʽ����!= op1!=op2
	else if(mid[index-1].type==NOTEQLOP){							//branch if equal
		fprintf(mipsfile,"\tbeq $t0 $t1 %s\n",mid[index].op1);				//beq $t0 $t1 op1
	}
	//��һ��ʽ����> op1>op2
	else if(mid[index-1].type==BIGOP){								//branch if less of equal
		fprintf(mipsfile,"\tble $t0 $t1 %s\n",mid[index].op1);				//ble $t0 $t1 op1
	}
	//��һ��ʽ����< op1<op2
	else if(mid[index-1].type==SMALLOP){							//branch if greater of equal
		fprintf(mipsfile,"\tbge $t0 $t1 %s\n",mid[index].op1);				//bge $t0 $t1 op1
	}
	else{
		//�����ܵ�����
		printf("bz error\n %d",index);
	}
}


void mips(){
	int i=0;
	int funcnum=-1;			

	mips_init();			//����.data��,��亯�����ű�

	//printfunctab();			//��ӡ���ű�
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
			//������Ծֲ���������Ϊȫ�ֳ��������Ѿ���.data�η����˿ռ�
			//�ֲ�������ʼ��
			//mips_localconst(i,funcnum);
		}
		else if(mid[i].type==INTFUNCDEF //��������
			|| mid[i].type==CHARFUNCDEF 
			|| mid[i].type==VOIDFUNCDEF){
			funcnum++;
			mips_funcdef(i,funcnum);
		}
		else if(mid[i].type==FUNCCALL){	//��������
			//fprintf(mipsfile,"#call %s\n",mid[i].op1);
			int temp=0;
			int j=0;
			for(j=0;j<funclen;j++){
				if(strcmp(mid[i].op1,tab[funcindex[j]].id)==0){
					temp=j;
					break;
				}
			}
			//tempΪ�����ú���������
			mips_funccall(i,temp);
		}
		else if(mid[i].type==PARAOP){	//��������
			//fprintf(mipsfile,"#push %s\n",mid[i].op1);
			mips_pushpara(i,funcnum);
		}
		else if(mid[i].type==RETEXPOP || mid[i].type==RETNULLOP){	//return (a); �� return NULL
			//fprintf(mipsfile,"#return value\n");
			mips_retvalue(i,funcnum);
		}
		else if(mid[i].type==VARASSIGN){							//Ϊ������ֵop1 = op2
			//fprintf(mipsfile,"#%s = %s\n",mid[i].op1,mid[i].op2);
			mips_varassign(i,funcnum);
		}
		else if(mid[i].type==ARRASSIGN){							//Ϊ���鸳ֵop1[op2] = result
			//fprintf(mipsfile,"#%s[%s] = %s\n",mid[i].op1,mid[i].op2,mid[i].result);	
			mips_arrassign(i,funcnum);
		}
		else if(mid[i].type==ARRUSE){								//���鸳�����������result = op1[op2]
			//fprintf(mipsfile,"#%s = %s[%s]\n",mid[i].result,mid[i].op1,mid[i].op2);
			mips_arruse(i,funcnum);
		}
		else if(mid[i].type==PRINTSTROP){							//��ӡ�ַ��� print string
			//fprintf(mipsfile,"#print %s\n",mid[i].op1);	
			mips_printstr(i);
			//������Ϊ����
			//fprintf(mipsfile,"\tli $v0 11\n");
			//fprintf(mipsfile,"\tli $a0 10\n");
			//fprintf(mipsfile,"\tsyscall\n");
		}
		else if(mid[i].type==PRINTINTOP){							//��ӡ���� print integer
			//fprintf(mipsfile,"#print %s\n",mid[i].op1);
			mips_printint(i,funcnum);
			//������Ϊ����
			//fprintf(mipsfile,"\tli $v0 11\n");
			//fprintf(mipsfile,"\tli $a0 10\n");
			//fprintf(mipsfile,"\tsyscall\n");
		}
		else if(mid[i].type==PRINTCHAROP){							//��ӡ�ַ� print char
			//fprintf(mipsfile,"#print %s\n",mid[i].op1);
			mips_printchar(i,funcnum);
			//������Ϊ����
			//fprintf(mipsfile,"\tli $v0 11\n");
			//fprintf(mipsfile,"\tli $a0 10\n");
			//fprintf(mipsfile,"\tsyscall\n");
		}
		else if(mid[i].type==PRINTLINE){							//����
			fprintf(mipsfile,"\tli $v0 11\n");
			fprintf(mipsfile,"\tli $a0 10\n");
			fprintf(mipsfile,"\tsyscall\n");
		}
		else if(mid[i].type==SCANINTOP){							//��ȡ scan a(int)
			//fprintf(mipsfile,"#scan int %s\n",mid[i].op1);
			mips_scanint(i,funcnum);
		}
		else if(mid[i].type==SCANCHAROP){							//��ȡ scan a(char)
			//fprintf(mipsfile,"#scan char %s\n",mid[i].op1);
			mips_scanchar(i,funcnum);
		}
		else if(mid[i].type==FUNCRET){
			//fprintf(mipsfile,"#%s = RET\n",mid[i].op1);
			mips_funcret(i,funcnum);								//t1=RET
		}
		else if(mid[i].type==SETLABEL){								//����label
			fprintf(mipsfile,"%s:\n",mid[i].op1);					//label:
		}
		else if(mid[i].type==GOTOOP){
			fprintf(mipsfile,"\tj %s\n",mid[i].op1);				//j op1
		}
		else if(mid[i].type==BZOP){
			//fprintf(mipsfile,"#BZ %s\n",mid[i].op1);
			mips_bz(i,funcnum);										//Ϊ0������ת
		}
		else{
			continue;
		}
	}


}






int main(void){
	char* path=(char *)malloc(50*sizeof(char));
	int isoptimalize=0;
	printf("path of file:");			//������Գ���ľ���·��
	scanf("%s",path);
	printf("is optimalize?(1/0)");
	scanf("%d",&isoptimalize);
	fp=fopen(path,"r");
	grammarfile=fopen("grammar.txt","w");		//�﷨�����������ڹ���Ŀ¼��grammar.txt
	if(fp==NULL){
		printf("can not open this file.");
	}
	midfile=fopen("midcode.txt","w");			//�м��������ڹ���Ŀ¼�µ�midcode.txt
	if(midfile==NULL){
		printf("can not open this file.");
	}
	mipsfile=fopen("mips.txt","w");				//mips��������ڹ���Ŀ¼�µ�mips.txt
	if(mipsfile==NULL){
		printf("can not open this file.");
	}
	errorfile=fopen("error.txt","w");			//������Ϣ����ڹ���Ŀ¼�µ�error.txt
	if(errorfile==NULL){
		printf("can not open this file.");
	}

	program();				//�����﷨�����������м����
	if(!iserror){			//���û�д���
		//printtab();			//��ӡ���յķ��ű�
		if(isoptimalize){
			copyconv();			//���ƴ����������ϲ�
		}
		writemidcode();			//���м����д���ļ�midcode.txt
		mips();					//����mips����
		if(isoptimalize){
			//�����Ż�,ɾ�������lw
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
