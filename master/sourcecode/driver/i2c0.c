#include "includes.h"

static uint8 *i2c_rbuf;
static OS_EVENT *I2cSem;
static OS_EVENT *I2cMbox;
static int16 i2c_rnbyte;
static uint8 I2cAddr;
static uint8 i2c_mode;
static uint16 i2c_wnbyte;
static uint8 *i2c_wbuf;

/*********************************************************************************************************
** 函数名称: I2cInit
** 功能描述: 初始化I2c（主模式）
** 输　入: FI2c:I2c总线频率
**
** 输　出:TRUE  :成功
**        FALSE:失败
** 全局变量: I2cSem,I2cMbox
** 调用模块: OSSemCreate
**
** 作　者: 陈明计
** 日　期: 2003年7月8日
**-------------------------------------------------------------------------------------------------------
** 修改人: 陈明计
** 日　期: 2003年7月10日
**-------------------------------------------------------------------------------------------------------
** 修改人: 陈明计
** 日　期: 2003年7月21日
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
void I2c0Init(uint32 rate)

{
    VICIntEnClr = 1 << 9;                           /* 禁止能I2c中断 */
    if (rate > 400000)
    {
    }
    PINSEL0 = (PINSEL0 & 0xffffff0f) | 0x50;    /* 选择管脚为I2c */
    I2CONCLR = 0x6C;                            /* 清除控制寄存器 */
    I2SCLH = (Fpclk / rate + 1) / 2;            /* 设置高电平时间 */
    I2SCLL = (Fpclk / rate) / 2;                /* 设置低电平时间 */
    I2cSem = OSSemCreate(1);                    /* 信号量用于互斥操作总线 */
	if (I2cSem == NULL) {
		while(1);
	}
    I2cMbox = OSMboxCreate(NULL);               /* 消息邮箱用于中断向任务传递操作结果 */
    if (I2cMbox == NULL)
    {
        while(1);
    }
}



/*********************************************************************************************************
** 函数名称: I2c0SendBytes
** 功能描述: 读I2C，但不发送STOP信号 
** 输　入: Addr:从机地址
**        Data:将要写的数据
**        写的数据数目
** 输　出:发送的数据字节数
**
** 全局变量: I2cAddr,i2c_rnbyte,i2c_rbuf
** 调用模块: OSMboxPend
**
** 作　者: 陈明计
** 日　期: 2003年7月8日
**-------------------------------------------------------------------------------------------------------
** 修改人: 陈明计
** 日　期: 2003年7月21日
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint16 I2c0WriteBytes(uint8 chip_addr, uint8 *wdat, uint16 wnbyte)
{
    uint8 err;
    unsigned int Rt;

	I2CONCLR = 0x6C;
	I2CONSET = 0x40;							/* 使能I2c */

	i2c_mode = 1;
    I2cAddr = chip_addr & 0xfe;                              /* 存储发送地址 */
    i2c_wnbyte = wnbyte;                                   /* 存储写字节数 */
    i2c_wbuf = wdat;                                      /* 存储写的数据的指针 */
    I2CONSET = 0x24;                                    /* 设置为主机，并启动总线 */
    VICIntEnable = 1 << 9;                      /* 使能I2c中断 */
    Rt = (unsigned int) OSMboxPend(I2cMbox, 0, &err);   /* 等待操作结束 */
	VICIntEnClr = 1 << 9;					   /* 禁止能I2c中断 */
    return Rt;
}

uint16 I2c0WriteMemery(uint8 chip_addr, uint8 *addr, uint8 addr_len, uint8 *wdat, uint16 wnbyte)
{
	uint16 err;
	err = I2c0WriteBytes(chip_addr,addr,addr_len);
	if (err != I2C_WRITE_END)
	{
		return err;
	}
	err = I2c0WriteBytes(chip_addr,wdat,wnbyte);
	return err;
}

/*********************************************************************************************************
** 函数名称: I2cRead
** 功能描述: 从I2c从器件读数据
** 输　入: Addr:从机地址
**        Ret:指向返回数据存储位置的指针
**        Eaddr:扩展地址存储位置
**        EaddrNByte:扩展地址字节数，0为无
**        ReadNbyte:将要读取的字节数目
** 输　出:已读取的字节数
**
** 全局变量: I2cSem,I2cAddr,i2c_rnbyte,i2c_rbuf
** 调用模块: OSSemPend,__I2cWrite,OSMboxPend,OSSemPost
**
** 作　者: 陈明计
** 日　期: 2003年7月8日
**-------------------------------------------------------------------------------------------------------
** 修改人: 陈明计
** 日　期: 2003年7月21日
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint16 I2c0ReadBytes(uint8 chip_addr,uint8 *rdat,int16 rnbyte)
{
    uint8 err;
    
    //OSSemPend(I2cSem, 0, &err);

    I2CONCLR = 0x6C;
    I2CONSET = 0x40;                            /* 使能I2c */
	i2c_mode = 2;
    I2cAddr = chip_addr | 0x01;                  /* 存储发送地址 */
    i2c_rnbyte = rnbyte;                      	 /* 存储读字节数 */
    i2c_rbuf = rdat;                               /* 存储读到的数据 */
    I2CONCLR = 0x28;
    I2CONSET = 0x24;                            /* 设置为主机，并启动总线 */
    VICIntEnable = 1 << 9;                      /* 使能I2c中断 */

    OSMboxPend(I2cMbox, 0, &err);               /* 等待操作结束 */

    VICIntEnClr = 1 << 9;                       /* 禁止能I2c中断 */
    //OSSemPost(I2cSem);
    return (rnbyte - i2c_rnbyte);
}

