#include "includes.h"

#define SendToPad(c)		Uart1SendByte(c,0)
#define RecFromPad(a,b,c)	Uart1RecByte(a,b,c)


#define MAX_RECEIVE_LENGTH			250		//	n字节
#define MAX_COMM_DELAY				30		//	n*5ms
#define SHELL_LENGTH				5		//	头
#define MAX_STATION_NUM				30
#define PAD_COMM_WAIT_TIME			delay_time

#define	AllowRJ45RecAndSend()		(IO0CLR |= RJ45_CTS)
#define DisableRJ45RecAndSend()		(IO0SET |= RJ45_CTS)

static _pad_com_task rec_com,send_com;
static OS_EVENT *pad_event_sem;
uint32 delay_time = 60;			//	调试代码

uint8 station_quantity;
char promptmess[100]={"欢迎光临! welcome to here!"};
char PAD_version[16];

_df_device_and_pad_comm comm_rec_temp;
static OS_EVENT *pad_ack_sem;
static uint8 send_package_num;


#define GetPackageNum()				(send_package_num++)
#define ResetPackageNum()			(send_package_num=0)
/*
*	PAD通信初始化
*/
void PadCommInit(void) {
	IO0SET |= RJ45_RST;
	AllowRJ45RecAndSend();				//	
	pad_ack_sem = OSSemCreate(0);
	if (pad_ack_sem ==  NULL) {
		while(1);
	}
	pad_event_sem = OSSemCreate(1);
	if (pad_event_sem ==  NULL) {
		while(1);
	}
}

/*
*	请求PAD 通信资源
*/
void RequestPADCommunication(void)
{
	uint8 err;
	OSSemPend(pad_event_sem,0,&err);
}

/*
*	释放PAD 通信资源
*/

void FreePADCommunication(void)
{
	OSSemPost(pad_event_sem);
}



/*****************************************************************************************************
** 函数原型: 	uint8 SendToPadCom(const _df_device_and_pad_comm *dat,uint8 flag)
** 功能描述:		CRT711 获取命令返回的函数。
** 输　入:		命令 
** 输　出: 		无
** 全局变量: 	无
**					
** 调用模块:  	无
**					
** 资源占用:		局部变量:	
**					堆栈:	
**					连续运行时间:
**					
** 作　者:	 杜其俊
** 日　期: 	2012年5月14日
*****************************************************************************************************/

uint8 SendToPadCom(const _df_device_and_pad_comm *dat,uint8 flag) {
	uint8 err,i;
	uint16 bcc = 0;
	
	err = SendToPad('<');			//	发送头'<'
	if (err != TRUE) {			//	发送失败返回
		return err;					//	返回错误
	}
	bcc = CRCByte(bcc,'<');				//	计算校验码
	
	err = SendToPad(dat->len);			//	发送长度
	if (err != TRUE) {
		return err;
	}
	bcc = CRCByte(bcc,dat->len);
	
	err = SendToPad(dat->backage_num);			//	发送包号
	if (err != TRUE) {
		return err;
	}
	bcc = CRCByte(bcc,dat->backage_num);
	if (flag) {
		err = SendToPad((uint8)(dat->err_no>>8));			//	发送错误编号高字节
		if (err != TRUE) {
			return err;
		}
		bcc = CRCByte(bcc,(uint8)(dat->err_no>>8));

		err = SendToPad((uint8)(dat->err_no));			//	发送错误编号低字节
		if (err != TRUE) {
			return err;
		}
		bcc = CRCByte(bcc,(uint8)(dat->err_no));
	}

	err = SendToPad(dat->cmd);			//	发送错误编号高字节
	if (err != TRUE) {
		return err;
	}
	bcc = CRCByte(bcc,dat->cmd);

	err = SendToPad(dat->arg);			//	发送错误编号低字节
	if (err != TRUE) {
		return err;
	}
	bcc = CRCByte(bcc,dat->arg);

	if (flag) {
		for (i=0; i<(dat->len-4); i++) {			//	发送数据
			err = SendToPad(*(dat->dat+i));
			if (err != TRUE) {
				return err;
			}
			bcc = CRCByte(bcc,*(dat->dat+i));
		}
	}
	else {
		for (i=0; i<(dat->len-2); i++) {			//	发送数据
			err = SendToPad(*(dat->dat+i));
			if (err != TRUE) {
				return err;
			}
			bcc = CRCByte(bcc,*(dat->dat+i));
		}
	}
	err = SendToPad((uint8)(bcc>>8));	//	发送校验高字节
	if (err != TRUE) {
		return err;
	}
	err = SendToPad((uint8)bcc);	//	发送校验低字节			
	if (err != TRUE) {
		return err;
	}
	err = SendToPad('>');			//	发送尾'>'			
	if (err != TRUE) {
		return err;
	}
	return 0;			//	发送完成，返回发送成功
}

uint8 changesite_flag;
uint8 old_station;
uint8 curr_station;
uint8 run_direction;



uint16 LogOut(void *arg) {
	uint8 err,data_temp;
	_df_device_and_pad_comm temp;
	err = 0;
	if (sys_state.ss.st_cmd.se.logout.exe_flag == 0) {
		sys_state.ss.st_cmd.se.logout.exe_st = EXE_WRITED;
	}
	OSTimeDly(2);
	temp.len = 2+2+1;
	temp.backage_num = rec_com.package_num;
	temp.err_no = 0x0000;
	temp.cmd = rec_com.cmd;
	temp.arg = rec_com.arg;
	data_temp = sys_state.ss.st_major.ssm.st_user;
	temp.dat = &data_temp;
	RequestUart1(1,0);
	SendToPadCom(&temp,1);
	FreeUart1();
	return TRUE;
}
uint16 GetDeviceState(void *arg) {			//	路线编号以自动生成的GUID 代替
	_df_device_and_pad_comm temp;
	temp.len = 2+2+16;
	temp.backage_num = rec_com.package_num;
	temp.err_no = 0x0000;
	temp.cmd = rec_com.cmd;
	temp.arg = rec_com.arg;
	temp.dat = sys_state.ss_b;		//	发送状态
	RequestUart1(1,0);
	SendToPadCom(&temp,1);
	FreeUart1();
	return TRUE;
}

