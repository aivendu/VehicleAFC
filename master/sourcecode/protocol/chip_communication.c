#include "includes.h"

uint32 chip_tick;
_line_mess_s curr_line;
_device_control_s device_control;
uint8  chip_communication_temp[32];
OS_EVENT *data_upload_sem;

void ChipCommInit(void) {
	data_upload_sem = OSSemCreate(0);
	if (data_upload_sem == NULL) {
		while(1);
	}
}

void RequestUpload(void) {
	OSSemPost(data_upload_sem);
}
uint8 ChipWriteFrame(uint8 fun,uint16 addr,uint8 len,void *data) {
	uint8 i;
	uint16 bcc = 0;
	uint16 res_bcc = 0;
	Spi0TranceByte('<');
	addr = (addr & 0xfff) | ((fun<<12) & 0x7000);		//	合成地址数据
	Spi0TranceByte((addr>>8)&0x00ff);
	bcc = CRCByte(bcc,(uint8)((addr>>8)&0xff));
	Spi0TranceByte(addr&0x00ff);
	bcc = CRCByte(bcc,(uint8)(addr & 0xff));
	Spi0TranceByte(len);
	bcc = CRCByte(bcc,len);
	for (i=0;i<len;i++) {
		Spi0TranceByte(((uint8 *)data)[i]);
		bcc = CRCByte(bcc,((uint8 *)data)[i]);
	}
	res_bcc = Spi0TranceByte((bcc >> 8)& 0xff);
	res_bcc = ((res_bcc << 8) & 0xff00) + Spi0TranceByte((uint8)(bcc & 0xff));
	OSTimeDly(2);
	if (res_bcc == bcc) {
		return TRUE;
	}
	return FALSE;
}


uint8 ChipReadFrame(uint8 fun,uint16 addr,uint8 len,void *data) {
	uint8 i,temp;
	uint16 bcc=0,res_bcc=0;
	if (len > 32) {
		return FALSE;
	}
	Spi0TranceByte('<');
	addr = (addr & 0xfff) | ((fun<<12) & 0x7000) | 0x8000;		//	合成地址数据
	Spi0TranceByte((addr>>8)&0x00ff);
	bcc = CRCByte(bcc,(uint8)((addr>>8)&0xff));
	Spi0TranceByte(addr&0x00ff);
	bcc = CRCByte(bcc,(uint8)(addr & 0xff));
	Spi0TranceByte(len);
	bcc = CRCByte(bcc,len);
	for (i=0;i<len;i++) {
		temp = Spi0TranceByte(0x00);
		((uint8 *)data)[i] = temp;
		bcc = CRCByte(bcc,((uint8 *)data)[i]);
	}
	res_bcc = Spi0TranceByte((bcc >> 8)& 0xff);
	res_bcc = ((res_bcc << 8) & 0xff00) + Spi0TranceByte((uint8)(bcc & 0xff));
	OSTimeDly(2);
	if (res_bcc == bcc) {
		return TRUE;
	}
	return FALSE;
}


