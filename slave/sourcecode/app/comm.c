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

uint32 delayus(uint32 m) {		//	������11.0592Mʱ����
	uint32 i;
	for (i=0;i<m;i++) {		//	����28*m+ ������
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
** ��������: crcByte
** ��������: ����CRC16������
** �䡡��: crc_data ԭʼ���ݣ� b ��������crc����
** �䡡��:  ���ؼ�����ϵ�����
** ȫ�ֱ���: ��
** ����ģ��: 
**
** ������: �򿡷�
** �ա���: 2011��9��2��
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
** ��������: TimeDay
** ��������: �����ڵ�ʱ���Ϊ��2000��1��1�յ����������
** �䡡��: ��
**		   
** �䡡��: ����
**        
** ȫ�ֱ���: 
** ����ģ��:
** ������: �򿡷�
** �ա���: 2011��9��2�� 
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

		times		 = CTIME0;			// ��ȡ������ʱ�ӼĴ���
		datas 		 = CTIME1;
		yearnow  	 = (datas >> 16) & 0xfff;	//��
		monthnow  	 = (datas >> 8) & 0x0f;		// ��ȡ ��
		daynow    	 = datas & 0x1f;				// ��ȡ ��	
		hournow   	 = (times >> 16) & 0x1f;		// ��ȡ Сʱ
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
** ��������: TimeMin
** ��������: �����ڵ�ʱ���Ϊ��2000��1��1�յ�����ķ�����
** �䡡��: ��
**		   
** �䡡��: ����
**        
** ȫ�ֱ���: 
** ����ģ��:
** ������: �򿡷�
** �ա���: 2011��9��2�� 
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
** ��������: GetDateTimeFromSecond
** ��������: ������תΪ ���� ���� data Ϊ����ָ�� data[0] Ϊ year - 2000��������YYMMDDHHMMSS
** �䡡��: lSec 2010 �굽���ڵ�������tTime ����ֵָ�롣
**		   
** �䡡��: 
**        
** ȫ�ֱ���: 
** ����ģ��:
** ������: 
** �ա���:  
********************************************************************************************************/
const uint8 DayOfMon[12] = {31,28,31,30,31,30,31,31,30,31,30,31};   

/* ����ʱ�������2010/1/1/0/0������ */ 
void GetDateTimeFromSecond(uint32 lSec, uint8 *tTime) 
{ 
	uint16 i,j,iDay; 
	uint32 lDay; 
 
	lDay = lSec / 86400      ;		/* תΪ�������ʱ�� */ 
	lSec = lSec % 86400      ; 
 
	i = 2010; 
	while(lDay > 365) 
	{ 
	  if(((i%4==0)&&(i%100!=0)) || (i%400==0))    /* ���� */ 
	    lDay -= 366; 
	  else 
		lDay -= 365; 
	  i++; 
	} 
	if((lDay == 365) && !(((i%4==0)&&(i%100!=0)) || (i%400==0)))  /* ƽ�� */ 
	{	 
	  lDay -= 365; 
	  i++; 
	} 
	tTime[0] = i - 2000;
	for(j=0;j<12;j++)     /* �����·� */ 
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