uint16 ExceptionHandle(void *arg) {
	//uint16 err_no = 0x0101;
	_df_device_and_pad_comm temp;
	uint8 data_temp=PAD_NACK;
	temp.len = 2+2+1;
	temp.backage_num = rec_com.package_num;
	temp.err_no = *(uint16 *)arg;
	temp.cmd = rec_com.cmd;
	temp.arg = rec_com.arg;
	temp.dat = &data_temp;
	RequestUart1(1,0);
	SendToPadCom(&temp,1);
	FreeUart1();
	return TRUE;
}


uint16 ChangeSite(void *arg) {
	_df_device_and_pad_comm temp;
	uint8 data_temp[2];
	old_station = rec_com.data[0];			//	保存收到的历史站点
	curr_station = rec_com.data[1];			//	保存收到的当前站点
	run_direction = rec_com.data[2];		//	更新行驶方向
	changesite_flag  =  1;
	OSTimeDly(2);
	data_temp[0] = curr_station;			//	返回当前站点和运行方向。
	data_temp[1] = run_direction;
	temp.len = 2+2+2;
	temp.backage_num = rec_com.package_num;
	temp.err_no = 0x0000;
	temp.cmd = rec_com.cmd;
	temp.arg = rec_com.arg;
	temp.dat = data_temp;
	RequestUart1(1,0);
	SendToPadCom(&temp,1);
	FreeUart1();
	return TRUE;
}

uint16 RideMess(void *arg) {
	_df_device_and_pad_comm temp;
	//_ride_mess_s *mess_temp;
	uint8 data_temp;
	uint8 index=0;
	if (rec_com.old_package_num != rec_com.package_num)
	{
		device_control.trade.tm.needpay = (rec_com.data[1]<<8) + rec_com.data[2];
		device_control.trade.tm.des_num = 0;
		for (index = 3; index < rec_com.len; index+=3) {
			device_control.trade.rm[device_control.trade.tm.des_num].trade_start_st = rec_com.data[0];
			device_control.trade.rm[device_control.trade.tm.des_num].trade_end_st = rec_com.data[index+0];
			device_control.trade.rm[device_control.trade.tm.des_num].price = rec_com.data[index+1];
			device_control.trade.rm[device_control.trade.tm.des_num].number_of_people = rec_com.data[index+2];
			if ((++device_control.trade.tm.des_num) > MAX_RIDE_NUMBER_OF_PEOPLE) break;
			
		}
	}
	/*field_temp = (char *)(&rec_com.data[0]);			//	准备乘车信息
	next_field = field_temp;
	while (*next_field != ';') next_field++;			//	解析起点站编号
	*next_field = '\0';									//	形成字符串
	next_field++;										//	更新至下一个字段
	exe_trade.rm[index].trade_start_st = (uint8)atoi(field_temp);	//	存储起始站点
	field_temp = next_field;
	
	while (*next_field != ';') next_field++;			//	解析需付
	*next_field = '\0';
	next_field++;
	exe_trade.tm.needpay = (uint16)atoi(field_temp);
	field_temp = next_field;

	while (index<MAX_RIDE_NUMBER_OF_PEOPLE) {			//	解析乘车人到站信息
		while (*next_field != '*') next_field++;		//	解析到站编号
		*next_field = '\0';
		next_field++;
		exe_trade.rm[index].trade_end_st = (uint8)atoi(field_temp);
		exe_trade.rm[index].trade_start_st = exe_trade.rm[0].trade_start_st;
		field_temp = next_field;

		while (*next_field != ';') next_field++;		//	解析到该站点的人数
		*next_field = '\0';
		next_field++;
		exe_trade.rm[index].number_of_people = (uint8)atoi(field_temp);
		field_temp = next_field;

		while ((*next_field != ';') || (*next_field != ',') || (*next_field != '.')) next_field++;		//	解析单价
		if ((*next_field == ',') || (*next_field == '.')) {			//	是否是最后的一个字段
			*next_field = '\0';										//	形成字符串
			next_field++;											//	更新至下一个字段
			exe_trade.rm[index].price = (uint16)atoi(field_temp);	//	存储票价
			field_temp = next_field;								//	下一个字段
			exe_trade.rm[index].next = NULL;						//	后面没有字段
			break;
		}
		else {
			*next_field = '\0';										//	形成字符串
			next_field++;											//	更新至下一个字段
			exe_trade.rm[index].price = (uint16)atoi(field_temp);	//	存储票价
			field_temp = next_field;								//	下一个字段
			exe_trade.rm[index].next = &exe_trade.rm[index+1];		//	准备下一个到站信息存储
			index ++;
		}
	}
*/
	//	准备回复数据
	temp.len = 2+2+1;
	temp.backage_num = rec_com.package_num;
	if (device_control.trade.tm.des_num > MAX_RIDE_NUMBER_OF_PEOPLE) {	
		temp.err_no = 0x0101;		//	没有错误
		data_temp = PAD_NACK;
	}
	else {
		temp.err_no = 0x0000;		//	没有错误
		data_temp = PAD_ACK;
		sys_state.ss.st_cmd.se.speak.exe_st = EXE_WRITED;
		RequestUpload();
	}
	temp.cmd = rec_com.cmd;
	temp.arg = rec_com.arg;
	temp.dat = &data_temp;		//	回复ACK
	RequestUart1(1,0);
	SendToPadCom(&temp,1);
	FreeUart1();
	return TRUE;
}

