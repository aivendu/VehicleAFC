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
	uint32 st_sys_time_upload	: 2;		//	0--未更新，1--通过gps 更新，2--通过服务器更新，3 -- 通过设备内部时钟更新
	
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
	uint8 exe_flag				: 1;		//	是否可以操作该命令,0--可以，1--不可以
	uint8 exe_unused			: 3;
	uint8 exe_st				: 4;
	/*	状态说明
	 *	0--等待命令，
	 *	1--有命令还未执行，
	 *	2--命令执行中，
	 *	3--命令执行完毕，
	 *	14--执行异常，
	 *	15--命令无效，*/
} _exe_s;

typedef struct
{
	_exe_s shutdown;			//	执行下点操作
	_exe_s makechange;			//	执行找零
	_exe_s print;				//	执行打印
	_exe_s speak;
	_exe_s config_match;		//	执行配置匹配
	_exe_s logout;				//	执行注销
	_exe_s login;
	_exe_s upload_route_data;
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
	_sys_st_major_u	st_major;		//	设备主要状态
	_sys_st_other_u st_other;		//	设备其他状态
	_sys_st_pad_u	st_pad;			//	PAD 状态
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


#define SetMotherBoardSt(c)				(sys_state.ss.st_other.sso.st_motherboard = c)
#define GetMotherBoardSt()				(sys_state.ss.st_other.sso.st_motherboard)

#define SetStUser(c)					(sys_state.ss.st_major.ssm.st_user = c)
#define GetStUser()						(sys_state.ss.st_major.ssm.st_user)
#define SetStServerOnline(c)			(sys_state.ss.st_major.ssm.st_server_online = c)
#define GetStServerOnline()				(sys_state.ss.st_major.ssm.st_server_online)
#define SetStPADOnline(c)				(sys_state.ss.st_major.ssm.st_pad_online = c)
#define GetStPADOnline()				(sys_state.ss.st_major.ssm.st_pad_online)
#define SetStSysInit(c)					(sys_state.ss.st_major.ssm.st_init = c)
#define GetStSysInit()					(sys_state.ss.st_major.ssm.st_init)
#define SetStAllowTrade(c)				(sys_state.ss.st_major.ssm.st_allow_trade = c)
#define GetStAllowTrade()				(sys_state.ss.st_major.ssm.st_allow_trade)
#define SetStDeveiceAbort(c)			(sys_state.ss.st_major.ssm.st_device_abort = c)
#define GetStDeveiceAbort()				(sys_state.ss.st_major.ssm.st_device_abort)
#define GetStTimeUploadState()			(sys_state.ss.st_major.ssm.st_sys_time_upload)
#define SetStTimeUploadState(c)			(sys_state.ss.st_major.ssm.st_sys_time_upload = c)

#define SetCmdShutdown(c)				(sys_state.ss.st_cmd.se.shutdown.exe_st = c)
#define GetCmdShutdown()				(sys_state.ss.st_cmd.se.shutdown.exe_st)
#define SetCmdMakechange(c)				(sys_state.ss.st_cmd.se.makechange.exe_st = c)
#define GetCmdMakechange()				(sys_state.ss.st_cmd.se.makechange.exe_st)
#define SetCmdPrint(c)					(sys_state.ss.st_cmd.se.print.exe_st = c)
#define GetCmdPrint()					(sys_state.ss.st_cmd.se.print.exe_st)
#define SetCmdSpeak(c)					(sys_state.ss.st_cmd.se.speak.exe_st = c)
#define GetCmdSpeak()					(sys_state.ss.st_cmd.se.speak.exe_st)
#define SetCmdConfigMatch(c)			(sys_state.ss.st_cmd.se.config_match.exe_st = c)
#define GetCmdConfigMatch()				(sys_state.ss.st_cmd.se.config_match.exe_st)
#define SetCmdLogout(c)					(sys_state.ss.st_cmd.se.logout.exe_st = c)
#define GetCmdLogout()					(sys_state.ss.st_cmd.se.logout.exe_st)
#define SetCmdLogin(c)					(sys_state.ss.st_cmd.se.login.exe_st = c)
#define GetCmdLogin()					(sys_state.ss.st_cmd.se.login.exe_st)
#define SetCmdUploadRouteData(c)		(sys_state.ss.st_cmd.se.upload_route_data.exe_st = c)
#define GetCmdUploadRouteData()			(sys_state.ss.st_cmd.se.upload_route_data.exe_st)
#define SetCmdChangeSite(c)				(sys_state.ss.st_cmd.se.change_site.exe_st = c)
#define GetCmdChangeSite()				(sys_state.ss.st_cmd.se.change_site.exe_st)
#define SetCmdStoreTradeData(c)			(sys_state.ss.st_cmd.se.store_trade_data.exe_st = c)
#define GetCmdStoreTradeData()			(sys_state.ss.st_cmd.se.store_trade_data.exe_st)
#define SetCmdPrintAmount(c)			(sys_state.ss.st_cmd.se.printamount.exe_st = c)
#define GetCmdPrintAmount()				(sys_state.ss.st_cmd.se.printamount.exe_st)
#define SetCmdSaveConfig(c)				(sys_state.ss.st_cmd.se.saveconfig.exe_st = c)
#define GetCmdSaveConfig()				(sys_state.ss.st_cmd.se.saveconfig.exe_st)
#define SetCmdUploadMemery(c)			(sys_state.ss.st_cmd.se.uploadmemery.exe_st = c)
#define GetCmdUploadMemery()			(sys_state.ss.st_cmd.se.uploadmemery.exe_st)
#define SetCmdChangeRideMess(c)			(sys_state.ss.st_cmd.se.change_ride_mess.exe_st = c)
#define GetCmdChangeRideMess()			(sys_state.ss.st_cmd.se.change_ride_mess.exe_st)
#define SetCmdUploadTime(c)				(sys_state.ss.st_cmd.se.upload_time.exe_st = c)
#define GetCmdUploadTime()				(sys_state.ss.st_cmd.se.upload_time.exe_st)













#endif
