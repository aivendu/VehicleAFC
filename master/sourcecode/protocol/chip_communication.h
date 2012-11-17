#ifndef _CHIP_COMMUNICATION_H
#define _CHIP_COMMUNICATION_H

#include "comm.h"
#include "virtual_memery.h"

#define	CHIP_READ			0
#define	CHIP_WRITE			1

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
	uint8 station_no;				//	վ����
	char  station_name[11];			//	վ������
	uint32 gps_data[2];				//	 gps_data[0] -- ����;  gps_data[1]--ά��
} _station_mess_s;

typedef struct {
	uint16 line_no;
	uint8  line_station_amount;
	char   line_version[9];  
	_station_mess_s station[30];
} _line_mess_s;

typedef struct {
	uint8 regist_time[4];		//	��ʱ��
	uint8 user_role;			//	�û���ɫ�������б��û��Ĺ���Ȩ��
	char staffid[7];			//	���ţ�֧�����7���ַ�
	char driver_name[8];		//	˾�����֣�֧�����4���ֵ�����
	uint8 ID_card[8];			//	˾�����֤�ţ�X��0��ʾ��������
} _user_info_s;

typedef struct {
	uint8 guid[16];				//	GUID, ·��Ψһ���
	char vehicle_plate[8];		//	���ƺ�
	uint16 routenum;			//	·�߱��
} _route_info_s;

//	оƬ��ͨ��ͬ������
typedef struct {
	_user_info_s uinfo;
	_route_info_s rinfo;
} _ccd_card_s;		

typedef struct {
	uint32 gps_state;
	uint32 gps_latitude;
	uint32 gps_longitude;
	uint32 gps_movingspeed;
	uint32 gps_time;
} _gps_data_s;

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

typedef struct {
	uint8 trade_start_st;
	uint8 trade_end_st;
	uint8 price;
	uint8 number_of_people;
} _ride_mess_s;

typedef union {
	uint8 rm_b[sizeof(_ride_mess_s)];
	_ride_mess_s rm;
} _ride_mess_u;


typedef struct {
	uint8 year;
	uint8 month;
	uint8 day;
	uint8 hour;
	uint8 min;
	uint8 sec;
	uint16 serail_num;
	uint16 needpay;
	uint16 realpay;
	uint16 changemoney;			//	��Ҫ����Ľ��
	uint16 changenum;			//  �洢��ʽΪ(��������* 2000 +5Ԫ����*100 +1Ԫ����)
	uint8  des_num;
} _trade_mess_s;

typedef struct {
	uint8 note_errcode;
	uint8 cass1_dis;
	uint8 cass2_dis;
	uint8 cass1_reject;
	uint8 cass2_reject;
	uint8 coin_dis;
	uint8 coin_reject;
} _change_result_s;

typedef struct {
	uint8 trade_exe				:1;		//	���׽���״̬��1--�������ڽ���
	uint8 change_note			:1;		//	ֽ�һ�����״̬��1--������ֽ��
	uint8 change_coin			:1;		//	Ӳ�һ�����״̬��1--������ֽ��
	uint8 print_end				:1;		//	��ӡ����ӡ״̬��1--���ڴ�ӡ
	uint8 unused 				:4;
} _trade_state_s;


typedef struct {
	_trade_mess_s tm;
	_ride_mess_s  rm[MAX_RIDE_NUMBER_OF_PEOPLE];
	_trade_state_s ts;
	_change_result_s cr;
} _trade_mess_whole_s;


typedef union {
	uint8 tm_b[sizeof(_trade_mess_s)];
	_trade_mess_s tm;
} _trade_mess_u;

typedef struct {
	_exe_s speak;
	_exe_s changemoney;
	_exe_s print;
	_exe_s power_off;
	_exe_s print_amount;
	_exe_s print_record;
} _cmd_exe_s;