uint16 MakeChange(void *arg) {
	uint8 err;
	_df_device_and_pad_comm temp;
	uint8 data_temp[6];
	err = 0;
	if (rec_com.arg == 0x31) {
		if ((rec_com.old_package_num != rec_com.package_num) && (sys_state.ss.st_cmd.se.makechange.exe_flag == EXE_WAIT)) {
			sys_state.ss.st_cmd.se.makechange.exe_st = EXE_WRITED;			//	执行找零命令
			device_control.trade.tm.serail_num = rec_com.data[0]*256+rec_com.data[1];	//	存储此次交易的流水号
			memcpy(&device_control.trade.tm,&rec_com.data[2],6);						//	存储交易时间
			device_control.trade.tm.realpay = rec_com.data[8]*256+rec_com.data[9];
			device_control.trade.tm.changemoney = rec_com.data[12]*256+rec_com.data[13];
			if (device_control.trade.tm.changemoney == (device_control.trade.tm.realpay - device_control.trade.tm.needpay)) {
				RequestUpload();
				data_temp[0]= PAD_ACK;
				temp.err_no = 0x0000;
			}
			else {
				device_control.trade.tm.changemoney = device_control.trade.tm.realpay - device_control.trade.tm.needpay;
				RequestUpload();
				data_temp[0]= PAD_ACK;
				temp.err_no = 0x0000;		//	计算错误
			}
		}
		else if (sys_state.ss.st_cmd.se.makechange.exe_flag != 0) {
			data_temp[0] = PAD_NACK;
			temp.err_no = 0x0104;
		}
		else {
			data_temp[0] = PAD_ACK;
			temp.err_no = 0x0000;
		}
		//	准备回复数据
		temp.len = 2+2+1;
		temp.backage_num = rec_com.package_num;
		temp.cmd = rec_com.cmd;
		temp.arg = rec_com.arg;
		temp.dat = data_temp;		
	}
	else if (rec_com.arg == 0x32) {
		if (((sys_state.ss.st_cmd.se.makechange.exe_st == EXE_RUN_END) || (sys_state.ss.st_cmd.se.makechange.exe_st == EXE_WAIT))
			&& (device_control.trade.ts.change_note == 0)
			&& (device_control.trade.ts.change_coin == 0)) {
			sys_state.ss.st_cmd.se.makechange.exe_st = EXE_WAIT;
			temp.len = 2+2+6;
			temp.backage_num = rec_com.package_num;
			temp.err_no = 0x0000;
			temp.cmd = rec_com.cmd;
			temp.arg = rec_com.arg;
			data_temp[0] = device_control.sys_device.note_machine_state;
			data_temp[1] = device_control.sys_device.coin_machine_state;
			data_temp[2] = device_control.trade.tm.changemoney;
			data_temp[3] = device_control.trade.cr.coin_dis;
			data_temp[4] = device_control.trade.cr.cass1_dis;
			data_temp[5] = device_control.trade.cr.cass2_dis;
			temp.dat = data_temp;
		}
		else {
			temp.len = 2+2+1;
			temp.backage_num = rec_com.package_num;
			temp.err_no = 0x0101;
			temp.cmd = rec_com.cmd;
			temp.arg = rec_com.arg;
			temp.dat = data_temp;
			data_temp[0] = PAD_NACK;
		}
	}
	else {
		temp.len = 2+2+1;
		temp.backage_num = rec_com.package_num;
		temp.err_no = 0x0101;
		temp.cmd = rec_com.cmd;
		temp.arg = rec_com.arg;
		temp.dat = data_temp;
		data_temp[0] = PAD_NACK;
	}
	RequestUart1(1,0);
	SendToPadCom(&temp,1);
	FreeUart1();
	return TRUE;
}


uint16 Print(void *arg) {
	uint8 err;
	_df_device_and_pad_comm temp;
	err = 0;
	if (rec_com.arg == 0x31) {
		if ((rec_com.old_package_num != rec_com.package_num)) {
			sys_state.ss.st_cmd.se.print.exe_st = EXE_WRITED;
			RequestUpload();
			temp.err_no = 0x0000;
			err = PAD_ACK;
		}
		else if (sys_state.ss.st_cmd.se.print.exe_flag != 0) {
			err = PAD_NACK;
			temp.err_no = 0x0101;
		}
		else {
			err = PAD_ACK;
		}
		//	准备回复数据
		//err = PAD_ACK;
		temp.len = 2+2+1;
		temp.backage_num = rec_com.package_num;
		temp.cmd = rec_com.cmd;
		temp.arg = rec_com.arg;
		temp.dat = &err;	
	}
	else if (rec_com.arg == 0x32) {
		temp.len = 2+2+1;
		temp.backage_num = rec_com.package_num;
		temp.err_no = 0x0000;
		temp.cmd = rec_com.cmd;
		temp.arg = rec_com.arg;
		temp.dat = &err;
		if ((device_control.cmd.print.exe_st == EXE_RUN_END) || (device_control.cmd.print.exe_st == EXE_WAIT))
		{
			sys_state.ss.st_cmd.se.print.exe_st = EXE_WAIT;
			err = (uint8)device_control.sys_device.print_machine_state;
		}
		else
		{
			temp.err_no = 0x0101;
			err = PAD_NACK;
		}
	}
	else if (rec_com.arg == 0x33)
	{
		sys_state.ss.st_cmd.se.printamount.exe_st = EXE_WRITED;
		RequestUpload();
		temp.len = 2+2+1;
		temp.backage_num = rec_com.package_num;
		temp.err_no = 0x0000;
		temp.cmd = rec_com.cmd;
		temp.arg = rec_com.arg;
		temp.dat = &err;
		err = PAD_ACK;
	}
	else if (rec_com.arg == 0x34)
	{
		sys_state.ss.st_cmd.se.printamount.exe_st = EXE_WRITED;
		RequestUpload();
		temp.len = 2+2+1;
		temp.backage_num = rec_com.package_num;
		temp.err_no = 0x0000;
		temp.cmd = rec_com.cmd;
		temp.arg = rec_com.arg;
		temp.dat = &err;
		err = PAD_ACK;
	}
	else {
		temp.len = 2+2+1;
		temp.backage_num = rec_com.package_num;
		temp.err_no = 0x0001;
		temp.cmd = rec_com.cmd;
		temp.arg = rec_com.arg;
		err = PAD_NACK;
		temp.dat = &err;
	}
	RequestUart1(1,0);
	SendToPadCom(&temp,1);
	FreeUart1();
	return TRUE;
}

uint16 StopTrade(void *arg) {
	uint8 err;
	_df_device_and_pad_comm temp;
	err = 0;
	//	准备回复数据
	err = PAD_ACK;
	temp.len = 2+2+1;
	temp.backage_num = rec_com.package_num;
	temp.err_no = 0x0000;
	temp.cmd = rec_com.cmd;
	temp.arg = rec_com.arg;
	temp.dat = &err;
	RequestUart1(1,0);
	SendToPadCom(&temp,1);
	FreeUart1();
	return TRUE;
}

