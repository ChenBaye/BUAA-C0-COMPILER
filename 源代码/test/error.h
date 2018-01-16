#ifndef _ERROR_H_
#define _ERROR_H_
#include<stdio.h> 
#include<ctype.h>
#include<string.h>
#include<stdlib.h>

//*******************************************
//错误类型编码
//*******************************************
#define ZEROSTARTNUM_ERROR	1	//数字有前零
#define ILEGALCH_ERROR		2	//非法字符
#define SINGLEQ_ERROR		3	//缺少单引号
#define DOUBLEQ_ERROR		4	//缺少双引号
#define ASSIGN_ERROR		5	//缺少赋值号
#define ID_ERROR			6	//缺少标识符
#define LPAR_ERROR			7	//缺少(
#define RPAR_ERROR			8	//缺少)
#define LBRACE_ERROR		9	//缺少{
#define RBRACE_ERROR		10	//缺少}
#define LBRACKET_ERROR		11	//缺少[
#define RBRACKET_ERROR		12	//缺少]
#define FUNCTYPE_ERROR		13	//缺少函数类型标识符(int char void)
#define STA_ERROR			14	//不是语句
#define	SEMICOLON_ERROR		15	//缺少;
#define FACTOR_ERROR		16	//不是因子
#define ARRLEN_ERROR		17	//数组长度定义时错误
#define PLUSZERO_ERROR		18	//0前有+ -
#define INTEGER_ERROR		19	//缺少整数
#define CHAR_ERROR			20	//缺少字符
#define TYPE_ERROR			21	//缺少类型标识符(int char)
#define COLON_ERROR			22	//缺少:
#define CONSTVALUE_ERROR	23	//缺少常量
#define CASE_ERROR			24	//缺少case
#define ADDSUB_ERROR		25	//缺少+ -
#define STEP_ERROR			26	//步长错误
#define REOP_ERROR			27	//缺少关系运算符(< > <= >= == !=)
#define OUTOFBOUND_ERROR	28	//数组越界
#define VARDEF_ERROR		29	//变量定义位置错误
#define DEFAULT_ERROR		30	//缺少default
#define OUTOFTAB_ERROR		31	//符号表溢出
#define GLOBALNAME_ERROR	32	//全局量、函数与全局量、函数不可重名
#define LOCALNAME_ERROR		33	//局部常、变量重名	
#define UNDEFID_ERROR		34	//未定义标识符
#define VARIABLE_ERROR		35	//应为char int变量
#define NOTARR_ERROR		36	//应该数组的地方使用了非数组变量
#define HASRETFUNC_ERROR	37	//应为有返回值函数
#define DIFFVAR_ERROR		38	//循环控制变量前后不一
#define PARACOUNT_ERROR		39	//参数数目错误
#define TYPECONFL_ERROR		40	//类型不一致

FILE* errorfile;				//接收错误信息的文件

int iserror=0;


