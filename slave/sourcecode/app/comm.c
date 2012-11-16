#include "includes.h"


uint32 delay10us(uint32 m)	// 12MHz Xtal, close to ms value
{
    uint32 j;
    uint32 i;
	for(i=0;i<m;i++) {
		for(j=0;j<85;j++);
	}
	return 1;
}

uint32 delayus(uint32 m) {		//	运行在11.0592M时钟下
	uint32 i;
	for (i=0;i<m;i++) {		//	运行28*m+ 条命令
		__asm{
			MOV R1,#0
	DLE1:	ADD R1,R1,#1
			CMP R1,#6
			NOP
			BCC DLE1
		}
	}
	return 1;
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
** 功能描述: 将现在的时间变为从2000年1月1日到当天的秒数
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
	
	Continueddaynow=0;

		times		 = CTIME0;			// 读取完整的时钟寄存器
		datas 		 = CTIME1;
		yearnow  	 = (datas >> 16) & 0xfff;	//年
		monthnow  	 = (datas >> 8) & 0x0f;		// 获取 月
		daynow    	 = datas & 0x1f;				// 获取 日	
		hournow   	 = (times >> 16) & 0x1f;		// 获取 小时
		minnow    	 = (times >> 8) & 0x3f;	
	
	if (((yearnow%4==0)&&(yearnow%100!=0))||(yearnow%400==0))	 iflag=1; 
	else 				iflag=0;
	if (monthnow>=3) 	  Continueddaynow = 13 * (monthnow+1) / 5  - 7 + (monthnow-1) * 28 + daynow + iflag;	
	else if (monthnow==1) Continueddaynow = daynow;
	else Continueddaynow=31+daynow;
	
 
	iflag = 0;
	datas = 2000;
	for (; datas < yearnow; ++datas) {
		if ((((datas % 4) == 0) && ((datas % 100) != 0)) || ((datas % 400) == 0))
			iflag++;
	} 
	
	Continueddaynow = Continueddaynow - 1 ;
	
	//Continueddaynow = Continueddaynow +  365 * ( yearnow - 2010) + iflag - 1;
	Continueddaynow = ((Continueddaynow * 24 + hournow)*60+minnow)*60+SEC;
	
	
	return(Continueddaynow);
}

/*********************************************************************************************************
** 函数名称: TimeMin
** 功能描述: 将现在的时间变为从2000年1月1日到当天的分钟数
** 输　入: 无
**		   
** 输　出: 分数
**        
** 全局变量: 
** 调用模块:
** 作　者: 沈俊峰
** 日　期: 2011年9月2日 
********************************************************************************************************/
uint32 TimeSec(uint16 yearnow,uint8 monthnow, uint8 daynow, uint8 hournow, uint8 minnow ,uint8 secnow)
{	
	uint8  iflag;
	uint32 datas;
	
	uint32 Continueddaynow;
	
	Continueddaynow=0;

	if (((yearnow%4==0)&&(yearnow%100!=0))||(yearnow%400==0))	 iflag=1; 
	else 				iflag=0;
	if (monthnow>=3) 	  Continueddaynow = 13 * (monthnow+1) / 5  - 7 + (monthnow-1) * 28 + daynow + iflag;	
	else if (monthnow==1) Continueddaynow = daynow;
	else Continueddaynow=31+daynow;
	
	
	iflag = 0;
	datas = 2000;
	for (; datas < yearnow; ++datas) {
		if ((((datas % 4) == 0) && ((datas % 100) != 0)) || ((datas % 400) == 0))
			iflag++;
	}
	


	Continueddaynow = Continueddaynow +  365 * ( yearnow - 2010) + iflag - 1;
	Continueddaynow = ((Continueddaynow * 24 + hournow)*60+minnow)*60+secnow;	
	
	
	return(Continueddaynow);
}

/*********************************************************************************************************
** 函数名称: GetDateTimeFromSecond
** 功能描述: 将秒数转为 日期 其中 data 为返回指针 data[0] 为 year - 2000，其它都YYMMDDHHMMSS
** 输　入: lSec 2010 年到现在的秒数。tTime 返回值指针。
**		   
** 输　出: 
**        
** 全局变量: 
** 调用模块:
** 作　者: 
** 日　期:  
********************************************************************************************************/
const uint8 DayOfMon[12] = {31,28,31,30,31,30,31,31,30,31,30,31};   

/* 由秒时计算基于2010/1/1/0/0的日期 */ 
void GetDateTimeFromSecond(uint32 lSec, uint8 *tTime) 
{ 
	uint16 i,j,iDay; 
	uint32 lDay; 
 
	lDay = lSec / 86400      ;		/* 转为基于天的时间 */ 
	lSec = lSec % 86400      ; 
 
	i = 2010; 
	while(lDay > 365) 
	{ 
	  if(((i%4==0)&&(i%100!=0)) || (i%400==0))    /* 闰年 */ 
	    lDay -= 366; 
	  else 
		lDay -= 365; 
	  i++; 
	} 
	if((lDay == 365) && !(((i%4==0)&&(i%100!=0)) || (i%400==0)))  /* 平年 */ 
	{	 
	  lDay -= 365; 
	  i++; 
	} 
	tTime[0] = i - 2000;
	for(j=0;j<12;j++)     /* 计算月份 */ 
	{ 
	  if((j==1) && (((i%4==0)&&(i%100!=0)) || (i%400==0))) 
		iDay = 29; 
	  else 
		iDay = DayOfMon[j]; 
	  if(lDay >= iDay) lDay -= iDay; 
	  else break; 
	}
	
	 tTime[1] = j+1;
	 tTime[2] = lDay+1; 
	 tTime[3] = lSec / 3600; 
	 tTime[4] = (lSec % 3600) / 60;
	 tTime[5] = (lSec % 3600) % 60; 
} 