uint8 ChipDataUpload(uint8 flag,uint8 fun,uint16 addr,uint16 len,void *data){
	uint8 num;
	if (flag == CHIP_WRITE) {
		while (1) {
			if (len > 32) {
				memcpy(chip_communication_temp,data,32);	//	取数据
				num = 0;
				while (ChipWriteFrame(fun,addr,32,chip_communication_temp) == FALSE) { // 发送失败，重发
					if ((num++) > 4) return FALSE;		//	失败4 次返回
				}
				len -= 32;		//	长度递减
				addr += 32;		//	地址递增
				data = (void *)((uint32)data+32);		//	地址递增
			}
			else {
				memcpy(chip_communication_temp,data,len);	//	取数据
				while (ChipWriteFrame(fun,addr,len,chip_communication_temp) == FALSE) { // 发送失败，重发
					if ((num++) > 4) return FALSE;		//	失败4 次返回
				}
				break;
			}
		}
	}
	else if (flag == CHIP_READ){
		while (1) {
			if (len > 32) {
				num = 0;
				while (ChipReadFrame(fun,addr,32,chip_communication_temp) == FALSE) { // 发送失败，重发
					if ((num++) > 4) return FALSE;		//	失败4 次返回
				}
				memcpy(data,chip_communication_temp,32);
				len -= 32;		//	长度递减
				addr += 32;		//	地址递增
				data = (void *)((uint32)data+32);		//	地址递增
			}
			else {
				while (ChipReadFrame(fun,addr,len,chip_communication_temp) == FALSE) { // 发送失败，重发
					if ((num++) > 4) return FALSE;		//	失败4 次返回
				}
				memcpy(data,chip_communication_temp,len);
				break;
			}
		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

uint8 SysCommandHandle(uint8 state,uint8 cmd)
{
	switch (state)
	{
		case EXE_WAIT:
			break;
			
		case EXE_WRITED:
			if (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_CMD_CHANGE_INDEX_ADDR,CONTROL_CMD_CHANGE_LENGHT,&device_control.cmd.changemoney) == TRUE) 
			{
				state = EXE_RUNNING;
			}
			break;
			
		case EXE_RUNNING:
			break;
			
		case EXE_RUN_END:
			device_control.cmd.changemoney.exe_st = EXE_WAIT;
			sys_state.ss.st_cmd.se.makechange.exe_st = EXE_RUN_END;
			state = EXE_WAIT;
			break;
			
		case EXE_RUN_ABORT:
			break;
			
		default:
			device_control.cmd.changemoney.exe_st = EXE_WAIT;
			break;
	}
	return state;
}


uint8 TradeHandle(uint8 state)
{
	switch (state)
	{
		case EXE_WAIT:
			if (sys_state.ss.st_cmd.se.makechange.exe_st == EXE_WRITED)
			{
				device_control.cmd.changemoney.exe_st = EXE_WRITED;
				state = EXE_WRITED;
			}
			break;
		case EXE_WRITED:
			if (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_CMD_CHANGE_INDEX_ADDR,CONTROL_CMD_CHANGE_LENGHT,&device_control.cmd.changemoney) == TRUE) 
			{
				state = EXE_RUNNING;
			}
			break;
		case EXE_RUNNING:
			break;
		case EXE_RUN_END:
			device_control.cmd.changemoney.exe_st = EXE_WAIT;
			sys_state.ss.st_cmd.se.makechange.exe_st = EXE_RUN_END;
			state = EXE_WAIT;
			break;
		case EXE_RUN_ABORT:
			break;
		default:
			device_control.cmd.changemoney.exe_st = EXE_WAIT;
			break;
	}
	return state;
}

//uint8 data_spi[10000];
//uint8 test_command[] = {'<',0x00,0x01,0x00,0x01,0x05,0x00,0x05};
//uint32 time_delay;
char user_staffid_old[8];
void TaskChipComm(void *pdata) {

	uint8 err=0,i;
	uint8 temp=0;

	void * point_temp;
	//uint8 data_temp;
	//uint8 addr[2];
	pdata = pdata;
#if 0
	while (1) {
		//delay10us(1);
		/*//	稳定性测试
		temp = Spi0TranceByte((uint8)data_num);
		if ((((temp+1)!=(uint8)data_num) && ((uint8)data_num!=0))) {
			if (data_index < 10000)
				data_spi[data_index++] = temp;
			else 
				while(1);
		}
		//else {
			data_num++;
		//}*/
		//OSTimeDly(100);

		//	命令测试
		for (temp=0; temp<(sizeof(test_command)-1);temp++) {
			data_temp = Spi0TranceByte(test_command[temp]);
			OSTimeDly(time_delay);
		}
		if (data_temp == test_command[temp]) {
			err = 1;
		}
		else {
			err = 0;
		}
		OSTimeDly(1);

		
	}
#else

	while(1) {
		OSSemPend(data_upload_sem,40,&err);		//	判断是否有
		if (err == OS_NO_ERR) {
			if (ChipDataUpload(CHIP_READ,0x00,CONTROL_CMD_INDEX_ADDR,CONTROL_CMD_LENGHT,&device_control.cmd) == TRUE)
			{
				while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_TRADE_INDEX_ADDR,CONTROL_TRADE_LENGHT,&device_control.trade.tm) != TRUE)
				{
					OSTimeDly(2);
				}
				if ((sys_state.ss.st_cmd.se.speak.exe_st == EXE_WRITED) 
					&& ((device_control.cmd.speak.exe_st == EXE_WAIT) || (device_control.cmd.speak.exe_st == EXE_RUN_END)))
				{
					device_control.cmd.speak.exe_st = EXE_WRITED;
					while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_CMD_SPEAK_INDEX_ADDR,CONTROL_CMD_SPEAK_LENGHT,&device_control.cmd.speak) != TRUE) 
					{
						OSTimeDly(2);
					}
				}
				else if (sys_state.ss.st_cmd.se.speak.exe_st == EXE_WRITED)
				{
					OSSemPost(data_upload_sem);
				}
				
				if ((sys_state.ss.st_cmd.se.makechange.exe_st == EXE_WRITED)
					&& ((device_control.cmd.changemoney.exe_st == EXE_WAIT) || (device_control.cmd.changemoney.exe_st == EXE_RUN_END)))
				{
					
					if(device_control.trade.tm.changemoney > 99)
					{
						device_control.cmd.changemoney.exe_st = EXE_WAIT;
						memset(promptmess,0,sizeof(promptmess));
						sprintf(promptmess,"找零错误:01");
						temp = 0x33;
						DisplayMessage(&temp);
					}
					else
					{
						device_control.cmd.changemoney.exe_st = EXE_WRITED;
						while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_CMD_CHANGE_INDEX_ADDR,CONTROL_CMD_CHANGE_LENGHT,&device_control.cmd.changemoney) != TRUE) 
						{
							OSTimeDly(2);
						}
					}
				}
				else if (sys_state.ss.st_cmd.se.makechange.exe_st == EXE_WRITED)
				{
					OSSemPost(data_upload_sem);
				}
				
				if ((sys_state.ss.st_cmd.se.print.exe_st == EXE_WRITED)
					&& ((device_control.cmd.print.exe_st == EXE_WAIT) || (device_control.cmd.print.exe_st == EXE_RUN_END)))
				{
					device_control.cmd.print.exe_st = EXE_WRITED;
					while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_CMD_PRINT_INDEX_ADDR,CONTROL_CMD_PRINT_LENGHT,&device_control.cmd.print) != TRUE) 
					{
						OSTimeDly(2);
					}
				}
				else if (sys_state.ss.st_cmd.se.print.exe_st == EXE_WRITED)
				{
					OSSemPost(data_upload_sem);
				}
				
				if ((sys_state.ss.st_cmd.se.shutdown.exe_st == EXE_WRITED)
					&& ((device_control.cmd.power_off.exe_st == EXE_WAIT) || (device_control.cmd.power_off.exe_st == EXE_RUN_END)))
				{
					device_control.cmd.power_off.exe_st = EXE_WRITED;
					
					while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_CMD_POWEROFF_INDEX_ADDR,CONTROL_CMD_POWEROFF_LENGHT,&device_control.cmd.power_off) != TRUE) 
					{
						OSTimeDly(2);
					}
				}
				else if (sys_state.ss.st_cmd.se.shutdown.exe_st == EXE_WRITED)
				{
					OSSemPost(data_upload_sem);
				}
				if ((sys_state.ss.st_cmd.se.printamount.exe_st == EXE_WRITED)/* && (sys_state.ss.st_major.ssm.st_user == USER_NO_CARD)*/
					&& ((device_control.cmd.print_amount.exe_st == EXE_WAIT) || (device_control.cmd.print_amount.exe_st == EXE_RUN_END)))
				{
					point_temp = GetPADCommandData();
					if (point_temp != NULL)
					{
						device_control.trade_amount.year = ((_trade_manage_data_s *)point_temp)->year;
						device_control.trade_amount.month= ((_trade_manage_data_s *)point_temp)->month;
						device_control.trade_amount.day= ((_trade_manage_data_s *)point_temp)->day;
						device_control.trade_amount.realpay_amount = ((_trade_manage_data_s *)point_temp)->realpay_amount;
						device_control.trade_amount.needpay_amount = ((_trade_manage_data_s *)point_temp)->needpay_amount;
						device_control.trade_amount.coin_dis_amount = ((_trade_manage_data_s *)point_temp)->coin_dis_amount;
						device_control.trade_amount.note_1_dis_amount = ((_trade_manage_data_s *)point_temp)->note_1_dis_amount;
						device_control.trade_amount.note_2_dis_amount = ((_trade_manage_data_s *)point_temp)->note_2_dis_amount;
						device_control.trade_amount.trade_num = ((_trade_manage_data_s *)point_temp)->trade_num;
						device_control.trade_amount.trade_people = ((_trade_manage_data_s *)point_temp)->people_amount;
						memcpy(device_control.trade_amount.driver_id,((_trade_manage_data_s *)point_temp)->driver_id,7);
						sys_state.ss.st_cmd.se.printamount.exe_st = EXE_RUNNING;
						while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_TRADEAMOUNT_INDEX_ADDR,CONTROL_TRADEAMOUNT_LENGHT,&device_control.trade_amount) != TRUE) 
						{
							OSTimeDly(2);
						}
						device_control.cmd.print_amount.exe_st = EXE_WRITED;
						while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_CMD_PRINTAMOUNT_INDEX_ADDR,CONTROL_CMD_PRINTAMOUNT_LENGHT,&device_control.cmd.print_amount) != TRUE) 
						{
							OSTimeDly(2);
						}
					}
					else
					{
						sys_state.ss.st_cmd.se.printamount.exe_st = EXE_WAIT;
					}
					
				}
				/*else if ((sys_state.ss.st_cmd.se.printamount.exe_st == EXE_WRITED) && (sys_state.ss.st_major.ssm.st_user != USER_NO_CARD))
				{
					sys_state.ss.st_cmd.se.printamount.exe_st = EXE_WAIT;
				}*/
				else if (sys_state.ss.st_cmd.se.printamount.exe_st == EXE_WRITED)
				{
					OSSemPost(data_upload_sem);
				}
				
				if (sys_state.ss.st_cmd.se.upload_line_data.exe_st == EXE_WRITED) {
					while (ChipDataUpload(CHIP_WRITE,0x07,0,sizeof(_line_mess_s),&curr_line) != TRUE) {
						OSTimeDly(2);
					}
					sys_state.ss.st_cmd.se.upload_line_data.exe_st = EXE_RUNNING;
				}
			}
			else
			{
				OSSemPost(data_upload_sem);
			}
		}
		
		//	更新交易状态信息
		if (ChipDataUpload(CHIP_READ,0x00,CONTROL_SYS_DEVICE_INDEX_ADDR,CONTROL_SYS_DEVICE_LENGHT,&device_control.sys_device) == TRUE) {
			memcpy(sys_state.ss.st_other.sso_b,&device_control.sys_device,3);
		}
		if (ChipDataUpload(CHIP_READ,0x00,CONTROL_CMD_INDEX_ADDR,CONTROL_CMD_LENGHT,&device_control.cmd) == TRUE)
		{
			if (device_control.cmd.changemoney.exe_st == EXE_RUN_END)
			{
				sys_state.ss.st_cmd.se.makechange.exe_st = EXE_RUN_END;
				device_control.cmd.changemoney.exe_st = EXE_WAIT;
				device_control.cmd.print.exe_st = EXE_WRITED;
				//	更新找零命令
				while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_CMD_CHANGE_INDEX_ADDR,CONTROL_CMD_CHANGE_LENGHT,&device_control.cmd.changemoney) != TRUE)
				{
					OSTimeDly(2);
				}
				//	更新打印命令
				while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_CMD_PRINT_INDEX_ADDR,CONTROL_CMD_PRINT_LENGHT,&device_control.cmd.print) != TRUE)
				{
					OSTimeDly(2);
				}
				//	获取找零结果
				while (ChipDataUpload(CHIP_READ,0x00,CONTROL_TRADE_STATE_INDEX_ADDR,CONTROL_TRADE_STATE_LENGHT,&device_control.trade.ts) != TRUE)
				{
					OSTimeDly(2);
				}
				//	存储找零数据
				sys_state.ss.st_cmd.se.store_trade_data.exe_st = EXE_WRITED;
			}
			else if (device_control.cmd.print.exe_st == EXE_RUN_END)
			{
				device_control.cmd.print.exe_st = EXE_WAIT;
				sys_state.ss.st_cmd.se.print.exe_st = EXE_RUN_END;
#if 0
				//device_control.cmd.changemoney.exe_st = EXE_WRITED;
				sys_state.ss.st_cmd.se.makechange.exe_st = EXE_WRITED;
				OSSemPost(data_upload_sem);
				OSTimeDly(OS_TICKS_PER_SEC*5);
#endif
				while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_CMD_PRINT_INDEX_ADDR,CONTROL_CMD_PRINT_LENGHT,&device_control.cmd.print) != TRUE)
				{
					OSTimeDly(2);
				}
