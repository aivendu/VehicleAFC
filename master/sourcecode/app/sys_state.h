#ifndef _SYS_STATE_H
#define _SYS_STATE_H


typedef struct {
	uint32 st_user				:4;
	uint32 st_server_online		:2;
	uint32 st_pad_online		:2;
	uint32 st_init				:1;
	uint32 st_allow_trade		:1;
	uint32 st_device_abort		:1;
	uint32 unused				:21;
} _sys_st_major_s;

typedef union {
	uint8 ssm_b[sizeof(_sys_st_major_s)];
	_sys_st_major_s ssm;
} _sys_st_major_u;

typedef struct {
	uint32 st_ic_machine		:4;
	uint32 st_print_machine		:4;
	uint32 st_bill_machine		:4;
	uint32 st_coin_machine		:4;
	uint32 st_gps_machine		:4;
	uint32 st_motherboard		:4;
	uint32 unused1				:8;
	uint32 unused2				:32;
	uint32 unused3				:32;
} _sys_st_other_s;

typedef union {
	uint8 sso_b[sizeof(_sys_st_other_s)];
	_sys_st_other_s sso;
} _sys_st_other_u;

typedef struct {
	uint8 exe_flag				:1;		//	是否可以操作该命令,0--可以，1--不可以
	uint8 exe_unused			:3;
	uint8 exe_st				:4;		
	/*	状态说明
	 *	0--等待命令，
	 *	1--有命令还未执行，
	 *	2--命令执行中，
	 *	3--命令执行完毕，
	 *	14--执行异常，
	 *	15--命令无效，*/
} _exe_s;

typedef struct {
	_exe_s shutdown;			//	执行下点操作
	_exe_s makechange;			//	执行找零
	_exe_s print;				//	执行打印
	_exe_s speak;
	_exe_s config_match;		//	执行配置匹配
	_exe_s logout;				//	执行注销
	_exe_s login;
	_exe_s updata_line_data;
	_exe_s change_site;
	_exe_s store_trade_data;
	_exe_s printamount;
} _sys_exe_st_s;

typedef union {
	uint8 se_b[sizeof(_sys_exe_st_s)];
	_sys_exe_st_s se;
} _sys_exe_st_u;

typedef struct
{
	uint32 login_st			:2;
	uint32 power_st			:2;
	uint32 wifi_st			:1;
	uint32 rj45_st			:1;
	uint32 gprs_st			:1;
	uint32 unused1			:1;
	uint32 GUI_st			:4;
	uint32 volume			:4;
	uint32 line_no			:16;
	uint32 current_state	:8;
	uint32 unused2			:24;
} _sys_pad_st_u;

typedef struct {
	_sys_st_major_u	st_major;
	_sys_st_other_u st_other;
	_sys_exe_st_u   st_cmd;
	_sys_pad_st_u	st_pad;
} _sys_st_s;

typedef union {
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




#endif
