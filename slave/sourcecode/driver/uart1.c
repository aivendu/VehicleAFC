/****************************************Copyright (c)**************************************************
**                               ������������Ƭ����չ���޹�˾
**                                     ��    ��    ��
**                                        ��Ʒһ��
**
**                                 http://www.zlgmcu.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: uart0.c
**��   ��   ��: ������
**����޸�����: 2003��7��4��
**��        ��: ��COS-II��LPC210x��UART1�ײ�����
**
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ������
** ��  ��: v1.0
** �ա���: 2003��7��4��
** �衡��: ԭʼ�汾
**
**------------------------------------------------------------------------------------------------------
** �޸���: ������
** ��  ��: v1.1
** �ա���: 2003��7��21��
** �衡��: ������ʽ�ĵ����ļĴ�����
**
**--------------��ǰ�汾�޶�------------------------------------------------------------------------------
** �޸���:
** �ա���:
** �衡��:
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/


#include "includes.h"

OS_EVENT *Uart1Sem;
static OS_EVENT *Uart1_Channel_Sem;
OS_EVENT *Rec1Sem;
// volatile uint32 o_num = 0;
//volatile uint32  o_j = 0;

uint8 UART1SendBuf[UART1_SEND_QUEUE_LENGHT];
uint8 UART1RecBuf[UART1_REC_QUEUE_LENGHT];

static uint8 uart1_channel_ic_flag;


/*********************************************************************************************************
** ��������: Uart1WriteFull
** ��������: ���ݶ���д���������
** �䡡��: Buf:ָ�����ݶ���
**        Data:��Ҫ��ӵ�����
**        Mod: ��ӷ�ʽ
** �䡡��:TRUE  :�ɹ�
**        FALSE:ʧ��
** ȫ�ֱ���: ��
** ����ģ��: ��
**
** ������: ������
** �ա���: 2003��7��4��
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint8 Uart1WriteFull(DataQueue *Buf, QUEUE_DATA_TYPE Data, uint8 Mod)
{
	uint16 temp;

	Mod = Mod;

	temp = QueueSize((void *)Buf);
	while (temp <= QueueNData((void *)Buf))         /* �ȴ����ݶ��в��� */
	{
		OSTimeDly(2);
	}
	return QueueWrite((void *)Buf, Data);           /* ����������� */
}

/*********************************************************************************************************
** ��������: UART1Init
** ��������: ��ʼ��UART1
** �䡡��: bps��������
**
** �䡡��:TRUE  :�ɹ�
**        FALSE:ʧ��
** ȫ�ֱ���: Uart0Sem
** ����ģ��: QueueCreate,OSSemCreate
**
** ������: ������
** �ա���: 2003��7��4��
**-------------------------------------------------------------------------------------------------------
** �޸���: ������
** �ա���: 2003��7��8��
**-------------------------------------------------------------------------------------------------------
** �޸���: ������
** �ա���: 2003��7��21��
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint8 UART1Init(void)
{
#if 1
	uint16 Fdiv;

	PINSEL0 = (PINSEL0 & 0xfffffff0) | 0x05;    /* ѡ��ܽ�ΪUART1 */
#if	UART1_MULTIPLEX_EN >= 1
	PINSEL0 = (PINSEL0 & 0x3fffffff);
	PINSEL1 = (PINSEL1 & 0xfffffffc);
	IO0DIR = IO0DIR | UART1_A ;
	IO0DIR = IO0DIR | UART1_B ;

	IO0SET = IO0SET | ( UART1_A );
	IO0SET = IO0SET | ( UART1_B );
#endif
	U1LCR = 0x80;                               /* ������ʷ�Ƶ���ӼĴ��� */
	Fdiv = (Fpclk / 16) / 9600;                  /* ���ò����� */
	U1DLM = Fdiv / 256;
	U1DLL = Fdiv % 256;
	U1LCR = 0x03;                               /* ��ֹ���ʷ�Ƶ���ӼĴ��� */
	/* ������Ϊ8,1,n */
	U1IER = 0x05;                               /* ������պͷ����ж� */
	U1FCR = 0x87;                               /* ��ʼ��FIFO */

