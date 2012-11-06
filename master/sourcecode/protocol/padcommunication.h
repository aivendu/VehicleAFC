#ifndef _PAD_COMMUNICATION_H
#define _PAD_COMMUNICATION_H
#include "sys_state.h"


/*
typedef union {
	_exe_s shutdown;			//	执行下点操作
	_exe_s makechange;			//	执行找零
	_exe_s print;				//	执行打印
	_exe_s config_match;		//	执行配置匹配
	_exe_s logout;				//	执行注销
} _sys_exe_st_s;

typedef union {
	uint8 se_b[sizeof(_sys_exe_st_s)];
	_sys_exe_st_s se;
} _sys_exe_st_u;
*/

typedef struct {
	uint8 len;				//	发送数据的长度, 包括"包号","错误号","命令字","参数","数据"
	uint8 backage_num;		//	发送数据的包号
	uint16 err_no;			//	返回PAD 数据时的错误号
	uint8 cmd;				//	发送的命令字
	uint8 arg;				//	发送的参数
	uint8 *dat;				//	发送的数据，从参数后一个字节记起
}_df_device_and_pad_comm;


typedef struct {
	_variable_flag_s change_flag;	//	命令改变过的标记
	uint8 old_package_num;	//	上一条数据的包号
	uint8 package_num;		//	收到的数据的包号
	uint8 comm_task_prio;	//	通信的Task 优先级
	uint16 err_no;		//	收到PAD返回数据的错误号
	uint8 cmd;			//	收到的数据的命令字
	uint8 arg;			//	收到的数据的参数
	uint8 len;			//	接收到的数据的长度，从参数后一个字节记起
	uint8 data[248];	//	收到的数据，从参数后一个字节记起
} _pad_com_task;


typedef struct {
	uint8 cmd;			//	命令字
	_pfunc *func;		//	对于命令字的处理函数
} _rj45_command_s;


#define PAD_ACK			0x06
#define PAD_NACK		0x15

#define RETRY_TIME					3
#define RJ45_COMM_NUMBER			11

#define RJ45_CTS					(1<<15)
#define RJ45_RTS					(1<<16)
#define RJ45_RST					(1<<17)

//	PAD 通信错误代码
/*
1.	0x0000：无错
2.	0x0101：普通错误
3.	0x0102：当前不能交易，请先登录
4.	0x0103：没有交易信息，不能找零
5.	0x0104：正在找零，该命令无效
6.	0x0105：设备异常
7.	0x0181：无效命令
8.	0x0182：载入路线编号和原编号相同,仅主界面和找零界面会触发
9.	0x0183：载入路线编号和原编号不同,仅主界面和找零界面会触发,   登录界面将以后一个路线编号为准
10.	0x0184：找不到GUID对应的文件
11.	0x0185：没有接收到GUID
*/
#define PAD_COMMUNICATION_NO_ERROR					0X0000
#define PAD_COMMUNICATION_GENERAL_ERROR				0x0101
#define PAD_COMMUNICATION_NEED_LOGIN_ERROR			0x0102
#define PAD_COMMUNICATION_NO_RIDE_MESSEGE_ERROR		0x0103
#define PAD_COMMUNICATION_IS_CHANGE_ERROR			0x0104
#define PAD_COMMUNICATION_DEVICE_ERROR				0x0105
#define PAD_COMMUNICATION_INVAILD_COMMAND			0x0181
#define PAD_COMMUNICATION_SAME_GUID					0x0182
#define PAD_COMMUNICATION_DIFFERENT_GUID			0x0183
#define PAD_COMMUNICATION_NO_FILE_WITH_GUID			0x0184
#define PAD_COMMUNICATION_NO_GUID					0x0185
#define PAD_COMMUNICATION_FILE_FORM_ERROR			0x0186


















extern uint8 old_station;
extern uint8 curr_station;
extern uint8 run_direction;
extern uint8 changesite_flag;

extern void PadCommInit(void);
extern void TaskDeviceCommand(void *pdata);
extern void TaskPADRecHandle(void *pdata);

#endif