uint16 ShutDown(void *arg) {
	uint8 err;
	_df_device_and_pad_comm temp;
	err = 0;
	sys_state.ss.st_cmd.se.shutdown.exe_st = EXE_WRITED;			//	执行关机
	sys_state.ss.st_cmd.se.printamount.exe_st = EXE_WRITED;
	RequestUpload();
	//	准备回复数据
	err = PAD_ACK;
	temp.len = 2+2+1;
	temp.backage_num = rec_com.package_num;
	temp.err_no = 0x0000;
	temp.cmd = rec_com.cmd;
	temp.arg = rec_com.arg;
	temp.dat = &err;
	RequestUart1(1,0);
	SendToPadCom(&temp,1);
	FreeUart1();
	return TRUE;
}

uint16 RequestLogin(void *arg)
{
	uint8 err;
	_df_device_and_pad_comm temp;
	err = 0;
	sys_state.ss.st_cmd.se.login.exe_st = EXE_WRITED;			//	执行登陆
	OSTimeDly(2);
	//	准备回复数据
	err = PAD_ACK;
	temp.len = 2+2+1;
	temp.backage_num = rec_com.package_num;
	temp.err_no = 0x0000;
	temp.cmd = rec_com.cmd;
	temp.arg = rec_com.arg;
	temp.dat = &err;
	RequestUart1(1,0);
	SendToPadCom(&temp,1);
	FreeUart1();
	return TRUE;
}

uint16 CashboxInit(void *arg){
	uint8 err;
	_df_device_and_pad_comm temp;
	err = 0;
	
	OSTimeDly(2);
	//	准备回复数据
	err = PAD_ACK;
	temp.len = 2+2+1;
	temp.backage_num = rec_com.package_num;
	temp.err_no = 0x0000;
	temp.cmd = rec_com.cmd;
	temp.arg = rec_com.arg;
	temp.dat = &err;
	RequestUart1(1,0);
	SendToPadCom(&temp,1);
	FreeUart1();
	return TRUE;
}

//	在PAD 上显示提示信息
uint16 DisplayMessage(void *arg)
{
	uint8 err,i=1;
	_df_device_and_pad_comm temp;
	//uint8 *point_temp;
	err = 0;

	temp.backage_num = GetPackageNum() & (~0x80);
	temp.cmd = 0x31;
	temp.arg = 0x33;
	//temp.dat = data_temp;
	switch (*(uint8 *)arg) {
		case 0x33:
			temp.len = 2+strlen(promptmess);
			temp.dat = (uint8 *)promptmess;				//	发送提示信息
			break;
			
		default:
			return PARAMITER_ERR;
	}
	i=0;
	while(1) {
		RequestPADCommunication();
		RequestUart1(1,0);
		do {
			OSSemPend(pad_ack_sem,1,&err);
		}while (err != OS_TIMEOUT);
		SendToPadCom(&temp,0);
		FreeUart1();
		OSSemPend(pad_ack_sem,delay_time,&err);		//	等待100ms 
		FreePADCommunication();
		if (err == OS_NO_ERR) {
			if ((send_com.package_num == temp.backage_num) && (send_com.err_no == 0x0000)) {
				if (*(uint8 *)arg == 0x33) {
					
				}
				else {
					return PAD_COMMUNICATION_DATA_ERR;
				}
				return SYS_NO_ERR;
			}
			else {
				return send_com.err_no;
			}
		}
		else {
			
		}
		if (i<RETRY_TIME) {		//	重发三次
			i++;
		}
		else {
			return COMMUNICATION_TIMEOUT;
		}
	}
}

uint16 Online(void *arg) {
	uint8 err,i=1;
	_df_device_and_pad_comm temp;
	//uint8 *point_temp;
	err = 0;

	temp.backage_num = GetPackageNum() & (~0x80);
	temp.cmd = 0x31;
	temp.arg = *(uint8 *)arg;
	//temp.dat = data_temp;
	switch (*(uint8 *)arg) {
		case 0x31:
			temp.len = 2+16;
			temp.dat = (uint8 *)completeversion;		//	发送下位机版本号
			break;

		case 0x32:
			temp.len = 2 + sizeof(_sys_st_major_s);		//	发送设备主要状态
			temp.dat = sys_state.ss_b;
			break;

		default:
			return PARAMITER_ERR;
	}
	i=0;
	while(1) {
		RequestPADCommunication();
		RequestUart1(1,0);
		do {
			OSSemPend(pad_ack_sem,1,&err);
		}while (err != OS_TIMEOUT);
		SendToPadCom(&temp,0);
		FreeUart1();
		OSSemPend(pad_ack_sem,delay_time,&err);		//	等待100ms 
		FreePADCommunication();
		if (err == OS_NO_ERR) {
			if ((send_com.package_num == temp.backage_num) && (send_com.err_no == 0x0000)) {
				if (*(uint8 *)arg == 0x31) {
					memcpy(PAD_version,send_com.data,16);
					PAD_version[15] = 0;
				}
				else if (*(uint8 *)arg == 0x32) {
					memcpy(&sys_state.ss.st_pad,send_com.data,8);
				}
				else {
					return PAD_COMMUNICATION_DATA_ERR;
				}
				return SYS_NO_ERR;
			}
			else {
				return send_com.err_no;
			}
		}
		else {
			
		}
		if (i<RETRY_TIME) {		//	重发三次
			i++;
		}
		else {
			return COMMUNICATION_TIMEOUT;
		}
	}
}

