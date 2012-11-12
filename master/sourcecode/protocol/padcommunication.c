#include "includes.h"

#define SendToPad(c)		Uart1SendByte(c,0)
#define RecFromPad(a,b,c)	Uart1RecByte(a,b,c)


#define MAX_RECEIVE_LENGTH			250		//	n�ֽ�
#define MAX_COMM_DELAY				30		//	n*5ms
#define SHELL_LENGTH				5		//	ͷ
#define MAX_STATION_NUM				30
#define PAD_COMM_WAIT_TIME			delay_time

#define	AllowRJ45RecAndSend()		(IO0CLR |= RJ45_CTS)
#define DisableRJ45RecAndSend()		(IO0SET |= RJ45_CTS)

static _pad_com_task rec_com,send_com;
static OS_EVENT *pad_event_sem;
uint32 delay_time = 60;			//	���Դ���

uint8 station_quantity;
char promptmess[100]={"��ӭ����! welcome to here!"};
char PAD_version[16];

_df_device_and_pad_comm comm_rec_temp;
static OS_EVENT *pad_ack_sem;
static uint8 send_package_num;


#define GetPackageNum()				(send_package_num++)
#define ResetPackageNum()			(send_package_num=0)
/*
*	PADͨ�ų�ʼ��
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
*	����PAD ͨ����Դ
*/
void RequestPADCommunication(void)
{
	uint8 err;
	OSSemPend(pad_event_sem,0,&err);
}

/*
*	�ͷ�PAD ͨ����Դ
*/

void FreePADCommunication(void)
{
	OSSemPost(pad_event_sem);
}



/*****************************************************************************************************
** ����ԭ��: 	uint8 SendToPadCom(const _df_device_and_pad_comm *dat,uint8 flag)
** ��������:		CRT711 ��ȡ����صĺ�����
** �䡡��:		���� 
** �䡡��: 		��
** ȫ�ֱ���: 	��
**					
** ����ģ��:  	��
**					
** ��Դռ��:		�ֲ�����:	
**					��ջ:	
**					��������ʱ��:
**					
** ������:	 ���信
** �ա���: 	2012��5��14��
*****************************************************************************************************/

