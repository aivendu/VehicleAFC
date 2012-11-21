#include "includes.h"

_trade_data_to_server_s trade_data_temp;
_trade_manage_data_s	trade_manage_data_temp;
uint32	current_trade_index;

#define	IDLE					0		//	空闲状态
#define	CHANGE_MONEY			1		//	找零
#define	PRINT					2		//	打印
#define	TRADE_END				3		//	交易结束处理
#define	STORE					4		//	存储交易数据
#define OBTAIN_RIDE_MESS		5		//	更新交易信息

void InitTradeManageData(void)
{
	ReadExternMemery(&current_trade_index,TRADE_DATA_START_ADDR,4);	
	//	如果还没有存入过交易数据或者数据错误
	if ((current_trade_index <= TRADE_DATA_START_ADDR) || (current_trade_index >= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE)))
	{
		current_trade_index = TRADE_DATA_START_ADDR;
		memset(&trade_manage_data_temp,0,sizeof(_trade_manage_data_s));		//	不需要读取交易数据
	}
	else
	{
		//	读取最后一天的总帐数据
		ReadExternMemery(&trade_manage_data_temp,current_trade_index,sizeof(_trade_manage_data_s));
	}
}

uint8 TradeAmountDataSave(_trade_manage_data_s *data)
{
	trade_manage_data_temp.realpay_amount += trade_data_temp.realpay;
	trade_manage_data_temp.needpay_amount += trade_data_temp.needpay;
	trade_manage_data_temp.coin_dis_amount += trade_data_temp.change_cashbox_1;
	trade_manage_data_temp.note_1_dis_amount += trade_data_temp.change_cashbox_2;
	trade_manage_data_temp.note_2_dis_amount += trade_data_temp.change_cashbox_3;
	trade_manage_data_temp.trade_num++;
	trade_manage_data_temp.people_amount += trade_data_temp.people_amount;
	trade_manage_data_temp.in += sizeof(_trade_data_to_server_s);
	WriteExternMemery(&trade_manage_data_temp,current_trade_index,sizeof(_trade_manage_data_s));
	return 1;
}

uint8 TradeAmountDataRead(_trade_manage_data_s *data)
{
	return 1;
}

uint8 TradeDataSave(_trade_manage_data_s *data)
{
	return 1;
	
}

uint8 TradeDataRead(_trade_manage_data_s *data)
{
	return 1;
}