#endif

	if (QueueCreate((void *)UART1SendBuf,
	                sizeof(UART1SendBuf),
	                NULL,
	                (uint8 ( *)())Uart0WriteFull)
	        == NOT_OK)
	{
		return FALSE;
	}
	Uart1Sem = OSSemCreate(0);
	if (Uart1Sem != NULL)
	{
		// return TRUE;
	}
	else
	{
		return FALSE;
	}

	if (QueueCreate((void *)UART1RecBuf,
	                sizeof(UART1RecBuf),
	                NULL,
	                NULL) == NOT_OK)
	{
		return FALSE;
	}

	Uart1_Channel_Sem = OSSemCreate(0);
	if (Uart1_Channel_Sem != NULL)
	{
		//
	}
	else
	{
		return FALSE;
	}
	OSSemPost(Uart1_Channel_Sem);
	return TRUE;

}


static uint8 Uart1ChangeCh(uint8 ch, uint32 bps)
{
	uint16 Fdiv;

	OS_ENTER_CRITICAL();
#if	UART1_MULTIPLEX_EN >= 1
	if (ch == 3)			// ICReader
	{
		IO0SET = IO0SET | ( UART1_A );
		IO0SET = IO0SET | ( UART1_B );
	}
	else if (ch == 2)			// Printer
	{
		IO0CLR = IO0CLR | ( UART1_A );
		IO0SET = IO0SET | ( UART1_B );
	}
	else if (ch == 1)
	{
		IO0SET = IO0SET | ( UART1_A);
		IO0CLR = IO0CLR | ( UART1_B);
	}
	else if(ch == 0)
	{
		IO0CLR = IO0CLR | ( UART1_A );
		IO0CLR = IO0CLR | ( UART1_B );
	}

	delay10us(2);

	QueueFlush(UART1RecBuf);				//	�������buffer

	//PINSEL0 = (PINSEL0 & 0xfffffff0) | 0x05;    /* ѡ��ܽ�ΪUART1 */

	U1LCR = 0x80;                               /* ������ʷ�Ƶ���ӼĴ��� */
	Fdiv = (Fpclk / 16) / bps;                  /* ���ò����� */
	U1DLM = Fdiv / 256;
	U1DLL = Fdiv % 256;
	U1LCR = 0x03;                               /* ��ֹ���ʷ�Ƶ���ӼĴ��� */
	/* ������Ϊ8,1,n */
	U1IER = 0x05;                               /* ������պͷ����ж� */
	U1FCR = 0x87;                               /* ��ʼ��FIFO */


	OS_EXIT_CRITICAL();


	return 1;

}
#endif


//	����UART1  ��Դ
uint8 RequestUart1(uint16 t, uint8 ch, uint32 bps)
{
	uint8 err;
	OSSemPend(Uart1_Channel_Sem, t, &err);  //	ȡ����Դ
	if ((err == OS_NO_ERR) && (uart1_channel_ic_flag != ch))
	{
		while (QueueNData(UART1SendBuf) != 0)
		{
			OSTimeDly(1);
		}
		uart1_channel_ic_flag = ch;				//	���浱ǰͨ����
#if	UART1_MULTIPLEX_EN >= 1
		Uart1ChangeCh(ch, bps);  					//	�л�ͨ��
#endif
	}
	OSTimeDly(2);
	return err;
}

void FreeUart1(void)
{
	OSSemPost(Uart1_Channel_Sem);
}

/*********************************************************************************************************
** ��������: UART1Putch
** ��������: ����һ���ֽ�����
** �䡡��: Data�����͵���������
** �䡡��:��
** ȫ�ֱ���: ��
** ����ģ��: ��
**
** ������: ������
** �ա���: 2003��7��4��
**-------------------------------------------------------------------------------------------------------
** �޸���: ������
** �ա���: 2003��7��8��
**-------------------------------------------------------------------------------------------------------
** �޸���: ������
** �ա���: 2003��7��21��
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint8 Uart1SendByte(uint8 data, uint8 flag)
{
	uint8 temp;
	if (flag == 0)  				//	�����buffer
	{

	}
	else if (flag == 1)  				//	ֻ��շ���buffer
	{
		QueueFlush(UART1SendBuf);
	}
	else if (flag == 2)  			//	��շ��ͺͽ���buffer
	{
		QueueFlush(UART1SendBuf);
		QueueFlush(UART1RecBuf);
	}
	else  			//	���������buffer
	{

	}
	if (QueueWrite((void *)UART1SendBuf, data) != QUEUE_OK)           /* ������� */
	{
		return FALSE;
	}

	OS_ENTER_CRITICAL();
	if ((U1LSR & 0x00000020) != 0)
	{
		/* UART1���ͱ��ּĴ����� */
		QueueRead(&temp, UART1SendBuf);             /* ���������ӵ����� */
		U1THR = temp;
		U1IER = U1IER | 0x02;                       /* �������ж� */
	}
	OS_EXIT_CRITICAL();
	return TRUE;
}


