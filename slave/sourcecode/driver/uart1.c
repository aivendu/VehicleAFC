/****************************************Copyright (c)**************************************************
**                               广州周立功单片机发展有限公司
**                                     研    究    所
**                                        产品一部
**
**                                 http://www.zlgmcu.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: uart0.c
**创   建   人: 陈明计
**最后修改日期: 2003年7月4日
**描        述: μCOS-II下LPC210x的UART1底层驱动
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 陈明计
** 版  本: v1.0
** 日　期: 2003年7月4日
** 描　述: 原始版本
**
**------------------------------------------------------------------------------------------------------
** 修改人: 陈明计
** 版  本: v1.1
** 日　期: 2003年7月21日
** 描　述: 根据正式文档更改寄存器名
**
**--------------当前版本修订------------------------------------------------------------------------------
** 修改人:
** 日　期:
** 描　述:
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
** 函数名称: Uart1WriteFull
** 功能描述: 数据队列写满处理程序
** 输　入: Buf:指向数据队列
**        Data:将要入队的数据
**        Mod: 入队方式
** 输　出:TRUE  :成功
**        FALSE:失败
** 全局变量: 无
** 调用模块: 无
**
** 作　者: 陈明计
** 日　期: 2003年7月4日
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint8 Uart1WriteFull(DataQueue *Buf, QUEUE_DATA_TYPE Data, uint8 Mod)
{
	uint16 temp;

	Mod = Mod;

	temp = QueueSize((void *)Buf);
	while (temp <= QueueNData((void *)Buf))         /* 等待数据队列不满 */
	{
		OSTimeDly(2);
	}
	return QueueWrite((void *)Buf, Data);           /* 数据重新入队 */
}

/*********************************************************************************************************
** 函数名称: UART1Init
** 功能描述: 初始化UART1
** 输　入: bps：波特率
**
** 输　出:TRUE  :成功
**        FALSE:失败
** 全局变量: Uart0Sem
** 调用模块: QueueCreate,OSSemCreate
**
** 作　者: 陈明计
** 日　期: 2003年7月4日
**-------------------------------------------------------------------------------------------------------
** 修改人: 陈明计
** 日　期: 2003年7月8日
**-------------------------------------------------------------------------------------------------------
** 修改人: 陈明计
** 日　期: 2003年7月21日
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint8 UART1Init(void)
{
#if 1
	uint16 Fdiv;

	PINSEL0 = (PINSEL0 & 0xfffffff0) | 0x05;    /* 选择管脚为UART1 */
#if	UART1_MULTIPLEX_EN >= 1
	PINSEL0 = (PINSEL0 & 0x3fffffff);
	PINSEL1 = (PINSEL1 & 0xfffffffc);
	IO0DIR = IO0DIR | UART1_A ;
	IO0DIR = IO0DIR | UART1_B ;

	IO0SET = IO0SET | ( UART1_A );
	IO0SET = IO0SET | ( UART1_B );
#endif
	U1LCR = 0x80;                               /* 允许访问分频因子寄存器 */
	Fdiv = (Fpclk / 16) / 9600;                  /* 设置波特率 */
	U1DLM = Fdiv / 256;
	U1DLL = Fdiv % 256;
	U1LCR = 0x03;                               /* 禁止访问分频因子寄存器 */
	/* 且设置为8,1,n */
	U1IER = 0x05;                               /* 允许接收和发送中断 */
	U1FCR = 0x87;                               /* 初始化FIFO */

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

	QueueFlush(UART1RecBuf);				//	清除接收buffer

	//PINSEL0 = (PINSEL0 & 0xfffffff0) | 0x05;    /* 选择管脚为UART1 */

	U1LCR = 0x80;                               /* 允许访问分频因子寄存器 */
	Fdiv = (Fpclk / 16) / bps;                  /* 设置波特率 */
	U1DLM = Fdiv / 256;
	U1DLL = Fdiv % 256;
	U1LCR = 0x03;                               /* 禁止访问分频因子寄存器 */
	/* 且设置为8,1,n */
	U1IER = 0x05;                               /* 允许接收和发送中断 */
	U1FCR = 0x87;                               /* 初始化FIFO */


	OS_EXIT_CRITICAL();


	return 1;

}
#endif


