#ifndef _CONFIG_MANAGE_H
#define _CONFIG_MANAGE_H

#include "uart0.h"

#define CONFIG_SAVE_START_ADDR			256

//	�����õĵ�ַ��Χ��������Χ�̶�ΪFFFF
#define MAX_DEVICE_ADDR					0xFFFF
#define MIN_DEVICE_ADDR					0x1000

#if 0

static _uart_config_s const uart0_config[] =
{
	{0,	9600		},		//	0- init
	{3,	38400		},		//	1- print machine
	{2,	9600		},		//	2- coin machine
	{1,	9600		},		//	3- note machine
};


static _uart_config_s const uart1_config[] =
{
	{0,	9600		},		//	0- init
	{1,	9600		},		//	1- speaker
	{2,	19200		},		//	2- IC Card machine
	{3,	9600		},		//	3- GPS module
};

uc_rj45;
uc_gprs;
uc_gps;
uc_bill;
uc_coin;
uc_print;
uc_voice;
uc_icmachine;
unused1;
unused2;

#endif


typedef enum
{
    UART_RJ45_CHANNEL,		//	RJ45 ͨ��
    UART_GPRS_CHANNEL,		//	gprs ͨ��
    GPS_UART1,				//	GPSͨ��
    NOTE_MACHINE_UART0,		//	ֽ�һ�ͨ��
    COINMACHINE_UART0,		//	Ӳ�һ�ͨ��
    PRINTER_UART0,			//	��ӡͨ��
    SPEAKER_UART1,			//	����ͨ��
    IC_MACHINE_UART1,		//	IC ��ͨ��
    UNUSED1,
    UNUSED2,
} _uart_fun;



//	��������
typedef struct
{
	uint8 channal;
	uint8 device_addr;
	uint32 bps;
} _uart_config_s;



//	����Ϳ���ϵͳ����
typedef struct
{
	uint32	config_state			: 2;		//	����״̬��0--δ��ʼ����1--Ĭ�ϳ�ʼ����2--�û�����
	uint32	login_mod				: 2;		//	��¼ģʽ��0--����¼��1--�ʺŵ�¼��3--��ϵ�¼
	uint32	trade_upload_state		: 1;		//	���������ϴ���ɣ�0--�����ϴ���ɣ�1--����û�ϴ�������
	uint32	unused					: 28;
} _function_config_s;

typedef union
{
	_function_config_s fc;
	uint32 fc_b[sizeof(_function_config_s)];							//	�޶�ֻ��ռ��4 byte
} _function_config_u;


//	����Ϳ���ϵͳ����
typedef struct
{
	uint16	gprs_answer_response_time;	//	GPRS �����Ӧʱ��
	uint16	gps_sampling_time;			//	GPS ����ʱ��
	uint16	gprs_offline_response_time;	//	GPRS �ѻ���Ӧʱ��
	uint16	login_remain_time;			//	��¼����ʱ��
	uint16	server_port;				//	�������˿ں�
	uint8	server_ip[4];				//	������IP ��ַ
} _parameter_s;

typedef union
{
	_parameter_s pa;
	uint8 pa_b[sizeof(_parameter_s)];	//	�޶�ֻ��ռ��18 byte
} _parameter_u;


//	Ǯ������
typedef struct
{
	uint8	cashbox1_value;		//	Ӳ��Ǯ����ֵ����λ1 ��
	uint8	cashbox2_value;		//	ֽ��Ǯ��һ��ֵ����λԪ
	uint8	cashbox3_value;		//	ֽ��Ǯ�����ֵ����λԪ
	uint8	cashbox1_alarm_threshold;	//	Ӳ��Ǯ�䱨����ֵ
	uint8	cashbox2_alarm_threshold;	//	ֽ��Ǯ��һ������ֵ
	uint8	cashbox3_alarm_threshold;	//	ֽ��Ǯ���������ֵ
	uint16	cashbox1_balance;	//	Ӳ��Ǯ�����
	uint16	cashbox2_balance;	//	ֽ��Ǯ��һ�����
	uint16	cashbox3_balance;	//	ֽ��Ǯ��������
	uint16	cashbox1_deposit;	//	Ӳ��Ǯ������ܶ�
	uint16	cashbox2_deposit;	//	ֽ��Ǯ��һ�����ܶ�
	uint16	cashbox3_deposit;	//	ֽ��Ǯ��������ܶ�
	uint8	unused[2];	//
} _cashbox_config_s;

typedef union
{
	_cashbox_config_s cc;
	uint8 cc_b[sizeof(_cashbox_config_s)];						//	�޶�ֻ��ռ��8 byte
} _cashbox_config_u;


//	��������
typedef struct
{
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
	_uart_config_s um_array[10];
	uint8 um_b[sizeof(_uart_manage_s)];		//	�޶�ֻ��ռ��80 byte
} _uart_manage_u;


