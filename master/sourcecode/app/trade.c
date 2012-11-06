#include "includes.h"

_trade_data_to_server_s trade_data_temp;
_trade_manage_data_s	trade_manage_data_temp;
uint32	current_trade_index;

#define	IDLE					0
#define	CHANGE_MONEY			1
#define	PRINT					2
#define	UPDATA					3
#define	STORE					4

void InitTradeManageData(void)
{
	ReadExternMemery(&current_trade_index,TRADE_DATA_START_ADDR,4);
	ReadExternMemery(&trade_manage_data_temp,current_trade_index,sizeof(_trade_manage_data_s));
}

void TaskTrade(void *pdata)
{
	uint8 trade_state = IDLE,i;
	uint32 addr;
	pdata = pdata;

	OSTimeDly(20);
	InitTradeManageData();
	while (1)
	{
		if (GetTimeUPDataState() != 0)
		{
			if ((YEAR > trade_manage_data_temp.year)
				|| ((YEAR == trade_manage_data_temp.year)
					&& ((MONTH > trade_manage_data_temp.month)
						|| ((MONTH == trade_manage_data_temp.month) && (DOM > trade_manage_data_temp.day))
						)
					)
				)
			{
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
				current_trade_index = trade_manage_data_temp.in;
				trade_manage_data_temp.out = trade_manage_data_temp.in;
				WriteExternMemery(&trade_manage_data_temp,current_trade_index,sizeof(_trade_manage_data_s));
				WriteExternMemery(&current_trade_index, TRADE_DATA_START_ADDR,4);
			}
		}
		switch (trade_state)
		{
			case IDLE:
				if (sys_state.ss.st_cmd.se.store_trade_data.exe_st == EXE_WRITED)
				{
					trade_state = STORE;
				}
				else
				{
					OSTimeDly(2);
				}
				break;

			case CHANGE_MONEY:
				break;

			case PRINT:
				break;

			case UPDATA:
				break;

			case STORE:
				addr = sizeof(_trade_data_to_server_s);
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

				//	×Ü¶îÍ³¼Æ
				trade_manage_data_temp.realpay_amount += trade_data_temp.realpay;
				trade_manage_data_temp.needpay_amount += trade_data_temp.needpay;
				trade_manage_data_temp.coin_dis_amount += trade_data_temp.change_cashbox_1;
				trade_manage_data_temp.note_1_dis_amount += trade_data_temp.change_cashbox_2;
				trade_manage_data_temp.note_2_dis_amount += trade_data_temp.change_cashbox_3;
				trade_manage_data_temp.trade_num++;
				trade_manage_data_temp.people_amount += trade_data_temp.people_amount;
				trade_manage_data_temp.in += sizeof(_trade_data_to_server_s);
				WriteExternMemery(&trade_manage_data_temp,current_trade_index,sizeof(_trade_manage_data_s));

				//	
				break;

			default:
				trade_state = IDLE;
				break;
		}
		if (GetChangeMoneyCommand() == EXE_WAIT)
		{
			
		}
	}
}

