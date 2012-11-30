#ifndef _SYS_STATE_H
#define _SYS_STATE_H




/*	定义错误编号*/
#define SYS_NO_ERR						0
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




typedef struct
{
	uint32 st_user				: 4;
	uint32 st_server_online		: 2;
	uint32 st_pad_online		: 2;
	uint32 st_init				: 1;
	uint32 st_allow_trade		: 1;
	uint32 st_device_abort		: 1;
	uint32 unused				: 21;
} _sys_st_major_s;

typedef union
{
	uint8 ssm_b[sizeof(_sys_st_major_s)];
	_sys_st_major_s ssm;
} _sys_st_major_u;

typedef struct
{
	uint32 st_ic_machine		: 4;
	uint32 st_print_machine		: 4;
	uint32 st_bill_machine		: 4;
	uint32 st_coin_machine		: 4;
	uint32 st_gps_machine		: 4;
	uint32 st_motherboard		: 4;
	uint32 unused1				: 8;
	uint32 unused2				: 32;
	uint32 unused3				: 32;
} _sys_st_other_s;

typedef union
{
	uint8 sso_b[sizeof(_sys_st_other_s)];
	_sys_st_other_s sso;
} _sys_st_other_u;

typedef struct
{
	uint8 exe_flag				: 1;		//	是否可以操作该命令,0--可以，1--不可以
	uint8 exe_unused			: 3;
	uint8 exe_st				: 4;
	/*	状态说明
	 *	0--等待命令，
	 *	1--有命令还未执行，
	 *	2--命令执行中，
	 *	3--命令执行结束，
	 *	4--执行异常，
	 *	15--命令无效，*/
} _exe_s;

#define CMD_WITE			0
#define CMD_NO_EXE			1
#define CMD_RUNNING			2
#define CMD_EXE_END			3
#define CMD_ABORT			14
#define CMD_INALID			15

typedef union
{
	_exe_s shutdown;			//	执行下点操作
	_exe_s makechange;			//	执行找零
	_exe_s print;				//	执行打印
	_exe_s config_match;		//	执行配置匹配
	_exe_s logout;				//	执行注销
} _sys_exe_st_s;

typedef union
{
	uint8 se_b[sizeof(_sys_exe_st_s)];
	_sys_exe_st_s se;
} _sys_exe_st_u;

typedef struct
{
	_sys_st_major_u	st_major;
	_sys_st_other_u st_other;
	_sys_exe_st_u   st_cmd;
} _sys_st_s;

typedef union
{
	uint8 ss_b[sizeof(_sys_st_s)];
	_sys_st_s ss;
} _sys_st_u;

typedef enum
{
    EXE_WAIT		,
    EXE_WRITED		,
    EXE_RUNNING		,
    EXE_RUN_ABORT	,
    EXE_INVALID
} _exe_st_e;

extern _exe_s	coinchange_exe;
extern _exe_s	notechange_exe;
extern _exe_s	print_exe;
extern _exe_s  print_amount_exe;
extern _sys_st_u sys_state;

#define GetMakeChangeCommand()		(sys_state.ss.st_cmd.se.makechange)
#define GetPrintCommand()			(sys_state.ss.st_cmd.se.print.exe_st == EXE_WRITED)
#define SetPrintCommand(c)			(sys_state.ss.st_cmd.se.print.exe_st = c)
#define GetShutDownCommand()		(sys_state.ss.st_cmd.se.shutdown)
#define GetLogoutCommand()			(sys_state.ss.st_cmd.se.logout)
#define GetcConfigMatch()			(sys_state.ss.st_cmd.se.config_match)

uint8 GetExeCommand(_exe_s *comm);
uint8 SetExeCommand(_exe_s *comm, uint8 st, uint8 *err);


#endif