uint16 Login(void *arg) {
	uint8 err,i=1;
	_df_device_and_pad_comm temp;
	uint8 data_temp[20];
	err = 0;
	temp.backage_num = GetPackageNum() & (~0x80);
	temp.len = 2+1+8+8;
	temp.cmd = 0x32;
	temp.arg = 0x31;
	temp.dat = data_temp; 
	memset(data_temp,0,20);
	data_temp[0] = *(uint8 *)arg;
	if ((*(uint8 *)arg == USER_VALIDATED) || (*(uint8 *)arg == USER_SUPERUSER))
	{
		memcpy(&data_temp[1],device_control.user.uinfo.staffid,7);
		memcpy(&data_temp[9],device_control.user.uinfo.driver_name,8);
	}
	
	while(1) {
		RequestPADCommunication();
		RequestUart1(1,0);
		do {
			OSSemPend(pad_ack_sem,1,&err);
		}while (err != OS_TIMEOUT);
		SendToPadCom(&temp,0);
		FreeUart1();
		OSSemPend(pad_ack_sem,PAD_COMM_WAIT_TIME,&err);		//	等待100ms 
		FreePADCommunication();
		if (err == OS_NO_ERR) {
			if ((send_com.package_num == temp.backage_num) && (send_com.err_no == 0x0000)		//	正常返回
				&& (send_com.data[0] != PAD_NACK)) {
				return SYS_NO_ERR;
			}
			else {
				return send_com.err_no;
			}
		}
		else {
			
		}
		if (i<RETRY_TIME) {		//	重发三次
			i++;
		}
		else {
			return COMMUNICATION_TIMEOUT;
		}
	}
}

uint16 TimeSync(void *arg) {
	uint8 err,i=1;
	//uint16 time_temp;
	_df_device_and_pad_comm temp;
	uint8 data_temp[9];
	err = 0;
	temp.backage_num = GetPackageNum() & (~0x80);
	temp.cmd = 0x33;
	temp.arg = 0x31;
	data_temp[6] = device_control.time.sec;
	data_temp[5] = device_control.time.min;
	data_temp[4] = device_control.time.hour;
	data_temp[3] = device_control.time.day;
	data_temp[2] = device_control.time.month;
	data_temp[1] = (uint8)(device_control.time.year%100);
	data_temp[0] = (uint8)(device_control.time.year/100);
	temp.dat = (uint8 *)&data_temp;
	temp.len = 2+7; 
	while(1) {
		RequestPADCommunication();
		RequestUart1(1,0);
		do {
			OSSemPend(pad_ack_sem,1,&err);
		}while (err != OS_TIMEOUT);
		SendToPadCom(&temp,0);
		FreeUart1();
		OSSemPend(pad_ack_sem,PAD_COMM_WAIT_TIME,&err);		//	等待100ms 
		FreePADCommunication();
		if (err == OS_NO_ERR) {
			if ((send_com.package_num == temp.backage_num) && (send_com.err_no == 0x0000)		//	正常返回
				&& (send_com.data[0] != PAD_NACK)) {
				return SYS_NO_ERR;
			}
			else {
				return send_com.err_no;
			}
		}
		else {
			
		}
		if (i<RETRY_TIME) {		//	重发三次
			i++;
		}
		else {
			return COMMUNICATION_TIMEOUT;
		}
	}
}

uint16 GetStationMess(void *arg) {
	uint8 err,i=1;
	_df_device_and_pad_comm temp;
	uint8 data_temp;
	temp.backage_num = GetPackageNum() & (~0x80);
	temp.cmd = 0x35;
	temp.arg = *(uint8 *)arg;
	if (*(uint8 *)arg == 0x31) {
		temp.len = 2+16;
		temp.dat = device_control.user.rinfo.guid;
	}
	else if (*(uint8 *)arg == 0x32) {
		temp.len = 2+1;
		temp.dat = &data_temp;
		data_temp = ((uint8 *)arg)[1];	//	参数第二个字节为需要查询的站点编号
	}
	else if (*(uint8 *)arg == 0x33) {
		temp.len = 2;
		temp.dat = NULL;
	}
	else {
		return PARAMITER_ERR;
	}
	while (1) {
		RequestPADCommunication();
		RequestUart1(1,0);
		do {
			OSSemPend(pad_ack_sem,1,&err);
		}while (err != OS_TIMEOUT);
		SendToPadCom(&temp,0);
		FreeUart1();

		OSSemPend(pad_ack_sem,PAD_COMM_WAIT_TIME,&err);		//	等待100ms 
		FreePADCommunication();
		if (err == OS_NO_ERR) {
			if (*(uint8 *)arg == 0x31) {
				if ((send_com.package_num == temp.backage_num) && (send_com.err_no == 0x0000)) {		//	正常返回
					return SYS_NO_ERR;
				}
				else {
					return send_com.err_no;
				}
			}
			else if (*(uint8 *)arg == 0x32) {
				if ((send_com.package_num == temp.backage_num) && (send_com.err_no == 0x0000)) {		//	正常返回
					curr_line.station[send_com.data[0]-1].station_no = send_com.data[0];					//	存储站点编号
					memcpy(curr_line.station[send_com.data[0]-1].station_name,&send_com.data[1],8); 		//	存储站点名字
					curr_line.station[send_com.data[0]-1].gps_data[0] = 									//	存储经度
						(send_com.data[9]<<8*3)+(send_com.data[10]<<8*2)+(send_com.data[11]<<8)+send_com.data[12];
					curr_line.station[send_com.data[0]-1].gps_data[1] = 									//	存储维度
						(send_com.data[13]<<8*3)+(send_com.data[14]<<8*2)+(send_com.data[15]<<8)+send_com.data[16];
					return SYS_NO_ERR;
				}
				else {
					return send_com.err_no;
				}
			}
			else  if (*(uint8 *)arg == 0x33) {
				if ((send_com.package_num == temp.backage_num) && (send_com.err_no == 0x0000)
					&& (send_com.len == 3)) {		//	正常返回
					curr_line.line_no = (send_com.data[0]<<8)+send_com.data[1];
					curr_line.line_station_amount = send_com.data[2];
					return SYS_NO_ERR;
				}
				else {
					return send_com.err_no;
				}

			}
		}
		else {
			
		}
		if (i<RETRY_TIME) {		//	重发三次
			i++;
		}
		else {
			return COMMUNICATION_TIMEOUT;
		}
	}
}

