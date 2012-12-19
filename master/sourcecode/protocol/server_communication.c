#include "includes.h"


#define ServerSendByte(a)			Uart0SendByte(a,0)
#define ServerSendString(a)		Uart0SendString(a,0)
#define ServerReceiveByte(a)		Uart0RecByte(a,0,0)

#define SERVER_HEAD				0		//	SERVER ����ͷ
#define	SERVER_ADDR				1		//	SERVER ��ַ����
#define	SERVER_PACKAGE			2		//	SERVER ���Ž���
#define	SERVER_LENGHT				3		//	SERVER ���Ƚ���
#define	SERVER_COMMAND			4		//	SERVER �������
#define	SERVER_ARGU				5		//	SERVER ��������
#define	SERVER_DATA				6		//	SERVER ���ݽ���
#define	SERVER_CHECK				7		//	SERVER У���ֽ���
#define	SERVER_END				8		//	SERVER β����
#define SERVER_HANDLE				9		//	SERVER ���ݴ���


static OS_EVENT *server_communication_sem, *server_return_sem;
_server_communication_s server_rec_buffer, server_send_buffer;
static uint8 server_package;

#define GetServerPackage()			server_package
#define ClearServerPackage()		(server_package = 0)

uint8 GetNextPackage(void)
{
	if ((++server_package) > 99)
		server_package = 0;
	return server_package;
}


void ServerCommInit(void)
{
	server_communication_sem = OSMboxCreate(NULL);
	while(server_communication_sem == NULL);		//	��ʼ��ʧ��ֹͣ��

	server_return_sem = OSSemCreate(0);
	while (server_return_sem == NULL);
}

//	������server ͨ��ͨ��
void RequestServerCommunication(void)
{
	uint8 err;
	OSSemPend(server_communication_sem, 0, &err);
}

//	�ͷ���server ͨ��ͨ��
void FreeServerCommunication(void)
{
	OSSemPost(server_communication_sem);
}


uint8 ServerSendFrameToServer(_server_communication_s *command)
{
	char temp[6];
	uint8 i = 0;
	command->check = 0;
	if (	(command->package_no < 100)		// У������Ƿ���ȷ
	        ||	(command->data_lenght < 100)
	   )
	{

	}
	else
	{
		return FALSE;
	}
	ServerSendByte(SERVER_COMMUNICATION_HEAD);	//	����ͷ
	memset(temp, 0, sizeof(temp));
	memcpy(temp, GetDeviceAddr(), 4);					//	ȡ�豸��ַ
	ServerSendString(temp);							//	�����豸��ַ
	command->check = CrcString(command->check, temp);	//	У���豸��ַ

	memset(temp, 0, sizeof(temp));
	sprintf(temp, "%02d", command->package_no);
	ServerSendString(temp);							//	���Ͱ���
	command->check = CrcString(command->check, temp);	//	У��

	memset(temp, 0, sizeof(temp));
	sprintf(temp, "%02d", command->data_lenght);
	ServerSendString(temp);
	command->check = CrcString(command->check, temp);

	ServerSendByte(command->command[0]);
	command->check = CRCByte(command->check, command->command[0]);
	command->data_lenght--;
	ServerSendByte(command->command[1]);
	command->check = CRCByte(command->check, command->command[1]);
	command->data_lenght--;
	ServerSendByte(command->argument[0]);
	command->check = CRCByte(command->check, command->argument[0]);
	command->data_lenght--;
	ServerSendByte(command->argument[1]);
	command->check = CRCByte(command->check, command->argument[1]);
	command->data_lenght--;

	for (i = 0; i < command->data_lenght; i++)
	{
		if (command->data[i] == 0)
		{
			ServerSendByte(0x20);
			command->check = CRCByte(command->check, 0x20);
		}
		else
		{
			ServerSendByte((uint8)command->data[i]);
			command->check = CRCByte(command->check, (uint8)command->data[i]);
		}
	}
	//ServerSendByte((uint8)(command->check >> 8));		//	����У��
	//ServerSendByte((uint8)command->check);
	memset(temp, 0, sizeof(temp));
	sprintf(temp, "%04X", command->check);
	ServerSendString(temp);

	ServerSendByte(SERVER_COMMUNICATION_END);			//	����β
	return TRUE;
}


