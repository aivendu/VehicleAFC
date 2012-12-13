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
**��        ��: ��COS-II��LPC210x��UART0�ײ�����
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


static OS_EVENT *Uart0Sem;

static OS_EVENT *Uart0_Channel_Sem;

static uint8 UART0SendBuf[UART0_SEND_QUEUE_LENGHT];
static uint8 UART0RecBuf[UART0_REC_QUEUE_LENGHT];


static uint8 uart0_channel_ic_flag;

static _uart_config_s const uart0_config[] =
{
	{0,0,	9600		},		//	0- init
	{0,0,	9600		},		//	1- IC Reader
	{1,0,	9600		},		//	2- GPS
	{2,0,	9600		},		//	3- note machine
};

//

/*********************************************************************************************************
** ��������: Uart0WriteFull
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
uint8 Uart0WriteFull(DataQueue *Buf, QUEUE_DATA_TYPE Data, uint8 Mod)
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
** ��������: UART0Init
** ��������: ��ʼ��UART0
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
uint8 UART0Init(void)
{
#if 1
	uint16 Fdiv;

	PINSEL0 = (PINSEL0 & 0xfffffff0) | 0x05;    /* ѡ��ܽ�ΪUART0 */
#if	UART0_MULTIPLEX_EN >= 1
	IO0SET = IO0SET | ( UART0_A );
	IO0SET = IO0SET | ( UART0_B );
#endif
	U0LCR = 0x80;                               /* ������ʷ�Ƶ���ӼĴ��� */
	Fdiv = (Fpclk / 16) / uart0_config[UART0_INIT].bps;                  /* ���ò����� */
	U0DLM = Fdiv / 256;
	U0DLL = Fdiv % 256;
	U0LCR = 0x03;                               /* ��ֹ���ʷ�Ƶ���ӼĴ��� */
	/* ������Ϊ8,1,n */
	U0IER = 0x05;                               /* ������պͷ����ж� */
	U0FCR = 0x87;                               /* ��ʼ��FIFO */

#endif

	if (QueueCreate((void *)UART0SendBuf,
	                sizeof(UART0SendBuf),
	                NULL,
	                (uint8 ( *)())Uart0WriteFull)
	        == NOT_OK)
	{
		return FALSE;
	}
	Uart0Sem = OSSemCreate(0);
	if (Uart0Sem != NULL)
	{
		// return TRUE;
	}
	else
	{
		return FALSE;
	}

	if (QueueCreate((void *)UART0RecBuf,
	                sizeof(UART0RecBuf),
	                NULL,
	                NULL) == NOT_OK)
	{
		return FALSE;
	}

	Uart0_Channel_Sem = OSSemCreate(0);
	if (Uart0_Channel_Sem != NULL)
	{
		//
	}
	else
	{
		return FALSE;
	}
	OSSemPost(Uart0_Channel_Sem);
	return TRUE;

}


#if	UART0_MULTIPLEX_EN >= 1
static uint8 Uart0ChangeCh(uint8 ch)
{
	uint16 Fdiv;
	uint32 bps;

	bps = uart0_config[ch].bps;
	ch = uart0_config[ch].channal;
	OS_ENTER_CRITICAL();
	if (ch == 3)			// ICReader
	{
		IO0SET = IO0SET | ( UART0_A );
		IO0SET = IO0SET | ( UART0_B );
	}
	else if (ch == 2)			// Printer
	{
		IO0CLR = IO0CLR | ( UART0_A );
		IO0SET = IO0SET | ( UART0_B );
	}
	else if (ch == 1)
	{
		IO0SET = IO0SET | ( UART0_A);
		IO0CLR = IO0CLR | ( UART0_B);
	}
	else if(ch == 0)
	{
		IO0CLR = IO0CLR | ( UART0_A );
		IO0CLR = IO0CLR | ( UART0_B );
	}

	delay10us(2);

	//PINSEL0 = (PINSEL0 & 0xfffffff0) | 0x05;    /* ѡ��ܽ�ΪUART0 */

	U0LCR = 0x80;                               /* ������ʷ�Ƶ���ӼĴ��� */
	Fdiv = (Fpclk / 16) / bps;                  /* ���ò����� */
	U0DLM = Fdiv / 256;
	U0DLL = Fdiv % 256;
	U0LCR = 0x03;                               /* ��ֹ���ʷ�Ƶ���ӼĴ��� */
	/* ������Ϊ8,1,n */
	U0IER = 0x05;                               /* ������պͷ����ж� */
	U0FCR = 0x87;                               /* ��ʼ��FIFO */


	OS_EXIT_CRITICAL();


	return 1;

}
#endif

//	����UART0  ��Դ
uint8 RequestUart0(uint8 ch, uint16 t)
{
	uint8 err;
	OSSemPend(Uart0_Channel_Sem, t, &err);  //	ȡ����Դ
	if ((err == OS_NO_ERR) && (uart0_channel_ic_flag != ch))
	{
		uart0_channel_ic_flag = ch;				//	���浱ǰͨ����
#if	UART0_MULTIPLEX_EN >= 1
		Uart0ChangeCh(ch);  					//	�л�ͨ��
		QueueFlush(UART0SendBuf);				//	�������buffer
		QueueFlush(UART0RecBuf);				//	�������buffer
#endif
	}
	/*else if (uart0_channel_ic_flag == ch) {
		QueueFlush(UART0SendBuf);				//	�������buffer
		QueueFlush(UART0RecBuf);				//	�������buffer
	}*/
	return err;
}

