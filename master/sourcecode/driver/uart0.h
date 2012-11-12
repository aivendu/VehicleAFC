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
#ifndef IN_UART0
#define IN_UART0

//	uart0 配置宏定义
#define UART0_MULTIPLEX_EN			0		//	串口0 硬件复用功能使能




//#define UART0_FIFO_LENGHT         16
#define UART0_FIFO_LENGHT         8

#define UART0_SEND_QUEUE_LENGHT		128
#define UART0_REC_QUEUE_LENGHT		128

#define UART0_A 		(1<<12)
#define UART0_B 		(1<<13)

typedef enum {
	UART0_INIT,
	GPRS,
} _uart0_fun;


typedef struct {
	uint8 channal;
	//uint8 unused[3];
	uint32 bps;
} _uart_config_s;


//extern OS_EVENT *Uart0_Channel_Sem;

/*********************************************************************************************************
** 函数名称: UART0Init
** 功能描述: 初始化UART0 
** 输　入: bps：波特率
**
** 输　出:TRUE  :成功
**        FALSE:失败
** 全局变量: Uart0Sem
** 调用模块: QueueCreate,OSSemCreate
********************************************************************************************************/
extern uint8 UART0Init(void);

//	请求UART0 资源
extern uint8 RequestUart0(uint8 ch,uint16 t);

//	 释放资源
extern void FreeUart0(void);



/*********************************************************************************************************
** 函数名称: Uart0SendByte
** 功能描述: 发送一个字节数据 
** 输　入: data：发送的数据
**			flag:是否清空buffer。 0--不清空；1--清空发送buffer；2--清空发送和接收buffer；
** 输　出:无
** 全局变量: 无
** 调用模块: QueueWrite,QueueRead
********************************************************************************************************/
extern uint8 Uart0SendByte(uint8 data, uint8 flag);

/*********************************************************************************************************
** 函数名称: Uart0SendString
** 功能描述: 发送一个字符串 
** 输　入: str:发送数据存储位置
**        flag:是否清空buffer。 0--不清空；1--清空发送buffer；2--清空发送和接收buffer；
** 输　出:无
** 全局变量: 无
** 调用模块: Uart0SendByte
********************************************************************************************************/
extern uint8 Uart0SendString(const char * str,uint8 flag);

/*********************************************************************************************************
** 函数名称: Uart0SendBytes
** 功能描述: 发送多个字节数据 
** 输　入: dat:发送数据存储位置
**        flag:发送数据个数
** 输　出:无
** 全局变量: 无
** 调用模块: Uart0SendByte
********************************************************************************************************/
extern uint8 Uart0SendBytes(const uint8 *dat, uint8 len, uint8 flag);


/*********************************************************************************************************
** 函数名称: UART0Getch
** 功能描述: 接收一个字节数据
** 输　入: 	dat:	
** 输　出: 接收到的数据
** 全局变量: 无
** 调用模块: OSSemPend
********************************************************************************************************/
extern uint8 Uart0RecByte(uint8 * const dat,uint8 flag,uint32 timedelay);

/*********************************************************************************************************
** 函数名称: UART0_Exception
** 功能描述: UART0中断服务程序
** 输　入: 无
**
** 输　出: 无
**         
** 全局变量: 无
** 调用模块: OSSemPost,QueueRead
********************************************************************************************************/
extern void UART0_Exception(void);


extern uint8 Uart0WriteFull(DataQueue *Buf, QUEUE_DATA_TYPE Data, uint8 Mod);
#endif

//#define UART0_SEND_QUEUE_LENGHT   60    /* 给UART0发送数据队列分配的空间大小 */
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
