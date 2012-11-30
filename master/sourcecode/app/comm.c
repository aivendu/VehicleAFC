#include "includes.h"


uint32 delay10us(uint32 m)	// 12MHz Xtal, close to ms value
{
	uint32 j;
	uint32 i;
	for(i = 0; i < m; i++)
	{
		for(j = 0; j < 85; j++);
	}
	return 1;
}

uint32 delayus(uint32 m)  		//	运行在11.0592M时钟下
{
	uint32 i;
	for (i = 0; i < m; i++)  		//	运行28*m+ 条命令
	{
		__asm
		{
			MOV R1, #0
			DLE1:	ADD R1, R1, #1
			CMP R1, #6
			NOP
			BCC DLE1
		}
	}
	return 1;
}

/**********************************************************************
//	原型:	unsigned char stoi(unsigned char scale,unsigned char strlen,const char *str,unsigned long * const ret)
//
//	功能:	字符串转整形数
//
//	参数:	scale -- 	进制，只能为2,8,10,16
//			strlen --	字符串长度
//			str	--	待转换的字符串
//			err	--	转换是否成功. 0	--	转换失败; 1	--	转换成功
//
//	返回:	转换完成的整数
//
//	资源:	局部变量:	5byte

//	说明:	该函数只能转换二进制32位数，
			八进制10位数，十进制9位，十六进制8位。
************************************************************************/
unsigned long stoi(unsigned char scale, unsigned char strlen, const char *str, unsigned char *const err)
{
	unsigned long temp_32 = 0;
	unsigned char temp_8 = 0;
	if (	((scale == 2) && (strlen <= 32))
	        ||	((scale == 8) && (strlen <= 10))
	        ||	((scale == 10) && (strlen <= 9))
	        ||	((scale == 16) && (strlen <= 8))
	   )
	{
		// 参数正确，可以转换
	}
	else
	{
		*err = 0;
		return 0xffffffff;	//	参数不正确，不可以转换
	}

	while ((*str) && (temp_8 < strlen))
	{
		if ((*str >= '0') && (*str <= '1'))
		{
			temp_32 = scale * temp_32 + (*str - '0');
		}
		else if ((*str >= '0') && (*str <= '7') && (scale >= 8))
		{
			temp_32 = scale * temp_32 + (*str - '0');
		}
		else if ((*str >= '0') && (*str <= '9') && (scale >= 10))
		{
			temp_32 = scale * temp_32 + (*str - '0');
		}
		else if ((*str >= 'A') && (*str <= 'F') && (scale == 16))
		{
			temp_32 = scale * temp_32 + (*str - 'A') + 10;
		}
		else if ((*str >= 'a') && (*str <= 'f') && (scale == 16))
		{
			temp_32 = scale * temp_32 + (*str - 'a') + 10;
		}
		else
		{
			*err = 0;
			return 0xffffffff;
		}
		str ++;
		temp_8 ++;
	}
	*err = 1;
	return temp_32;
}




/**************************************************
** 函数名称: crcByte
** 功能描述: 计算CRC16的数据
** 输　入: crc_data 原始数据， b 参与计算的crc数据
** 输　出:  返回计算完毕的数据
** 全局变量: 无
** 调用模块:
**
** 作　者: 沈俊峰
** 日　期: 2011年9月2日
**************************************************/
unsigned int CRCByte(unsigned int crc_data, unsigned char b)
{
	crc_data = (unsigned char)(crc_data >> 8) | (crc_data << 8);
	crc_data ^= b;
	crc_data ^= (unsigned char)(crc_data & 0xff) >> 4;
	crc_data ^= crc_data << 12;
	crc_data ^= (crc_data & 0xff) << 5;
	return crc_data;
}

/*********************************************************************************************************
** 函数名称: TimeDay
** 功能描述: 将现在的时间变为从2010年1月1日到当天的秒数
** 输　入: 无
**
** 输　出: 秒数
**
** 全局变量:
** 调用模块:
** 作　者: 沈俊峰
** 日　期: 2011年9月2日
********************************************************************************************************/

uint32 TimeDay(void)
{
	uint8  iflag;
	uint32 yearnow;
	uint8  monthnow;
	uint8  daynow;
	uint8  hournow;
	uint8  minnow;
	uint32 datas;
	uint32 times;
	uint32 Continueddaynow;

	Continueddaynow = 0;

	times		 = CTIME0;			// 读取完整的时钟寄存器
	datas 		 = CTIME1;
	yearnow  	 = (datas >> 16) & 0xfff;	//年
	monthnow  	 = (datas >> 8) & 0x0f;		// 获取 月
	daynow    	 = datas & 0x1f;				// 获取 日
	hournow   	 = (times >> 16) & 0x1f;		// 获取 小时
	minnow    	 = (times >> 8) & 0x3f;

	if (((yearnow % 4 == 0) && (yearnow % 100 != 0)) || (yearnow % 400 == 0))	 iflag = 1;
	else 				iflag = 0;
	if (monthnow >= 3) 	  Continueddaynow = 13 * (monthnow + 1) / 5  - 7 + (monthnow - 1) * 28 + daynow + iflag;
	else if (monthnow == 1) Continueddaynow = daynow;
	else Continueddaynow = 31 + daynow;


	/*
	iflag = 0;
	datas = 2010;
	for (; datas < yearnow; ++datas) {
		if ((((datas % 4) == 0) && ((datas % 100) != 0)) || ((datas % 400) == 0))
			iflag++;
	}
	*/

	Continueddaynow = Continueddaynow - 1 ;

	//Continueddaynow = Continueddaynow +  365 * ( yearnow - 2010) + iflag - 1;
	Continueddaynow = ((Continueddaynow * 24 + hournow) * 60 + minnow) * 60 + SEC;


	return(Continueddaynow);
}

/*********************************************************************************************************
** 函数名称: TimeMin
** 功能描述: 将现在的时间变为从2010年1月1日到当天的分钟数
** 输　入: 无
**
** 输　出: 分数
**
** 全局变量:
** 调用模块:
** 作　者: 沈俊峰
** 日　期: 2011年9月2日
********************************************************************************************************/
uint32 TimeSec(uint16 yearnow, uint8 monthnow, uint8 daynow, uint8 hournow, uint8 minnow , uint8 secnow)
{
	uint8  iflag;
	uint32 datas;

	uint32 Continueddaynow;

	Continueddaynow = 0;

	if (((yearnow % 4 == 0) && (yearnow % 100 != 0)) || (yearnow % 400 == 0))	 iflag = 1;
	else 				iflag = 0;
	if (monthnow >= 3) 	  Continueddaynow = 13 * (monthnow + 1) / 5  - 7 + (monthnow - 1) * 28 + daynow + iflag;
	else if (monthnow == 1) Continueddaynow = daynow;
	else Continueddaynow = 31 + daynow;


	iflag = 0;
	datas = 2010;
	for (; datas < yearnow; ++datas)
	{
		if ((((datas % 4) == 0) && ((datas % 100) != 0)) || ((datas % 400) == 0))
			iflag++;
	}



	Continueddaynow = Continueddaynow +  365 * ( yearnow - 2010) + iflag - 1;
	Continueddaynow = ((Continueddaynow * 24 + hournow) * 60 + minnow) * 60 + secnow;


	return(Continueddaynow);
}