//	��������
uint8 ServerOnLine(void)
{
	uint8 err;
	RequestServerCommunication();
	server_send_buffer.package_no = server_package;
	memcpy(server_send_buffer.command, "00", 2);
	memcpy(server_send_buffer.argument, "00", 2);
	server_send_buffer.data_lenght = 4;
	ServerSendFrameToServer(&server_send_buffer);

	OSSemPend(server_return_sem, SERVER_MAX_TIME_DELAY * 2, &err);
	if (err == SERVER_DATA_NO_ERR)
	{
		if ((memcmp(server_rec_buffer.command, "00", 2) == 0)
		        && (memcmp(server_rec_buffer.data, "OK", 2) == 0))
		{
			err = SERVER_DATA_NO_ERR;
		}
		else
		{
			err = SERVER_DATA_RETURN_ERR;
		}
	}
	FreeServerCommunication();
	return err;
}

//	�ϴ���������
uint8 ServerUploadTradeData(_trade_data_to_server_s *data)
{
	uint8 err;

	RequestServerCommunication();
	server_send_buffer.package_no = server_package;
	memcpy(server_send_buffer.command, "TD", 2);
	memcpy(server_send_buffer.argument, "00", 2);
	sprintf(server_send_buffer.data, "%04d%02d%02d%02d%02d%02d%05d%05d%05d%02d%02d%02d%01d%02d",
	        data->year,
	        data->month,
	        data->day,
	        data->hour,
	        data->minute,
	        data->second,
	        data->serial_number,
	        data->needpay,
	        data->realpay,
	        data->change_cashbox_1,
	        data->change_cashbox_2,
	        data->change_cashbox_3,
	        data->destination_num,
	        data->current_station);
	for (err = 0; ((err < data->destination_num) && (err < 6)); err++)
	{
		sprintf(&server_send_buffer.data[38 + (err * 7)], "%02d%03d%02d",
		        data->destination[err].destination_station,
		        data->destination[err].price,
		        data->destination[err].people_num);
	}
	sprintf(server_send_buffer.data,"%s%05d%05d%05d",server_send_buffer.data,
		data->cashbox_1_balance,data->cashbox_2_balance,data->cashbox_3_balance);
	
	server_send_buffer.data_lenght = 4 + strlen(server_send_buffer.data);
	ServerSendFrameToServer(&server_send_buffer);

	OSSemPend(server_return_sem, SERVER_MAX_TIME_DELAY * 2, &err);
	if (err == SERVER_DATA_NO_ERR)
	{
		if ((memcmp(server_rec_buffer.command, "TD", 2) == 0)
		        && (memcmp(server_rec_buffer.data, "OK", 2) == 0))
		{
			err = SERVER_DATA_NO_ERR;
		}
		else
		{
			err = SERVER_DATA_RETURN_ERR;
		}
	}
	FreeServerCommunication();
	return err;
}

//	ʱ��ͬ��
uint8 ServerTimeSync(void)
{
	uint8 err;

	RequestServerCommunication();
	server_send_buffer.package_no = server_package;
	memcpy(server_send_buffer.command, "TS", 2);
	memcpy(server_send_buffer.argument, "00", 2);
	server_send_buffer.data_lenght = 4;
	ServerSendFrameToServer(&server_send_buffer);

	OSSemPend(server_return_sem, SERVER_MAX_TIME_DELAY * 2, &err);
	if (err == SERVER_DATA_NO_ERR)
	{
		if ((memcmp(server_rec_buffer.command, "TS", 2) == 0)
		        && (memcmp(server_rec_buffer.data, "OK", 2) == 0))
		{
			err = SERVER_DATA_NO_ERR;
		}
		else
		{
			err = SERVER_DATA_RETURN_ERR;
		}
	}
	FreeServerCommunication();
	return err;
}

//	Ǯ����
uint8 ServerCashBoxBalance(_log_deposit_cmd_s *data)
{
	uint8 err;

	RequestServerCommunication();
	server_send_buffer.package_no = server_package;
	memcpy(server_send_buffer.command, "MB", 2);
	memcpy(server_send_buffer.argument, "00", 2);
	sprintf(server_send_buffer.data, "%08d%04d%02d%02d%02d%02d%02d%05d%05d%05d", 0, data->year, data->month,
		data->day, data->hour, data->min, data->sec, data->cashbox1_amount, data->cashbox2_amount, data->cashbox3_amount);
	memcpy(server_send_buffer.data,data->staffid,8);
	server_send_buffer.data_lenght = 4 + 37;
	ServerSendFrameToServer(&server_send_buffer);

	OSSemPend(server_return_sem, SERVER_MAX_TIME_DELAY * 2, &err);
	if (err == SERVER_DATA_NO_ERR)
	{
		if ((memcmp(server_rec_buffer.command, "MB", 2) == 0)
		        && (memcmp(server_rec_buffer.data, "OK", 2) == 0))
		{
			err = SERVER_DATA_NO_ERR;
		}
		else
		{
			err = SERVER_DATA_RETURN_ERR;
		}
	}
	FreeServerCommunication();
	return err;
}

