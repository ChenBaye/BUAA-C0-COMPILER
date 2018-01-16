#ifndef _ERROR_H_
#define _ERROR_H_
#include<stdio.h> 
#include<ctype.h>
#include<string.h>
#include<stdlib.h>

//*******************************************
//�������ͱ���
//*******************************************
#define ZEROSTARTNUM_ERROR	1	//������ǰ��
#define ILEGALCH_ERROR		2	//�Ƿ��ַ�
#define SINGLEQ_ERROR		3	//ȱ�ٵ�����
#define DOUBLEQ_ERROR		4	//ȱ��˫����
#define ASSIGN_ERROR		5	//ȱ�ٸ�ֵ��
#define ID_ERROR			6	//ȱ�ٱ�ʶ��
#define LPAR_ERROR			7	//ȱ��(
#define RPAR_ERROR			8	//ȱ��)
#define LBRACE_ERROR		9	//ȱ��{
#define RBRACE_ERROR		10	//ȱ��}
#define LBRACKET_ERROR		11	//ȱ��[
#define RBRACKET_ERROR		12	//ȱ��]
#define FUNCTYPE_ERROR		13	//ȱ�ٺ������ͱ�ʶ��(int char void)
#define STA_ERROR			14	//�������
#define	SEMICOLON_ERROR		15	//ȱ��;
#define FACTOR_ERROR		16	//��������
#define ARRLEN_ERROR		17	//���鳤�ȶ���ʱ����
#define PLUSZERO_ERROR		18	//0ǰ��+ -
#define INTEGER_ERROR		19	//ȱ������
#define CHAR_ERROR			20	//ȱ���ַ�
#define TYPE_ERROR			21	//ȱ�����ͱ�ʶ��(int char)
#define COLON_ERROR			22	//ȱ��:
#define CONSTVALUE_ERROR	23	//ȱ�ٳ���
#define CASE_ERROR			24	//ȱ��case
#define ADDSUB_ERROR		25	//ȱ��+ -
#define STEP_ERROR			26	//��������
#define REOP_ERROR			27	//ȱ�ٹ�ϵ�����(< > <= >= == !=)
#define OUTOFBOUND_ERROR	28	//����Խ��
#define VARDEF_ERROR		29	//��������λ�ô���
#define DEFAULT_ERROR		30	//ȱ��default
#define OUTOFTAB_ERROR		31	//���ű����
#define GLOBALNAME_ERROR	32	//ȫ������������ȫ������������������
#define LOCALNAME_ERROR		33	//�ֲ�������������	
#define UNDEFID_ERROR		34	//δ�����ʶ��
#define VARIABLE_ERROR		35	//ӦΪchar int����
#define NOTARR_ERROR		36	//Ӧ������ĵط�ʹ���˷��������
#define HASRETFUNC_ERROR	37	//ӦΪ�з���ֵ����
#define DIFFVAR_ERROR		38	//ѭ�����Ʊ���ǰ��һ
#define PARACOUNT_ERROR		39	//������Ŀ����
#define TYPECONFL_ERROR		40	//���Ͳ�һ��

FILE* errorfile;				//���մ�����Ϣ���ļ�

int iserror=0;


