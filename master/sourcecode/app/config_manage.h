#ifndef _CONFIG_MANAGE_H
#define _CONFIG_MANAGE_H

#include "uart0.h"

#define CONFIG_SAVE_START_ADDR			0



//	����Ϳ���ϵͳ����
typedef struct
{
	uint8	config_flag				:1;
	uint32 login_mod				:2;
	uint32 unused					:29;
} _function_config_s;


//	����Ϳ���ϵͳ����
typedef struct
{
	char	device_addr[4];		//	�豸��ַ, ASCII �ַ�, 0000-9999
	uint16	gprs_answer_response_time;	//	GPRS �����Ӧʱ��
	uint16	gps_sampling_time;			//	GPS ����ʱ��
	uint16	gprs_offline_response_time;	//	GPRS �ѻ���Ӧʱ��
	uint16	login_remain_time;			//	��¼����ʱ��
	uint16	server_port;				//	�������˿ں�
	uint8	server_ip[4];				//	������IP ��ַ
} _parameter_s;



//	Ǯ������
typedef struct
{
	uint8	cashbox1_value;		//	Ӳ��Ǯ����ֵ����λ1 ��
	uint8	cashbox2_value;		//	ֽ��Ǯ��һ��ֵ����λԪ
	uint8	cashbox3_value;		//	ֽ��Ǯ�����ֵ����λԪ
	uint8	cashbox1_alarm_threshold;	//	Ӳ��Ǯ�䱨����ֵ
	uint8	cashbox2_alarm_threshold;	//	ֽ��Ǯ��һ������ֵ
	uint8	cashbox3_alarm_threshold;	//	ֽ��Ǯ���������ֵ
	uint8	unused[2];
}_cashbox_config_s;

//	��������
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
//	����ϵͳ��Ҫ���ĵ�����
typedef struct
{
	uint16	cashbox1_amount;	//	Ӳ��Ǯ���ܶ�
	uint16	cashbox2_amount;	//	ֽ��Ǯ��һ�ܶ�
	uint16	cashbox3_amount;	//	ֽ��Ǯ����ܶ�
} _cashbox_amount;
*/

typedef struct
{
	char config_version[8];			//	���ð汾��
	char customer[16];				//	�ͻ����ƣ��ṩ��ӡ
	_function_config_s fc;
	_cashbox_config_s  cc;
	_uart_manage_s uc;
	_parameter_s pa;
} _config_s;



#define ComputerExternalMemoryAddr(a,head)			((uint32)a-(uint32)head)


extern _config_s config_ram;

extern void ConfigInit(void);


#endif