void errormessage(int error,int line,int no){	//输出错误信息
	iserror=1;
	printf("************************************ERROR************************************\n");
	char message[200]={'\0'};
	switch(error){
	case ZEROSTARTNUM_ERROR://数字有前零 1
		sprintf(message,"(WARNING)\tline:%d char:%d a number start with zero\n",line,no);
		break;
	case ILEGALCH_ERROR://非法字符 2
		sprintf(message,"(ERROR)\tline:%d char:%d an ilegal char\n",line,no);
		break;
	case SINGLEQ_ERROR://缺少单引号 3
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'\'\'\n",line,no);
		//在getsym.h中进行回退
		break;
	case DOUBLEQ_ERROR://缺少双引号 4
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'\"\'\n",line,no);
		//在getsym.h中进行回退
		break;
	case ASSIGN_ERROR://缺少赋值号 5
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'=\'\n",line,no);
		break;
	case ID_ERROR://缺少标识符 6
		sprintf(message,"(ERROR)\tline:%d char:%d lost an id\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case LPAR_ERROR://缺少( 7
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'(\'\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case RPAR_ERROR://缺少) 8
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \')\'\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case LBRACE_ERROR://缺少{ 9
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'{\'\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case RBRACE_ERROR://缺少} 10
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'}\'\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case LBRACKET_ERROR://缺少[ 11
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'[\'\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case RBRACKET_ERROR://缺少] 12
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \']\'\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case FUNCTYPE_ERROR://缺少函数类型标识符(int char void) 13
		sprintf(message,"(ERROR)\tline:%d char:%d lost int/char/void at the head of a function\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case STA_ERROR://读到的符号不符合任何<语句>的开始符号 14
		sprintf(message,"(ERROR)\tline:%d char:%d read an ilegal symbol at the head of a sta\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case SEMICOLON_ERROR://缺少; 15
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \';\'\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case FACTOR_ERROR://读到的符号不符合任何因子的开始符号 16
		sprintf(message,"(ERROR)\tline:%d char:%d read an ilegal symbol at the head of a factor\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case ARRLEN_ERROR://数组长度定义时错误 17
		sprintf(message,"(ERROR)\tline:%d char:%d the lenth of an array must be a no-symbol-integer(>0) \n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case PLUSZERO_ERROR://0前有+ - 18
		sprintf(message,"(ERROR)\tline:%d char:%d zero can not start with \'+\'/\'-\' \n",line,no);
		////skip(SEMICOLON);//跳读到;
		break;
	case INTEGER_ERROR://缺少整数 19
		sprintf(message,"(ERROR)\tline:%d char:%d lost an integer\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case CHAR_ERROR://缺少字符 20
		sprintf(message,"(ERROR)\tline:%d char:%d lost a char\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case TYPE_ERROR://缺少类型标识符(int char) 21
		sprintf(message,"(ERROR)\tline:%d char:%d lost an \"int\" or \"char\"\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case COLON_ERROR://缺少: 22
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \';\'\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case CONSTVALUE_ERROR://缺少常量 23
		sprintf(message,"(ERROR)\tline:%d char:%d lost a const-value(an integer or a char)\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case CASE_ERROR://缺少case 24
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \"case\"\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case ADDSUB_ERROR://缺少+ - 25
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'+\'/\'-\'\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case STEP_ERROR://步长错误 26
		sprintf(message,"(ERROR)\tline:%d char:%d step must be an no-symbol-integer(>0)\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case REOP_ERROR://缺少关系运算符(< > <= >= == !=) 27
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'<\'/\'>\'/\"<=\"/\">=\"/\"==\"/\"!=\"\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case OUTOFBOUND_ERROR://数组越界 28
		sprintf(message,"(ERROR)\tline:%d char:%d the index is out of the bound of array\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case VARDEF_ERROR://变量定义位置错误 29
		sprintf(message,"(WARNING)\tline:%d char:%d variable define should not be here\n",line,no);
		////skip(SEMICOLON);//跳读到;
		break;
	case DEFAULT_ERROR://缺少default 30
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \"default\"\n",line,no);
		//skip(SEMICOLON);//跳读到;
		break;
	case OUTOFTAB_ERROR://符号表溢出 31
		sprintf(message,"(FATA ERROR)\ttoo much symbol\n",line,no);
		exit(0);		//严重错误，直接结束程序
		break;
	case GLOBALNAME_ERROR:		//全局量、函数与全局量、函数不可重名 32
		sprintf(message,"(ERROR)\tline:%d char:%global name complict \"default\"\n",line,no);
		break;
	case LOCALNAME_ERROR:	//局部常、变量重名 33
		sprintf(message,"(ERROR)\tline:%d char:%local name complict \n",line,no);
		break;
	case UNDEFID_ERROR:		//未定义标识符 34
		sprintf(message,"(ERROR)\tline:%d char:%d the ID is not defined \n",line,no);
		break;
	case VARIABLE_ERROR:	//应为char int变量 35
		sprintf(message,"(ERROR)\tline:%d char:%d should be a variable \n",line,no);
		break;
	case NOTARR_ERROR:		//应该数组的地方使用了非数组变量 36
		sprintf(message,"(ERROR)\tline:%d char:%d should be an array\n",line,no);
		break;
	case HASRETFUNC_ERROR:	//应为有返回值函数 37
		sprintf(message,"(ERROR)\tline:%d char:%d should be a function with return-value\n",line,no);
		break;
	case DIFFVAR_ERROR:		//循环控制变量前后不一 38
		sprintf(message,"(ERROR)\tline:%d char:%d the id of for-loop-control is different\n",line,no);
		break;
	case PARACOUNT_ERROR:	//参数数目错误 39
		sprintf(message,"(ERROR)\tline:%d char:%d the number of para is fault\n",line,no);
		break;
	case TYPECONFL_ERROR:	//类型不一致
		sprintf(message,"(WARNING)\tline:%d char:%d types are conflicting \n",line,no);
		break;
	default:
		break;
	}

	printf(message);			//打印到控制台
	fprintf(errorfile,message);	//打印到文件error.txt
}


#endif