uint8 SendToPadCom(const _df_device_and_pad_comm *dat,uint8 flag) {
	uint8 err,i;
	uint16 bcc = 0;
	
	err = SendToPad('<');			//	����ͷ'<'
	if (err != TRUE) {			//	����ʧ�ܷ���
		return err;					//	���ش���
	}
	bcc = CRCByte(bcc,'<');				//	����У����
	
	err = SendToPad(dat->len);			//	���ͳ���
	if (err != TRUE) {
		return err;
	}
	bcc = CRCByte(bcc,dat->len);
	
	err = SendToPad(dat->backage_num);			//	���Ͱ���
	if (err != TRUE) {
		return err;
	}
	bcc = CRCByte(bcc,dat->backage_num);
	if (flag) {
		err = SendToPad((uint8)(dat->err_no>>8));			//	���ʹ����Ÿ��ֽ�
		if (err != TRUE) {
			return err;
		}
		bcc = CRCByte(bcc,(uint8)(dat->err_no>>8));

		err = SendToPad((uint8)(dat->err_no));			//	���ʹ����ŵ��ֽ�
		if (err != TRUE) {
			return err;
		}
		bcc = CRCByte(bcc,(uint8)(dat->err_no));
	}

	err = SendToPad(dat->cmd);			//	���ʹ����Ÿ��ֽ�
	if (err != TRUE) {
		return err;
	}
	bcc = CRCByte(bcc,dat->cmd);

	err = SendToPad(dat->arg);			//	���ʹ����ŵ��ֽ�
	if (err != TRUE) {
		return err;
	}
	bcc = CRCByte(bcc,dat->arg);

	if (flag) {
		for (i=0; i<(dat->len-4); i++) {			//	��������
			err = SendToPad(*(dat->dat+i));
			if (err != TRUE) {
				return err;
			}
			bcc = CRCByte(bcc,*(dat->dat+i));
		}
	}
	else {
		for (i=0; i<(dat->len-2); i++) {			//	��������
			err = SendToPad(*(dat->dat+i));
			if (err != TRUE) {
				return err;
			}
			bcc = CRCByte(bcc,*(dat->dat+i));
		}
	}
	err = SendToPad((uint8)(bcc>>8));	//	����У����ֽ�
	if (err != TRUE) {
		return err;
	}
	err = SendToPad((uint8)bcc);	//	����У����ֽ�			
	if (err != TRUE) {
		return err;
	}
	err = SendToPad('>');			//	����β'>'			
	if (err != TRUE) {
		return err;
	}
	return 0;			//	������ɣ����ط��ͳɹ�
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
uint16 GetDeviceState(void *arg) {			//	·�߱�����Զ����ɵ�GUID ����
	_df_device_and_pad_comm temp;
	temp.len = 2+2+16;
	temp.backage_num = rec_com.package_num;
	temp.err_no = 0x0000;
	temp.cmd = rec_com.cmd;
	temp.arg = rec_com.arg;
	temp.dat = sys_state.ss_b;		//	����״̬
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
	old_station = rec_com.data[0];			//	�����յ�����ʷվ��
	curr_station = rec_com.data[1];			//	�����յ��ĵ�ǰվ��
	run_direction = rec_com.data[2];		//	������ʻ����
	changesite_flag  =  1;
	OSTimeDly(2);
	data_temp[0] = curr_station;			//	���ص�ǰվ������з���
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
	/*field_temp = (char *)(&rec_com.data[0]);			//	׼���˳���Ϣ
	next_field = field_temp;
	while (*next_field != ';') next_field++;			//	�������վ���
	*next_field = '\0';									//	�γ��ַ���
	next_field++;										//	��������һ���ֶ�
	exe_trade.rm[index].trade_start_st = (uint8)atoi(field_temp);	//	�洢��ʼվ��
	field_temp = next_field;
	
	while (*next_field != ';') next_field++;			//	�����踶
	*next_field = '\0';
	next_field++;
	exe_trade.tm.needpay = (uint16)atoi(field_temp);
	field_temp = next_field;

	while (index<MAX_RIDE_NUMBER_OF_PEOPLE) {			//	�����˳��˵�վ��Ϣ
		while (*next_field != '*') next_field++;		//	������վ���
		*next_field = '\0';
		next_field++;
		exe_trade.rm[index].trade_end_st = (uint8)atoi(field_temp);
		exe_trade.rm[index].trade_start_st = exe_trade.rm[0].trade_start_st;
		field_temp = next_field;

		while (*next_field != ';') next_field++;		//	��������վ�������
		*next_field = '\0';
		next_field++;
		exe_trade.rm[index].number_of_people = (uint8)atoi(field_temp);
		field_temp = next_field;

		while ((*next_field != ';') || (*next_field != ',') || (*next_field != '.')) next_field++;		//	��������
		if ((*next_field == ',') || (*next_field == '.')) {			//	�Ƿ�������һ���ֶ�
			*next_field = '\0';										//	�γ��ַ���
			next_field++;											//	��������һ���ֶ�
			exe_trade.rm[index].price = (uint16)atoi(field_temp);	//	�洢Ʊ��
			field_temp = next_field;								//	��һ���ֶ�
			exe_trade.rm[index].next = NULL;						//	����û���ֶ�
			break;
		}
		else {
			*next_field = '\0';										//	�γ��ַ���
			next_field++;											//	��������һ���ֶ�
			exe_trade.rm[index].price = (uint16)atoi(field_temp);	//	�洢Ʊ��
			field_temp = next_field;								//	��һ���ֶ�
			exe_trade.rm[index].next = &exe_trade.rm[index+1];		//	׼����һ����վ��Ϣ�洢
			index ++;
		}
	}
*/
	//	׼���ظ�����
	temp.len = 2+2+1;
	temp.backage_num = rec_com.package_num;
	if (device_control.trade.tm.des_num > MAX_RIDE_NUMBER_OF_PEOPLE) {	
		temp.err_no = 0x0101;		//	û�д���
		data_temp = PAD_NACK;
	}
	else {
		temp.err_no = 0x0000;		//	û�д���
		data_temp = PAD_ACK;
		sys_state.ss.st_cmd.se.speak.exe_st = EXE_WRITED;
		RequestUpload();
	}
	temp.cmd = rec_com.cmd;
	temp.arg = rec_com.arg;
	temp.dat = &data_temp;		//	�ظ�ACK
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
			sys_state.ss.st_cmd.se.makechange.exe_st = EXE_WRITED;			//	ִ����������
			device_control.trade.tm.serail_num = rec_com.data[0]*256+rec_com.data[1];	//	�洢�˴ν��׵���ˮ��
			memcpy(&device_control.trade.tm,&rec_com.data[2],6);						//	�洢����ʱ��
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
				temp.err_no = 0x0000;		//	�������
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
		//	׼���ظ�����
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
		//	׼���ظ�����
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
	//	׼���ظ�����
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
	sys_state.ss.st_cmd.se.shutdown.exe_st = EXE_WRITED;			//	ִ�йػ�
	sys_state.ss.st_cmd.se.printamount.exe_st = EXE_WRITED;
	RequestUpload();
	//	׼���ظ�����
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
	sys_state.ss.st_cmd.se.login.exe_st = EXE_WRITED;			//	ִ�е�½
	OSTimeDly(2);
	//	׼���ظ�����
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
	//	׼���ظ�����
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

//	��PAD ����ʾ��ʾ��Ϣ
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
			temp.dat = (uint8 *)promptmess;				//	������ʾ��Ϣ
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
		OSSemPend(pad_ack_sem,delay_time,&err);		//	�ȴ�100ms 
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
		if (i<RETRY_TIME) {		//	�ط�����
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
			temp.dat = (uint8 *)completeversion;		//	������λ���汾��
			break;

		case 0x32:
			temp.len = 2 + sizeof(_sys_st_major_s);		//	�����豸��Ҫ״̬
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
		OSSemPend(pad_ack_sem,delay_time,&err);		//	�ȴ�100ms 
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
		if (i<RETRY_TIME) {		//	�ط�����
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
		OSSemPend(pad_ack_sem,PAD_COMM_WAIT_TIME,&err);		//	�ȴ�100ms 
		FreePADCommunication();
		if (err == OS_NO_ERR) {
			if ((send_com.package_num == temp.backage_num) && (send_com.err_no == 0x0000)		//	��������
				&& (send_com.data[0] != PAD_NACK)) {
				return SYS_NO_ERR;
			}
			else {
				return send_com.err_no;
			}
		}
		else {
			
		}
		if (i<RETRY_TIME) {		//	�ط�����
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
		OSSemPend(pad_ack_sem,PAD_COMM_WAIT_TIME,&err);		//	�ȴ�100ms 
		FreePADCommunication();
		if (err == OS_NO_ERR) {
			if ((send_com.package_num == temp.backage_num) && (send_com.err_no == 0x0000)		//	��������
				&& (send_com.data[0] != PAD_NACK)) {
				return SYS_NO_ERR;
			}
			else {
				return send_com.err_no;
			}
		}
		else {
			
		}
		if (i<RETRY_TIME) {		//	�ط�����
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
		data_temp = ((uint8 *)arg)[1];	//	�����ڶ����ֽ�Ϊ��Ҫ��ѯ��վ����
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

		OSSemPend(pad_ack_sem,PAD_COMM_WAIT_TIME,&err);		//	�ȴ�100ms 
		FreePADCommunication();
		if (err == OS_NO_ERR) {
			if (*(uint8 *)arg == 0x31) {
				if ((send_com.package_num == temp.backage_num) && (send_com.err_no == 0x0000)) {		//	��������
					return SYS_NO_ERR;
				}
				else {
					return send_com.err_no;
				}
			}
			else if (*(uint8 *)arg == 0x32) {
				if ((send_com.package_num == temp.backage_num) && (send_com.err_no == 0x0000)) {		//	��������
					curr_line.station[send_com.data[0]-1].station_no = send_com.data[0];					//	�洢վ����
					memcpy(curr_line.station[send_com.data[0]-1].station_name,&send_com.data[1],8); 		//	�洢վ������
					curr_line.station[send_com.data[0]-1].gps_data[0] = 									//	�洢����
						(send_com.data[9]<<8*3)+(send_com.data[10]<<8*2)+(send_com.data[11]<<8)+send_com.data[12];
					curr_line.station[send_com.data[0]-1].gps_data[1] = 									//	�洢ά��
						(send_com.data[13]<<8*3)+(send_com.data[14]<<8*2)+(send_com.data[15]<<8)+send_com.data[16];
					return SYS_NO_ERR;
				}
				else {
					return send_com.err_no;
				}
			}
			else  if (*(uint8 *)arg == 0x33) {
				if ((send_com.package_num == temp.backage_num) && (send_com.err_no == 0x0000)
					&& (send_com.len == 3)) {		//	��������
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
		if (i<RETRY_TIME) {		//	�ط�����
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
		OSSemPend(pad_ack_sem,PAD_COMM_WAIT_TIME,&err);		//	�ȴ�100ms 
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
		if (i<RETRY_TIME) {		//	�ط�����
			i++;
		}
		else {
			return COMMUNICATION_TIMEOUT;
		}
	}
}

const _rj45_command_s rj45_comm[RJ45_COMM_NUMBER]= {
	{0x34,	LogOut},			//	�˳���½
	//{0x35,	GetRoute},			//	��ȡ·��
	{0x36,	GetDeviceState},	//	��ȡ״̬
	{0x37,	ChangeSite},		//	����վ��
	{0x38,	RideMess},			//	�˳���Ϣ
	{0x39,	MakeChange},		//	����
	{0x3a,	StopTrade},			//	ȡ������
	{0x3c,	Print},				//	��ӡ
	{0x3e,	ShutDown},			//	�ػ�
	{0x41,	RequestLogin},		//	�����½
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
			case RJ45_HEAD:				//	����ͷ
				if (rec_temp == '<') {
					rec_state = RJ45_LENGTH;
					bcc = 0;
					bcc = CRCByte(bcc,rec_temp);
					//bcc = CRCByte(0,rec_temp);
				}				
				break;

			case RJ45_LENGTH:			//	���ճ���
				dat_len = rec_temp;
				if (dat_len > MAX_RECEIVE_LENGTH) {
					rec_state = RJ45_HEAD;
				}
				else {
					rec_state = RJ45_PACKAGENUM;
					bcc = CRCByte(bcc,rec_temp);
				}
				break;

			case RJ45_PACKAGENUM:		//	���հ���
				if ((rec_temp & 0x80) == 0x80) {
					flag = 1;						//	PAD��
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

			case RJ45_DATA:				//	���հ�������
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

			case RJ45_CRC:				//	����У��
				crc_rec[rec_len-1] = rec_temp;
				if (rec_len > 1) {
					if (bcc == (crc_rec[0]*256+crc_rec[1])) {	//	У��
						rec_state = RJ45_END;					//	У��ɹ�������β
					}
					else {
						rec_state = RJ45_EXCEPTION;				//	У�鲻�ɹ��������쳣����
						if (flag == 1) {
							rec_com.change_flag.invalid_flag = 0;
						}
						else {
							send_com.change_flag.invalid_flag = 0;
						}
					}
					
				}
				break;

			case RJ45_END:				//	����β
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
				
				rj45_comm[rec_len-1].func(NULL);			//	ִ����Ӧ������
				
				rec_com.old_package_num = rec_com.package_num;
				
			}
			else { 
				OSSemPost(pad_ack_sem);
			}
			//analysispadprotol();//��������
			rec_state = RJ45_HEAD;
		}
		else {
			
		}
		
	}
}


/************************************************************************************************
** ����ԭ��: 	void TaskDeviceCommand(void *pdata)
** ��������:		�豸Ϊ���ĳ���
** �䡡��:		
** �䡡��: 		��
** ȫ�ֱ���: 	��
**					
** ����ģ��:  	��
**					
** ��Դռ��:		�ֲ�����:	
**					��ջ:	
**					��������ʱ��:
**					
** ������:	 ���信
** �ա���: 	2012��5��14��
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
				err = Online(&arg);		//	��������ȡ�汾��
				if (err == SYS_NO_ERR) 
				{
				}
				else
				{
					OSTimeDly(OS_TICKS_PER_SEC);	//	ͨ��ʧ�ܣ���1S �ٷ�
					break;
				}
				
				sys_state.ss.st_major.ssm.st_user = USER_NO_CARD;
				arg[0] = 0x32;
				err = Online(&arg);		//	����, ��ȡ״̬
				if (err == SYS_NO_ERR)
				{
				}
				else 
				{
					OSTimeDly(OS_TICKS_PER_SEC);	//	ͨ��ʧ�ܣ���1S �ٷ�
					break;
				}
				while (device_control.time.year < 2012) OSTimeDly(20);
				err = TimeSync(NULL);	//	ͬ��ʱ��
				if (err == SYS_NO_ERR)
				{
					run_state = NO_LOGIN;
					arg[0] = 0x31;
				}
				else
				{
					sys_state.ss.st_major.ssm.st_pad_online = PAD_COMMUNICATION_ON_LINE;
					OSTimeDly(OS_TICKS_PER_SEC);	//	ͨ��ʧ�ܣ���1S �ٷ�
				}
				break;

			case NO_LOGIN:
				if (sys_state.ss.st_major.ssm.st_user == USER_VALIDATED)	//	�Ƿ��¼
				{
					arg[0] = 0x31;
					err = GetStationMess(&arg);					//	��ѯ·���ļ�
					if (err == SYS_NO_ERR)
					{
						OSTimeDly(OS_TICKS_PER_SEC);
						while (1)
						{
							arg[0] = 0x33;
							err = GetStationMess(&arg);			//	�ȴ�PAD��ȡ����
							if (err == SYS_NO_ERR)
							{
								sys_state.ss.st_major.ssm.st_user = USER_VALIDATED;
								run_state = LOGIN;		//	��¼
								break;
							}
							else if (err == PAD_COMMUNICATION_FILE_FORM_ERROR)	//	�ļ���ʽ����
							{
								arg[0] = 0x33;
								memset(promptmess,0,sizeof(promptmess));
								sprintf(promptmess,"SD�����ļ���ʽ����! ���PAD����������·���ļ���");
								DisplayMessage(arg);					//	PAD ����ʾ��ʾ��Ϣ
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
					else if (err == PAD_COMMUNICATION_NO_FILE_WITH_GUID)	//	û��GUID ��Ӧ���ļ�
					{
						arg[0] = 0x33;
						memset(promptmess,0,sizeof(promptmess));
						sprintf(promptmess,"SD����û�ж�Ӧ��·���ļ�! ���PAD����������·���ļ���");
						DisplayMessage(arg);					//	PAD ����ʾ��ʾ��Ϣ
						break;
					}
					else if (err == PAD_COMMUNICATION_SAME_GUID)	//	�Ѿ�����ͬ��GUID ��¼
					{
						arg[0] = 0x33;
						err = GetStationMess(&arg);			//	�ȴ�PAD��ȡ����
						if (err == SYS_NO_ERR)
						{
							sys_state.ss.st_major.ssm.st_user = USER_VALIDATED;		//	��¼
							arg[0] = 0x32;
							arg[1] = 1;					//	��ʼ���ص�һ��վ������
							station = 1;
							run_state = DOWNLOAD_DATA;
						}
					}
					else if (err == PAD_COMMUNICATION_DIFFERENT_GUID)	//	�Ѿ��в�ͬ��GUID ��¼
					{
						run_state = LOGOUT;		//	���µ�¼
					}
					else if (err == COMMUNICATION_TIMEOUT)
					{
						run_state = OUT_LINE;
						break;
					}
				}
				else
				{
					OSTimeDly(OS_TICKS_PER_SEC/100);	//	û���û��������ȴ�
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
				if (err == SYS_NO_ERR) {	//	��ȡ·����Ϣ
					//	����һ��վ�����ݳɹ�
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
					OSTimeDly(OS_TICKS_PER_SEC);	//	ͨ��ʧ�ܣ���1S �ٷ�
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
					run_state = DOWNLOAD_DATA;			//	��������
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