void errormessage(int error,int line,int no){	//���������Ϣ
	iserror=1;
	printf("************************************ERROR************************************\n");
	char message[200]={'\0'};
	switch(error){
	case ZEROSTARTNUM_ERROR://������ǰ�� 1
		sprintf(message,"(WARNING)\tline:%d char:%d a number start with zero\n",line,no);
		break;
	case ILEGALCH_ERROR://�Ƿ��ַ� 2
		sprintf(message,"(ERROR)\tline:%d char:%d an ilegal char\n",line,no);
		break;
	case SINGLEQ_ERROR://ȱ�ٵ����� 3
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'\'\'\n",line,no);
		//��getsym.h�н��л���
		break;
	case DOUBLEQ_ERROR://ȱ��˫���� 4
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'\"\'\n",line,no);
		//��getsym.h�н��л���
		break;
	case ASSIGN_ERROR://ȱ�ٸ�ֵ�� 5
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'=\'\n",line,no);
		break;
	case ID_ERROR://ȱ�ٱ�ʶ�� 6
		sprintf(message,"(ERROR)\tline:%d char:%d lost an id\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case LPAR_ERROR://ȱ��( 7
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'(\'\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case RPAR_ERROR://ȱ��) 8
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \')\'\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case LBRACE_ERROR://ȱ��{ 9
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'{\'\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case RBRACE_ERROR://ȱ��} 10
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'}\'\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case LBRACKET_ERROR://ȱ��[ 11
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'[\'\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case RBRACKET_ERROR://ȱ��] 12
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \']\'\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case FUNCTYPE_ERROR://ȱ�ٺ������ͱ�ʶ��(int char void) 13
		sprintf(message,"(ERROR)\tline:%d char:%d lost int/char/void at the head of a function\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case STA_ERROR://�����ķ��Ų������κ�<���>�Ŀ�ʼ���� 14
		sprintf(message,"(ERROR)\tline:%d char:%d read an ilegal symbol at the head of a sta\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case SEMICOLON_ERROR://ȱ��; 15
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \';\'\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case FACTOR_ERROR://�����ķ��Ų������κ����ӵĿ�ʼ���� 16
		sprintf(message,"(ERROR)\tline:%d char:%d read an ilegal symbol at the head of a factor\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case ARRLEN_ERROR://���鳤�ȶ���ʱ���� 17
		sprintf(message,"(ERROR)\tline:%d char:%d the lenth of an array must be a no-symbol-integer(>0) \n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case PLUSZERO_ERROR://0ǰ��+ - 18
		sprintf(message,"(ERROR)\tline:%d char:%d zero can not start with \'+\'/\'-\' \n",line,no);
		////skip(SEMICOLON);//������;
		break;
	case INTEGER_ERROR://ȱ������ 19
		sprintf(message,"(ERROR)\tline:%d char:%d lost an integer\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case CHAR_ERROR://ȱ���ַ� 20
		sprintf(message,"(ERROR)\tline:%d char:%d lost a char\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case TYPE_ERROR://ȱ�����ͱ�ʶ��(int char) 21
		sprintf(message,"(ERROR)\tline:%d char:%d lost an \"int\" or \"char\"\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case COLON_ERROR://ȱ��: 22
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \';\'\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case CONSTVALUE_ERROR://ȱ�ٳ��� 23
		sprintf(message,"(ERROR)\tline:%d char:%d lost a const-value(an integer or a char)\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case CASE_ERROR://ȱ��case 24
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \"case\"\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case ADDSUB_ERROR://ȱ��+ - 25
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'+\'/\'-\'\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case STEP_ERROR://�������� 26
		sprintf(message,"(ERROR)\tline:%d char:%d step must be an no-symbol-integer(>0)\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case REOP_ERROR://ȱ�ٹ�ϵ�����(< > <= >= == !=) 27
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \'<\'/\'>\'/\"<=\"/\">=\"/\"==\"/\"!=\"\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case OUTOFBOUND_ERROR://����Խ�� 28
		sprintf(message,"(ERROR)\tline:%d char:%d the index is out of the bound of array\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case VARDEF_ERROR://��������λ�ô��� 29
		sprintf(message,"(WARNING)\tline:%d char:%d variable define should not be here\n",line,no);
		////skip(SEMICOLON);//������;
		break;
	case DEFAULT_ERROR://ȱ��default 30
		sprintf(message,"(ERROR)\tline:%d char:%d lost a \"default\"\n",line,no);
		//skip(SEMICOLON);//������;
		break;
	case OUTOFTAB_ERROR://���ű���� 31
		sprintf(message,"(FATA ERROR)\ttoo much symbol\n",line,no);
		exit(0);		//���ش���ֱ�ӽ�������
		break;
	case GLOBALNAME_ERROR:		//ȫ������������ȫ������������������ 32
		sprintf(message,"(ERROR)\tline:%d char:%global name complict \"default\"\n",line,no);
		break;
	case LOCALNAME_ERROR:	//�ֲ������������� 33
		sprintf(message,"(ERROR)\tline:%d char:%local name complict \n",line,no);
		break;
	case UNDEFID_ERROR:		//δ�����ʶ�� 34
		sprintf(message,"(ERROR)\tline:%d char:%d the ID is not defined \n",line,no);
		break;
	case VARIABLE_ERROR:	//ӦΪchar int���� 35
		sprintf(message,"(ERROR)\tline:%d char:%d should be a variable \n",line,no);
		break;
	case NOTARR_ERROR:		//Ӧ������ĵط�ʹ���˷�������� 36
		sprintf(message,"(ERROR)\tline:%d char:%d should be an array\n",line,no);
		break;
	case HASRETFUNC_ERROR:	//ӦΪ�з���ֵ���� 37
		sprintf(message,"(ERROR)\tline:%d char:%d should be a function with return-value\n",line,no);
		break;
	case DIFFVAR_ERROR:		//ѭ�����Ʊ���ǰ��һ 38
		sprintf(message,"(ERROR)\tline:%d char:%d the id of for-loop-control is different\n",line,no);
		break;
	case PARACOUNT_ERROR:	//������Ŀ���� 39
		sprintf(message,"(ERROR)\tline:%d char:%d the number of para is fault\n",line,no);
		break;
	case TYPECONFL_ERROR:	//���Ͳ�һ��
		sprintf(message,"(WARNING)\tline:%d char:%d types are conflicting \n",line,no);
		break;
	default:
		break;
	}

	printf(message);			//��ӡ������̨
	fprintf(errorfile,message);	//��ӡ���ļ�error.txt
}


#endif