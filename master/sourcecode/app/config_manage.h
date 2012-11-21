#ifndef _CONFIG_MANAGE_H
#define _CONFIG_MANAGE_H

#include "uart0.h"

#define CONFIG_SAVE_START_ADDR			256

//	可设置的地址范围，超出范围固定为FFFF
#define MAX_DEVICE_ADDR					0xFFFF
#define MIN_DEVICE_ADDR					0x1000

//	定义和控制系统功能
typedef struct
{
	uint32	config_state			:2;		//	配置状态，0--未初始化，1--默认初始化，2--用户配置
	uint32	login_mod				:2;		//	登录模式，0--卡登录，1--帐号登录，3--混合登录
	uint32	trade_upload_state		:1;		//	交易数据上传完成，0--数据上传完成，1--还有没上传的数据
	uint32	intelligent_change		:1;		//	智能找零功能，可以根据钱箱余额分配找零张数，0--禁止，1--使能
	uint32	unused					:27;
} _function_config_s;

typedef union
{
	_function_config_s fc;
	uint32 fc_b[sizeof(_function_config_s)];							//	限定只能占用4 byte
} _function_config_u;


//	定义和控制系统参数
typedef struct
{
	uint16	gprs_answer_response_time;	//	GPRS 最迟响应时间
	uint16	gps_sampling_time;			//	GPS 采样时间
	uint16	gprs_offline_response_time;	//	GPRS 脱机响应时间
	uint16	login_remain_time;			//	登录保持时间
	uint16	server_port;				//	服务器端口号
	uint8	server_ip[4];				//	服务器IP 地址
} _parameter_s;

typedef union
{
	_parameter_s pa;
	uint8 pa_b[sizeof(_parameter_s)];	//	限定只能占用18 byte
} _parameter_u;


//	钱箱设置
typedef struct
{
	uint8	cashbox1_value;		//	硬币钱箱面值，单位1 分
	uint8	cashbox2_value;		//	纸币钱箱一面值，单位元
	uint8	cashbox3_value;		//	纸币钱箱二面值，单位元
	uint8	cashbox1_alarm_threshold;	//	硬币钱箱报警阈值
	uint8	cashbox2_alarm_threshold;	//	纸币钱箱一报警阈值
	uint8	cashbox3_alarm_threshold;	//	纸币钱箱二报警阈值
	uint16	cashbox1_balance;	//	硬币钱箱余额
	uint16	cashbox2_balance;	//	纸币钱箱一报余额
	uint16	cashbox3_balance;	//	纸币钱箱二报余额
	uint16	cashbox1_deposit;	//	硬币钱箱存入总额
	uint16	cashbox2_deposit;	//	纸币钱箱一存入总额
	uint16	cashbox3_deposit;	//	纸币钱箱二存入总额
	uint8	unused[2];	//	
}_cashbox_config_s;

typedef union
{
	_cashbox_config_s cc;
	uint8 cc_b[sizeof(_cashbox_config_s)];						//	限定只能占用8 byte
} _cashbox_config_u;


//	串口配置
typedef struct {
	_uart_config_s uc_rj45;
	_uart_config_s uc_gprs;
	_uart_config_s uc_gps;
	_uart_config_s uc_bill;
	_uart_config_s uc_coin;
	_uart_config_s uc_print;
	_uart_config_s uc_voice;
	_uart_config_s uc_icmachine;
	_uart_config_s unused1;
	_uart_config_s unused2;
} _uart_manage_s;

typedef union
{
	_uart_manage_s um;
	uint8 um_b[sizeof(_uart_manage_s)];						//	限定只能占用80 byte
} _uart_manage_u;


//	配置项
typedef struct
{
	char print_customer[16];		//	客户名称，提供打印
	_function_config_u fc;			//	功能配置
	_cashbox_config_u  cc;			//	
	_uart_manage_u um;
	_parameter_u pa;
	char device_addr[4];			//	设备地址, ASCII 字符, 0000-FFFF
	char lisence_plate_num[8];		//	车牌号
	char trade_form_version[6];		//	交易数据格式版本号
	char config_version[9];			//	配置版本号
} _config_s;