/*
//	�����ַ���
*/
uint8 Uart1SendString(const char *str, uint8 flag)
{
	if (flag == 0)  				//	�����buffer
	{

	}
	else if (flag == 1)  				//	ֻ��շ���buffer
	{
		QueueFlush(UART1SendBuf);
	}
	else if (flag == 2)  			//	��շ��ͺͽ���buffer
	{
		QueueFlush(UART1SendBuf);
		QueueFlush(UART1RecBuf);
	}
	else  			//	���������buffer
	{
		return FALSE;
	}

	//	���buffer�ռ䲻�������ô�����ݣ����ش��󣬵��ϲ��жϣ��Ƿ��͡�
	if (strlen(str) > (QueueSize(UART1SendBuf) - QueueNData(UART1SendBuf)))
	{
		return FALSE;
	}
	while (*str)  			//	�������ݵ�buffer
	{
		Uart1SendByte(*(uint8 *)str++, 0);
	}
	return TRUE;
}

/*
//	���Ͷ�����
*/
uint8 Uart1SendBytes(const uint8 *dat, uint8 len, uint8 flag)
{

	if (flag == 0)  				//	�����buffer
	{

	}
	else if (flag == 1)  				//	ֻ��շ���buffer
	{
		QueueFlush(UART1SendBuf);
	}
	else if (flag == 2)  			//	��շ��ͺͽ���buffer
	{
		QueueFlush(UART1SendBuf);
		QueueFlush(UART1RecBuf);
	}
	else  			//	���������buffer
	{
		return FALSE;
	}
	//	���buffer�ռ䲻�������ô�����ݣ����ش��󣬵��ϲ��жϣ��Ƿ��͡�
	if (len > (QueueSize(UART1SendBuf) - QueueNData(UART1SendBuf)))
	{
		return FALSE;
	}
	while (len--)  				//	�������ݵ�buffer
	{
		Uart1SendByte(*dat++, 0);
	}
	return TRUE;
}


/*
//	����һ���ַ�
*/
uint8 Uart1RecByte(uint8 *const dat, uint8 flag, uint32 timedelay)
{
	uint8 err;

	while (QueueRead((uint8 *)dat, UART1RecBuf) != QUEUE_OK)
	{
		if (flag)
		{
			OSSemPend(Uart1Sem, timedelay, &err);
			if (err != OS_NO_ERR)
			{
				return FALSE;
			}
			else
			{
				continue;
			}
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;

}


/*********************************************************************************************************
** ��������: UART1_Exception
** ��������: UART1�жϷ������
** �䡡��: ��
**
** �䡡��: ��
**
** ȫ�ֱ���: ��
** ����ģ��: QueueRead,OSSemPost
**
** ������: ������
** �ա���: 2003��7��4��
**-------------------------------------------------------------------------------------------------------
** �޸���: ������
** �ա���: 2003��7��21��
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/


void UART1_Exception(void)
{
	uint8 IIR, temp;


	OS_ENTER_CRITICAL();
	while(((IIR = U1IIR) & 0x01) == 0)
	{
		/* ���ж�δ������ */
		switch (IIR & 0x0e)
		{

		case 0x04: 								    /* �������ݿ��� */
		case 0x0c:


			// U1IER = U1IER & (~0x01);                /* ��ֹ���ռ��ַ���ʱ�ж� */


			do
			{
				QueueWrite(UART1RecBuf, U1RBR);
			}
			while((U1LSR & 0x00000001) != 0);

			OSSemPost(Uart1Sem);                    /* ֪ͨ�������� */



			break;

		case 0x06:                                  /* ������״̬   */
			temp = U1LSR;
			break;

		case 0x02:                                  /* THRE�ж�    */
			//    for (i = 0; i < 8; i++) /* ����FIFO������� */
		{
			if (QueueRead(&temp, UART1SendBuf) == QUEUE_OK)
			{
				U1THR = temp;
			}
			else
			{
				U1IER = U1IER & (~0x02);        /* ���пգ����ֹ�����ж� */
				goto ends;
			}
		}
		break;

		default :
			//delayms(10);

			break;
		}
	}
ends:
	VICVectAddr = 0;            // ֪ͨ�жϿ������жϽ���
	OS_EXIT_CRITICAL();
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