uint8 ServerGPSData(uint8 flag, uint32 latitude, uint32 longitude, uint32 speed)
{
	uint8 err;

	RequestServerCommunication();
	server_send_buffer.package_no = server_package;
	memcpy(server_send_buffer.command, "GD", 2);
	if (flag)
	{
		memcpy(server_send_buffer.argument, "01", 2);
		sprintf(server_send_buffer.data, "%010d%010d%03d.%02d", longitude, latitude, speed / 1000, (speed % 1000) / 10);
		server_send_buffer.data_lenght = 4 + 26;
	}
	else
	{
		memcpy(server_send_buffer.argument, "00", 2);
		server_send_buffer.data_lenght = 4;
	}
	ServerSendFrameToServer(&server_send_buffer);

	OSSemPend(server_return_sem, SERVER_MAX_TIME_DELAY * 2, &err);
	if (err == SERVER_DATA_NO_ERR)
	{
		if ((memcmp(server_rec_buffer.command, "GD", 2) == 0)
		        && (memcmp(server_rec_buffer.data, "OK", 2) == 0))
		{
			err = SERVER_DATA_NO_ERR;
		}
		else
		{
			err = SERVER_DATA_RETURN_ERR;
		}
	}
	FreeServerCommunication();
	return err;
}

uint8 ServerLogin(void *data)
{
	uint8 err;

	RequestServerCommunication();
	server_send_buffer.package_no = server_package;
	memcpy(server_send_buffer.command, "LI", 2);
	memcpy(server_send_buffer.argument, "00", 2);
	memset(server_send_buffer.data, 0, 9);
	memcpy(server_send_buffer.data, ((_log_device_use_cmd_s *)data)->staffid, 8);
	sprintf(server_send_buffer.data, "%s%04d%02d%02d%02d%02d%02d",
	        ((_log_device_use_cmd_s *)data)->staffid, ((_log_device_use_cmd_s *)data)->year, ((_log_device_use_cmd_s *)data)->month,
	        ((_log_device_use_cmd_s *)data)->day, ((_log_device_use_cmd_s *)data)->hour, ((_log_device_use_cmd_s *)data)->min, ((_log_device_use_cmd_s *)data)->sec);
	server_send_buffer.data_lenght = 4 + 8;
	ServerSendFrameToServer(&server_send_buffer);

	OSSemPend(server_return_sem, SERVER_MAX_TIME_DELAY * 2, &err);
	if (err == SERVER_DATA_NO_ERR)
	{
		if ((memcmp(server_rec_buffer.command, "LI", 2) == 0)
		        && (memcmp(server_rec_buffer.data, "OK", 2) == 0)
		   )
		{
			err = SERVER_DATA_NO_ERR;
		}
		else
		{
			err = SERVER_DATA_RETURN_ERR;
		}
	}
	FreeServerCommunication();
	return err;
}

uint8 ServerLogout(void *data)
{
	uint8 err;

	RequestServerCommunication();
	server_send_buffer.package_no = server_package;
	memcpy(server_send_buffer.command, "LO", 2);
	memcpy(server_send_buffer.argument, "00", 2);
	memset(server_send_buffer.data, 0, 9);
	memcpy(server_send_buffer.data, ((_log_device_use_cmd_s *)data)->staffid, 8);
	sprintf(server_send_buffer.data, "%s%04d%02d%02d%02d%02d%02d",
	        ((_log_device_use_cmd_s *)data)->staffid, ((_log_device_use_cmd_s *)data)->year, ((_log_device_use_cmd_s *)data)->month,
	        ((_log_device_use_cmd_s *)data)->day, ((_log_device_use_cmd_s *)data)->hour, ((_log_device_use_cmd_s *)data)->min, ((_log_device_use_cmd_s *)data)->sec);
	server_send_buffer.data_lenght = 4 + 8;
	ServerSendFrameToServer(&server_send_buffer);

	OSSemPend(server_return_sem, SERVER_MAX_TIME_DELAY * 2, &err);
	if (err == SERVER_DATA_NO_ERR)
	{
		if ((memcmp(server_rec_buffer.command, "LO", 2) == 0)
		        && (memcmp(server_rec_buffer.data, "OK", 2) == 0))
		{
			err =  SERVER_DATA_NO_ERR;
		}
		else
		{
			err = SERVER_DATA_RETURN_ERR;
		}
	}
	FreeServerCommunication();
	return err;
}



uint16 GetVersion(void *arg)
{
	if (strlen(PAD_version) == 0)
	{
		sprintf(arg, "%s;0;0", VERSION);
	}
	else
	{
		sprintf(arg, "%s;%0s;0", VERSION, PAD_version);
	}
	return strlen(arg);
}

