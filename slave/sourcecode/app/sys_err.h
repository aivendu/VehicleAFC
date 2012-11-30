#ifndef _SYS_ERR_H
#define _SYS_ERR_H

/*	��Ҫ��־λ������ָʾ�豸����Ҫ״̬4byte*/
typedef struct
{
	uint32 login_state:			4;
	uint32 server_comm_state:	2;
	uint32 pad_comm_state:		2;
	uint32 init_end_flag:		1;
	uint32 trade_allowtrade:	1;
	uint32 device_state:		1;
	uint32 unused:				21;
} _major_flag_sys_s;

typedef union
{
	uint8 mfs_byte[sizeof(_major_flag_sys_s)];
	_major_flag_sys_s mfs_v;
} _major_flag_sys_u;

/*	����״̬��־λ������ָʾ���������������״̬��8byte*/
typedef struct
{
	uint32 a;
} _board_state_flag_s;

typedef union
{
	uint8 bsf_byte[sizeof(_board_state_flag_s)];
	_board_state_flag_s bsf_v;
} _board_state_flag_u;

/*	������־λ����ָʾ������Χ�豸��״̬4byte*/
typedef struct
{
	uint32 ic_machine_state:	4;
	uint32 print_machine_state:	4;
	uint32 bill_machine_state:	4;
	uint32 coin_machine_state:	4;
	uint32 gps_machine_state:	4;
	uint32 gprs_device_state:	4;
} _other_flag_sys_s;

typedef union
{
	uint8 ofs_byte[sizeof(_other_flag_sys_s)];
	_other_flag_sys_s ofs_v;
} _other_flag_sys_u;

/*	ϵͳ��־����*/
typedef struct
{
	_major_flag_sys_s	major_flag;
	_board_state_flag_s	board_state;
	_other_flag_sys_s	other_state;
} _vehicle_afc_state_flag_s;

typedef union
{
	uint8 vasf_byte[sizeof(_vehicle_afc_state_flag_s)];
	_vehicle_afc_state_flag_s vasf_v;
} _vehicle_afc_state_flag_u;


/*	������*/
#define VAFC_NO_ERROR				0x00
#define COMM_CHECK_ERROR			0x01
#define COMM_COMMAND_ERROR			0x02
#define VAFC_LENGHT_ERROR			0x03
#define IC_CARD_TYPE_ERROR			0x04
#define IC_NO_CARD_ERROR			0x05
#define IC_CARD_INVALID_ERROR		0x06
#define USER_NAME_ERROR				0x07
#define USER_PASSWORD_ERROR			0x08
#define GPS_NO_LOCATION_ERROR		0x09
//#define



/*	ϵͳģ�������*/
#define VAFC_SYS_NO_ERROR				0x00
#define VAFC_PAD_COMM_ERROR				0x01
#define VAFC_SERVER_COMM_ERROR			0x02
#define VAFC_IC_MACHINE_ERROR			0X03
#define VAFC_
#define VAFC_
#define VAFC_
#define VAFC_
#define VAFC_












#endif
