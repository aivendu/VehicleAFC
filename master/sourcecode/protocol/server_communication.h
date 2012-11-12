#ifndef _SERVER_COMMUNICATION_H
#define _SERVER_COMMUNICATION_H
#include "sys_config.h"

#define GPRS_FRAME_MAX_LENGHT				(sizeof(_server_communication_s) + 2 + 5)
#define SERVER_COMMUNICATION_HEAD			'<'
#define SERVER_COMMUNICATION_END			'>'

#define GPRS_MAX_TIME_DELAY					500			//	单位10 ms

//	GPRS错误定义
#define GPRS_DATA_NO_ERR					0
#define GPRS_TIME_OUT						1
#define GPRS_UART_FAULT						2
#define GPRS_CHECKE_ERR						3
#define GPRS_END_DATA_ERR					4
#define GPRS_DATA_RETURN_ERR				5


typedef struct
{
	uint8	package_no;			//	包号
	uint8	data_lenght;		//	数据长度
	char	command[2];			//	命令字
	char	argument[2];		//	参数
	char	data[86];			//	数据
	uint16	check;				//	校验
} _server_communication_s;

typedef struct
{
	uint8	start_station;			//	起点站
	uint8	destination_station;	//	目的地站点编号
	uint8	price;					//	票价
	uint8	people_num;				//	人数
} _destination_s;


typedef struct
{
	uint16	year;					//	年
	uint8	month;					//	月
	uint8	day;					//	天
	uint8	hour;					//	十
	uint8	minute;					//	分
	uint8	second;					//	秒
	uint8	current_station;		//	当前站点
	uint16	serial_number;			//	流水号
	uint16	needpay;				//	需付
	uint16	realpay;				//	实付
	uint8	change_cashbox_1;		//	钱箱1 找零张数
	uint8	change_cashbox_2;		//	钱箱2 找零张数
	uint8	change_cashbox_3;		//	钱箱3 找零张数
	uint8	destination_num;		//	目的地站点数量
	uint8	people_amount;
	uint8	unused;
	_destination_s	destination[MAX_DESTINATION_NUM];		//	目的地详情
} _trade_data_to_server_s;

//	串口配置
typedef struct
{
	char channal_num;
	char bps[6];
	char separator;
} _uart_config_from_server_s;

//	系统参数配置
typedef struct
{
	char login_mode;
} _sys_config_from_server_s;

//	钱箱配置模式
typedef struct
{
	char cashbox1_par_value[4];
	char cashbox2_par_value[4];
	char cashbox3_par_value[4];
	char cashbox1_threshold[4];
	char cashbox2_threshold[4];
	char cashbox3_threshold[4];
} _cashbox_config_from_server_s;

//	系统性能配置
typedef struct
{
	char device_addr[4];
	char gprs_response_time[5];
	char gps_sampling_time[5];
	char gprs_off_line_delay_time[5];
	char user_logout_delay_time[5];
} _sys_performance_config_from_server_s;


//	服务器连接配置
typedef struct
{
	char ip[4][3];
	char port[5];
} _server_communication_config_from_server;

//	打印设置
typedef struct
{
	char line1[16];
} _print_config_from_server;

extern char PAD_version[16];

extern void ServerCommInit(void);
extern void TaskServerHandle(void *pdata);


extern uint8 ServerOnLine(void);
extern uint8 ServerUploadTradeData(_trade_data_to_server_s *data);
extern uint8 ServerTimeSync(void);
extern uint8 ServerCashBoxBalance(uint16 cashbox1, uint16 cashbox2, uint16 cashbox3);
extern uint8 ServerGPSData(uint8 flag,uint32 latitude, uint32 longitude, uint32 speed);
extern uint8 ServerLogin(char *staffid);
extern uint8 ServerLogout(char *staffid);















#endif
