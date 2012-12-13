#ifndef _LOG_DATA_HANDLE_H
#define _LOG_DATA_HANDLE_H



#define LOG_START_ADDR				(0x20100)
#define LOG_MEMERY_LENGHT			(0x40000)


#define PAD_COMMAND_LOG					1	//	PAD 命令日志
#define SERVER_COMMAND_LOG				2	//	服务器命令日志
#define DEVICE_COMMAND_LOG				3	//	设备命令日志
#define LOGIN_COMMAND_LOG				4	//	用户登录
#define LOGOUT_COMMAND_LOG				5	//	退出登录命令
#define DEPOSIT_COMMAND_LOG				6	//	存钱命令

typedef struct
{
	uint32 log_start;
	uint32 log_end;
} _log_manage_s;


typedef struct
{
	uint8	type;			//	日志类型
	uint8	state;			//	日志状态
	uint16	lenght;			//	日志数据长度
	uint32	last_log;		//	上一条日志地址
} _log_s;

typedef struct
{
	char   staffid[8];
	uint16 year;
	uint8  month;
	uint8  day;
	uint8  hour;
	uint8  min;
	uint8  sec;
	uint16 cashbox1_amount;
	uint16 cashbox2_amount;
	uint16 cashbox3_amount;
} _log_deposit_cmd_s;

typedef struct
{
	char   staffid[8];
	uint16 year;
	uint8  month;
	uint8  day;
	uint8  hour;
	uint8  min;
	uint8  sec;
} _log_device_use_cmd_s;


extern _log_manage_s log_index;			//	最后一个log 的管理块地址
extern _log_s	log_data;


extern void InitLog(void);
extern uint8 StoreLog(uint16 type, void *data, uint16 data_len);
extern _log_s *ReadLog(uint8 flag, void *data, uint16 data_lenght, uint32 read_index);
extern uint8 LogStoreLogin(void);
extern uint8 LogStoreLogout(void);
extern uint8 LogStoreDeposit(void);


#endif