uint16 StationSync(void *arg) {
	uint8 err,i=1;
	_df_device_and_pad_comm temp;
	uint8 data_temp[4];
	err = 0;
	temp.backage_num = GetPackageNum() & (~0x80);
	temp.len = 2+2;
	temp.cmd = 0x40;
	temp.arg = 0x31;
	
	if ((curr_station == 0) || (curr_station > curr_line.line_station_amount))
	{
		curr_station = 1;
	}
	data_temp[0] = curr_station;
	data_temp[1] = run_direction;
	temp.dat = data_temp; 
	while(1) {
		RequestPADCommunication();
		RequestUart1(1,0);
		do {
			OSSemPend(pad_ack_sem,1,&err);
		}while (err != OS_TIMEOUT);
		SendToPadCom(&temp,0);
		FreeUart1();
		OSSemPend(pad_ack_sem,PAD_COMM_WAIT_TIME,&err);		//	等待100ms 
		FreePADCommunication();
		if (err == OS_NO_ERR) {
			if ((send_com.package_num == temp.backage_num) && (send_com.err_no == 0x0000)
				&& (send_com.data[0] != PAD_NACK)) {
				return SYS_NO_ERR;
			}
			else {
				return send_com.err_no;
			}
		}
		else {
			
		}
		if (i<RETRY_TIME) {		//	重发三次
			i++;
		}
		else {
			return COMMUNICATION_TIMEOUT;
		}
	}
}

const _rj45_command_s rj45_comm[RJ45_COMM_NUMBER]= {
	{0x34,	LogOut},			//	退出登陆
	//{0x35,	GetRoute},			//	获取路线
	{0x36,	GetDeviceState},	//	获取状态
	{0x37,	ChangeSite},		//	更改站点
	{0x38,	RideMess},			//	乘车信息
	{0x39,	MakeChange},		//	找零
	{0x3a,	StopTrade},			//	取消交易
	{0x3c,	Print},				//	打印
	{0x3e,	ShutDown},			//	关机
	{0x41,	RequestLogin},		//	请求登陆
	{0X42,	CashboxInit},
	{0xff,	ExceptionHandle},
};

const _rj45_command_s rj45_device[]={
	{0x31,	Online},
	{0x32,	Login},
	{0x33,	TimeSync},
	{0x35,	GetStationMess},
	{0x40,	StationSync},
};

#define RJ45_HEAD				1
#define RJ45_LENGTH				2
#define RJ45_PACKAGENUM			3
#define RJ45_DATA				4
#define RJ45_CRC				5
#define RJ45_END				6
#define RJ45_HANDLE				7
#define RJ45_EXCEPTION			8
void TaskPADRecHandle(void *pdata) {

	uint8 err;
	uint8 dat_len=0,rec_len=0;
	uint8 flag;
	uint8 crc_rec[2];
	uint8 rec_temp;
	uint16 bcc;
	uint8 rec_state=RJ45_HEAD;
	pdata = pdata;
	//memset(rec,0,6);
	err = 0;
	while (1) {
		//OSSemPend(Uart0Sem,0,&err);
		if ((rec_state != RJ45_HANDLE) && (rec_state != RJ45_EXCEPTION)) {
			RecFromPad(&rec_temp,1,0);
		}
		switch (rec_state) {				
			case RJ45_HEAD:				//	接收头
				if (rec_temp == '<') {
					rec_state = RJ45_LENGTH;
					bcc = 0;
					bcc = CRCByte(bcc,rec_temp);
					//bcc = CRCByte(0,rec_temp);
				}				
				break;

			case RJ45_LENGTH:			//	接收长度
				dat_len = rec_temp;
				if (dat_len > MAX_RECEIVE_LENGTH) {
					rec_state = RJ45_HEAD;
				}
				else {
					rec_state = RJ45_PACKAGENUM;
					bcc = CRCByte(bcc,rec_temp);
				}
				break;

			case RJ45_PACKAGENUM:		//	接收包号
				if ((rec_temp & 0x80) == 0x80) {
					flag = 1;						//	PAD主
					if (rec_com.old_package_num != rec_temp) {
						rec_com.package_num = rec_temp;
						rec_com.len = dat_len-2;
						rec_com.change_flag.change_flag = 1;
					}
				}
				else {
					if (send_com.old_package_num != rec_temp) {
						send_com.len = dat_len-4;
						send_com.package_num = rec_temp;
						send_com.change_flag.change_flag = 1;
					}
					flag = 0;
				}
				bcc = CRCByte(bcc,rec_temp);
				rec_state = RJ45_DATA;
				rec_len = 0;
				break;

			case RJ45_DATA:				//	接收包内数据
				if (rec_len == 0) {
					break;
				}
				if (flag) {
					if (rec_len == 1) {
						rec_com.cmd = rec_temp;					
					}
					else if (rec_len == 2) {
						rec_com.arg = rec_temp;	
						if (dat_len == 2) {
							rec_state = RJ45_CRC;
							rec_len = 0;
						}
					}
					else if (rec_len < dat_len) {
						rec_com.data[rec_len-3] = rec_temp;
					}
					else {
						rec_com.data[rec_len-3] = rec_temp;
						rec_state = RJ45_CRC;
						rec_len = 0;
					}
				}
				else {
					if (rec_len == 1) {
						send_com.err_no = rec_temp;					
					}
					else if (rec_len == 2) {
						send_com.err_no = (send_com.err_no<<8)+rec_temp;	
					}
					else if (rec_len == 3) {
						send_com.cmd = rec_temp;					
					}
					else if (rec_len == 4) {
						send_com.arg = rec_temp;
						if (dat_len == 4) {
							rec_state = RJ45_CRC;
							rec_len = 0;
						}
					}
					else if (rec_len < dat_len) {
						send_com.data[rec_len-5] = rec_temp;
					}
					else {
						send_com.data[rec_len-5] = rec_temp;
						rec_state = RJ45_CRC;
						rec_len = 0;
					}
				}
				bcc = CRCByte(bcc,rec_temp);
				break;

			case RJ45_CRC:				//	接收校验
				crc_rec[rec_len-1] = rec_temp;
				if (rec_len > 1) {
					if (bcc == (crc_rec[0]*256+crc_rec[1])) {	//	校验
						rec_state = RJ45_END;					//	校验成功，接收尾
					}
					else {
						rec_state = RJ45_EXCEPTION;				//	校验不成功，进入异常处理
						if (flag == 1) {
							rec_com.change_flag.invalid_flag = 0;
						}
						else {
							send_com.change_flag.invalid_flag = 0;
						}
					}
					
				}
				break;

			case RJ45_END:				//	接收尾
				if (rec_temp == '>') {
					if (flag == 1) {
						rec_com.change_flag.invalid_flag = 1;
					}
					else {
						send_com.change_flag.invalid_flag = 1;
					}
					rec_state = RJ45_HANDLE;
				}
				else {
					if (flag == 1) {
						rec_com.change_flag.invalid_flag = 0;
					}
					else {
						send_com.change_flag.invalid_flag = 0;
					}
					rec_state = RJ45_EXCEPTION;
				}
				break;

			case RJ45_HANDLE:
				break;

			case RJ45_EXCEPTION:
				ExceptionHandle(NULL);
				rec_state = RJ45_HEAD;
				break;

			default:
				rec_state = RJ45_HEAD;
				break;	
		
		}
		rec_len ++;
		if (rec_state == RJ45_HANDLE) {
			if (flag == 1) {
				rec_len = 0;
				while((rj45_comm[rec_len++].cmd != rec_com.cmd) && (rec_len < RJ45_COMM_NUMBER)); 
				
				rj45_comm[rec_len-1].func(NULL);			//	执行相应处理函数
				
				rec_com.old_package_num = rec_com.package_num;
				
			}
			else { 
				OSSemPost(pad_ack_sem);
			}
			//analysispadprotol();//解析数据
			rec_state = RJ45_HEAD;
		}
		else {
			
		}
		
	}
}