#if 0
				//	上传数据
				trade_data_temp.year = device_control.trade.tm.year+2000;
				trade_data_temp.month= device_control.trade.tm.month;
				trade_data_temp.day = device_control.trade.tm.day;
				trade_data_temp.hour = device_control.trade.tm.hour;
				trade_data_temp.minute= device_control.trade.tm.min;
				trade_data_temp.second= device_control.trade.tm.sec;
				trade_data_temp.current_station= device_control.trade.rm[0].trade_start_st;
				trade_data_temp.serial_number= device_control.trade.tm.serail_num;
				trade_data_temp.needpay= device_control.trade.tm.needpay;
				trade_data_temp.realpay= device_control.trade.tm.realpay;
				trade_data_temp.change_cashbox_1= device_control.trade.cr.coin_dis;
				trade_data_temp.change_cashbox_2= device_control.trade.cr.cass1_dis;
				trade_data_temp.change_cashbox_3= device_control.trade.cr.cass2_dis;
				trade_data_temp.destination_num= device_control.trade.tm.des_num;
				trade_data_temp.people_amount=0;
				for (i=0;i<trade_data_temp.destination_num;i++)
				{
					trade_data_temp.destination[i].start_station= device_control.trade.rm[i].trade_start_st;
					trade_data_temp.destination[i].destination_station= device_control.trade.rm[i].trade_end_st;
					trade_data_temp.destination[i].price= device_control.trade.rm[i].price;
					trade_data_temp.destination[i].people_num= device_control.trade.rm[i].number_of_people;
					trade_data_temp.people_amount += device_control.trade.rm[i].number_of_people;
				}
				GetNextPackage();
				ServerUploadTradeData(&trade_data_temp);