uint16 GetUartConfig(void *arg)
{
	sprintf(arg, "%d%06d;%d%06d;%d%06d;%d%06d;%d%06d;%d%06d;%d%06d;%d%06d;%d%06d;%d%06d;",
	        GetRj45UartChannal(), GetRj45UartBps(),
	        GetGprsUartChannal(), GetGprsUartBps(),
	        GetGpsUartChannal(), GetGpsUartBps(),
	        GetBillUartChannal(), GetBillUartBps(),
	        GetCoinUartChannal(), GetCoinUartBps(),
	        GetPrintUartChannal(), GetPrintUartBps(),
	        GetVoiceUartChannal(), GetVoiceUartBps(),
	        GetICMachineUartChannal(), GetICMachineUartBps(),
	        0x00, 0x00,
	        0x00, 0x00
	       );
	return 80;
}

uint16 UartConfig(void *arg)
{
	SetRj45UartChannal(((_uart_config_from_server_s *)arg)[0].channal_num - '0');
	((_uart_config_from_server_s *)arg)[0].separator = 0x00;
	SetRj45UartBps(atoi(((_uart_config_from_server_s *)arg)[0].bps));

	SetGprsUartChannal(((_uart_config_from_server_s *)arg)[1].channal_num - '0');
	((_uart_config_from_server_s *)arg)[1].separator = 0x00;
	SetGprsUartBps(atoi(((_uart_config_from_server_s *)arg)[1].bps));

	SetGpsUartChannal(((_uart_config_from_server_s *)arg)[2].channal_num - '0');
	((_uart_config_from_server_s *)arg)[2].separator = 0x00;
	SetGpsUartBps(atoi(((_uart_config_from_server_s *)arg)[2].bps));

	SetBillUartChannal(((_uart_config_from_server_s *)arg)[3].channal_num - '0');
	((_uart_config_from_server_s *)arg)[3].separator = 0x00;
	SetBillUartBps(atoi(((_uart_config_from_server_s *)arg)[3].bps));

	SetCoinUartChannal(((_uart_config_from_server_s *)arg)[4].channal_num - '0');
	((_uart_config_from_server_s *)arg)[4].separator = 0x00;
	SetCoinUartBps(atoi(((_uart_config_from_server_s *)arg)[4].bps));

	SetPrintUartChannal(((_uart_config_from_server_s *)arg)[5].channal_num - '0');
	((_uart_config_from_server_s *)arg)[5].separator = 0x00;
	SetPrintUartBps(atoi(((_uart_config_from_server_s *)arg)[5].bps));

	SetICMachineUartChannal(((_uart_config_from_server_s *)arg)[7].channal_num - '0');
	((_uart_config_from_server_s *)arg)[6].separator = 0x00;
	SetICMachineUartBps(atoi(((_uart_config_from_server_s *)arg)[7].bps));
	return TRUE;
}


uint16 GetFuncConfig(void *arg)
{
	sprintf(arg, "%d%031d", GetLoginMod(), 0x00);
	return 32;
}

uint16 FuncConfig(void *arg)
{
	SetLoginMod(((uint8 *)arg)[0] - '0');
	return TRUE;
}

uint16 GetCashboxConfig(void *arg)
{
	sprintf(arg, "%03d%03d%03d%03d%03d%03d%05d%05d%05d%05d%05d%05d",
	        GetCashbox1Value(),
	        GetCashbox2Value(),
	        GetCashbox3Value(),
	        GetCashbox1AlarmThreshold(),
	        GetCashbox2AlarmThreshold(),
	        GetCashbox3AlarmThreshold(),
	        GetCashbox1Balance(),
	        GetCashbox2Balance(),
	        GetCashbox3Balance(),
	        GetCashbox1Deposit(),
	        GetCashbox2Deposit(),
	        GetCashbox3Deposit()
	       );
	return 48;
}

