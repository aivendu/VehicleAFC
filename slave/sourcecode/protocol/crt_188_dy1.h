#ifndef _CRT_188_DY1_H
#define _CRT_188_DY1_H

#define CHECK_DATA_LENGHT			16
#define CRT188_MAX_DATA_LENGHT		135


#define CRT188_STX	0X02	//通讯文本起始字符 
#define CRT188_ETX	0X03	//通讯文本结束字符 
#define CRT188_ENQ	0X05	//发送请求命令（主机->读卡器） 
#define CRT188_ACK	0X06	//肯定应答（读卡器->主机） 
#define CRT188_NAK	0X15	//否定应答（读卡器->主机） 
#define CRT188_EOT	0X04	//取消通信 

#define CRT188_CMD_RESET			0x30		//复位卡机，返回卡机软件版本信息 
#define CRT188_CMD_READ_SERIAL		0x30		//读写序列号
#define CRT188_CMD_READ_STATE		0x31		//查状态
#define CRT188_CMD_TEST_IC_TYPE		0x31		//测 IC 卡类型
#define CRT188_CMD_IC_POWER_OP		0x33		//IC 卡上下电
#define CRT188_CMD_SIM_POWER_OP		0x4A		//SIM 卡下电
#define CRT188_CMD_SET_UART_BPS		0x34		//设置串口波特率

#define CRT188_ARV_RESET_1			0x30		//复位卡机，返回卡机软件版本信息 
#define CRT188_ARV_RESET_2			0x31		//复位卡机，返回卡机软件版本信息 
#define CRT188_ARV_RESET_3			0x32		//复位卡机，返回卡机软件版本信息
#define CRT188_ARV_SERIAL_READ		0x3A		//读卡器的序列号  
#define CRT188_ARV_SERIAL_WRITE		0x3B		//写读卡器的序列号 
#define CRT188_ARV_SERSOR_STATE		0x2F		//查卡机各传感器的状态信息  
#define CRT188_ARV_CARD_STATE		0x30		//查卡机状态（卡机内有无卡等）信息
#define CRT188_ARV_IC_TYPE			0x31		//测 IC 卡类型
#define CRT188_ARV_IC_POWER_ON		0x30		//IC 卡上电
#define CRT188_ARV_IC_POWER_OFF		0x31		//IC 卡下电
#define CRT188_ARV_SIM_POWER_OFF	0x31		//SIM 卡下电 
#define CRT188_ARV_1200_BPS			0x30		//UART=1200 BPS 
#define CRT188_ARV_2400_BPS			0x31		//UART=2400 BPS 
#define CRT188_ARV_4800_BPS			0x32		//UART=4800 BPS 
#define CRT188_ARV_9600_BPS			0x33		//UART=9600 BPS 
#define CRT188_ARV_19200_BPS		0x34		//UART=19200 BPS 
#define CRT188_ARV_38400_BPS		0x35		//UART=38400 BPS 

#define CARD_TYPE_NO_CARD			1
#define CARD_TYPE_UNKOWN_TYPE		2
#define CARD_TYPE_POS_ERR			3
#define CARD_TYPE_M1				4
#define CARD_TYPE_T0_CPU			5
#define CARD_TYPE_T1_CPU			6
#define CARD_TYPE_24C01				7
#define CARD_TYPE_24C02				8
#define CARD_TYPE_24C04				9
#define CARD_TYPE_24C08				10
#define CARD_TYPE_24C16				11
#define CARD_TYPE_24C32				12
#define CARD_TYPE_24C64				13
#define CARD_TYPE_SL4442			14
#define CARD_TYPE_SL4428			15
#define CARD_TYPE_AT88S102			16
#define CARD_TYPE_AT88S1604			17
#define CARD_TYPE_AT45D041			18
#define CARD_TYPE_AT88SC1608		19


typedef struct
{
	uint8  lenght_h;
	uint8  lenght_l;
	uint8  cmd;
	uint8  arg;
	uint8  dat_lenght;
	uint8  dat[CRT188_MAX_DATA_LENGHT];
} _crt188_cmd_s;

typedef struct
{
	uint8  		stx;
	_crt188_cmd_s	*data;
	uint8		ext;
	uint8		bcc;
} _crt188_s;

typedef struct
{
	uint8  stx;
	uint16 lenght;
	uint8  no;
	uint8  cmd;
	uint8  err_code;
	uint8  ext;
	uint8  bcc;
} _crt188_err_return_s;

typedef enum
{
    CRT188_CMD_ERR = 0x00,				//  命令字错误，发送的通讯包中有不符合通讯协议规定的命令字 CM
    CRT188_ARG_ERR = 0x01,				//  命令参数错误，发送的通讯包中有不符合通讯协议规定的命令参数 PM
    CRT188_NO_EXE_ERR = 0x02,			//  命令不能被执行，发送的命令受限制不能执行该命令。
    CRT188_DATA_ERR = 0x04				//  命令数据包错误，发送的通讯包中数据包部分有不符合通讯协议规定的数据。
} _crt188_err_code_e;

typedef struct
{
	uint8 crt188_state;
	uint8 errcode;
	_card_plaintxt_u df_card;
} _card_read_s;
/*
typedef struct {

} _card_write_s;
*/
extern _card_plaintxt_u card_read;

extern void Crt188Init(void);
extern void TaskICCard(void *pdata);





#endif