#endif
			}
			if (device_control.cmd.print_amount.exe_st == EXE_RUN_END)
			{
				device_control.cmd.print_amount.exe_st = EXE_WAIT;
				sys_state.ss.st_cmd.se.printamount.exe_st = EXE_RUN_END;
				while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_CMD_PRINTAMOUNT_INDEX_ADDR,CONTROL_CMD_PRINTAMOUNT_LENGHT,&device_control.cmd.print_amount) != TRUE) 
				{
					OSTimeDly(2);
				}
			}
		}
		//	更新时间、用户信息、gps信息
		if (ChipDataUpload(CHIP_READ,0x00,CONTROL_TIME_INDEX_ADDR,CONTROL_TIME_LENGHT+CONTROL_USER_LENGHT+CONTROL_GPS_LENGHT,&device_control.time) == TRUE)
		{
			YEAR = device_control.time.year;
			MONTH = device_control.time.month;
			DOM = device_control.time.day;
			HOUR = device_control.time.hour;
			MIN = device_control.time.min;
			SEC = device_control.time.sec;
			for (i = 0; i < curr_line.line_station_amount; i++)
			{
				if (device_control.gps.gps_state != GPS_MODE_NORMAL)
				{
					break;
				}
				if (((curr_line.station[i].gps_data[1] - 20000) < device_control.gps.gps_longitude)		//	判断当前站点
					&& ((curr_line.station[i].gps_data[1] + 20000) > device_control.gps.gps_longitude)
					&& ((curr_line.station[i].gps_data[0] - 20000) < device_control.gps.gps_latitude)
					&& ((curr_line.station[i].gps_data[0] + 20000) > device_control.gps.gps_latitude))
				{
					/*if (changesite_flag != 0)
					{
						if (curr_station == curr_line.station[i].station_no)
						{
							changesite_flag = 0;
						}
					}
					else */if (curr_station != curr_line.station[i].station_no)		//	站点不同，更新站点
					{
						old_station  = curr_station;
						curr_station = curr_line.station[i].station_no;
						/*if (curr_station == (old_station + 1))
						{
							run_direction = 0;
						}
						else if (curr_station == (old_station - 1))
						{
							run_direction = 1;
						}*/
						if (curr_station == 1)
						{
							run_direction = 0;
						}
						else if (curr_station == curr_line.line_station_amount)
						{
							run_direction = 1;
						}
						sys_state.ss.st_cmd.se.change_site.exe_st = EXE_WRITED;
					}
				}
			}
		}
		if (sys_state.ss.st_other.sso.st_ic_machine == IC_MACHINE_HAVE_CARD)
		{
			if (sys_state.ss.st_major.ssm.st_user != USER_HAVE_CARD_NO_LOGIN)
			{
				if (strncmp(device_control.user.rinfo.vehicle_plate,"苏A00099",8) == 0) {
					memset(user_staffid_old,0,8);
					memcpy(user_staffid_old,device_control.user.uinfo.staffid,7);
					sys_state.ss.st_major.ssm.st_user = USER_VALIDATED;
				}
				else
				{
					sys_state.ss.st_major.ssm.st_user = USER_LOGIN_NAME_ERR;
				}
			}
		}
		else if (sys_state.ss.st_other.sso.st_ic_machine == IC_MACHINE_NO_CARD)
		{
			if (sys_state.ss.st_major.ssm.st_user != USER_NO_CARD)
			{
				GetNextPackage();
				ServerLogout(user_staffid_old);
				sys_state.ss.st_major.ssm.st_user = USER_NO_CARD;
			}
		}
		
	}
#endif
}









