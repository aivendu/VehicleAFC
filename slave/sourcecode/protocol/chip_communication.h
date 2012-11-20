#ifndef _CHIP_COMMUNICATION_H
#define _CHIP_COMMUNICATION_H

#include "comm.h"
#include "eeprom.h"

#define DEVICE_CONTROL_DATA_LENGHT			(sizeof(_device_control_s))		//	��оƬ���ݳش�С
#define MAX_STATION_NUM						30
typedef struct {
	uint16	year;
	uint8	month;
	uint8	day;
	uint8	hour;
	uint8	min;
	uint8	sec;
	uint8	msec;
} _time_s;

typedef struct {
	uint32 flag;
	uint8 *data;
} _slave_command_s;

typedef struct {
	_variable_flag_s flag;			//	���ݶ�д��־
	_trade_mess_s tm; 
} _voice_prompt_s;

typedef struct {
	_variable_flag_s flag;
	uint8 makechange;
} _make_change_s;

typedef struct {
	uint32 makechange			:1;
	uint32 print				:1;
	uint32 reset				:1;
	uint32 shutdown				:1;
	uint32 login				:1;
	uint32 unused				:28;
} _no_variable_command;

typedef struct {
	uint32 flag;
	_voice_prompt_s voice_prompt;
	_no_variable_command nvc;
} _chip_comm_data_s;

typedef union {
	void *ccd_b[sizeof(_chip_comm_data_s)/4];
	_chip_comm_data_s ccd;
} _chip_comm_data_u;


typedef struct {
	_variable_flag_s flag;
	uint16 virtual_addr;
	uint16 data_len;
	void* data;
}_read_data_s;

typedef struct {
	_variable_flag_s flag;
	uint16 virtual_addr;
	uint16 data_len;
	void *data;
}_write_data_s;


typedef struct {
	uint8 command;
	uint16 addr;
	uint8 len;
	uint8 *data;
} _chip_comm_form_s;

//	 վ������
typedef struct {
	uint8 station_no;				//	վ����
	char  station_name[11];			//	վ������
	uint32 gps_data[2];				//	 gps_data[0] -- ����;  gps_data[1]--ά��
} _station_mess_s;

//	·������
typedef struct {
	_station_mess_s station[MAX_STATION_NUM];
	uint16 line_no;
	uint8  line_station_amount;
	char   line_version[9];  
} _line_mess_s;


//	оƬ��ͨ��ͬ������
typedef struct {
	_user_info_s uinfo;
	_route_info_s rinfo;
} _ccd_card_s;			

//	�豸״̬
typedef struct {
	uint32 ic_machine_state			:4;
	uint32 print_machine_state		:4;
	uint32 note_machine_state		:4;
	uint32 coin_machine_state		:4;
	uint32 gps_mode_state			:4;
	uint32 sys_state				:4;
	uint32 unused					:8;
} _device_state_s;

//	��������
typedef struct {
	uint8 io_0						:1;
	uint8 io_1						:1;
	uint8 io_2						:1;
} _io_control_s;

typedef struct {
	_exe_s speak;
	_exe_s changemoney;
	_exe_s print;
	_exe_s power_down;
	_exe_s print_amount;
	_exe_s print_record;
} _cmd_exe_s;

typedef struct
{
	uint16 year;
	uint8  month;
	uint8  day;
	uint32 realpay_amount;
	uint32 needpay_amount;
	uint16 coin_dis_amount;
	uint16 note_1_dis_amount;
	uint16 note_2_dis_amount;
	uint16 trade_num;
	uint16 trade_people;
	char   driver_id[10];
} _trade_amount_s;


typedef struct {
	_time_s		time;
	_ccd_card_s user;	//	ICcard
	_gps_data_s gps;	//	GPS
	_cmd_exe_s  cmd;	//	ִ������
	_trade_mess_whole_s trade;//	TRADE
	_device_state_s sys_device;
	_trade_amount_s trade_amount;
} _device_control_s;


#define WRITE			2
#define	READ			1


extern _line_mess_s curr_line;
extern _device_control_s device_control;

extern void ChipCommInit(void);
extern void TaskChipComm(void *pdata);

#endif
