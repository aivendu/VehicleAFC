#ifndef _LOG_DATA_HANDLE_H
#define _LOG_DATA_HANDLE_H



#define LOG_START_ADDR				(0x20100)
#define LOG_MEMERY_LENGHT			(0x40000)


#define PAD_COMMAND_LOG					1	//	PAD ������־
#define SERVER_COMMAND_LOG				2	//	������������־
#define DEVICE_COMMAND_LOG				3	//	�豸������־
#define LOGIN_COMMAND_LOG				4	//	�û���¼
#define LOGOUT_COMMAND_LOG				5	//	�˳���¼����
#define DEPOSIT_COMMAND_LOG				6	//	��Ǯ����

typedef struct
{
	uint32 log_start;
	uint32 log_end;
} _log_manage_s;


typedef struct
{
	uint8	type;			//	��־����
	uint8	state;			//	��־״̬
	uint16	lenght;			//	��־���ݳ���
	uint32	last_log;		//	��һ����־��ַ
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


extern _log_manage_s log_index;			//	���һ��log �Ĺ�����ַ
extern _log_s	log_data;


extern void InitLog(void);
extern uint8 StoreLog(uint16 type, void *data, uint16 data_len);
extern _log_s *ReadLog(uint8 flag, void *data, uint16 data_lenght, uint32 read_index);
extern uint8 LogStoreLogin(void);
extern uint8 LogStoreLogout(void);
extern uint8 LogStoreDeposit(void);


#endif