void FreeUart0(void)
{
	OSSemPost(Uart0_Channel_Sem);			//	�ͷ���Դ
}

/*********************************************************************************************************
** ��������: UART0Putch
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
uint8 Uart0SendByte(uint8 data, uint8 flag)
{
	uint8 temp;
	if (flag == 0)  				//	�����buffer
	{

	}
	else if (flag == 1)  				//	ֻ��շ���buffer
	{
		QueueFlush(UART0SendBuf);
	}
	else if (flag == 2)  			//	��շ��ͺͽ���buffer
	{
		QueueFlush(UART0SendBuf);
		QueueFlush(UART0RecBuf);
	}
	else  			//	���������buffer
	{

	}
	if (QueueWrite((void *)UART0SendBuf, data) != QUEUE_OK)           /* ������� */
	{
		return FALSE;
	}

	OS_ENTER_CRITICAL();
	if ((U0LSR & 0x00000020) != 0)
	{
		/* UART0���ͱ��ּĴ����� */
		QueueRead(&temp, UART0SendBuf);             /* ���������ӵ����� */
		U0THR = temp;
		U0IER = U0IER | 0x02;                       /* �������ж� */
	}
	OS_EXIT_CRITICAL();
	return TRUE;
}


/*
//	�����ַ���
*/
uint8 Uart0SendString(const char *str, uint8 flag)
{
	if (flag == 0)  				//	�����buffer
	{

	}
	else if (flag == 1)  				//	ֻ��շ���buffer
	{
		QueueFlush(UART0SendBuf);
	}
	else if (flag == 2)  			//	��շ��ͺͽ���buffer
	{
		QueueFlush(UART0SendBuf);
		QueueFlush(UART0RecBuf);
	}
	else  			//	���������buffer
	{
		return FALSE;
	}

	//	���buffer�ռ䲻�������ô�����ݣ����ش��󣬵��ϲ��жϣ��Ƿ��͡�
	if (strlen(str) > (QueueSize(UART0SendBuf) - QueueNData(UART0SendBuf)))
	{
		return FALSE;
	}
	while (*str)  			//	�������ݵ�buffer
	{
		Uart0SendByte(*(uint8 *)str++, 0);
	}
	return TRUE;
}

/*
//	���Ͷ�����
*/
uint8 Uart0SendBytes(const uint8 *dat, uint8 len, uint8 flag)
{

	if (flag == 0)  				//	�����buffer
	{

	}
	else if (flag == 1)  				//	ֻ��շ���buffer
	{
		QueueFlush(UART0SendBuf);
	}
	else if (flag == 2)  			//	��շ��ͺͽ���buffer
	{
		QueueFlush(UART0SendBuf);
		QueueFlush(UART0RecBuf);
	}
	else  			//	���������buffer
	{
		return FALSE;
	}
	//	���buffer�ռ䲻�������ô�����ݣ����ش��󣬵��ϲ��жϣ��Ƿ��͡�
	if (len > (QueueSize(UART0SendBuf) - QueueNData(UART0SendBuf)))
	{
		return FALSE;
	}
	while (len--)  				//	�������ݵ�buffer
	{
		Uart0SendByte(*dat++, 0);
	}
	return TRUE;
}


/*
//	����һ���ַ�
*/
uint8 Uart0RecByte(uint8 *const dat, uint8 flag, uint32 timedelay)
{
	uint8 err;

	while (QueueRead((uint8 *)dat, UART0RecBuf) != QUEUE_OK)
	{
		if (flag)
		{
			OSSemPend(Uart0Sem, timedelay, &err);
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
** ��������: UART0_Exception
** ��������: UART0�жϷ������
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
void UART0_Exception(void)
{
	uint8 IIR, temp;

	OS_ENTER_CRITICAL();
	while(((IIR = U0IIR) & 0x01) == 0)
	{
		/* ���ж�δ������ */
		switch (IIR & 0x0e)
		{
		case 0x02:                                  /* THRE�ж�    */
			if (QueueRead(&temp, UART0SendBuf) == QUEUE_OK)
			{
				U0THR = temp;
			}
			else
			{
				U0IER = U0IER & (~0x02);        /* ���пգ����ֹ�����ж� */
			}
			break;


		case 0x06:                                  /* ������״̬   */
			temp = U0LSR;
			break;
		case 0x04:

		case 0x0c:                                  /* �ַ���ʱָʾ */
			do
			{

				QueueWrite(UART0RecBuf, U0RBR);
			}
			while((U0LSR & 0x00000001) != 0);

			OSSemPost(Uart0Sem);                    /* ֪ͨ�������� */
			//  U0IER = U0IER & (~0x01);                /* ��ֹ���ռ��ַ���ʱ�ж� */
			break;
		default :
			break;
		}
	}
	VICVectAddr = 0;            // ֪ͨ�жϿ������жϽ���
	OS_EXIT_CRITICAL();
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