#define GetGprsAnswerResponseTime()		(config_ram.pa.pa.gprs_answer_response_time)
#define SetGprsAnswerResponseTime(c)	(config_ram.pa.pa.gprs_answer_response_time = c)
#define GetGpsSamplingTime()			(config_ram.pa.pa.gps_sampling_time)
#define SetGpsSamplingTime(c)			(config_ram.pa.pa.gps_sampling_time = c)
#define GetGprsOfflineResponseTime()	(config_ram.pa.pa.gprs_offline_response_time)
#define SetGprsOfflineResponseTime(c)	(config_ram.pa.pa.gprs_offline_response_time = c)
#define GetLoginRemainTime()			(config_ram.pa.pa.login_remain_time)
#define SetLoginRemainTime(c)			(config_ram.pa.pa.login_remain_time = c)
#define GetServerPort()					(config_ram.pa.pa.server_port)
#define SetServerPort(c)				(config_ram.pa.pa.server_port = c)
#define GetServerIp(c)					(config_ram.pa.pa.server_ip[c])
#define SetServerIp(a,b,c,d)			{config_ram.pa.pa.server_ip[0] = a;\
										 config_ram.pa.pa.server_ip[1] = b;\
										 config_ram.pa.pa.server_ip[2] = c;\
										 config_ram.pa.pa.server_ip[3] = d;}
#define GetLisencePlateNum()			(config_ram.lisence_plate_num)
#define SetLisencePlateNum(c)			(memcpy(config_ram.lisence_plate_num,c,8))

#define GetPrintCustomer()				(config_ram.print_customer)
#define SetPrintCustomer(c)				{memset(config_ram.print_customer,0,sizeof(config_ram.print_customer));\
										 strncpy(config_ram.print_customer,c,sizeof(config_ram.print_customer));}

#define GetConfigVersion()				(config_ram.config_version)
#define GetTradeDataVersion()			(config_ram.trade_form_version)

#define GetConfigState()				(config_ram.fc.fc.config_state)
#define SetConfigState(c)				(config_ram.fc.fc.config_state = c)
#define GetLoginMod()					(config_ram.fc.fc.login_mod)
#define SetLoginMod(c)					(config_ram.fc.fc.login_mod = c)
#define GetTradeUploadState()			(config_ram.fc.fc.trade_upload_state)
#define SetTradeUploadState(c)			(config_ram.fc.fc.trade_upload_state = c)
#define GetIntelligentChange()			(config_ram.fc.fc.intelligent_change)
#define SetIntelligentChange(c)			(config_ram.fc.fc.intelligent_change = c)


#define GetCashbox1Value()				(config_ram.cc.cc.cashbox1_value)
#define SetCashbox1Value(c)				(config_ram.cc.cc.cashbox1_value = c)
#define GetCashbox2Value()				(config_ram.cc.cc.cashbox2_value)
#define SetCashbox2Value(c)				(config_ram.cc.cc.cashbox2_value = c)
#define GetCashbox3Value()				(config_ram.cc.cc.cashbox3_value)
#define SetCashbox3Value(c)				(config_ram.cc.cc.cashbox3_value = c)
#define GetCashbox1AlarmThreshold()		(config_ram.cc.cc.cashbox1_alarm_threshold)
#define SetCashbox1AlarmThreshold(c)	(config_ram.cc.cc.cashbox1_alarm_threshold = c)	
#define GetCashbox2AlarmThreshold()		(config_ram.cc.cc.cashbox2_alarm_threshold)
#define SetCashbox2AlarmThreshold(c)	(config_ram.cc.cc.cashbox2_alarm_threshold = c)
#define GetCashbox3AlarmThreshold()		(config_ram.cc.cc.cashbox3_alarm_threshold)
#define SetCashbox3AlarmThreshold(c)	(config_ram.cc.cc.cashbox3_alarm_threshold = c)
#define GetCashbox1Balance()			(config_ram.cc.cc.cashbox1_balance)
#define SetCashbox1Balance(c)			(config_ram.cc.cc.cashbox1_balance = c)
#define GetCashbox2Balance()			(config_ram.cc.cc.cashbox2_balance)
#define SetCashbox2Balance(c)			(config_ram.cc.cc.cashbox2_balance = c)
#define GetCashbox3Balance()			(config_ram.cc.cc.cashbox3_balance)
#define SetCashbox3Balance(c)			(config_ram.cc.cc.cashbox3_balance = c)
#define GetCashbox1Deposit()			(config_ram.cc.cc.cashbox1_deposit)
#define SetCashbox1Deposit(c)			(config_ram.cc.cc.cashbox1_deposit = c)
#define GetCashbox2Deposit()			(config_ram.cc.cc.cashbox2_deposit)
#define SetCashbox2Deposit(c)			(config_ram.cc.cc.cashbox2_deposit = c)
#define GetCashbox3Deposit()			(config_ram.cc.cc.cashbox3_deposit)
#define SetCashbox3Deposit(c)			(config_ram.cc.cc.cashbox3_deposit = c)

