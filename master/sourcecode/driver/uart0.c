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
**描        述: μCOS-II下LPC210x的UART0底层驱动
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
** 函数名称: Uart0WriteFull
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
uint8 Uart0WriteFull(DataQueue *Buf, QUEUE_DATA_TYPE Data, uint8 Mod)
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
** 函数名称: UART0Init
** 功能描述: 初始化UART0
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
uint8 UART0Init(void)
{
#if 1
	uint16 Fdiv;

	PINSEL0 = (PINSEL0 & 0xfffffff0) | 0x05;    /* 选择管脚为UART0 */
#if	UART0_MULTIPLEX_EN >= 1
	IO0SET = IO0SET | ( UART0_A );
	IO0SET = IO0SET | ( UART0_B );
#endif
	U0LCR = 0x80;                               /* 允许访问分频因子寄存器 */
	Fdiv = (Fpclk / 16) / uart0_config[UART0_INIT].bps;                  /* 设置波特率 */
	U0DLM = Fdiv / 256;
	U0DLL = Fdiv % 256;
	U0LCR = 0x03;                               /* 禁止访问分频因子寄存器 */
	/* 且设置为8,1,n */
	U0IER = 0x05;                               /* 允许接收和发送中断 */
	U0FCR = 0x87;                               /* 初始化FIFO */

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

	//PINSEL0 = (PINSEL0 & 0xfffffff0) | 0x05;    /* 选择管脚为UART0 */

	U0LCR = 0x80;                               /* 允许访问分频因子寄存器 */
	Fdiv = (Fpclk / 16) / bps;                  /* 设置波特率 */
	U0DLM = Fdiv / 256;
	U0DLL = Fdiv % 256;
	U0LCR = 0x03;                               /* 禁止访问分频因子寄存器 */
	/* 且设置为8,1,n */
	U0IER = 0x05;                               /* 允许接收和发送中断 */
	U0FCR = 0x87;                               /* 初始化FIFO */


	OS_EXIT_CRITICAL();


	return 1;

}
#endif

//	申请UART0  资源
uint8 RequestUart0(uint8 ch, uint16 t)
{
	uint8 err;
	OSSemPend(Uart0_Channel_Sem, t, &err);  //	取得资源
	if ((err == OS_NO_ERR) && (uart0_channel_ic_flag != ch))
	{
		uart0_channel_ic_flag = ch;				//	保存当前通道号
#if	UART0_MULTIPLEX_EN >= 1
		Uart0ChangeCh(ch);  					//	切换通道
		QueueFlush(UART0SendBuf);				//	清除发送buffer
		QueueFlush(UART0RecBuf);				//	清除接收buffer
#endif
	}
	/*else if (uart0_channel_ic_flag == ch) {
		QueueFlush(UART0SendBuf);				//	清除发送buffer
		QueueFlush(UART0RecBuf);				//	清除接收buffer
	}*/
	return err;
}

void FreeUart0(void)
{
	OSSemPost(Uart0_Channel_Sem);			//	释放资源
}

/*********************************************************************************************************
** 函数名称: UART0Putch
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
uint8 Uart0SendByte(uint8 data, uint8 flag)
{
	uint8 temp;
	if (flag == 0)  				//	不清空buffer
	{

	}
	else if (flag == 1)  				//	只清空发送buffer
	{
		QueueFlush(UART0SendBuf);
	}
	else if (flag == 2)  			//	清空发送和接收buffer
	{
		QueueFlush(UART0SendBuf);
		QueueFlush(UART0RecBuf);
	}
	else  			//	其它不清空buffer
	{

	}
	if (QueueWrite((void *)UART0SendBuf, data) != QUEUE_OK)           /* 数据入队 */
	{
		return FALSE;
	}

	OS_ENTER_CRITICAL();
	if ((U0LSR & 0x00000020) != 0)
	{
		/* UART0发送保持寄存器空 */
		QueueRead(&temp, UART0SendBuf);             /* 发送最初入队的数据 */
		U0THR = temp;
		U0IER = U0IER | 0x02;                       /* 允许发送中断 */
	}
	OS_EXIT_CRITICAL();
	return TRUE;
}


/*
//	发送字符串
*/
uint8 Uart0SendString(const char *str, uint8 flag)
{
	if (flag == 0)  				//	不清空buffer
	{

	}
	else if (flag == 1)  				//	只清空发送buffer
	{
		QueueFlush(UART0SendBuf);
	}
	else if (flag == 2)  			//	清空发送和接收buffer
	{
		QueueFlush(UART0SendBuf);
		QueueFlush(UART0RecBuf);
	}
	else  			//	其它不清空buffer
	{
		return FALSE;
	}

	//	如果buffer空间不够存放这么多数据，返回错误，等上层判断，是否发送。
	if (strlen(str) > (QueueSize(UART0SendBuf) - QueueNData(UART0SendBuf)))
	{
		return FALSE;
	}
	while (*str)  			//	发送数据到buffer
	{
		Uart0SendByte(*(uint8 *)str++, 0);
	}
	return TRUE;
}

/*
//	发送多数据
*/
uint8 Uart0SendBytes(const uint8 *dat, uint8 len, uint8 flag)
{

	if (flag == 0)  				//	不清空buffer
	{

	}
	else if (flag == 1)  				//	只清空发送buffer
	{
		QueueFlush(UART0SendBuf);
	}
	else if (flag == 2)  			//	清空发送和接收buffer
	{
		QueueFlush(UART0SendBuf);
		QueueFlush(UART0RecBuf);
	}
	else  			//	其它不清空buffer
	{
		return FALSE;
	}
	//	如果buffer空间不够存放这么多数据，返回错误，等上层判断，是否发送。
	if (len > (QueueSize(UART0SendBuf) - QueueNData(UART0SendBuf)))
	{
		return FALSE;
	}
	while (len--)  				//	发送数据到buffer
	{
		Uart0SendByte(*dat++, 0);
	}
	return TRUE;
}


/*
//	接收一个字符
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
** 函数名称: UART0_Exception
** 功能描述: UART0中断服务程序
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
void UART0_Exception(void)
{
	uint8 IIR, temp;

	OS_ENTER_CRITICAL();
	while(((IIR = U0IIR) & 0x01) == 0)
	{
		/* 有中断未处理完 */
		switch (IIR & 0x0e)
		{
		case 0x02:                                  /* THRE中断    */
			if (QueueRead(&temp, UART0SendBuf) == QUEUE_OK)
			{
				U0THR = temp;
			}
			else
			{
				U0IER = U0IER & (~0x02);        /* 队列空，则禁止发送中断 */
			}
			break;


		case 0x06:                                  /* 接收线状态   */
			temp = U0LSR;
			break;
		case 0x04:

		case 0x0c:                                  /* 字符超时指示 */
			do
			{

				QueueWrite(UART0RecBuf, U0RBR);
			}
			while((U0LSR & 0x00000001) != 0);

			OSSemPost(Uart0Sem);                    /* 通知接收任务 */
			//  U0IER = U0IER & (~0x01);                /* 禁止接收及字符超时中断 */
			break;
		default :
			break;
		}
	}
	VICVectAddr = 0;            // 通知中断控制器中断结束
	OS_EXIT_CRITICAL();
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