uint16 CashboxConfig(void *arg)
{
	uint8 err;
	uint32 temp_32;
	_cashbox_config_from_server_s *temp = (_cashbox_config_from_server_s *)arg;

	temp_32 = stoi(10, 3, temp->cashbox1_par_value, &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox1Value((uint8)temp_32);

	temp_32 = stoi(10, 3, temp->cashbox2_par_value, &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox2Value((uint8)temp_32);

	temp_32 = stoi(10, 3, temp->cashbox3_par_value, &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox3Value((uint8)temp_32);

	temp_32 = stoi(10, 3, temp->cashbox1_threshold, &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox1AlarmThreshold((uint8)temp_32);

	temp_32 = stoi(10, 3, temp->cashbox2_threshold, &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox2AlarmThreshold((uint8)temp_32);

	temp_32 = stoi(10, 3, temp->cashbox3_threshold, &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox3AlarmThreshold((uint8)temp_32);

	temp_32 = stoi(10, 5, temp->cashbox1_balance, &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox1Balance((uint16)temp_32);

	temp_32 = stoi(10, 5, temp->cashbox2_balance, &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox2Balance((uint16)temp_32);

	temp_32 = stoi(10, 5, temp->cashbox3_balance, &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox3Balance((uint16)temp_32);

	temp_32 = stoi(10, 5, temp->cashbox1_deposit, &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox1Deposit((uint16)temp_32);

	temp_32 = stoi(10, 5, temp->cashbox2_deposit, &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox2Deposit((uint16)temp_32);

	temp_32 = stoi(10, 5, temp->cashbox3_deposit, &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox3Deposit((uint16)temp_32);
	return TRUE;
}

uint16 GetSysPerformanceConfig(void *arg)
{
	sprintf(arg, "%04d%05d%05d%05d%05d",
	        0x00,
	        GetServerAnswerResponseTime(),
	        GetGpsSamplingTime(),
	        GetServerOfflineResponseTime(),
	        GetLoginRemainTime()
	       );
	memcpy(arg, GetDeviceAddr(), 4);
	return 24;
}


uint16 SysPerformanceConfig(void *arg)
{
	uint8 err;
	_sys_performance_config_from_server_s *temp;
	temp = (_sys_performance_config_from_server_s *)arg;
	if (SetDeviceAddr(temp->device_addr) == FALSE)
	{
		return FALSE;
	}
	SetServerAnswerResponseTime((uint16)stoi(10, 5, temp->server_response_time, &err));
	if (err == FALSE)
	{
		return FALSE;
	}
	SetGpsSamplingTime((uint16)stoi(10, 5, temp->gps_sampling_time, &err));
	if (err == FALSE)
	{
		return FALSE;
	}
	SetServerOfflineResponseTime((uint16)stoi(10, 5, temp->server_off_line_delay_time, &err));
	if (err == FALSE)
	{
		return FALSE;
	}
	SetLoginRemainTime((uint16)stoi(10, 5, temp->user_logout_delay_time, &err));
	if (err == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

uint16 GetServerCommConfig(void *arg)
{
	sprintf(arg, "%03d%03d%03d%03d%05d",
	        GetServerIp(0),
	        GetServerIp(1),
	        GetServerIp(2),
	        GetServerIp(3),
	        GetServerPort()
	       );
	return 17;
}

uint16 ServerCommConfig(void *arg)
{
	uint8 err;
	uint8 ip_temp[4];
	uint16 port;
	_server_communication_config_from_server *temp = (_server_communication_config_from_server *)arg;
	ip_temp[0] = (uint8)stoi(10, 3, temp->ip[0], &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	ip_temp[1] = (uint8)stoi(10, 3, temp->ip[1], &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	ip_temp[2] = (uint8)stoi(10, 3, temp->ip[2], &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	ip_temp[3] = (uint8)stoi(10, 3, temp->ip[3], &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	port = (uint16)stoi(10, 5, temp->port, &err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetServerIp(ip_temp[0], ip_temp[1], ip_temp[2], ip_temp[3]);
	SetServerPort(port);
	return TRUE;
}

uint16 GetPrintConfig(void *arg)
{
	memcpy(arg, GetPrintCustomer(), 16);

	return 16;
}


uint16 PrintConfig(void *arg)
{
	SetPrintCustomer(arg);
	return TRUE;
}

uint16 GetDeviceStateToServer(void *arg)
{
	uint8 i;
	memset(arg, 0, 2);
	for (i = 0; i < sizeof(_sys_st_major_s); i++)
	{
		sprintf(arg, "%s%02X", arg, sys_state.ss.st_major.ssm_b[i]);		//	׼���豸��Ҫ״̬
	}
	for (i = 0; i < sizeof(_sys_st_other_s); i++)
	{
		sprintf(arg, "%s%02X", arg, sys_state.ss.st_other.sso_b[i]);		//	׼���豸����״̬
	}
	for (i = 0; i < sizeof(_sys_st_pad_s); i++)
	{
		sprintf(arg, "%s%02X", arg, sys_state.ss.st_pad.ssp_b[i]);		//	׼���豸����״̬
	}
	return ((sizeof(_sys_st_major_s) + sizeof(_sys_st_other_s) + sizeof(_sys_st_pad_s)) * 2);
}

uint16 LisencePlateNumConfig(void *arg)
{
	SetLisencePlateNum(arg);
	return TRUE;
}

uint16 GetLisencePlateNumConfig(void *arg)
{
	memset(arg, 0, 9);
	memcpy(arg, GetLisencePlateNum(), 8);
	return 8;
}

uint16 GetAllMemery(uint8 flag, uint32 start, uint32 end)
{
	//RequestUploadAllMemery();
	return TRUE;
}

typedef struct
{
	char command[3];
	_pfunc *funcset;
	_pfunc *funcget;
} _config_corresponding_s;			//	����ƥ���

//	���������������
const _config_corresponding_s config_corresponding_table[] =
{
	{"00", NULL, GetVersion},
	{"01", UartConfig, GetUartConfig},
	{"02", FuncConfig, GetFuncConfig},
	{"03", CashboxConfig, GetCashboxConfig},
	{"04", SysPerformanceConfig, GetSysPerformanceConfig},
	{"05", ServerCommConfig, GetServerCommConfig},
	{"06", PrintConfig, GetPrintConfig},
	{"07", LisencePlateNumConfig, GetLisencePlateNumConfig},
	{"08", NULL, GetDeviceStateToServer},
	{"FF", NULL, NULL}
};
uint8 ServerDownloadConfigData(void)
{
	uint8 i = 0;
	while (1)
	{
		if (memcmp(server_rec_buffer.argument, config_corresponding_table[i].command, 2) == 0)
		{
			if (config_corresponding_table[i].funcset(server_rec_buffer.data) == TRUE)
			{
				SetConfigState(2);			//	�ı�����״̬Ϊ�û�����
				SetCmdSaveConfig(EXE_WRITED);	//	��������
				return TRUE;
			}
			else
			{
				return FALSE;
			}

		}
		if (memcmp(config_corresponding_table[++i].command, "FF", 2) == 0)
		{
			return FALSE;
		}
	}
}

uint8 ServerGetConfigData(void *arg)
{
	uint8 i = 0;
	while (1)
	{
		if (memcmp(server_rec_buffer.argument, config_corresponding_table[i].command, 2) == 0)
		{
			return config_corresponding_table[i].funcget(arg);

		}
		if (memcmp(config_corresponding_table[++i].command, "FF", 2) == 0)
		{
			return FALSE;
		}
	}
}


// ����
uint8 ReturnWriteConfig(char *arg, uint8 package_no)
{
	//uint8 err;

	_server_communication_s server_communication_temp;
	memcpy(server_communication_temp.command, "WS", 2);
	memcpy(server_communication_temp.argument, arg, 2);
	server_communication_temp.data_lenght = 4 + 2;
	server_communication_temp.package_no = package_no;
	memcpy(server_communication_temp.data, "OK", 2);
	ServerSendFrameToServer(&server_communication_temp);
	return SERVER_DATA_NO_ERR;
}

//
uint8 ReturnReadConfig(char *arg, uint8 package_no)
{
	_server_communication_s server_communication_temp;
	memcpy(server_communication_temp.command, "RS", 2);
	memcpy(server_communication_temp.argument, arg, 2);
	server_communication_temp.data_lenght = ServerGetConfigData(server_communication_temp.data) + 4;
	server_communication_temp.package_no = package_no;
	ServerSendFrameToServer(&server_communication_temp);
	return SERVER_DATA_NO_ERR;
}

// ����
uint8 ReturnMS(uint8 flag, uint16 err_no, char *arg, uint8 package_no)
{
	//uint8 err;

	_server_communication_s server_communication_temp;
	memcpy(server_communication_temp.command, "MS", 2);
	memcpy(server_communication_temp.argument, arg, 2);
	server_communication_temp.package_no = package_no;
	switch (flag)
	{
	case 0:			//	�ظ�ERROR
		sprintf(server_communication_temp.data, "ER%04d", err_no);
		server_communication_temp.data_lenght = 4+strlen(server_communication_temp.data);
		break;

	case 1:			//	�ظ�OK
		memcpy(server_communication_temp.data, "OK", 2);
		server_communication_temp.data_lenght = 4+2;
		break;
	}
	ServerSendFrameToServer(&server_communication_temp);
	return SERVER_DATA_NO_ERR;
}


uint8 ServerReceiveFrameFromServer(void)
{
	uint8 state = SERVER_HEAD, err;
	char device_addr[4];
	char check_temp[4];
	uint8 data_temp, data_num = 0, data_amount = 0;
	uint16 check_sun, timer_num = 0;

	while (1)
	{
		if (ServerReceiveByte(&data_temp) == FALSE)
		{
			OSTimeDly(2);
			if ((++timer_num) > SERVER_MAX_TIME_DELAY)	return SERVER_TIME_OUT;		//	��ʱ�˳������ش���
			else										continue;			//	������һ�ν���
		}
		else
		{
			timer_num = 0;			//	�����ݣ�������ʱ
		}
		if ((++data_amount) > SERVER_FRAME_MAX_LENGHT)		//	��·���Ƿ��кܶ���������
		{
			return SERVER_UART_FAULT;
		}
		switch (state)
		{
		case SERVER_HEAD:
			if (data_temp == SERVER_COMMUNICATION_HEAD)
			{
				//	���յ�ͷ��׼�����ܵ�ַ����
				state = SERVER_ADDR;
				data_num = 0;
				check_sun = 0;
			}
			break;

		case SERVER_ADDR:
			if (((data_temp >= '0') && (data_temp <= '9')) || ((data_temp >= 'A') && (data_temp <= 'F'))
			        || ((data_temp >= 'a') && (data_temp <= 'f')))
			{
				//	��������ݵĵ�ַ
				device_addr[data_num] = data_temp;
				check_sun = CRCByte(check_sun, data_temp);
			}
			else
			{
				state = SERVER_HEAD;			//	���½���
				data_num = 0;
			}
			if ((++data_num) >= 4)
			{
				//	��ַ�������
				state = SERVER_PACKAGE;
				server_rec_buffer.package_no = 0;
				data_num = 0;
			}
			break;

		case SERVER_PACKAGE:
			if ((data_temp >= '0') && (data_temp <= '9'))
			{
				//	����ת��Ϊ����
				server_rec_buffer.package_no = server_rec_buffer.package_no * 10 + (data_temp - '0');
				check_sun = CRCByte(check_sun, data_temp);
			}
			else
			{
				state = SERVER_HEAD;			//	���½���
				data_num = 0;
			}
			if ((++data_num) >= 2)
			{
				//	���Ž������
				state = SERVER_LENGHT;			//	׼�����ճ���
				server_rec_buffer.data_lenght = 0;
				data_num = 0;
			}
			break;

		case SERVER_LENGHT:
			if ((data_temp >= '0') && (data_temp <= '9'))
			{
				//	���㳤�ȣ�ת��Ϊ����
				server_rec_buffer.data_lenght = server_rec_buffer.data_lenght * 10 + (data_temp - '0');
				check_sun = CRCByte(check_sun, data_temp);		//	����У��
			}
			else
			{
				state = SERVER_HEAD;			//	���½���
				data_num = 0;
			}
			if ((++data_num) >= 2)
			{
				//	���Ƚ������
				if ((server_rec_buffer.data_lenght > (86 + 2 + 2)) || (server_rec_buffer.data_lenght < 4))
				{
					//	���ȴ������½���
					state = SERVER_HEAD;			//	���½���
					data_num = 0;
				}
				else
				{
					state = SERVER_COMMAND;		//	׼����������
					data_num = 0;
				}
			}
			break;

		case SERVER_COMMAND:
			server_rec_buffer.command[data_num] = data_temp;
			check_sun = CRCByte(check_sun, data_temp);		//	����У��
			if ((++data_num) >= 2)
			{
				//	�����ֽ������
				state = SERVER_ARGU;		//	׼����������
				data_num = 0;
			}
			break;

		case SERVER_ARGU:
			server_rec_buffer.argument[data_num] = data_temp;
			check_sun = CRCByte(check_sun, data_temp);		//	����У��
			if ((++data_num) >= 2)
			{
				//	���������
				if (server_rec_buffer.data_lenght == 4)
				{
					//	��������
					server_rec_buffer.data[0] = 0;
					state = SERVER_CHECK;
					server_rec_buffer.check = 0;
					data_num = 0;
				}
				else
				{
					state = SERVER_DATA;		//	׼����������
					data_num = 0;
				}
			}
			break;

		case SERVER_DATA:
			server_rec_buffer.data[data_num] = data_temp;
			check_sun = CRCByte(check_sun, data_temp);		//	����У��
			if ((server_rec_buffer.data_lenght - 4) <= (++data_num))
			{
				//	�����������
				server_rec_buffer.data[data_num] = 0;
				state = SERVER_CHECK;
				data_num = 0;
				server_rec_buffer.check = 0;
			}
			break;

		case SERVER_CHECK:
			//server_rec_buffer.check = server_rec_buffer.check * 16 + data_temp - '0';
			check_temp[data_num] = data_temp;
			if ((++data_num) >= 4)
			{
				//	��������check ���ݣ�����У��
				//	ת��Ϊ����
				server_rec_buffer.check = (uint16)stoi(16, 4, check_temp, &err);
				if (server_rec_buffer.check == check_sun)
				{
					state = SERVER_END;
					data_num = 0;
				}
				else
				{
					return SERVER_CHECKE_ERR;
				}
			}
			break;

		case SERVER_END:
			if (data_temp == SERVER_COMMUNICATION_END)		//	��β�Ƿ���ȷ
			{
				state = SERVER_HANDLE;
			}
			else
			{
				state = SERVER_HEAD;
			}
			break;

		default:
			state = SERVER_HEAD;
			break;
		}
		if (state == SERVER_HANDLE)
		{
			if (memcmp(GetDeviceAddr(), device_addr, 4) != 0)
			{
				//	���ǵ�ǰ�豸��ַ�����ݣ��ظ������"99"
				//ReturnER("99",server_rec_buffer.package_no);
			}
			else if (memcmp(server_rec_buffer.command, "MS", 2) == 0)
			{
				//	������Ϣ����
				if (DisplayMessageUsePAD(server_rec_buffer.data) == SYS_NO_ERR)
				{
					//	��Ϣ���ͳɹ�������OK
					ReturnMS(1, 0, server_rec_buffer.argument, server_rec_buffer.package_no);
				}
				else
				{
					//	��Ϣ�����ʧ�ܣ�����ER
					ReturnMS(0, 1, server_rec_buffer.argument, server_rec_buffer.package_no);
				}
			}
			else if (memcmp(server_rec_buffer.command, "WS", 2) == 0)
			{
				//	����д��������
				ServerDownloadConfigData();
				ReturnWriteConfig(server_rec_buffer.argument, server_rec_buffer.package_no);
			}
			else if (memcmp(server_rec_buffer.command, "RS", 2) == 0)
			{
				//	�������������
				ReturnReadConfig(server_rec_buffer.argument, server_rec_buffer.package_no);
			}
			else if (memcmp(server_rec_buffer.command, "RM", 2) == 0)
			{
				//	����������


			}
			else if (server_rec_buffer.package_no != server_send_buffer.package_no)
			{
				//	�����ظ����ǵ�ǰ����Ļظ�������
			}
			else if ((memcmp(server_rec_buffer.command, "TD", 2) == 0)
			         || (memcmp(server_rec_buffer.command, "TS", 2) == 0)
			         || (memcmp(server_rec_buffer.command, "MB", 2) == 0)
			         || (memcmp(server_rec_buffer.command, "GD", 2) == 0)
			         || (memcmp(server_rec_buffer.command, "LI", 2) == 0)
			         || (memcmp(server_rec_buffer.command, "LO", 2) == 0)
			         || (memcmp(server_rec_buffer.command, "LG", 2) == 0)
			        )
			{
				//	����ظ� ������
				OSSemPost(server_return_sem);
			}
			state = SERVER_HEAD;
		}
	}
}

void TaskServerHandle(void *pdata)
{
	uint8 err;
	//_trade_data_to_server_s data;

	pdata = pdata;
	err = 0;
	OSTimeDly(200);

	//ConfigInit();
	RequestUart0(GPRS, 0);

	while (1)
	{

		//	������
		//OSTimeDly(200);
		if (ServerReceiveFrameFromServer() == SERVER_DATA_NO_ERR)
		{
		}
		//ServerGPSData(1,device_control.gps.gps_latitude,device_control.gps.gps_longitude,device_control.gps.gps_movingspeed);
#if 0
		RequestUart1(2, 0);
		switch (err)
		{
		case 1:
			ServerOnLine();
			break;
		case 2:
			data.year = 2012;
			data.month = 10;
			data.day = 30;
			data.hour = 13;
			data.minute = 59;
			data.second = 42;
			data.serial_number = 5;
			data.needpay = 22;
			data.realpay = 50;
			data.change_cashbox_1 = 3;
			data.change_cashbox_2 = 1;
			data.change_cashbox_3 = 2;
			data.destination_num = 2;
			data.current_station = 1;
			data.destination[0].destination_station = 9;
			data.destination[0].price = 10;
			data.destination[0].people_num = 1;
			data.destination[1].destination_station = 6;
			data.destination[1].price = 6;
			data.destination[1].people_num = 2;

			ServerUploadTradeData(&data);
			break;
		case 3:
			ServerTimeSync();
			break;
		case 4:
			//ServerCashBoxBalance(10, 20, 30);
			break;
		case 5:
			ServerGPSData(1, 1234567890, 123456789, 60000);
			break;
		case 6:
			ServerLogin("DW19203");
			break;
		case 7:
			ServerLogout("DW19203");
			break;
		}
		FreeUart1();
#endif
	}
}











