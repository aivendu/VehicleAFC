#include "includes.h"

uint32 chip_tick;
_line_mess_s curr_line;
_device_control_s device_control;
uint8  chip_communication_temp[32];
OS_EVENT *data_upload_sem, *chip_communication_sem;

void ChipCommInit(void)
{
	data_upload_sem = OSSemCreate(0);		//	用于PAD 命令处理
	if (data_upload_sem == NULL)
	{
		while(1);
	}
	chip_communication_sem = OSSemCreate(1);
	if (chip_communication_sem == NULL)
	{
		while(1);
	}
}

void RequestUpload(void)
{
	OSSemPost(data_upload_sem);
}
uint8 ChipWriteFrame(uint8 fun, uint16 addr, uint8 len, void *data)
{
	uint8 i;
	uint16 bcc = 0;
	uint16 res_bcc = 0;
	Spi0TranceByte('<');
	addr = (addr & 0xfff) | ((fun << 12) & 0x7000);		//	合成地址数据
	Spi0TranceByte((addr >> 8) & 0x00ff);
	bcc = CRCByte(bcc, (uint8)((addr >> 8) & 0xff));
	Spi0TranceByte(addr & 0x00ff);
	bcc = CRCByte(bcc, (uint8)(addr & 0xff));
	Spi0TranceByte(len);
	bcc = CRCByte(bcc, len);
	for (i = 0; i < len; i++)
	{
		Spi0TranceByte(((uint8 *)data)[i]);
		bcc = CRCByte(bcc, ((uint8 *)data)[i]);
	}
	res_bcc = Spi0TranceByte((bcc >> 8) & 0xff);
	res_bcc = ((res_bcc << 8) & 0xff00) + Spi0TranceByte((uint8)(bcc & 0xff));
	OSTimeDly(2);
	if (res_bcc == bcc)
	{
		return TRUE;
	}
	return FALSE;
}


uint8 ChipReadFrame(uint8 fun, uint16 addr, uint8 len, void *data)
{
	uint8 i, temp;
	uint16 bcc = 0, res_bcc = 0;
	if (len > 32)
	{
		return FALSE;
	}
	Spi0TranceByte('<');
	addr = (addr & 0xfff) | ((fun << 12) & 0x7000) | 0x8000;		//	合成地址数据
	Spi0TranceByte((addr >> 8) & 0x00ff);
	bcc = CRCByte(bcc, (uint8)((addr >> 8) & 0xff));
	Spi0TranceByte(addr & 0x00ff);
	bcc = CRCByte(bcc, (uint8)(addr & 0xff));
	Spi0TranceByte(len);
	bcc = CRCByte(bcc, len);
	for (i = 0; i < len; i++)
	{
		temp = Spi0TranceByte(0x00);
		((uint8 *)data)[i] = temp;
		bcc = CRCByte(bcc, ((uint8 *)data)[i]);
	}
	res_bcc = Spi0TranceByte((bcc >> 8) & 0xff);
	res_bcc = ((res_bcc << 8) & 0xff00) + Spi0TranceByte((uint8)(bcc & 0xff));
	OSTimeDly(2);
	if (res_bcc == bcc)
	{
		return TRUE;
	}
	return FALSE;
}


