#include "string.h"

/**********************************************************************
//	ԭ��:	unsigned char stoi(unsigned char scale,unsigned char strlen,const char *str,unsigned long * const ret)
//
//	����:	�ַ���ת������
//	
//	����:	scale -- 	���ƣ�ֻ��Ϊ2,8,10,16
//			strlen --	�ַ�������
//			str	--	��ת�����ַ���
//			ret	--	ת����ɵ�����
//
//	����:	FALSE	--	ת��ʧ��
//			TRUE	--	ת���ɹ�
//
//	��Դ:	�ֲ�����:	5byte

//	˵��:	�ú���ֻ��ת��������32λ����
			�˽���10λ����ʮ����9λ��ʮ������8λ��
************************************************************************/
unsigned char stoi(unsigned char scale,unsigned char strlen,const char *str,unsigned long * const ret) {
	unsigned long temp_32=0;
	unsigned char temp_8=0;
	if (	((scale == 2) && (strlen<=32)) 
		||	((scale == 8) && (strlen<=10)) 
		||	((scale == 10) && (strlen<=9))
		||	((scale == 16) && (strlen<=8))
		) {
						// ������ȷ������ת��
	}
	else  {
		return FALSE;	//	��������ȷ��������ת��
	}
	
	while ((*str) && (temp_8<strlen)) {
		if ((*str >= '0') && (*str<='1')) {
			temp_32 = scale*temp_32 + (*str - '0');
		}
		else if ((*str >= '0') && (*str<='7') && (scale>=8)) {
			temp_32 = scale*temp_32 + (*str - '0');
		}
		else if ((*str >= '0') && (*str<='9') && (scale>=10)) {
			temp_32 = scale*temp_32 + (*str - '0');
		}
		else if ((*str >= 'A') && (*str<='F') && (scale == 16)) {
			temp_32 = scale*temp_32 + (*str - 'A') + 10;
		}
		else if ((*str >= 'a') && (*str<='f') && (scale == 16)) {
			temp_32 = scale*temp_32 + (*str - 'a') + 10;
		}
		else {
			return FALSE;
		}
		str ++;
		temp_8 ++;
	}
	*ret=temp_32;
	return TRUE;
}