#define DATA_LENGHT_TEST		11
char const testdata[6] = "71236";
void TaskTrade(void *pdata)
{
	uint8 trade_state = IDLE,i;
	
#if 0
	uint32 addr;
	uint8 temp[3][DATA_LENGHT_TEST+3];
	OSTimeDly(20);

	//	测试铁电
	for (addr = 0; addr < 0xffff; addr += DATA_LENGHT_TEST)
	{
	
	sprintf((char *)temp[0],"00%05s%06d",testdata,addr);
	temp[0][0] = (addr >> 8) & 0xff;
	temp[0][1] = addr & 0xff;
	I2c0WriteBytes(FM24V10_1_0_ADDR,temp[0],DATA_LENGHT_TEST+2);
	
	sprintf((char *)temp[1],"%05s%06d",testdata,addr);
	WriteExternMemery(temp[1],FM24V10_2_0_VIRTUAL_ADDR+addr,DATA_LENGHT_TEST);
	
	sprintf((char *)temp[2],"%05s%06d",testdata,addr);
	I2c0WriteMemery(FM24V10_3_0_ADDR,&addr,2,temp[2],DATA_LENGHT_TEST);
	
	memset(temp[0],0,13);
	I2c0ReadMemery(FM24V10_1_0_ADDR,&addr,2,temp[0],DATA_LENGHT_TEST);
	
	memset(temp[1],0,13);
	ReadExternMemery(temp[1],FM24V10_2_0_VIRTUAL_ADDR+addr,DATA_LENGHT_TEST);
	
	memset(temp[2],0,13);
	temp[2][0] = (addr >> 8) & 0xff;
	temp[2][1] = addr & 0xff;
	I2c0WriteReadBytes(FM24V10_3_0_ADDR,temp[2],2,temp[2],DATA_LENGHT_TEST);
	
	}
#endif

	pdata = pdata;
	
	ConfigInit();
	InitTradeManageData();
	InitLog();
	while (1)
	{
		switch (trade_state)
		{
			case IDLE:
				if (sys_state.ss.st_cmd.se.change_ride_mess.exe_st == EXE_WRITED)
				{
					sys_state.ss.st_cmd.se.change_ride_mess.exe_st = EXE_RUNNING;
					trade_state = OBTAIN_RIDE_MESS;
				}
				else if (sys_state.ss.st_cmd.se.makechange.exe_st == EXE_WRITED)
				{
					sys_state.ss.st_cmd.se.makechange.exe_st = EXE_RUNNING;
					trade_state = CHANGE_MONEY;
				}
				/*else if (sys_state.ss.st_cmd.se.store_trade_data.exe_st == EXE_WRITED)
				{
					sys_state.ss.st_cmd.se.store_trade_data.exe_st = EXE_RUNNING;
					trade_state = STORE;
				}*/
				else
				{
					if (GetTimeUploadState() != 0)
					{
						//	时间已更新
						if ((YEAR > trade_manage_data_temp.year)
							|| ((YEAR == trade_manage_data_temp.year)
								&& ((MONTH > trade_manage_data_temp.month)
									|| ((MONTH == trade_manage_data_temp.month) && (DOM > trade_manage_data_temp.day))
									)
								)
							)
						{
							//	新的一天的交易数据
							if (GetIntelligentChange() == 0)
							{
								SetCashbox1Balance(200);
								SetCashbox2Balance(20);
								SetCashbox3Balance(40);
							}
							trade_manage_data_temp.last_day_addr = current_trade_index;
							trade_manage_data_temp.year = YEAR;
							trade_manage_data_temp.month = MONTH;
							trade_manage_data_temp.day = DOM;
							trade_manage_data_temp.needpay_amount = 0;
							trade_manage_data_temp.realpay_amount = 0;
							trade_manage_data_temp.coin_dis_amount = 0;
							trade_manage_data_temp.note_1_dis_amount = 0;
							trade_manage_data_temp.note_2_dis_amount = 0;
							trade_manage_data_temp.trade_num = 0;
							memcpy(trade_manage_data_temp.driver_id,device_control.user.uinfo.staffid,7);
							if (current_trade_index == TRADE_DATA_START_ADDR)		//	是否有交易数据存储
							{
								//	交易数据接着current_trade_index 存储
								current_trade_index = TRADE_DATA_START_ADDR + sizeof(current_trade_index);
								//	交易日志存储接着trade_manage_data_temp 存储
								trade_manage_data_temp.in = current_trade_index + sizeof(_trade_manage_data_s);
							}
							else
							{
								if ((trade_manage_data_temp.in + sizeof(_trade_manage_data_s)) 		//	下一天的交易是否会超出存储区域
									>= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
								{
									//	超出了，接着current_trade_index 开始存储
									current_trade_index = TRADE_DATA_START_ADDR + sizeof(current_trade_index);
									trade_manage_data_temp.in = current_trade_index + sizeof(_trade_manage_data_s);
								}
								else
								{
									//	修改当前日期的交易数据索引地址
									current_trade_index = trade_manage_data_temp.in;
									trade_manage_data_temp.in = current_trade_index + sizeof(_trade_manage_data_s);
									if ((trade_manage_data_temp.in + sizeof(_trade_data_to_server_s)) 	//	下一笔交易是否会超出存储区域
											>= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
									{
										//	超出了，接着current_trade_index 开始存储
										trade_manage_data_temp.in = TRADE_DATA_START_ADDR + sizeof(current_trade_index);
									}
								}
								
							}
							trade_manage_data_temp.out = trade_manage_data_temp.in;
							//	存储数据
							WriteExternMemery(&trade_manage_data_temp,current_trade_index,sizeof(_trade_manage_data_s));
							WriteExternMemery(&current_trade_index, TRADE_DATA_START_ADDR,sizeof(current_trade_index));
						}
					}
					OSTimeDly(2);
				}
				break;

			case OBTAIN_RIDE_MESS:
				//	更新交易数据
				while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_TRADE_INDEX_ADDR,CONTROL_TRADE_LENGHT,CONTROL_TRADE_ADDR) != TRUE)
				{
					OSTimeDly(2);
				}
				sys_state.ss.st_cmd.se.change_ride_mess.exe_st = EXE_WAIT;
				
				if (sys_state.ss.st_cmd.se.speak.exe_st == EXE_WRITED)
				{
					//	播放语音
					device_control.cmd.speak.exe_st = EXE_WRITED;
					while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_CMD_SPEAK_INDEX_ADDR,CONTROL_CMD_SPEAK_LENGHT,CONTROL_CMD_SPEAK_ADDR) != TRUE) 
					{
						OSTimeDly(2);
					}
					sys_state.ss.st_cmd.se.speak.exe_st = EXE_WAIT;
				}
				trade_state = IDLE;
				break;

			case CHANGE_MONEY:
				if ((device_control.trade.tm.changemoney != (device_control.trade.tm.realpay - device_control.trade.tm.needpay))
					|| (device_control.trade.tm.changemoney > 99)		//	最多找零99 元
					)
				{
					//	找零数据不正确
					memset(promptmess,0,sizeof(promptmess));
					sprintf(promptmess,"找零数据错误，只能找零0元到99元，请重新操作");
					i = 0x33;
					DisplayMessage(&i);
					sys_state.ss.st_cmd.se.makechange.exe_st = EXE_WAIT;
					trade_state = IDLE;
					break;
				}
				else{
					device_control.trade.tm.changenum = 0;
					
					if (GetIntelligentChange())			//	是否使用智能找零功能
					{
						//	计算第三钱箱找零个数
						if ((device_control.trade.tm.changemoney / GetCashbox3Value()) > GetCashbox3Balance())
						{
							//	第三钱箱的钱不够
							//	(第三钞箱* 2000 +第二钞箱个数*100 +第一钞箱个数)
							device_control.trade.tm.changenum = GetCashbox3Balance() * 2000;	//	把剩余金额找完
							device_control.trade.tm.changemoney -= GetCashbox3Balance() * GetCashbox3Value();
						}
						else
						{
							device_control.trade.tm.changenum = (device_control.trade.tm.changemoney / GetCashbox3Value()) * 2000;
							device_control.trade.tm.changemoney = device_control.trade.tm.changemoney % GetCashbox3Value();
						}
						//	计算第二钱箱找零个数
						if ((device_control.trade.tm.changemoney / GetCashbox2Value()) > GetCashbox2Balance())
						{
							//	第二钱箱的钱不够
							device_control.trade.tm.changenum += GetCashbox2Balance() * 100;		//	把剩余金额找完
							device_control.trade.tm.changemoney -= GetCashbox2Balance() * GetCashbox2Value();
						}
						else
						{
							device_control.trade.tm.changenum = (device_control.trade.tm.changemoney / GetCashbox2Value()) * 100;
							device_control.trade.tm.changemoney = device_control.trade.tm.changemoney % GetCashbox2Value();
							
						}
						//	计算第一钱箱找零个数
						if ((device_control.trade.tm.changemoney / GetCashbox1Value()) > GetCashbox1Balance())
						{
							//	第一钱箱的钱不够
							memset(promptmess,0,sizeof(promptmess));
							sprintf(promptmess,"钱箱金额不够，不能找零");
							i = 0x33;
							DisplayMessage(&i);
							sys_state.ss.st_cmd.se.makechange.exe_st = EXE_WAIT;
							trade_state = IDLE;
							break;
						}
						else
						{
							device_control.trade.tm.changenum += (device_control.trade.tm.changemoney / GetCashbox1Value());
							device_control.trade.tm.changemoney = device_control.trade.tm.changemoney % GetCashbox1Value();
							
						}
						device_control.trade.tm.changemoney = device_control.trade.tm.realpay - device_control.trade.tm.needpay;
					}
					else
					{
						device_control.trade.tm.changenum = device_control.trade.tm.changemoney / GetCashbox3Value() * 2000
												+ (device_control.trade.tm.changemoney % GetCashbox3Value()) / GetCashbox2Value() * 100
												+ (device_control.trade.tm.changemoney % GetCashbox2Value()) / GetCashbox1Value();
					}
					//	读找零和打印命令
					if (ChipDataUpload(CHIP_READ,0x00,CONTROL_CMD_CHANGE_INDEX_ADDR,CONTROL_CMD_CHANGE_LENGHT+CONTROL_CMD_PRINT_LENGHT,CONTROL_CMD_CHANGE_ADDR) == TRUE)
					{
						if (((device_control.cmd.changemoney.exe_st == EXE_RUN_END) || (device_control.cmd.changemoney.exe_st == EXE_WAIT))
							|| ((device_control.cmd.print.exe_st == EXE_RUN_END) || (device_control.cmd.print.exe_st == EXE_WAIT)))
						{
							//	设备空闲，可以继续进行找零
						}
						else
						{
							//	设备正在找零或打印，不能进行找零
							memset(promptmess,0,sizeof(promptmess));
							sprintf(promptmess,"设备正在找零或打印，不能进行新的找零或打印");
							i = 0x33;
							DisplayMessage(&i);
							sys_state.ss.st_cmd.se.makechange.exe_st = EXE_WAIT;
							trade_state = IDLE;
							break;
						}

						//	更新交易数据
						while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_TRADE_INDEX_ADDR,CONTROL_TRADE_LENGHT,CONTROL_TRADE_ADDR) != TRUE)
						{
							OSTimeDly(2);
						}
						if (device_control.trade.tm.changenum > 0)
						{
							//	找零
							device_control.cmd.changemoney.exe_st = EXE_WRITED;
							while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_CMD_CHANGE_INDEX_ADDR,CONTROL_CMD_CHANGE_LENGHT,CONTROL_CMD_CHANGE_ADDR) != TRUE) 
							{
								OSTimeDly(2);
							}
							OSTimeDly(200);
						}
						else
						{
							device_control.cmd.changemoney.exe_st = EXE_RUN_END;
						}
						sys_state.ss.st_cmd.se.makechange.exe_st = EXE_RUNNING;
						trade_state = PRINT;
					}
				}
				break;

			case PRINT:
				if (ChipDataUpload(CHIP_READ,0x00,CONTROL_CMD_CHANGE_INDEX_ADDR,CONTROL_CMD_CHANGE_LENGHT+CONTROL_CMD_PRINT_LENGHT,CONTROL_CMD_CHANGE_ADDR) == TRUE)
				{
					if (device_control.cmd.changemoney.exe_st == EXE_RUN_END)
					{
						sys_state.ss.st_cmd.se.makechange.exe_st = EXE_RUN_END;		//	找零结束
						sys_state.ss.st_cmd.se.print.exe_st = EXE_RUNNING;			//	开始打印
						//	读取找零结果
						while (ChipDataUpload(CHIP_READ,0x00,CONTROL_TRADE_STATE_INDEX_ADDR,CONTROL_TRADE_STATE_LENGHT,CONTROL_TRADE_STATE_ADDR) != TRUE)
						{
							OSTimeDly(2);
						}
						//	找零结束，开始打印
						device_control.cmd.changemoney.exe_st = EXE_WAIT;
						device_control.cmd.print.exe_st = EXE_WRITED;
						//	更新找零和打印命令
						while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_CMD_CHANGE_INDEX_ADDR,CONTROL_CMD_CHANGE_LENGHT+CONTROL_CMD_PRINT_LENGHT,CONTROL_CMD_CHANGE_ADDR) != TRUE)
						{
							OSTimeDly(2);
						}
						trade_state = STORE;
					}
				}
				break;

			case STORE:
				memset(promptmess,0,sizeof(promptmess));
				sprintf(promptmess,"实找%d元: 1元 %d个; 5元 %d张; 10元 %d张",
					(device_control.trade.cr.coin_dis+device_control.trade.cr.cass1_dis*5+device_control.trade.cr.cass2_dis*10),
					device_control.trade.cr.coin_dis,device_control.trade.cr.cass1_dis,device_control.trade.cr.cass2_dis);
				i = 0x33;
				DisplayMessage(&i);
				trade_data_temp.year = device_control.trade.tm.year+2000;
				trade_data_temp.month = device_control.trade.tm.month;
				trade_data_temp.day = device_control.trade.tm.day;
				trade_data_temp.hour = device_control.trade.tm.hour;
				trade_data_temp.minute = device_control.trade.tm.min;
				trade_data_temp.second = device_control.trade.tm.sec;
				trade_data_temp.serial_number = device_control.trade.tm.serail_num;
				trade_data_temp.needpay = device_control.trade.tm.needpay;
				trade_data_temp.realpay = device_control.trade.tm.realpay;
				trade_data_temp.change_cashbox_1 = device_control.trade.cr.coin_dis;
				trade_data_temp.change_cashbox_2 = device_control.trade.cr.cass1_dis;
				trade_data_temp.change_cashbox_3 = device_control.trade.cr.cass2_dis;
				trade_data_temp.current_station = device_control.trade.rm[0].trade_start_st;
				trade_data_temp.destination_num = device_control.trade.tm.des_num;
				trade_data_temp.people_amount = 0;
				for (i = 0; i < device_control.trade.tm.des_num; i++)
				{
					trade_data_temp.people_amount += device_control.trade.rm[i].number_of_people;
				}
				memcpy(trade_data_temp.destination,device_control.trade.rm,MAX_DESTINATION_NUM*4);
				WriteExternMemery(&trade_data_temp,trade_manage_data_temp.in,sizeof(_trade_data_to_server_s));

				//	总额统计
				trade_manage_data_temp.realpay_amount += trade_data_temp.realpay;
				trade_manage_data_temp.needpay_amount += trade_data_temp.needpay;
				trade_manage_data_temp.coin_dis_amount += trade_data_temp.change_cashbox_1;
				trade_manage_data_temp.note_1_dis_amount += trade_data_temp.change_cashbox_2;
				trade_manage_data_temp.note_2_dis_amount += trade_data_temp.change_cashbox_3;
				trade_manage_data_temp.trade_num++;
				trade_manage_data_temp.people_amount += trade_data_temp.people_amount;
				trade_manage_data_temp.in += sizeof(_trade_data_to_server_s);
				if ((trade_manage_data_temp.in + sizeof(_trade_data_to_server_s)) 
						>= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
				{
					trade_manage_data_temp.in = TRADE_DATA_START_ADDR + sizeof(current_trade_index);
				}
				WriteExternMemery(&trade_manage_data_temp,current_trade_index,sizeof(_trade_manage_data_s));
				
				//	更新余额
				SetCashbox1Balance(GetCashbox1Balance() - device_control.trade.cr.coin_dis);
				SetCashbox2Balance(GetCashbox2Balance() - device_control.trade.cr.cass1_dis);
				SetCashbox3Balance(GetCashbox3Balance() - device_control.trade.cr.cass2_dis);
				sys_state.ss.st_cmd.se.store_trade_data.exe_st = EXE_WAIT;
				if (GetTradeUploadState() == 0)
				{
					SetTradeUploadState(1);		//	进行上传数据
					SetSaveConfig(EXE_WRITED);
				}
				trade_state = TRADE_END;

				//	
				break;

			case TRADE_END:
				if (ChipDataUpload(CHIP_READ,0x00,CONTROL_CMD_CHANGE_INDEX_ADDR,CONTROL_CMD_CHANGE_LENGHT+CONTROL_CMD_PRINT_LENGHT,CONTROL_CMD_CHANGE_ADDR) == TRUE)
				{
					if (device_control.cmd.print.exe_st == EXE_RUN_END)
					{
						//	找零结束
						device_control.cmd.print.exe_st = EXE_WAIT;
						while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_CMD_CHANGE_INDEX_ADDR,CONTROL_CMD_CHANGE_LENGHT+CONTROL_CMD_PRINT_LENGHT,CONTROL_CMD_CHANGE_ADDR) != TRUE)
						{
							OSTimeDly(2);
						}
						sys_state.ss.st_cmd.se.print.exe_st = EXE_RUN_END;
						trade_state = IDLE;
					}
				}
				break;
			default:
				trade_state = IDLE;
				break;
		}
	}
}

//	搜索指定日期的交易总帐
uint8 SearchTradeData(uint16 year, uint8 month, uint8 day,_trade_manage_data_s *data)
{
	*data = trade_manage_data_temp;
	while ((data->year != year) || (data->month != month) || (data->day != day))
	{
		if (data->last_day_addr == TRADE_DATA_START_ADDR)
		{
			//	当前交易是第一天交易，没有搜索到数据
			return FALSE;
		}
		if (trade_manage_data_temp.in > current_trade_index)		//	判断当天的日志存储区域是否经过越界点
		{
			//	没有经过越界点
			if ((data->last_day_addr < trade_manage_data_temp.in) && (data->last_day_addr > current_trade_index))
			{
				//	上一日的日志已被覆盖，没有搜索到数据
				return FALSE;
			}
		}
		else
		{
			//	经过越界点
			if ((data->last_day_addr < trade_manage_data_temp.in) || (data->last_day_addr > current_trade_index))
			{
				//	上一日的日志已被覆盖，没有搜索到数据
				return FALSE;
			}
		}
		ReadExternMemery(data,data->last_day_addr,sizeof(_trade_manage_data_s));
	}
	return TRUE;
}


