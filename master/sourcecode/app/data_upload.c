#include "includes.h"


void TaskDataUpload(void *pdata)
{
	uint8 err;

	uint32 upload_index;
	_trade_manage_data_s upload_manage_temp;
	_trade_data_to_server_s upload_data_temp;
	_log_s *upload_log_temp;
	//uint8 log_data_temp[50];
	
	pdata = pdata;
	OSTimeDly(OS_TICKS_PER_SEC);
	memset(&upload_manage_temp, 0, sizeof(_trade_manage_data_s));
	memset(&upload_data_temp, 0, sizeof(_trade_data_to_server_s));
	while (1)
	{
		GetNextPackage();
		err = ServerGPSData((device_control.gps.gps_state == 3), device_control.gps.gps_latitude, device_control.gps.gps_longitude, device_control.gps.gps_movingspeed);
		if (err == SERVER_DATA_NO_ERR)
		{
			if (GetTradeUploadState() == 1)
			{
				if ((trade_manage_data_temp.last_day_addr < TRADE_DATA_START_ADDR)
					|| (trade_manage_data_temp.last_day_addr >= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE)))
				{
					//	当前数据不正确
					SetMotherBoardSt(MOTHERBOARD_MEMERY_DATA_ERROR);
					continue;
				}
				
				//	上传交易数据
				if (upload_manage_temp.out == trade_manage_data_temp.out)		//	是否当天的数据
				{
					if (trade_manage_data_temp.out != trade_manage_data_temp.in)
					{
						upload_manage_temp = trade_manage_data_temp;		//	更新上传数据
						//	有数据没有上传
						if ((upload_manage_temp.out + sizeof(_trade_data_to_server_s)) >= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
						{
							//	下一条数据已经超出界限
							//	读存在尾部(第一部分) 的数据
							ReadExternMemery(&upload_data_temp, upload_manage_temp.out, 
							(TRADE_DATA_START_ADDR + TRADE_DATA_SIZE) - upload_manage_temp.out);
							//	读存在头部(第二部分) 的数据
							ReadExternMemery((void *)((uint32)&upload_data_temp + ((TRADE_DATA_START_ADDR + TRADE_DATA_SIZE) - upload_manage_temp.out)), 
								TRADE_DATA_START_ADDR+sizeof(current_trade_index), sizeof(_trade_data_to_server_s) - ((TRADE_DATA_START_ADDR + TRADE_DATA_SIZE) - upload_manage_temp.out));
							
						}
						else
						{
							ReadExternMemery(&upload_data_temp, upload_manage_temp.out, sizeof(_trade_data_to_server_s));
						}
						//ReadExternMemery(&upload_data_temp, upload_manage_temp.out, sizeof(_trade_data_to_server_s));
						GetNextPackage();
						if (ServerUploadTradeData(&upload_data_temp) == SERVER_DATA_NO_ERR)
						{
							//upload_manage_temp.out += sizeof(_trade_data_to_server_s);
							//	计算下一条数据的起始地址
							if ((upload_manage_temp.out + sizeof(_trade_data_to_server_s)) >= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
							{
								//	下一条数据已经超出界限
								upload_manage_temp.out = upload_manage_temp.out + sizeof(_trade_data_to_server_s) - TRADE_DATA_SIZE + sizeof(current_trade_index);
							}
							else
							{
								//	下一条数据没有超出界限
								upload_manage_temp.out += sizeof(_trade_data_to_server_s);
							}
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
				else if (upload_manage_temp.out == upload_manage_temp.in)
				{
					//	刚启动或者一天的数据发送完成, 搜索下一天的数据
					if (trade_manage_data_temp.last_day_addr == TRADE_DATA_START_ADDR)
					{
						//	当天是第一天，直接读取当天的数据
						upload_manage_temp = trade_manage_data_temp;
					}
					else
					{
						//	以前有用过该设备，读取昨天的数据
						upload_index = trade_manage_data_temp.last_day_addr;
						ReadExternMemery(&upload_manage_temp, upload_index, sizeof(_trade_manage_data_s));
						//	搜索没有发送的最前面的数据
						while (upload_manage_temp.in != upload_manage_temp.out)
						{
							if (upload_manage_temp.last_day_addr == TRADE_DATA_START_ADDR)
							{
								//	当前是最早的交易数据
								break;
							}
							
							if ((trade_manage_data_temp.in > upload_manage_temp.last_day_addr) && (trade_manage_data_temp.in < upload_index))
							{
								//	上一日的日志已被覆盖
								break;
							}
							
							upload_index = upload_manage_temp.last_day_addr;		//	修改索引，读取下一天数据
							ReadExternMemery(&upload_manage_temp, upload_index, sizeof(_trade_manage_data_s));
						}
						if (upload_manage_temp.in == upload_manage_temp.out)
						{
							if ((upload_manage_temp.in + sizeof(_trade_manage_data_s)) > (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
							{
								//	下一数据在开头处
								upload_index = TRADE_DATA_START_ADDR + sizeof(current_trade_index);
							}
							else
							{
								upload_index = upload_manage_temp.in;		//	修改索引，读取下一天数据
							}
							ReadExternMemery(&upload_manage_temp, upload_index, sizeof(_trade_manage_data_s));
						}
					}
				}
				else 
				{
					//	上传以前的数据
					if (upload_manage_temp.out > (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
					{
						SetMotherBoardSt(MOTHERBOARD_MEMERY_DATA_ERROR);		//	存储数据异常
						continue;
					}
					//	读取和上传数据
					if ((upload_manage_temp.out + sizeof(_trade_data_to_server_s)) >= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
					{
						//	下一条数据已经超出界限
						//	读存在尾部(第一部分) 的数据
						ReadExternMemery(&upload_data_temp, upload_manage_temp.out, 
						(TRADE_DATA_START_ADDR + TRADE_DATA_SIZE) - upload_manage_temp.out);
						//	读存在头部(第二部分) 的数据
						ReadExternMemery((void *)((uint32)&upload_data_temp + ((TRADE_DATA_START_ADDR + TRADE_DATA_SIZE) - upload_manage_temp.out)), 
							TRADE_DATA_START_ADDR+sizeof(current_trade_index), sizeof(_trade_data_to_server_s) - ((TRADE_DATA_START_ADDR + TRADE_DATA_SIZE) - upload_manage_temp.out));
						
					}
					else
					{
						ReadExternMemery(&upload_data_temp, upload_manage_temp.out, sizeof(_trade_data_to_server_s));
					}
					GetNextPackage();
					if (ServerUploadTradeData(&upload_data_temp) == SERVER_DATA_NO_ERR)
					{
						//	更新数据管理数据
						//	计算下一条数据的起始地址
						if ((upload_manage_temp.out + sizeof(_trade_data_to_server_s)) >= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
						{
							//	下一条数据已经超出界限
							upload_manage_temp.out = upload_manage_temp.out + sizeof(_trade_data_to_server_s) - TRADE_DATA_SIZE + sizeof(current_trade_index);
						}
						else
						{
							//	下一条数据没有超出界限
							upload_manage_temp.out += sizeof(_trade_data_to_server_s);
						}
						if ((upload_index >= (TRADE_DATA_START_ADDR + sizeof(current_trade_index)))
						        && ((upload_index + sizeof(_trade_manage_data_s)) < (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE)))
						{
							WriteExternMemery(&upload_manage_temp, upload_index, sizeof(_trade_manage_data_s));
							if ((upload_manage_temp.out == upload_manage_temp.in) && (upload_index != current_trade_index))
							{
								//	读下一天的数据
								if ((upload_manage_temp.in + sizeof(_trade_manage_data_s)) > (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
								{
									//	下一数据在开头处
									upload_index = TRADE_DATA_START_ADDR + sizeof(current_trade_index);
								}
								else
								{
									upload_index = upload_manage_temp.in;		//	修改索引，读取下一天数据
								}
								ReadExternMemery(&upload_manage_temp, upload_index, sizeof(_trade_manage_data_s));
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
			OSTimeDly(OS_TICKS_PER_SEC * 5);
			//OSTaskSuspend(OS_PRIO_SELF);
		}
		else
		{
			OSTimeDly(OS_TICKS_PER_SEC * 10);

		}
		//err = ServerCashBoxBalance((_log_deposit_cmd_s *)log_data_temp);

	}
}

uint8 UploadAllMemery(uint32 start, uint32 end)
{
	//uint8 data_temp[32];
	while (1)
	{
		//ServerLogin();
	}
}

