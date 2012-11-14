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

/**********************************************************************
//	ԭ��:	unsigned char stoi(unsigned char scale,unsigned char strlen,const char *str,unsigned long * const ret)
//
//	����:	�ַ���ת������
//	
//	����:	scale -- 	���ƣ�ֻ��Ϊ2,8,10,16
//			strlen --	�ַ�������
//			str	--	��ת�����ַ���
//			err	--	ת���Ƿ�ɹ�. 0	--	ת��ʧ��; 1	--	ת���ɹ�
//	
//	����:	ת����ɵ�����
//
//	��Դ:	�ֲ�����:	5byte

//	˵��:	�ú���ֻ��ת��������32λ����
			�˽���10λ����ʮ����9λ��ʮ������8λ��
************************************************************************/
unsigned long stoi(unsigned char scale,unsigned char strlen,const char *str,unsigned char * const err) {
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
		*err = 0;
		return 0xffffffff;	//	��������ȷ��������ת��
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
** ��������: �����ڵ�ʱ���Ϊ��2010��1��1�յ����������
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
	Continueddaynow = ((Continueddaynow * 24 + hournow)*60+minnow)*60+SEC;
	
	
	return(Continueddaynow);
}

/*********************************************************************************************************
** ��������: TimeMin
** ��������: �����ڵ�ʱ���Ϊ��2010��1��1�յ�����ķ�����
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
	datas = 2010;
	for (; datas < yearnow; ++datas) {
		if ((((datas % 4) == 0) && ((datas % 100) != 0)) || ((datas % 400) == 0))
			iflag++;
	}
	


	Continueddaynow = Continueddaynow +  365 * ( yearnow - 2010) + iflag - 1;
	Continueddaynow = ((Continueddaynow * 24 + hournow)*60+minnow)*60+secnow;	
	
	
	return(Continueddaynow);
}