typedef struct {
	_time_s		time;
	_ccd_card_s user;	//	ICcard
	_gps_data_s gps;	//	GPS
	_cmd_exe_s  cmd;	//	ִ������
	_trade_mess_whole_s trade;		//	TRADE
	_device_state_s sys_device;
	_trade_amount_s trade_amount;
} _device_control_s;

#define CONTROL_HEAD_ADDR					((uint32)&device_control)
#define CONTROL_TIME_INDEX_ADDR				0
#define CONTROL_TIME_LENGHT					((uint32)&device_control.user - CONTROL_HEAD_ADDR)
#define CONTROL_USER_INDEX_ADDR				CONTROL_TIME_LENGHT
#define CONTROL_USER_LENGHT					((uint32)&device_control.gps-(uint32)&device_control.user)
#define CONTROL_GPS_INDEX_ADDR				(CONTROL_USER_INDEX_ADDR+CONTROL_USER_LENGHT)
#define CONTROL_GPS_LENGHT					((uint32)&device_control.cmd-(uint32)&device_control.gps)
#define CONTROL_CMD_INDEX_ADDR				(CONTROL_GPS_INDEX_ADDR+CONTROL_GPS_LENGHT)
#define CONTROL_CMD_LENGHT					((uint32)&device_control.trade-(uint32)&device_control.cmd)
#define CONTROL_TRADE_INDEX_ADDR			(CONTROL_CMD_INDEX_ADDR+CONTROL_CMD_LENGHT)
#define CONTROL_TRADE_LENGHT				((uint32)&device_control.trade.ts-(uint32)&device_control.trade)
#define CONTROL_TRADE_STATE_INDEX_ADDR		(CONTROL_TRADE_INDEX_ADDR+CONTROL_TRADE_LENGHT)
#define CONTROL_TRADE_STATE_LENGHT			((uint32)&device_control.sys_device-(uint32)&device_control.trade.ts)
#define CONTROL_SYS_DEVICE_INDEX_ADDR		((uint32)&device_control.sys_device - CONTROL_HEAD_ADDR)
#define CONTROL_SYS_DEVICE_LENGHT			sizeof(_device_state_s)
#define CONTROL_TRADEAMOUNT_INDEX_ADDR		((uint32)&device_control.trade_amount- CONTROL_HEAD_ADDR)
#define CONTROL_TRADEAMOUNT_LENGHT			sizeof(_trade_amount_s)



#define CONTROL_CMD_SPEAK_INDEX_ADDR		((uint32)&device_control.cmd.speak - CONTROL_HEAD_ADDR)
#define CONTROL_CMD_SPEAK_LENGHT			(sizeof(_exe_s))
#define CONTROL_CMD_CHANGE_INDEX_ADDR		((uint32)&device_control.cmd.changemoney - CONTROL_HEAD_ADDR)
#define CONTROL_CMD_CHANGE_LENGHT			(sizeof(_exe_s))
#define CONTROL_CMD_PRINT_INDEX_ADDR		((uint32)&device_control.cmd.print - CONTROL_HEAD_ADDR)
#define CONTROL_CMD_PRINT_LENGHT			(sizeof(_exe_s))
#define CONTROL_CMD_POWEROFF_INDEX_ADDR		((uint32)&device_control.cmd.power_off - CONTROL_HEAD_ADDR)
#define CONTROL_CMD_POWEROFF_LENGHT			(sizeof(_exe_s))
#define CONTROL_CMD_PRINTAMOUNT_INDEX_ADDR	((uint32)&device_control.cmd.print_amount- CONTROL_HEAD_ADDR)
#define CONTROL_CMD_PRINTAMOUNT_LENGHT		(sizeof(_exe_s))



#define WRITE			2
#define	READ			1


extern _line_mess_s curr_line;
extern _device_control_s device_control;



extern void ChipCommInit(void);
extern void RequestUpload(void);
extern void TaskChipComm(void *pdata);


#endif