uint8 ChipDataUpload(uint8 flag, uint8 fun, uint16 addr, uint16 len, void *data)
{
	uint8 num, err;
	OSSemPend(chip_communication_sem, 0, &err);
	if (flag == CHIP_WRITE)
	{
		while (1)
		{
			if (len > 32)
			{
				memcpy(chip_communication_temp, data, 32);	//	取数据
				num = 0;
				while (ChipWriteFrame(fun, addr, 32, chip_communication_temp) == FALSE) // 发送失败，重发
				{
					if ((num++) > 4)
					{
						OSSemPost(chip_communication_sem);
						return FALSE;		//	失败4 次返回
					}
				}
				len -= 32;		//	长度递减
				addr += 32;		//	地址递增
				data = (void *)((uint32)data + 32);		//	地址递增
			}
			else
			{
				memcpy(chip_communication_temp, data, len);	//	取数据
				while (ChipWriteFrame(fun, addr, len, chip_communication_temp) == FALSE) // 发送失败，重发
				{
					if ((num++) > 4)
					{
						OSSemPost(chip_communication_sem);
						return FALSE;		//	失败4 次返回
					}
				}
				break;
			}
		}
	}
	else if (flag == CHIP_READ)
	{
		while (1)
		{
			if (len > 32)
			{
				num = 0;
				while (ChipReadFrame(fun, addr, 32, chip_communication_temp) == FALSE) // 发送失败，重发
				{
					if ((num++) > 4)
					{
						OSSemPost(chip_communication_sem);
						return FALSE;		//	失败4 次返回
					}
				}
				memcpy(data, chip_communication_temp, 32);
				len -= 32;		//	长度递减
				addr += 32;		//	地址递增
				data = (void *)((uint32)data + 32);		//	地址递增
			}
			else
			{
				while (ChipReadFrame(fun, addr, len, chip_communication_temp) == FALSE) // 发送失败，重发
				{
					if ((num++) > 4)
					{
						OSSemPost(chip_communication_sem);
						return FALSE;		//	失败4 次返回
					}
				}
				memcpy(data, chip_communication_temp, len);
				break;
			}
		}
	}
	else
	{
		OSSemPost(chip_communication_sem);
		return FALSE;
	}
	OSSemPost(chip_communication_sem);
	return TRUE;
}
#if 0
uint8 SysCommandHandle(uint8 state, uint8 cmd)
{
	switch (state)
	{
	case EXE_WAIT:
		break;

	case EXE_WRITED:
		if (ChipDataUpload(CHIP_WRITE, 0x00, CONTROL_CMD_CHANGE_INDEX_ADDR, CONTROL_CMD_CHANGE_LENGHT, &device_control.cmd.changemoney) == TRUE)
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
		if (ChipDataUpload(CHIP_WRITE, 0x00, CONTROL_CMD_CHANGE_INDEX_ADDR, CONTROL_CMD_CHANGE_LENGHT, &device_control.cmd.changemoney) == TRUE)
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
#endif
//uint8 data_spi[10000];
//uint8 test_command[] = {'<',0x00,0x01,0x00,0x01,0x05,0x00,0x05};
//uint32 time_delay;
char user_staffid_old[8];
void TaskChipComm(void *pdata)
{

	uint8 err = 0, i;
	//uint8 temp=0;

	void *point_temp;
	//uint8 data_temp;
	//uint8 addr[2];
	pdata = pdata;
#if 0
	while (1)
	{
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
		for (temp = 0; temp < (sizeof(test_command) - 1); temp++)
		{
			data_temp = Spi0TranceByte(test_command[temp]);
			OSTimeDly(time_delay);
		}
		if (data_temp == test_command[temp])
		{
			err = 1;
		}
		else
		{
			err = 0;
		}
		OSTimeDly(1);


	}
#else

	while(1)
	{
		OSTimeDly(2);
		OSSemPend(data_upload_sem, 40, &err);		//	判断是否有
		if (err == OS_NO_ERR)
		{
			if (ChipDataUpload(CHIP_READ, 0x00, CONTROL_CMD_POWEROFF_INDEX_ADDR,
			                   CONTROL_CMD_POWEROFF_LENGHT * 3, CONTROL_CMD_POWEROFF_ADDR) == TRUE)
			{
				if ((sys_state.ss.st_cmd.se.shutdown.exe_st == EXE_WRITED)
				        && ((device_control.cmd.power_off.exe_st == EXE_WAIT) || (device_control.cmd.power_off.exe_st == EXE_RUN_END)))
				{
					device_control.cmd.power_off.exe_st = EXE_WRITED;
					sys_state.ss.st_cmd.se.shutdown.exe_st = EXE_RUNNING;
					while (ChipDataUpload(CHIP_WRITE, 0x00, CONTROL_CMD_POWEROFF_INDEX_ADDR, CONTROL_CMD_POWEROFF_LENGHT, CONTROL_CMD_POWEROFF_ADDR) != TRUE)
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
						device_control.trade_amount.month = ((_trade_manage_data_s *)point_temp)->month;
						device_control.trade_amount.day = ((_trade_manage_data_s *)point_temp)->day;
						device_control.trade_amount.realpay_amount = ((_trade_manage_data_s *)point_temp)->realpay_amount;
						device_control.trade_amount.needpay_amount = ((_trade_manage_data_s *)point_temp)->needpay_amount;
						device_control.trade_amount.coin_dis_amount = ((_trade_manage_data_s *)point_temp)->coin_dis_amount;
						device_control.trade_amount.note_1_dis_amount = ((_trade_manage_data_s *)point_temp)->note_1_dis_amount;
						device_control.trade_amount.note_2_dis_amount = ((_trade_manage_data_s *)point_temp)->note_2_dis_amount;
						device_control.trade_amount.trade_num = ((_trade_manage_data_s *)point_temp)->trade_num;
						device_control.trade_amount.trade_people = ((_trade_manage_data_s *)point_temp)->people_amount;
						memcpy(device_control.trade_amount.driver_id, ((_trade_manage_data_s *)point_temp)->driver_id, 7);
						sys_state.ss.st_cmd.se.printamount.exe_st = EXE_RUNNING;
						while (ChipDataUpload(CHIP_WRITE, 0x00, CONTROL_TRADEAMOUNT_INDEX_ADDR, CONTROL_TRADEAMOUNT_LENGHT, CONTROL_TRADEAMOUNT_ADDR) != TRUE)
						{
							OSTimeDly(2);
						}
						device_control.cmd.print_amount.exe_st = EXE_WRITED;
						while (ChipDataUpload(CHIP_WRITE, 0x00, CONTROL_CMD_PRINTAMOUNT_INDEX_ADDR, CONTROL_CMD_PRINTAMOUNT_LENGHT, CONTROL_CMD_PRINTAMOUNT_ADDR) != TRUE)
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

				if (sys_state.ss.st_cmd.se.upload_line_data.exe_st == EXE_WRITED)
				{
					while (ChipDataUpload(CHIP_WRITE, 0x07, 0, sizeof(_line_mess_s), &curr_line) != TRUE)
					{
						OSTimeDly(2);
					}
					sys_state.ss.st_cmd.se.upload_line_data.exe_st = EXE_WAIT;
				}
			}
			else
			{
				//OSSemPost(data_upload_sem);
			}
		}

		//	更新交易状态信息
		if (ChipDataUpload(CHIP_READ, 0x00, CONTROL_SYS_DEVICE_INDEX_ADDR, CONTROL_SYS_DEVICE_LENGHT, CONTROL_SYS_DEVICE_ADDR) == TRUE)
		{
			memcpy(sys_state.ss.st_other.sso_b, &device_control.sys_device, 3);
		}
		if (ChipDataUpload(CHIP_READ, 0x00, CONTROL_CMD_POWEROFF_INDEX_ADDR,
		                   CONTROL_CMD_POWEROFF_LENGHT * 3, CONTROL_CMD_POWEROFF_ADDR) == TRUE)
		{
			if (device_control.cmd.print_amount.exe_st == EXE_RUN_END)
			{
				device_control.cmd.print_amount.exe_st = EXE_WAIT;
				sys_state.ss.st_cmd.se.printamount.exe_st = EXE_RUN_END;
				while (ChipDataUpload(CHIP_WRITE, 0x00, CONTROL_CMD_PRINTAMOUNT_INDEX_ADDR, CONTROL_CMD_PRINTAMOUNT_LENGHT, CONTROL_CMD_PRINTAMOUNT_ADDR) != TRUE)
				{
					OSTimeDly(2);
				}
			}
		}
		//	更新时间、用户信息、gps信息
		if (ChipDataUpload(CHIP_READ, 0x00, CONTROL_TIME_INDEX_ADDR, CONTROL_TIME_LENGHT + CONTROL_USER_LENGHT + CONTROL_GPS_LENGHT, CONTROL_TIME_ADDR) == TRUE)
		{
			//	更新系统时间
			if (device_control.time.year >= 2012)
			{
				YEAR = device_control.time.year;
				MONTH = device_control.time.month;
				DOM = device_control.time.day;
				HOUR = device_control.time.hour;
				MIN = device_control.time.min;
				SEC = device_control.time.sec;
				if (sys_state.ss.st_other.sso.st_gps_machine == GPS_MODE_NORMAL)	//	gps 是否定位成功
				{
					if (GetTimeUploadState() != 1)
					{
						SetTimeUploadState(1);		//	时间通过GPS 更新
						SetUploadTime(EXE_WRITED);
					}
				}
				else
				{
					if (GetTimeUploadState() != 3)
					{
						SetTimeUploadState(3);		//	时间通过从芯片更新
						SetUploadTime(EXE_WRITED);
					}
				}
			}
			for (i = 0; i < curr_line.line_station_amount; i++)
			{
				if (device_control.gps.gps_state != GPS_MODE_NORMAL)
				{
					//	GPS 定位没有成功，不定位站点，直接返回
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
			//	有卡插入
			if (sys_state.ss.st_major.ssm.st_user != USER_HAVE_CARD_NO_LOGIN)
			{
				if ((device_control.user.uinfo.user_role == 1)
				        && (sys_state.ss.st_major.ssm.st_user != USER_SUPERUSER))
				{
					//	超级用户登录
					LogStoreLogin();
					sys_state.ss.st_major.ssm.st_user = USER_SUPERUSER;
				}
				else if ((strncmp(device_control.user.rinfo.vehicle_plate, GetLisencePlateNum(), 8) == 0)
				         && (sys_state.ss.st_major.ssm.st_user != USER_VALIDATED))
				{
					//	司机登录
					LogStoreLogin();
					sys_state.ss.st_major.ssm.st_user = USER_VALIDATED;
				}
				else if((strncmp(device_control.user.rinfo.vehicle_plate, GetLisencePlateNum(), 8) != 0)
				        && (device_control.user.uinfo.user_role != 1))
				{
					//	不允许登录
					sys_state.ss.st_major.ssm.st_user = USER_LOGIN_NAME_ERR;
				}
			}
		}
		else if (sys_state.ss.st_other.sso.st_ic_machine == IC_MACHINE_NO_CARD)
		{
			//	卡被拔出
			if (sys_state.ss.st_major.ssm.st_user != USER_NO_CARD)
			{
				//	用户退出登录
				LogStoreLogin();
				sys_state.ss.st_major.ssm.st_user = USER_NO_CARD;
			}
		}

	}
#endif
}









