#include "includes.h"

_trade_data_to_server_s trade_data_temp;
_trade_manage_data_s	trade_manage_data_temp;
uint32	current_trade_index;

#define	IDLE					0		//	����״̬
#define	CHANGE_MONEY			1		//	����
#define	PRINT					2		//	��ӡ
#define	TRADE_END				3		//	���׽�������
#define	STORE					4		//	�洢��������
#define OBTAIN_RIDE_MESS		5		//	���½�����Ϣ

void InitTradeManageData(void)
{
	ReadExternMemery(&current_trade_index,TRADE_DATA_START_ADDR,4);	
	//	�����û�д�����������ݻ������ݴ���
	if ((current_trade_index <= TRADE_DATA_START_ADDR) || (current_trade_index >= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE)))
	{
		current_trade_index = TRADE_DATA_START_ADDR;
		memset(&trade_manage_data_temp,0,sizeof(_trade_manage_data_s));		//	����Ҫ��ȡ��������
	}
	else
	{
		//	��ȡ���һ�����������
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

	//	��������
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
						//	ʱ���Ѹ���
						if ((YEAR > trade_manage_data_temp.year)
							|| ((YEAR == trade_manage_data_temp.year)
								&& ((MONTH > trade_manage_data_temp.month)
									|| ((MONTH == trade_manage_data_temp.month) && (DOM > trade_manage_data_temp.day))
									)
								)
							)
						{
							//	�µ�һ��Ľ�������
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
							if (current_trade_index == TRADE_DATA_START_ADDR)		//	�Ƿ��н������ݴ洢
							{
								//	�������ݽ���current_trade_index �洢
								current_trade_index = TRADE_DATA_START_ADDR + sizeof(current_trade_index);
								//	������־�洢����trade_manage_data_temp �洢
								trade_manage_data_temp.in = current_trade_index + sizeof(_trade_manage_data_s);
							}
							else
							{
								if ((trade_manage_data_temp.in + sizeof(_trade_manage_data_s)) 		//	��һ��Ľ����Ƿ�ᳬ���洢����
									>= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
								{
									//	�����ˣ�����current_trade_index ��ʼ�洢
									current_trade_index = TRADE_DATA_START_ADDR + sizeof(current_trade_index);
									trade_manage_data_temp.in = current_trade_index + sizeof(_trade_manage_data_s);
								}
								else
								{
									//	�޸ĵ�ǰ���ڵĽ�������������ַ
									current_trade_index = trade_manage_data_temp.in;
									trade_manage_data_temp.in = current_trade_index + sizeof(_trade_manage_data_s);
									if ((trade_manage_data_temp.in + sizeof(_trade_data_to_server_s)) 	//	��һ�ʽ����Ƿ�ᳬ���洢����
											>= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
									{
										//	�����ˣ�����current_trade_index ��ʼ�洢
										trade_manage_data_temp.in = TRADE_DATA_START_ADDR + sizeof(current_trade_index);
									}
								}
								
							}
							trade_manage_data_temp.out = trade_manage_data_temp.in;
							//	�洢����
							WriteExternMemery(&trade_manage_data_temp,current_trade_index,sizeof(_trade_manage_data_s));
							WriteExternMemery(&current_trade_index, TRADE_DATA_START_ADDR,sizeof(current_trade_index));
						}
					}
					OSTimeDly(2);
				}
				break;

			case OBTAIN_RIDE_MESS:
				//	���½�������
				while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_TRADE_INDEX_ADDR,CONTROL_TRADE_LENGHT,CONTROL_TRADE_ADDR) != TRUE)
				{
					OSTimeDly(2);
				}
				sys_state.ss.st_cmd.se.change_ride_mess.exe_st = EXE_WAIT;
				
				if (sys_state.ss.st_cmd.se.speak.exe_st == EXE_WRITED)
				{
					//	��������
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
					|| (device_control.trade.tm.changemoney > 99)		//	�������99 Ԫ
					)
				{
					//	�������ݲ���ȷ
					memset(promptmess,0,sizeof(promptmess));
					sprintf(promptmess,"�������ݴ���ֻ������0Ԫ��99Ԫ�������²���");
					i = 0x33;
					DisplayMessage(&i);
					sys_state.ss.st_cmd.se.makechange.exe_st = EXE_WAIT;
					trade_state = IDLE;
					break;
				}
				else{
					device_control.trade.tm.changenum = 0;
					
					if (GetIntelligentChange())			//	�Ƿ�ʹ���������㹦��
					{
						//	�������Ǯ���������
						if ((device_control.trade.tm.changemoney / GetCashbox3Value()) > GetCashbox3Balance())
						{
							//	����Ǯ���Ǯ����
							//	(��������* 2000 +�ڶ��������*100 +��һ�������)
							device_control.trade.tm.changenum = GetCashbox3Balance() * 2000;	//	��ʣ��������
							device_control.trade.tm.changemoney -= GetCashbox3Balance() * GetCashbox3Value();
						}
						else
						{
							device_control.trade.tm.changenum = (device_control.trade.tm.changemoney / GetCashbox3Value()) * 2000;
							device_control.trade.tm.changemoney = device_control.trade.tm.changemoney % GetCashbox3Value();
						}
						//	����ڶ�Ǯ���������
						if ((device_control.trade.tm.changemoney / GetCashbox2Value()) > GetCashbox2Balance())
						{
							//	�ڶ�Ǯ���Ǯ����
							device_control.trade.tm.changenum += GetCashbox2Balance() * 100;		//	��ʣ��������
							device_control.trade.tm.changemoney -= GetCashbox2Balance() * GetCashbox2Value();
						}
						else
						{
							device_control.trade.tm.changenum = (device_control.trade.tm.changemoney / GetCashbox2Value()) * 100;
							device_control.trade.tm.changemoney = device_control.trade.tm.changemoney % GetCashbox2Value();
							
						}
						//	�����һǮ���������
						if ((device_control.trade.tm.changemoney / GetCashbox1Value()) > GetCashbox1Balance())
						{
							//	��һǮ���Ǯ����
							memset(promptmess,0,sizeof(promptmess));
							sprintf(promptmess,"Ǯ���������������");
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
					//	������ʹ�ӡ����
					if (ChipDataUpload(CHIP_READ,0x00,CONTROL_CMD_CHANGE_INDEX_ADDR,CONTROL_CMD_CHANGE_LENGHT+CONTROL_CMD_PRINT_LENGHT,CONTROL_CMD_CHANGE_ADDR) == TRUE)
					{
						if (((device_control.cmd.changemoney.exe_st == EXE_RUN_END) || (device_control.cmd.changemoney.exe_st == EXE_WAIT))
							|| ((device_control.cmd.print.exe_st == EXE_RUN_END) || (device_control.cmd.print.exe_st == EXE_WAIT)))
						{
							//	�豸���У����Լ�����������
						}
						else
						{
							//	�豸����������ӡ�����ܽ�������
							memset(promptmess,0,sizeof(promptmess));
							sprintf(promptmess,"�豸����������ӡ�����ܽ����µ�������ӡ");
							i = 0x33;
							DisplayMessage(&i);
							sys_state.ss.st_cmd.se.makechange.exe_st = EXE_WAIT;
							trade_state = IDLE;
							break;
						}

						//	���½�������
						while (ChipDataUpload(CHIP_WRITE,0x00,CONTROL_TRADE_INDEX_ADDR,CONTROL_TRADE_LENGHT,CONTROL_TRADE_ADDR) != TRUE)
						{
							OSTimeDly(2);
						}
						if (device_control.trade.tm.changenum > 0)
						{
							//	����
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
						sys_state.ss.st_cmd.se.makechange.exe_st = EXE_RUN_END;		//	�������
						sys_state.ss.st_cmd.se.print.exe_st = EXE_RUNNING;			//	��ʼ��ӡ
						//	��ȡ������
						while (ChipDataUpload(CHIP_READ,0x00,CONTROL_TRADE_STATE_INDEX_ADDR,CONTROL_TRADE_STATE_LENGHT,CONTROL_TRADE_STATE_ADDR) != TRUE)
						{
							OSTimeDly(2);
						}
						//	�����������ʼ��ӡ
						device_control.cmd.changemoney.exe_st = EXE_WAIT;
						device_control.cmd.print.exe_st = EXE_WRITED;
						//	��������ʹ�ӡ����
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
				sprintf(promptmess,"ʵ��%dԪ: 1Ԫ %d��; 5Ԫ %d��; 10Ԫ %d��",
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

				//	�ܶ�ͳ��
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
				
				//	�������
				SetCashbox1Balance(GetCashbox1Balance() - device_control.trade.cr.coin_dis);
				SetCashbox2Balance(GetCashbox2Balance() - device_control.trade.cr.cass1_dis);
				SetCashbox3Balance(GetCashbox3Balance() - device_control.trade.cr.cass2_dis);
				sys_state.ss.st_cmd.se.store_trade_data.exe_st = EXE_WAIT;
				if (GetTradeUploadState() == 0)
				{
					SetTradeUploadState(1);		//	�����ϴ�����
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
						//	�������
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

//	����ָ�����ڵĽ�������
uint8 SearchTradeData(uint16 year, uint8 month, uint8 day,_trade_manage_data_s *data)
{
	*data = trade_manage_data_temp;
	while ((data->year != year) || (data->month != month) || (data->day != day))
	{
		if (data->last_day_addr == TRADE_DATA_START_ADDR)
		{
			//	��ǰ�����ǵ�һ�콻�ף�û������������
			return FALSE;
		}
		if (trade_manage_data_temp.in > current_trade_index)		//	�жϵ������־�洢�����Ƿ񾭹�Խ���
		{
			//	û�о���Խ���
			if ((data->last_day_addr < trade_manage_data_temp.in) && (data->last_day_addr > current_trade_index))
			{
				//	��һ�յ���־�ѱ����ǣ�û������������
				return FALSE;
			}
		}
		else
		{
			//	����Խ���
			if ((data->last_day_addr < trade_manage_data_temp.in) || (data->last_day_addr > current_trade_index))
			{
				//	��һ�յ���־�ѱ����ǣ�û������������
				return FALSE;
			}
		}
		ReadExternMemery(data,data->last_day_addr,sizeof(_trade_manage_data_s));
	}
	return TRUE;
}


