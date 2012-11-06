#ifndef _SYS_ERR_H
#define _SYS_ERR_H

/*	主要标志位，用于指示设备的主要状态4byte*/
typedef struct {
	uint32 login_state:			4;
	uint32 server_comm_state:	2;
	uint32 pad_comm_state:		2;
	uint32 init_end_flag:		1;
	uint32 trade_allowtrade:	1;
	uint32 device_state:		1;
	uint32 unused:				21;
} _major_flag_sys_s;

typedef union {
	uint8 mfs_byte[sizeof(_major_flag_sys_s)];
	_major_flag_sys_s mfs_v;
} _major_flag_sys_u;

/*	主板状态标志位，用于指示板载器件和软件的状态，8byte*/
typedef struct {
	uint32 a;
} _board_state_flag_s;

typedef union {
	uint8 bsf_byte[sizeof(_board_state_flag_s)];
	_board_state_flag_s bsf_v;
} _board_state_flag_u;

/*	其它标志位用于指示其它外围设备的状态4byte*/
typedef struct {
	uint32 ic_machine_state:	4;
	uint32 print_machine_state:	4;
	uint32 bill_machine_state:	4;
	uint32 coin_machine_state:	4;
	uint32 gps_machine_state:	4;
	uint32 gprs_device_state:	4;
} _other_flag_sys_s;

typedef union {
	uint8 ofs_byte[sizeof(_other_flag_sys_s)];
	_other_flag_sys_s ofs_v;
} _other_flag_sys_u;

/*	系统标志汇总*/
typedef struct {
	_major_flag_sys_s	major_flag;
	_board_state_flag_s	board_state;
	_other_flag_sys_s	other_state;
} _vehicle_afc_state_flag_s;

typedef union {
	uint8 vasf_byte[sizeof(_vehicle_afc_state_flag_s)];
	_vehicle_afc_state_flag_s vasf_v;
} _vehicle_afc_state_flag_u;


/*	错误定义*/
#define VAFC_NO_ERROR				0x00
#define COMM_CHECK_ERROR			0x01
#define COMM_COMMAND_ERROR			0x02
#define VAFC_LENGHT_ERROR			0x03
#define IC_CARD_TYPE_ERROR			0x04
#define IC_NO_CARD_ERROR			0x05
#define IC_CARD_INVALID_ERROR		0x06
#define USER_NAME_ERROR				0x07
#define USER_PASSWORD_ERROR			0x08
#define GPS_NO_LOCATION_ERROR		0x09
//#define 



/*	系统模块错误定义*/
#define VAFC_SYS_NO_ERROR				0x00
#define VAFC_PAD_COMM_ERROR				0x01
#define VAFC_SERVER_COMM_ERROR			0x02
#define VAFC_IC_MACHINE_ERROR			0X03



/*	定义错误编号*/
#define SYS_NO_ERR							0
#define COMMUNICATION_TIMEOUT				4
#define	COMMAND_ERR_EXE_CLASH				5				//	命令冲突
#define COMMAND_ERR_ST_INVALID				6				//	命令状态无效
#define COMMAND_ERR_LOST					7				//	上个命令丢失
#define COMMAND_ERR_CANNOT_CHANGE			8				//	命令状态不能改变
#define UART_COMMUNICATION_ERR_COMM_TIMEOUT				9				//	通信超时
#define UART_COMMUNICATION_ERR_COMM_NACK				10				//	通信回复NACK
#define UART_COMMUNICATION_ERR_DATA_REC_TIMEOUT			11				//	通信数据返回超时
#define UART_COMMUNICATION_ERR_RETURN_DATA_ERR			12				//	通信返回数据异常
#define CRT188_COMMAND_EXE_ERR				13				//	crt188 命令执行错误，crt188 返回'N'
#define CRT188_NO_CARD						14				//	crt188 卡机内无卡
#define CRT188_HAVE_CARD					16				//	crt188 卡机内有卡
#define SYS_ERR_CRT188_SERIAL_PORT_FAULT	17				//	crt188 串口故障
#define	SYS_ERR_CRT188_MACHINE_FAULT		18				//	crt188 卡机故障
#define PAD_COMMUNICATION_DATA_ERR			19				//	
#define PARAMITER_ERR						20







#endif