//	申请UART1  资源
uint8 RequestUart1(uint16 t, uint8 ch, uint32 bps)
{
	uint8 err;
	OSSemPend(Uart1_Channel_Sem, t, &err);  //	取得资源
	if ((err == OS_NO_ERR) && (uart1_channel_ic_flag != ch))
	{
		while (QueueNData(UART1SendBuf) != 0)
		{
			OSTimeDly(1);
		}
		uart1_channel_ic_flag = ch;				//	保存当前通道号
#if	UART1_MULTIPLEX_EN >= 1
		Uart1ChangeCh(ch, bps);  					//	切换通道
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
** 函数名称: UART1Putch
** 功能描述: 发送一个字节数据
** 输　入: Data：发送的数据数据
** 输　出:无
** 全局变量: 无
** 调用模块: 无
**
** 作　者: 陈明计
** 日　期: 2003年7月4日
**-------------------------------------------------------------------------------------------------------
** 修改人: 陈明计
** 日　期: 2003年7月8日
**-------------------------------------------------------------------------------------------------------
** 修改人: 陈明计
** 日　期: 2003年7月21日
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint8 Uart1SendByte(uint8 data, uint8 flag)
{
	uint8 temp;
	if (flag == 0)  				//	不清空buffer
	{

	}
	else if (flag == 1)  				//	只清空发送buffer
	{
		QueueFlush(UART1SendBuf);
	}
	else if (flag == 2)  			//	清空发送和接收buffer
	{
		QueueFlush(UART1SendBuf);
		QueueFlush(UART1RecBuf);
	}
	else  			//	其它不清空buffer
	{

	}
	if (QueueWrite((void *)UART1SendBuf, data) != QUEUE_OK)           /* 数据入队 */
	{
		return FALSE;
	}

	OS_ENTER_CRITICAL();
	if ((U1LSR & 0x00000020) != 0)
	{
		/* UART1发送保持寄存器空 */
		QueueRead(&temp, UART1SendBuf);             /* 发送最初入队的数据 */
		U1THR = temp;
		U1IER = U1IER | 0x02;                       /* 允许发送中断 */
	}
	OS_EXIT_CRITICAL();
	return TRUE;
}


/*
//	发送字符串
*/
uint8 Uart1SendString(const char *str, uint8 flag)
{
	if (flag == 0)  				//	不清空buffer
	{

	}
	else if (flag == 1)  				//	只清空发送buffer
	{
		QueueFlush(UART1SendBuf);
	}
	else if (flag == 2)  			//	清空发送和接收buffer
	{
		QueueFlush(UART1SendBuf);
		QueueFlush(UART1RecBuf);
	}
	else  			//	其它不清空buffer
	{
		return FALSE;
	}

	//	如果buffer空间不够存放这么多数据，返回错误，等上层判断，是否发送。
	if (strlen(str) > (QueueSize(UART1SendBuf) - QueueNData(UART1SendBuf)))
	{
		return FALSE;
	}
	while (*str)  			//	发送数据到buffer
	{
		Uart1SendByte(*(uint8 *)str++, 0);
	}
	return TRUE;
}

/*
//	发送多数据
*/
uint8 Uart1SendBytes(const uint8 *dat, uint8 len, uint8 flag)
{

	if (flag == 0)  				//	不清空buffer
	{

	}
	else if (flag == 1)  				//	只清空发送buffer
	{
		QueueFlush(UART1SendBuf);
	}
	else if (flag == 2)  			//	清空发送和接收buffer
	{
		QueueFlush(UART1SendBuf);
		QueueFlush(UART1RecBuf);
	}
	else  			//	其它不清空buffer
	{
		return FALSE;
	}
	//	如果buffer空间不够存放这么多数据，返回错误，等上层判断，是否发送。
	if (len > (QueueSize(UART1SendBuf) - QueueNData(UART1SendBuf)))
	{
		return FALSE;
	}
	while (len--)  				//	发送数据到buffer
	{
		Uart1SendByte(*dat++, 0);
	}
	return TRUE;
}


/*
//	接收一个字符
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
** 函数名称: UART1_Exception
** 功能描述: UART1中断服务程序
** 输　入: 无
**
** 输　出: 无
**
** 全局变量: 无
** 调用模块: QueueRead,OSSemPost
**
** 作　者: 陈明计
** 日　期: 2003年7月4日
**-------------------------------------------------------------------------------------------------------
** 修改人: 陈明计
** 日　期: 2003年7月21日
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/


void UART1_Exception(void)
{
	uint8 IIR, temp;


	OS_ENTER_CRITICAL();
	while(((IIR = U1IIR) & 0x01) == 0)
	{
		/* 有中断未处理完 */
		switch (IIR & 0x0e)
		{

		case 0x04: 								    /* 接收数据可用 */
		case 0x0c:


			// U1IER = U1IER & (~0x01);                /* 禁止接收及字符超时中断 */


			do
			{
				QueueWrite(UART1RecBuf, U1RBR);
			}
			while((U1LSR & 0x00000001) != 0);

			OSSemPost(Uart1Sem);                    /* 通知接收任务 */



			break;

		case 0x06:                                  /* 接收线状态   */
			temp = U1LSR;
			break;

		case 0x02:                                  /* THRE中断    */
			//    for (i = 0; i < 8; i++) /* 向发送FIFO填充数据 */
		{
			if (QueueRead(&temp, UART1SendBuf) == QUEUE_OK)
			{
				U1THR = temp;
			}
			else
			{
				U1IER = U1IER & (~0x02);        /* 队列空，则禁止发送中断 */
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
	VICVectAddr = 0;            // 通知中断控制器中断结束
	OS_EXIT_CRITICAL();
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