//	������
typedef struct
{
	char print_customer[16];		//	�ͻ����ƣ��ṩ��ӡ
	_function_config_u fc;			//	��������
	_cashbox_config_u  cc;			//
	_uart_manage_u um;
	_parameter_u pa;
	char device_addr[4];			//	�豸��ַ, ASCII �ַ�, 0000-FFFF
	char lisence_plate_num[8];		//	���ƺ�
	char trade_form_version[6];		//	�������ݸ�ʽ�汾��
	char config_version[9];			//	���ð汾��
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
#define GetRj45UartDeviceAddr()			(config_ram.um.um.uc_rj45.device_addr)
#define SetRj45UartDeviceAddr(c)		(config_ram.um.um.uc_rj45.device_addr = c)
#define GetRj45UartBps()				(config_ram.um.um.uc_rj45.bps)
#define SetRj45UartBps(c)				(config_ram.um.um.uc_rj45.bps = c)
#define GetGprsUartChannal()			(config_ram.um.um.uc_gprs.channal)
#define SetGprsUartChannal(c)			(config_ram.um.um.uc_gprs.channal = c)
#define GetGprsUartDeviceAddr()			(config_ram.um.um.uc_gprs.device_addr)
#define SetGprsUartDeviceAddr(c)		(config_ram.um.um.uc_gprs.device_addr = c)
#define GetGprsUartBps()				(config_ram.um.um.uc_gprs.bps)
#define SetGprsUartBps(c)				(config_ram.um.um.uc_gprs.bps = c)
#define GetGpsUartChannal()				(config_ram.um.um.uc_gps.channal)
#define SetGpsUartChannal(c)			(config_ram.um.um.uc_gps.channal = c)
#define GetGpsUartDeviceAddr()			(config_ram.um.um.uc_gps.device_addr)
#define SetGpsUartDeviceAddr(c)			(config_ram.um.um.uc_gps.device_addr = c)
#define GetGpsUartBps()					(config_ram.um.um.uc_gps.bps)
#define SetGpsUartBps(c)				(config_ram.um.um.uc_gps.bps = c)
#define GetBillUartChannal()			(config_ram.um.um.uc_bill.channal)
#define SetBillUartChannal(c)			(config_ram.um.um.uc_bill.channal = c)
#define GetBillUartDeviceAddr()			(config_ram.um.um.uc_bill.device_addr)
#define SetBillUartDeviceAddr(c)		(config_ram.um.um.uc_bill.device_addr = c)
#define GetBillUartBps()				(config_ram.um.um.uc_bill.bps)
#define SetBillUartBps(c)				(config_ram.um.um.uc_bill.bps = c)
#define GetCoinUartChannal()			(config_ram.um.um.uc_coin.channal)
#define SetCoinUartChannal(c)			(config_ram.um.um.uc_coin.channal = c)
#define GetCoinUartDeviceAddr()			(config_ram.um.um.uc_coin.device_addr)
#define SetCoinUartDeviceAddr(c)		(config_ram.um.um.uc_coin.device_addr = c)
#define GetCoinUartBps()				(config_ram.um.um.uc_coin.bps)
#define SetCoinUartBps(c)				(config_ram.um.um.uc_coin.bps = c)
#define GetPrintUartChannal()			(config_ram.um.um.uc_print.channal)
#define SetPrintUartChannal(c)			(config_ram.um.um.uc_print.channal = c)
#define GetPrintUartDeviceAddr()		(config_ram.um.um.uc_print.device_addr)
#define SetPrintUartDeviceAddr(c)		(config_ram.um.um.uc_print.device_addr = c)
#define GetPrintUartBps()				(config_ram.um.um.uc_print.bps)
#define SetPrintUartBps(c)				(config_ram.um.um.uc_print.bps = c)
#define GetVoiceUartChannal()			(config_ram.um.um.uc_voice.channal)
#define SetVoiceUartChannal(c)			(config_ram.um.um.uc_voice.channal = c)
#define GetVoiceUartDeviceAddr()		(config_ram.um.um.uc_voice.device_addr)
#define SetVoiceUartDeviceAddr(c)		(config_ram.um.um.uc_voice.device_addr = c)
#define GetVoiceUartBps()				(config_ram.um.um.uc_voice.bps)
#define SetVoiceUartBps(c)				(config_ram.um.um.uc_voice.bps = c)
#define GetICMachineUartChannal()		(config_ram.um.um.uc_icmachine.channal)
#define SetICMachineUartChannal(c)		(config_ram.um.um.uc_icmachine.channal = c)
#define GetICMachineUartDeviceAddr()	(config_ram.um.um.uc_icmachine.device_addr)
#define SetICMachineUartDeviceAddr(c)	(config_ram.um.um.uc_icmachine.device_addr = c)
#define GetICMachineUartBps()			(config_ram.um.um.uc_icmachine.bps)
#define SetICMachineUartBps(c)			(config_ram.um.um.uc_icmachine.bps = c)




#define ComputerExternalMemoryAddr(a,head)			((uint32)a-(uint32)head)


extern _config_s config_ram;

extern char *GetDeviceAddr(void);
extern uint8 SetDeviceAddr(char *arg);
extern void TaskSysConfig(void *pdata);
extern void ConfigInit(void);
extern uint8 RequestUart(uint8 ch, uint16 t);
extern void FreeUart(uint8 ch);


#endif
