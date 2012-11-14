#ifndef _LOG_DATA_HANDLE_H
#define _LOG_DATA_HANDLE_H



#define LOG_START_ADDR				(0x20100)
#define LOG_MEMERY_LENGHT			(0x40000)


#define PAD_COMMAND					1	//	PAD ������־
#define SERVER_COMMAND				2	//	������������־
#define DEVICE_COMMAND				3	//	�豸������־

typedef struct
{
	uint32 log_start;
	uint32 log_end;
} _log_manage_s;


typedef struct
{
	uint16	type;			//	��־����
	uint16	lenght;			//	��־���ݳ���
	uint32	last_log;			//	��һ����־��ַ
} _log_s;


extern _log_manage_s log_index;			//	���һ��log �Ĺ�����ַ
extern _log_s	log_data;


extern void InitLog(void);
extern uint8 StoreLog(uint16 type, void *data, uint16 data_len);
extern void * ReadLog(uint8 flag, void *data, uint16 data_lenght);


#endif

