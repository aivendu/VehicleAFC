/****************************************Copyright (c)**************************************************
**                               广州周立功单片机发展有限公司
**                                     研    究    所
**                                        产品一部
**
**                                 http://www.zlgmcu.com
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: uart0.h
**创   建   人: 陈明计
**最后修改日期: 2003年7月4日
**描        述: μCOS-II下LPC210x的UART0底层驱动头文件
**
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 陈明计
** 版  本: v1.0
** 日　期: 2003年7月4日
** 描　述: 原始版本
**
**--------------当前版本修订------------------------------------------------------------------------------
** 修改人: 陈明计
** 日　期: 2003年7月9日
** 描　述: 更正注释
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef IN_UART1
#define IN_UART1

#define	UART1_MULTIPLEX_EN			0

#define UART1_FIFO_LENGHT         16
#define UART1_SEND_QUEUE_LENGHT		128
#define UART1_REC_QUEUE_LENGHT		128

#define UART1_A						(1<<17)
#define UART1_B						(1<<18)

typedef enum
{
    UART1_INIT,
    RJ45,
} _uart1_fun;



/*********************************************************************************************************
** 函数名称: UART1Init
** 功能描述: 初始化UART1
** 输　入: bps：波特率
**
** 输　出:TRUE  :成功
**        FALSE:失败
** 全局变量: Uart1Sem
** 调用模块: QueueCreate,OSSemCreate
********************************************************************************************************/
extern uint8 UART1Init(void);

//	请求UART0 资源
extern uint8 RequestUart1(uint8 ch, uint16 t);


//	 释放资源
extern void FreeUart1(void);



/*********************************************************************************************************
** 函数名称: Uart1SendByte
** 功能描述: 发送一个字节数据
** 输　入: data：发送的数据
**			flag:是否清空buffer。 0--不清空；1--清空发送buffer；2--清空发送和接收buffer；
** 输　出:无
** 全局变量: 无
** 调用模块: QueueWrite,QueueRead
********************************************************************************************************/
extern uint8 Uart1SendByte(uint8 data, uint8 flag);

/*********************************************************************************************************
** 函数名称: Uart1SendString
** 功能描述: 发送一个字符串
** 输　入: str:发送数据存储位置
**        flag:是否清空buffer。 0--不清空；1--清空发送buffer；2--清空发送和接收buffer；
** 输　出:无
** 全局变量: 无
** 调用模块: Uart1SendByte
********************************************************************************************************/
extern uint8 Uart1SendString(const char *str, uint8 flag);

/*********************************************************************************************************
** 函数名称: Uart1SendBytes
** 功能描述: 发送多个字节数据
** 输　入: dat:发送数据存储位置
**        flag:发送数据个数
** 输　出:无
** 全局变量: 无
** 调用模块: Uart1SendByte
********************************************************************************************************/
extern uint8 Uart1SendBytes(const uint8 *dat, uint8 len, uint8 flag);


/*********************************************************************************************************
** 函数名称: UART1Getch
** 功能描述: 接收一个字节数据
** 输　入: 	dat:
** 输　出: 接收到的数据
** 全局变量: 无
** 调用模块: OSSemPend
********************************************************************************************************/
extern uint8 Uart1RecByte(uint8 *const dat, uint8 flag, uint32 timedelay);

/*********************************************************************************************************
** 函数名称: UART1_Exception
** 功能描述: UART1中断服务程序
** 输　入: 无
**
** 输　出: 无
**
** 全局变量: 无
** 调用模块: OSSemPost,QueueRead
********************************************************************************************************/
extern void UART1_Exception(void);



#endif

//#define UART0_SEND_QUEUE_LENGHT   60    /* 给UART0发送数据队列分配的空间大小 */
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