#define GetRj45UartChannal()			(config_ram.um.um.uc_rj45.channal)
#define SetRj45UartChannal(c)			(config_ram.um.um.uc_rj45.channal = c)
#define GetRj45UartBps()				(config_ram.um.um.uc_rj45.bps)
#define SetRj45UartBps(c)				(config_ram.um.um.uc_rj45.bps = c)
#define GetGprsUartChannal()			(config_ram.um.um.uc_gprs.channal)
#define SetGprsUartChannal(c)			(config_ram.um.um.uc_gprs.channal = c)
#define GetGprsUartBps()				(config_ram.um.um.uc_gprs.bps)
#define SetGprsUartBps(c)				(config_ram.um.um.uc_gprs.bps = c)
#define GetGpsUartChannal()				(config_ram.um.um.uc_gps.channal)
#define SetGpsUartChannal(c)			(config_ram.um.um.uc_gps.channal = c)
#define GetGpsUartBps()					(config_ram.um.um.uc_gps.bps)
#define SetGpsUartBps(c)				(config_ram.um.um.uc_gps.bps = c)
#define GetBillUartChannal()			(config_ram.um.um.uc_bill.channal)
#define SetBillUartChannal(c)			(config_ram.um.um.uc_bill.channal = c)
#define GetBillUartBps()				(config_ram.um.um.uc_bill.bps)
#define SetBillUartBps(c)				(config_ram.um.um.uc_bill.bps = c)
#define GetCoinUartChannal()			(config_ram.um.um.uc_coin.channal)
#define SetCoinUartChannal(c)			(config_ram.um.um.uc_coin.channal = c)
#define GetCoinUartBps()				(config_ram.um.um.uc_coin.bps)
#define SetCoinUartBps(c)				(config_ram.um.um.uc_coin.bps = c)
#define GetPrintUartChannal()			(config_ram.um.um.uc_print.channal)
#define SetPrintUartChannal(c)			(config_ram.um.um.uc_print.channal = c)
#define GetPrintUartBps()				(config_ram.um.um.uc_print.bps)
#define SetPrintUartBps(c)				(config_ram.um.um.uc_print.bps = c)
#define GetVoiceUartChannal()			(config_ram.um.um.uc_voice.channal)
#define SetVoiceUartChannal(c)			(config_ram.um.um.uc_voice.channal = c)
#define GetVoiceUartBps()				(config_ram.um.um.uc_voice.bps)
#define SetVoiceUartBps(c)				(config_ram.um.um.uc_voice.bps = c)
#define GetICMachineUartChannal()		(config_ram.um.um.uc_icmachine.channal)
#define SetICMachineUartChannal(c)		(config_ram.um.um.uc_icmachine.channal = c)
#define GetICMachineUartBps()			(config_ram.um.um.uc_icmachine.bps)
#define SetICMachineUartBps(c)			(config_ram.um.um.uc_icmachine.bps = c)



#define CONFIG_VERSION_INDEX_ADDR		((uint32)config_ram.config_version - (uint32)&config_ram)
#define ComputerExternalMemoryAddr(a,head)			((uint32)a-(uint32)head)


extern _config_s config_ram;

extern char * GetDeviceAddr(void);
extern uint8 SetDeviceAddr(char *arg);
extern void TaskSysConfig(void *pdata);
extern void ConfigInit(void);


#endif
