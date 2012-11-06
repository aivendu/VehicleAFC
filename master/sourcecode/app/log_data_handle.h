#ifndef _LOG_DATA_HANDLE_H
#define _LOG_DATA_HANDLE_H



#define LOG_START_ADDR				0x300FF


#define PAD_COMMAND					1	//	PAD 命令日志
#define SERVER_COMMAND				2	//	服务器命令日志
#define DEVICE_COMMAND				3	//	设备命令日志

typedef struct
{
	uint16	type;			//	日志类型
	uint16	lenght;			//	日志数据长度
	uint32	last_log;			//	上一条日志地址
} _log_s;

extern void InitLog(void);
extern void StoreLog(uint16 type, void *data, uint16 data_len);
extern void *ReadLog(uint8 flag,uint16 data_lenght);


#endif

