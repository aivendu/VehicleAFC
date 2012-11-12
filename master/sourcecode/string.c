#include "string.h"

/**********************************************************************
//	原型:	unsigned char stoi(unsigned char scale,unsigned char strlen,const char *str,unsigned long * const ret)
//
//	功能:	字符串转整形数
//	
//	参数:	scale -- 	进制，只能为2,8,10,16
//			strlen --	字符串长度
//			str	--	待转换的字符串
//			err	--	转换是否成功. FALSE	--	转换失败; TRUE	--	转换成功
//	
//	返回:	转换完成的整数
//
//	资源:	局部变量:	5byte

//	说明:	该函数只能转换二进制32位数，
			八进制10位数，十进制9位，十六进制8位。
************************************************************************/
unsigned long stoi(unsigned char scale,unsigned char strlen,const char *str,unsigned char * const err) {
	unsigned long temp_32=0;
	unsigned char temp_8=0;
	if (	((scale == 2) && (strlen<=32)) 
		||	((scale == 8) && (strlen<=10)) 
		||	((scale == 10) && (strlen<=9))
		||	((scale == 16) && (strlen<=8))
		) {
						// 参数正确，可以转换
	}
	else  {
		*err = FALSE;
		return 0;	//	参数不正确，不可以转换
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
			*err = FALSE;
			return 0;
		}
		str ++;
		temp_8 ++;
	}
	*err = TRUE;
	return temp_32;
}





