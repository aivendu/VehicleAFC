#include "includes.h"


void TaskDataUpload(void *pdata)
{
	uint8 err;

	//uint32 upload_index;
	_trade_manage_data_s upload_manage_temp;
	_trade_data_to_server_s upload_data_temp;
		
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
				if (upload_manage_temp.out == trade_manage_data_temp.out)		//	�Ƿ��������
				{
					ReadExternMemery(&upload_data_temp,upload_manage_temp.out,sizeof(_trade_data_to_server_s));
					GetNextPackage();
					if (ServerUploadTradeData(&upload_data_temp) == GPRS_DATA_NO_ERR)
					{
						upload_manage_temp.out += sizeof(_trade_data_to_server_s);
						trade_manage_data_temp.out = upload_manage_temp.out;
						if (trade_manage_data_temp.out == trade_manage_data_temp.in)
						{
							//	���ݷ������
							SetTradeUploadState(0);
							SetSaveConfig(EXE_WRITED);
						}
					}
				}
				else
				{
					//	�ϴ���ǰ������
					ReadExternMemery(&upload_manage_temp,current_trade_index,sizeof(_trade_manage_data_s));
					if (upload_manage_temp.last_day_addr == TRADE_DATA_START_ADDR)
					{
						//	��ǰ�����ǵ�һ�콻�ף�����Ҫ������ǰ������
						continue;
					}
					while (upload_manage_temp.in != upload_manage_temp.out)
					{
						if (trade_manage_data_temp.in > current_trade_index)		//	�жϵ������־�洢�����Ƿ񾭹�Խ���
						{
							//	û�о���Խ���
							if ((upload_manage_temp.last_day_addr < trade_manage_data_temp.in) && (upload_manage_temp.last_day_addr > current_trade_index))
							{
								//	��һ�յ���־�ѱ�����
								break;
							}
						}
						else
						{
							//	����Խ���
							if ((upload_manage_temp.last_day_addr < trade_manage_data_temp.in) || (upload_manage_temp.last_day_addr > current_trade_index))
							{
								//	��һ�յ���־�ѱ�����
								break;
							}
						}
						//upload_index = upload_manage_temp.last_day_addr;
						ReadExternMemery(&upload_manage_temp,upload_manage_temp.last_day_addr,sizeof(_trade_manage_data_s));
					}
					if (upload_manage_temp.in == upload_manage_temp.out)
					{
						ReadExternMemery(&upload_manage_temp,upload_manage_temp.in,sizeof(_trade_manage_data_s));
					}
					
					//	��ȡ���ϴ�����
					ReadExternMemery(&upload_data_temp,upload_manage_temp.out,sizeof(_trade_data_to_server_s));
					GetNextPackage();
					if (ServerUploadTradeData(&upload_data_temp) == GPRS_DATA_NO_ERR)
					{
						//	�������ݹ�������
						upload_manage_temp.out += sizeof(_trade_data_to_server_s);
						WriteExternMemery(&upload_manage_temp,upload_manage_temp.in,sizeof(_trade_manage_data_s));
						if (upload_manage_temp.out == upload_manage_temp.in)
						{
							//	����һ�������
							ReadExternMemery(&upload_manage_temp,upload_manage_temp.in,sizeof(_trade_manage_data_s));
						}
					}
				}
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



