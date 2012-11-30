#ifndef _SYS_STATE_H
#define _SYS_STATE_H


typedef struct
{
	uint32 st_user				: 4;
	uint32 st_server_online		: 2;
	uint32 st_pad_online		: 2;
	uint32 st_init				: 1;
	uint32 st_allow_trade		: 1;
	uint32 st_device_abort		: 1;
	uint32 st_sys_time_upload	: 2;		//	0--δ���£�1--ͨ��gps ���£�2--ͨ�����������£�3 -- ͨ���豸�ڲ�ʱ�Ӹ���
	uint32 unused				: 19;
} _sys_st_major_s;

typedef union
{
	uint8 ssm_b[sizeof(_sys_st_major_s)];
	_sys_st_major_s ssm;
} _sys_st_major_u;

typedef struct
{
	uint32 st_ic_machine		: 4;
	uint32 st_print_machine		: 4;
	uint32 st_bill_machine		: 4;
	uint32 st_coin_machine		: 4;
	uint32 st_gps_machine		: 4;
	uint32 st_motherboard		: 4;
	uint32 unused1				: 8;
} _sys_st_other_s;

typedef union
{
	uint8 sso_b[sizeof(_sys_st_other_s)];
	_sys_st_other_s sso;
} _sys_st_other_u;

typedef struct
{
	uint8 exe_flag				: 1;		//	�Ƿ���Բ���������,0--���ԣ�1--������
	uint8 exe_unused			: 3;
	uint8 exe_st				: 4;
	/*	״̬˵��
	 *	0--�ȴ����
	 *	1--�����δִ�У�
	 *	2--����ִ���У�
	 *	3--����ִ����ϣ�
	 *	14--ִ���쳣��
	 *	15--������Ч��*/
} _exe_s;

typedef struct
{
	_exe_s shutdown;			//	ִ���µ����
	_exe_s makechange;			//	ִ������
	_exe_s print;				//	ִ�д�ӡ
	_exe_s speak;
	_exe_s config_match;		//	ִ������ƥ��
	_exe_s logout;				//	ִ��ע��
	_exe_s login;
	_exe_s upload_line_data;
	_exe_s change_site;
	_exe_s store_trade_data;
	_exe_s printamount;
	_exe_s saveconfig;
	_exe_s uploadmemery;
	_exe_s change_ride_mess;
	_exe_s upload_time;
} _sys_st_exe_s;

typedef union
{
	uint8 se_b[sizeof(_sys_st_exe_s)];
	_sys_st_exe_s se;
} _sys_st_exe_u;

typedef struct
{
	uint32 login_st			: 2;
	uint32 power_st			: 2;
	uint32 wifi_st			: 1;
	uint32 rj45_st			: 1;
	uint32 gprs_st			: 1;
	uint32 unused1			: 1;
	uint32 GUI_st			: 4;
	uint32 volume			: 4;
	uint32 line_no			: 16;
	uint32 current_state	: 8;
	uint32 unused2			: 24;
} _sys_st_pad_s;

typedef union
{
	uint8 ssp_b[sizeof(_sys_st_pad_s)];
	_sys_st_pad_s ssp;
} _sys_st_pad_u;


typedef struct
{
	_sys_st_major_u	st_major;		//	�豸��Ҫ״̬
	_sys_st_other_u st_other;		//	�豸����״̬
	_sys_st_pad_u	st_pad;			//	PAD ״̬
	_sys_st_exe_u   st_cmd;
} _sys_st_s;

typedef union
{
	uint8 ss_b[sizeof(_sys_st_s)];
	_sys_st_s ss;
} _sys_st_u;

#define EXE_WAIT				0
#define EXE_WRITED				1
#define EXE_RUNNING				2
#define EXE_RUN_END				3
#define EXE_RUN_ABORT			14
#define EXE_INVALID				15


extern _sys_st_u sys_state;

#define GetChangeMoneyCommand()			(sys_state.ss.st_cmd.se.makechange.exe_st)
#define SetChangeMoneyCommand(c)		(sys_state.ss.st_cmd.se.makechange.exe_st = c)
#define GetTimeUploadState()			(sys_state.ss.st_major.ssm.st_sys_time_upload)
#define SetTimeUploadState(c)			(sys_state.ss.st_major.ssm.st_sys_time_upload = c)

#define SetSaveConfig(c)					(sys_state.ss.st_cmd.se.saveconfig.exe_st = c)
#define GetSaveConfig()					(sys_state.ss.st_cmd.se.saveconfig.exe_st)

#define SetUploadTime(c)					(sys_state.ss.st_cmd.se.upload_time.exe_st = c)
#define GetUploadTime()					(sys_state.ss.st_cmd.se.upload_time.exe_st)


#define GetCommandState()


#endif