/************************************************************************************************
** 函数原型: 	void TaskDeviceCommand(void *pdata)
** 功能描述:		设备为主的程序
** 输　入:		
** 输　出: 		无
** 全局变量: 	无
**					
** 调用模块:  	无
**					
** 资源占用:		局部变量:	
**					堆栈:	
**					连续运行时间:
**					
** 作　者:	 杜其俊
** 日　期: 	2012年5月14日
**************************************************************************************************/
#define OUT_LINE				0
#define LOGOUT					1
#define DOWNLOAD_DATA			2
#define LOGIN					3
#define RUNNING					4
#define NO_LOGIN				5
#define CHANGE_SITE				6




uint32 pad_comm_tick;

void TaskDeviceCommand(void *pdata) {
	uint16 err;
	//uint8 pad_communication_num=0;
	//uint8 guid_old[16];
	uint8 run_state;
	uint8 arg[2];
	uint8 station;
	uint16 time_delay_num=0;
	pdata = pdata;

	OSTimeDly(OS_TICKS_PER_SEC*3);
	run_state = OUT_LINE;
	while (1) {
		OSTimeDly(2);
		if (sys_state.ss.st_cmd.se.shutdown.exe_st == EXE_WRITED)
		{
			sys_state.ss.st_cmd.se.shutdown.exe_st = EXE_WAIT;
			sys_state.ss.st_major.ssm.st_pad_online = PAD_COMMUNICATION_DEVICE_OUTAGE;
			run_state = OUT_LINE;
		}
		switch (run_state)
		{
			case OUT_LINE:
				/*IO0CLR |= RJ45_RST;
				OSTimeDly(2);
				IO0SET |= RJ45_RST;
				OSTimeDly(OS_TICKS_PER_SEC*10);*/
				arg[0] = 0x31;
				err = Online(&arg);		//	联机，获取版本号
				if (err == SYS_NO_ERR) 
				{
				}
				else
				{
					OSTimeDly(OS_TICKS_PER_SEC);	//	通信失败，等1S 再发
					break;
				}
				
				sys_state.ss.st_major.ssm.st_user = USER_NO_CARD;
				arg[0] = 0x32;
				err = Online(&arg);		//	联机, 获取状态
				if (err == SYS_NO_ERR)
				{
				}
				else 
				{
					OSTimeDly(OS_TICKS_PER_SEC);	//	通信失败，等1S 再发
					break;
				}
				while (device_control.time.year < 2012) OSTimeDly(20);
				err = TimeSync(NULL);	//	同步时间
				if (err == SYS_NO_ERR)
				{
					run_state = NO_LOGIN;
					arg[0] = 0x31;
				}
				else
				{
					sys_state.ss.st_major.ssm.st_pad_online = PAD_COMMUNICATION_ON_LINE;
					OSTimeDly(OS_TICKS_PER_SEC);	//	通信失败，等1S 再发
				}
				break;

			case NO_LOGIN:
				if (sys_state.ss.st_major.ssm.st_user == USER_VALIDATED)	//	是否登录
				{
					arg[0] = 0x31;
					err = GetStationMess(&arg);					//	查询路线文件
					if (err == SYS_NO_ERR)
					{
						OSTimeDly(OS_TICKS_PER_SEC);
						while (1)
						{
							arg[0] = 0x33;
							err = GetStationMess(&arg);			//	等待PAD读取数据
							if (err == SYS_NO_ERR)
							{
								sys_state.ss.st_major.ssm.st_user = USER_VALIDATED;
								run_state = LOGIN;		//	登录
								break;
							}
							else if (err == PAD_COMMUNICATION_FILE_FORM_ERROR)	//	文件格式错误
							{
								arg[0] = 0x33;
								memset(promptmess,0,sizeof(promptmess));
								sprintf(promptmess,"SD卡中文件格式错误! 请持PAD到管理处更新路线文件。");
								DisplayMessage(arg);					//	PAD 上显示提示信息
								break;
							}
							else if (err == COMMUNICATION_TIMEOUT)
							{
								run_state = OUT_LINE;
								break;
							}
							OSTimeDly(OS_TICKS_PER_SEC/10);
						}
					}
					else if (err == PAD_COMMUNICATION_NO_FILE_WITH_GUID)	//	没有GUID 对应的文件
					{
						arg[0] = 0x33;
						memset(promptmess,0,sizeof(promptmess));
						sprintf(promptmess,"SD卡中没有对应的路线文件! 请持PAD到管理处更新路线文件。");
						DisplayMessage(arg);					//	PAD 上显示提示信息
						break;
					}
					else if (err == PAD_COMMUNICATION_SAME_GUID)	//	已经有相同的GUID 登录
					{
						arg[0] = 0x33;
						err = GetStationMess(&arg);			//	等待PAD读取数据
						if (err == SYS_NO_ERR)
						{
							sys_state.ss.st_major.ssm.st_user = USER_VALIDATED;		//	登录
							arg[0] = 0x32;
							arg[1] = 1;					//	开始下载第一个站点数据
							station = 1;
							run_state = DOWNLOAD_DATA;
						}
					}
					else if (err == PAD_COMMUNICATION_DIFFERENT_GUID)	//	已经有不同的GUID 登录
					{
						run_state = LOGOUT;		//	重新登录
					}
					else if (err == COMMUNICATION_TIMEOUT)
					{
						run_state = OUT_LINE;
						break;
					}
				}
				else
				{
					OSTimeDly(OS_TICKS_PER_SEC/100);	//	没有用户，继续等待
					if ((time_delay_num++) > (100 * 5))
					{
						arg[0] = 0x32;
						err = Online(&arg);
						time_delay_num = 0;
						if (err == SYS_NO_ERR)
						{
						}
						else
						{
							if (err == COMMUNICATION_TIMEOUT)
							{
								sys_state.ss.st_major.ssm.st_pad_online = PAD_COMMUNICATION_NOT_CONNECT;
								run_state = OUT_LINE;
							}
						}
					}
				}
				break;

			case DOWNLOAD_DATA:
				arg[0] = 0x32;
				arg[1] = station;
				err = GetStationMess(&arg);
				if (err == SYS_NO_ERR) {	//	读取路线信息
					//	下载一个站点数据成功
					if ((++station) > curr_line.line_station_amount) {
						run_state = RUNNING;
						time_delay_num = 26;
						sys_state.ss.st_cmd.se.upload_line_data.exe_st = EXE_WRITED;
						sys_state.ss.st_cmd.se.change_site.exe_st = EXE_WRITED;
						RequestUpload();
					}
					else
					{
						arg[1] = station;
					}
				}
				else
				{
					if (err == COMMUNICATION_TIMEOUT)
					{
						sys_state.ss.st_major.ssm.st_pad_online = PAD_COMMUNICATION_NOT_CONNECT;
						run_state = OUT_LINE;
					}
					else
					{
					}
					OSTimeDly(OS_TICKS_PER_SEC);	//	通信失败，等1S 再发
				}
				break;

			case LOGIN:
				//changesite_flag = 0;
				sys_state.ss.st_cmd.se.change_site.exe_st = EXE_WAIT;
				sys_state.ss.st_cmd.se.config_match.exe_st = EXE_WAIT;
				sys_state.ss.st_cmd.se.login.exe_st = EXE_WAIT;
				sys_state.ss.st_cmd.se.logout.exe_st = EXE_WAIT;
				sys_state.ss.st_cmd.se.makechange.exe_st = EXE_WAIT;
				sys_state.ss.st_cmd.se.print.exe_st = EXE_WAIT;
				sys_state.ss.st_cmd.se.shutdown.exe_st = EXE_WAIT;
				sys_state.ss.st_cmd.se.speak.exe_st = EXE_WAIT;
				sys_state.ss.st_cmd.se.upload_line_data.exe_st = EXE_WAIT;
				arg[0] = sys_state.ss.st_major.ssm.st_user;
				err = Login(arg);
				if (err == SYS_NO_ERR) {
					sys_state.ss.st_cmd.se.logout.exe_st = EXE_WAIT;
					run_state = DOWNLOAD_DATA;			//	下载数据
					arg[0] = 0x32;
					arg[1] = 1;
					station = 1;
				}
				else if (err == COMMUNICATION_TIMEOUT)
				{
					sys_state.ss.st_major.ssm.st_pad_online = PAD_COMMUNICATION_NOT_CONNECT;
					run_state = OUT_LINE;
				}
				break;
				
			case RUNNING:
				if (time_delay_num > 25)
				{
					arg[0] = 0x32;
					err = Online(&arg);
					time_delay_num = 0;
				}
				else
				{
					err = SYS_NO_ERR;
				}
				if (err == SYS_NO_ERR) {
					if (sys_state.ss.st_cmd.se.logout.exe_st == EXE_WRITED)
					{
						sys_state.ss.st_cmd.se.logout.exe_st = EXE_WAIT;
						sys_state.ss.st_major.ssm.st_user = USER_HAVE_CARD_NO_LOGIN;
						arg[0] = 0x31;
						run_state = NO_LOGIN;
					}
					else if ((sys_state.ss.st_major.ssm.st_user != USER_SUPERUSER) && (sys_state.ss.st_major.ssm.st_user != USER_VALIDATED))
					{
						run_state = LOGOUT;
						arg[0] = sys_state.ss.st_major.ssm.st_user;
					}
					else if (sys_state.ss.st_pad.login_st == 0x00)
					{
						run_state = NO_LOGIN;
					}
					else if (sys_state.ss.st_cmd.se.change_site.exe_st == EXE_WRITED) 
					{
						sys_state.ss.st_cmd.se.change_site.exe_st = EXE_RUNNING;
						run_state = CHANGE_SITE;
					}
					else
					{
						OSTimeDly(OS_TICKS_PER_SEC/5);
						time_delay_num++;
					}
				}
				else
				{
					if (err == COMMUNICATION_TIMEOUT)
					{
						sys_state.ss.st_major.ssm.st_pad_online = PAD_COMMUNICATION_NOT_CONNECT;
						run_state = OUT_LINE;
					}
				}
				break;
				
			case LOGOUT:
				arg[0] = USER_NO_CARD;
				err = Login(arg);
				if (err == SYS_NO_ERR)
				{
					arg[0] = 0x31;
					run_state = NO_LOGIN;
				}
				else
				{
					if (err == COMMUNICATION_TIMEOUT)
					{
						sys_state.ss.st_major.ssm.st_pad_online = PAD_COMMUNICATION_NOT_CONNECT;
						run_state = OUT_LINE;
					}
				}
				break;
				
			case CHANGE_SITE:
				err = StationSync(NULL);
				if (err == SYS_NO_ERR)
				{
					sys_state.ss.st_cmd.se.change_site.exe_st = EXE_WAIT;
					arg[0] = 0x32;
					run_state = RUNNING;
				}
				else
				{
					if (err == COMMUNICATION_TIMEOUT)
					{
						sys_state.ss.st_major.ssm.st_pad_online = PAD_COMMUNICATION_NOT_CONNECT;
						run_state = OUT_LINE;
					}
				}
				break;
				
			default:
				run_state = OUT_LINE;
				break;
		}

	}
	
}

