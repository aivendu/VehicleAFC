#ifndef _SERVER_COMMUNICATION_H
#define _SERVER_COMMUNICATION_H
#include "sys_config.h"

#define GPRS_FRAME_MAX_LENGHT				(sizeof(_server_communication_s) + 2 + 5)
#define SERVER_COMMUNICATION_HEAD			'<'
#define SERVER_COMMUNICATION_END			'>'

#define GPRS_MAX_TIME_DELAY					500			//	��λ10 ms

//	GPRS������
#define GPRS_DATA_NO_ERR					0
#define GPRS_TIME_OUT						1
#define GPRS_UART_FAULT						2
#define GPRS_CHECKE_ERR						3
#define GPRS_END_DATA_ERR					4
#define GPRS_DATA_RETURN_ERR				5


typedef struct
{
	uint8	package_no;			//	����
	uint8	data_lenght;		//	���ݳ���
	char	command[2];			//	������
	char	argument[2];		//	����
	char	data[86];			//	����
	uint16	check;				//	У��
} _server_communication_s;

typedef struct
{
	uint8	start_station;			//	���վ
	uint8	destination_station;	//	Ŀ�ĵ�վ����
	uint8	price;					//	Ʊ��
	uint8	people_num;				//	����
} _destination_s;


typedef struct
{
	uint16	year;					//	��
	uint8	month;					//	��
	uint8	day;					//	��
	uint8	hour;					//	ʮ
	uint8	minute;					//	��
	uint8	second;					//	��
	uint8	current_station;		//	��ǰվ��
	uint16	serial_number;			//	��ˮ��
	uint16	needpay;				//	�踶
	uint16	realpay;				//	ʵ��
	uint8	change_cashbox_1;		//	Ǯ��1 ��������
	uint8	change_cashbox_2;		//	Ǯ��2 ��������
	uint8	change_cashbox_3;		//	Ǯ��3 ��������
	uint8	destination_num;		//	Ŀ�ĵ�վ������
	uint8	people_amount;
	uint8	unused;
	_destination_s	destination[MAX_DESTINATION_NUM];		//	Ŀ�ĵ�����
} _trade_data_to_server_s;

//	��������
typedef struct
{
	char channal_num;
	char bps[6];
	char separator;
} _uart_config_from_server_s;

//	ϵͳ��������
typedef struct
{
	char login_mode;
} _sys_config_from_server_s;

//	Ǯ������ģʽ
typedef struct
{
	char cashbox1_par_value[4];
	char cashbox2_par_value[4];
	char cashbox3_par_value[4];
	char cashbox1_threshold[4];
	char cashbox2_threshold[4];
	char cashbox3_threshold[4];
} _cashbox_config_from_server_s;

//	ϵͳ��������
typedef struct
{
	char device_addr[4];
	char gprs_response_time[5];
	char gps_sampling_time[5];
	char gprs_off_line_delay_time[5];
	char user_logout_delay_time[5];
} _sys_performance_config_from_server_s;


//	��������������
typedef struct
{
	char ip[4][3];
	char port[5];
} _server_communication_config_from_server;

//	��ӡ����
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
