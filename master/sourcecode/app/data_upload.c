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
					//	��ǰ���ݲ���ȷ
					SetMotherBoardSt(MOTHERBOARD_MEMERY_DATA_ERROR);
					continue;
				}
				
				//	�ϴ���������
				if (upload_manage_temp.out == trade_manage_data_temp.out)		//	�Ƿ��������
				{
					if (trade_manage_data_temp.out != trade_manage_data_temp.in)
					{
						upload_manage_temp = trade_manage_data_temp;		//	�����ϴ�����
						//	������û���ϴ�
						if ((upload_manage_temp.out + sizeof(_trade_data_to_server_s)) >= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
						{
							//	��һ�������Ѿ���������
							//	������β��(��һ����) ������
							ReadExternMemery(&upload_data_temp, upload_manage_temp.out, 
							(TRADE_DATA_START_ADDR + TRADE_DATA_SIZE) - upload_manage_temp.out);
							//	������ͷ��(�ڶ�����) ������
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
							//	������һ�����ݵ���ʼ��ַ
							if ((upload_manage_temp.out + sizeof(_trade_data_to_server_s)) >= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
							{
								//	��һ�������Ѿ���������
								upload_manage_temp.out = upload_manage_temp.out + sizeof(_trade_data_to_server_s) - TRADE_DATA_SIZE + sizeof(current_trade_index);
							}
							else
							{
								//	��һ������û�г�������
								upload_manage_temp.out += sizeof(_trade_data_to_server_s);
							}
							trade_manage_data_temp.out = upload_manage_temp.out;
						}
					}
					if (trade_manage_data_temp.out == trade_manage_data_temp.in)
					{
						//	���ݷ������
						SetTradeUploadState(0);
						SetSaveConfig(EXE_WRITED);
					}
				}
				else if (upload_manage_temp.out == upload_manage_temp.in)
				{
					//	����������һ������ݷ������, ������һ�������
					if (trade_manage_data_temp.last_day_addr == TRADE_DATA_START_ADDR)
					{
						//	�����ǵ�һ�죬ֱ�Ӷ�ȡ���������
						upload_manage_temp = trade_manage_data_temp;
					}
					else
					{
						//	��ǰ���ù����豸����ȡ���������
						upload_index = trade_manage_data_temp.last_day_addr;
						ReadExternMemery(&upload_manage_temp, upload_index, sizeof(_trade_manage_data_s));
						//	����û�з��͵���ǰ�������
						while (upload_manage_temp.in != upload_manage_temp.out)
						{
							if (upload_manage_temp.last_day_addr == TRADE_DATA_START_ADDR)
							{
								//	��ǰ������Ľ�������
								break;
							}
							
							if ((trade_manage_data_temp.in > upload_manage_temp.last_day_addr) && (trade_manage_data_temp.in < upload_index))
							{
								//	��һ�յ���־�ѱ�����
								break;
							}
							
							upload_index = upload_manage_temp.last_day_addr;		//	�޸���������ȡ��һ������
							ReadExternMemery(&upload_manage_temp, upload_index, sizeof(_trade_manage_data_s));
						}
						if (upload_manage_temp.in == upload_manage_temp.out)
						{
							if ((upload_manage_temp.in + sizeof(_trade_manage_data_s)) > (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
							{
								//	��һ�����ڿ�ͷ��
								upload_index = TRADE_DATA_START_ADDR + sizeof(current_trade_index);
							}
							else
							{
								upload_index = upload_manage_temp.in;		//	�޸���������ȡ��һ������
							}
							ReadExternMemery(&upload_manage_temp, upload_index, sizeof(_trade_manage_data_s));
						}
					}
				}
				else 
				{
					//	�ϴ���ǰ������
					if (upload_manage_temp.out > (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
					{
						SetMotherBoardSt(MOTHERBOARD_MEMERY_DATA_ERROR);		//	�洢�����쳣
						continue;
					}
					//	��ȡ���ϴ�����
					if ((upload_manage_temp.out + sizeof(_trade_data_to_server_s)) >= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
					{
						//	��һ�������Ѿ���������
						//	������β��(��һ����) ������
						ReadExternMemery(&upload_data_temp, upload_manage_temp.out, 
						(TRADE_DATA_START_ADDR + TRADE_DATA_SIZE) - upload_manage_temp.out);
						//	������ͷ��(�ڶ�����) ������
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
						//	�������ݹ�������
						//	������һ�����ݵ���ʼ��ַ
						if ((upload_manage_temp.out + sizeof(_trade_data_to_server_s)) >= (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
						{
							//	��һ�������Ѿ���������
							upload_manage_temp.out = upload_manage_temp.out + sizeof(_trade_data_to_server_s) - TRADE_DATA_SIZE + sizeof(current_trade_index);
						}
						else
						{
							//	��һ������û�г�������
							upload_manage_temp.out += sizeof(_trade_data_to_server_s);
						}
						if ((upload_index >= (TRADE_DATA_START_ADDR + sizeof(current_trade_index)))
						        && ((upload_index + sizeof(_trade_manage_data_s)) < (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE)))
						{
							WriteExternMemery(&upload_manage_temp, upload_index, sizeof(_trade_manage_data_s));
							if ((upload_manage_temp.out == upload_manage_temp.in) && (upload_index != current_trade_index))
							{
								//	����һ�������
								if ((upload_manage_temp.in + sizeof(_trade_manage_data_s)) > (TRADE_DATA_START_ADDR + TRADE_DATA_SIZE))
								{
									//	��һ�����ڿ�ͷ��
									upload_index = TRADE_DATA_START_ADDR + sizeof(current_trade_index);
								}
								else
								{
									upload_index = upload_manage_temp.in;		//	�޸���������ȡ��һ������
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
					//	�ѷ�������
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

