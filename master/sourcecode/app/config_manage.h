#ifndef _CONFIG_MANAGE_H
#define _CONFIG_MANAGE_H

#include "uart0.h"

#define CONFIG_SAVE_START_ADDR			0



//	定义和控制系统功能
typedef struct
{
	uint8	config_flag				:1;
	uint32 login_mod				:2;
	uint32 unused					:29;
} _function_config_s;


//	定义和控制系统参数
typedef struct
{
	char	device_addr[4];		//	设备地址, ASCII 字符, 0000-9999
	uint16	gprs_answer_response_time;	//	GPRS 最迟响应时间
	uint16	gps_sampling_time;			//	GPS 采样时间
	uint16	gprs_offline_response_time;	//	GPRS 脱机响应时间
	uint16	login_remain_time;			//	登录保持时间
	uint16	server_port;				//	服务器端口号
	uint8	server_ip[4];				//	服务器IP 地址
} _parameter_s;



//	钱箱设置
typedef struct
{
	uint8	cashbox1_value;		//	硬币钱箱面值，单位1 分
	uint8	cashbox2_value;		//	纸币钱箱一面值，单位元
	uint8	cashbox3_value;		//	纸币钱箱二面值，单位元
	uint8	cashbox1_alarm_threshold;	//	硬币钱箱报警阈值
	uint8	cashbox2_alarm_threshold;	//	纸币钱箱一报警阈值
	uint8	cashbox3_alarm_threshold;	//	纸币钱箱二报警阈值
	uint8	unused[2];
}_cashbox_config_s;

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


/*
//	定义系统需要常改到参数
typedef struct
{
	uint16	cashbox1_amount;	//	硬币钱箱总额
	uint16	cashbox2_amount;	//	纸币钱箱一总额
	uint16	cashbox3_amount;	//	纸币钱箱二总额
} _cashbox_amount;
*/

typedef struct
{
	char config_version[8];			//	配置版本号
	char customer[16];				//	客户名称，提供打印
	_function_config_s fc;
	_cashbox_config_s  cc;
	_uart_manage_s uc;
	_parameter_s pa;
} _config_s;



#define ComputerExternalMemoryAddr(a,head)			((uint32)a-(uint32)head)


extern _config_s config_ram;

extern void ConfigInit(void);


#endif
