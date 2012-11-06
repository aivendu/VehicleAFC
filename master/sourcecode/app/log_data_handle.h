#ifndef _LOG_DATA_HANDLE_H
#define _LOG_DATA_HANDLE_H



#define LOG_START_ADDR				0x300FF


#define PAD_COMMAND					1	//	PAD ������־
#define SERVER_COMMAND				2	//	������������־
#define DEVICE_COMMAND				3	//	�豸������־

typedef struct
{
	uint16	type;			//	��־����
	uint16	lenght;			//	��־���ݳ���
	uint32	last_log;			//	��һ����־��ַ
} _log_s;

extern void InitLog(void);
extern void StoreLog(uint16 type, void *data, uint16 data_len);
extern void *ReadLog(uint8 flag,uint16 data_lenght);


#endif

