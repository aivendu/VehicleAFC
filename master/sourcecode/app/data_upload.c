#include "includes.h"


void TaskDataUpload(void *pdata)
{
	uint8 err;

	uint32 upload_index;
	_trade_manage_data_s upload_manage_temp;
	_trade_data_to_server_s upload_data_temp;
	_log_s *upload_log_temp;
		
	pdata = pdata;
	OSTimeDly(OS_TICKS_PER_SEC);
	memset(&upload_manage_temp,0,sizeof(_trade_manage_data_s));
	memset(&upload_data_temp,0,sizeof(_trade_data_to_server_s));
	while (1)
	{
		GetNextPackage();
		err = ServerGPSData((device_control.gps.gps_state == 3),device_control.gps.gps_latitude,device_control.gps.gps_longitude,device_control.gps.gps_movingspeed);
		if (err == GPRS_DATA_NO_ERR)
		{
			if (GetTradeUploadState() == 1)
			{
				//	上传交易数据
				if (upload_manage_temp.out == trade_manage_data_temp.out)		//	是否当天的数据
				{
					if (trade_manage_data_temp.out != trade_manage_data_temp.in)
					{
						//	有数据没有上传
						ReadExternMemery(&upload_data_temp,upload_manage_temp.out,sizeof(_trade_data_to_server_s));
						GetNextPackage();
						if (ServerUploadTradeData(&upload_data_temp) == GPRS_DATA_NO_ERR)
						{
							upload_manage_temp.out += sizeof(_trade_data_to_server_s);
							trade_manage_data_temp.out = upload_manage_temp.out;
						}
					}
					if (trade_manage_data_temp.out == trade_manage_data_temp.in)
					{
						//	数据发送完毕
						SetTradeUploadState(0);
						SetSaveConfig(EXE_WRITED);
					}
				}
				else
				{
					//	上传以前的数据
					upload_index = trade_manage_data_temp.last_day_addr;
					ReadExternMemery(&upload_manage_temp,upload_index,sizeof(_trade_manage_data_s));
					while (upload_manage_temp.in != upload_manage_temp.out)
					{
						if (upload_manage_temp.last_day_addr == TRADE_DATA_START_ADDR)
						{
							//	当前是最早的交易数据
							break;
						}
						
						if (trade_manage_data_temp.in > current_trade_index)		//	判断当天的日志存储区域是否经过越界点
						{
							//	没有经过越界点
							if ((upload_manage_temp.last_day_addr < trade_manage_data_temp.in) && (upload_manage_temp.last_day_addr > current_trade_index))
							{
								//	上一日的日志已被覆盖
								break;
							}
						}
						else
						{
							//	经过越界点
							if ((upload_manage_temp.last_day_addr < trade_manage_data_temp.in) || (upload_manage_temp.last_day_addr > current_trade_index))
							{
								//	上一日的日志已被覆盖
								break;
							}
						}
						upload_index = upload_manage_temp.last_day_addr;		//	修改索引，读取上一天数据
						ReadExternMemery(&upload_manage_temp,upload_index,sizeof(_trade_manage_data_s));
					}
					
					while ((upload_manage_temp.in == upload_manage_temp.out) && (upload_index != current_trade_index))
					{
						upload_index = upload_manage_temp.in;
						ReadExternMemery(&upload_manage_temp,upload_index,sizeof(_trade_manage_data_s));
					}

					if (upload_index == current_trade_index)
					{
						//	数据都上传完成
						continue;
					}
					//	读取和上传数据
					ReadExternMemery(&upload_data_temp,upload_manage_temp.out,sizeof(_trade_data_to_server_s));
					GetNextPackage();
					if (ServerUploadTradeData(&upload_data_temp) == GPRS_DATA_NO_ERR)
					{
						//	更新数据管理数据
						upload_manage_temp.out += sizeof(_trade_data_to_server_s);
						if ((upload_index >= (TRADE_DATA_START_ADDR + sizeof(current_trade_index))) 
							|| ((upload_index + sizeof(_trade_manage_data_s)) < (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE)))
						{
							WriteExternMemery(&upload_manage_temp,upload_index,sizeof(_trade_manage_data_s));
							if ((upload_manage_temp.out == upload_manage_temp.in) && (upload_index != current_trade_index))
							{
								//	读下一天的数据
								upload_index = upload_manage_temp.in;
								ReadExternMemery(&upload_manage_temp,upload_index,sizeof(_trade_manage_data_s));
							}
							
						}
						else
						{
						}
					}
				}
			}
			//upload_log_temp = ReadLog(2,NULL,0,0);
			if (upload_log_temp != NULL)
			{
				if (upload_log_temp->state == 1)
				{
					//	已发送数据
				}
				else
				{
					//ReadLog(2,NULL,0,0);
				}
			}
			else
			{
				
			}
			OSTimeDly(OS_TICKS_PER_SEC*5);
			//OSTaskSuspend(OS_PRIO_SELF);
		}
		else
		{
			OSTimeDly(OS_TICKS_PER_SEC*10);
			
		}
		
	}
}

uint8 UploadAllMemery(uint32 start, uint32 end)
{
	uint8 data_temp[32];
	while (1)
	{
		//ServerLogin();
	}
}