uint16 I2c0WriteReadBytes(uint8 chip_addr, uint8 *wdat, uint8 wnbyte, uint8 *rdat, uint16 rnbyte){
	uint8 err;
	
	err = 0;
	I2CONCLR = 0x6C;
	I2CONSET = 0x40;							/* 使能I2c */
	
	i2c_mode = 3;
    I2cAddr = chip_addr & 0xfe;                              /* 存储发送地址 */
    i2c_rnbyte = rnbyte;                                   /* 存储写字节数 */
    i2c_rbuf = rdat;                                      /* 存储写的数据的指针 */
	i2c_wbuf = wdat;
	i2c_wnbyte = wnbyte;
	I2CONCLR = 0x28;
    I2CONSET = 0x24;                                    /* 设置为主机，并启动总线 */
	VICIntEnable = 1 << 9;                      /* 使能I2c中断 */
	OSMboxPend(I2cMbox, 0, &err);				/* 等待操作结束 */
	
	VICIntEnClr = 1 << 9;					   /* 禁止能I2c中断 */
	//OSSemPost(I2cSem);
	return (rnbyte - i2c_rnbyte);
	
}

/*********************************************************************************************************
** 函数名称: I2c_Exception
** 功能描述: I2c中断服务程序
** 输　入: 无
**
** 输　出: 无
**         
** 全局变量: I2cAddr,i2c_rbuf,i2c_rnbyte,I2cMbox
** 调用模块: OSMboxPost
**
** 作　者: 陈明计
** 日　期: 2003年7月8日
**-------------------------------------------------------------------------------------------------------
** 修改人: 陈明计
** 日　期: 2003年7月21日
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
        void I2c0_Exception(void)
{
    OS_ENTER_CRITICAL();
    switch(I2STAT & 0xf8)
    {
        case  0x08:                     /* 已发送起始条件，与0x18相同处理 */
//            break;
        case  0x10:                     /* 已发送重复起始条件 */
            I2DAT = I2cAddr;            /* 发送地址 */
            I2CONCLR = 0x28;            /* 清除标志 */
            break;
        case  0x18:                     /* 已发送SLA+W，并已接收应答 */
            I2DAT = *i2c_wbuf++;
            i2c_wnbyte--;
            I2CONCLR = 0x28;            /* 清除标志 */
            break;
        case  0x28:                     /* 已发送I2C数据，并接收到应答 */
			
            if (i2c_wnbyte > 0)
            {
                I2DAT = *i2c_wbuf++;
                i2c_wnbyte--;
                I2CONCLR = 0x28;        /* 清除标志 */
            }
            else
            {
	            if (i2c_mode == 3) {
					//I2DAT = I2cAddr | 0x01;
					I2cAddr = I2cAddr&(~0x01)|0x01;
					I2CONCLR = 0x28;
					I2CONSET = 0x24;
	            }
				else {
	                OSMboxPost(I2cMbox, (void *)I2C_WRITE_END);
	                VICIntEnClr = 1 << 9;   /* 禁止能I2c中断 */
				}
			}
            break;
        case  0x20:                     /* 已发送SLA+W；已接收非ACK, 与0x48处理相同 */
//            break;                    
        case  0x30:                     /* 已发送I2DAT中的数据字节；已接收非ACK, 与0x48处理相同 */
//            break;
        case  0x48:                     /* 已发送SLA+R；已接收非ACK */
            I2CONSET = 1 << 4;          /* 发送停止信号 */
            OSMboxPost(I2cMbox, (void *)I2C_ACK_ERR);
            I2CONCLR = 0x28;            /* 清除标志 */
            break;
        case  0x38:                     /* 在SLA+R/W或数据字节中丢失仲裁 */
            OSMboxPost(I2cMbox, (void *)I2C_NOT_GET_BUS);
            I2CONCLR = 0x28;            /* 清除标志 */
            break;
        case  0x40:                     /* 已发送SLA+R；已接收ACK */
            if (i2c_rnbyte <= 1)
            {
                I2CONCLR = 1 << 2;      /* 下次发送非应答信号 */
            }
            else
            {
                I2CONSET= 1 << 2;       /* 下次发送应答信号 */
            }
            I2CONCLR = 0x28;            /* 清除标志 */
            break;
        case  0x50:                     /* 已接收数据字节；已发送ACK */
            *i2c_rbuf++ = I2DAT;          /* 接收数据 */
            i2c_rnbyte--;
            if (i2c_rnbyte <= 1)
            {
                I2CONCLR = 1 << 2;      /* 下次发送非应答信号 */
            }
            I2CONCLR = 0x28;            /* 清除标志 */
            break;
        case  0x58:                     /* 已接收数据字节；已返发送ACK */
            *i2c_rbuf = I2DAT;            /* 接收数据 */
            i2c_rnbyte--;
            I2CONSET= 1 << 4;           /* 结束总线 */
            OSMboxPost(I2cMbox, (void *)I2C_READ_END);
            I2CONCLR = 0x28;            /* 清除标志 */
            break;
        default:
            I2CONCLR = 0x28;            /* 清除标志 */
            break;
    }

    VICVectAddr = 0;                    /* 通知中断控制器中断结束 */
    OS_EXIT_CRITICAL();
}

