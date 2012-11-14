#include "includes.h"

#define PowerOffGdtu()			(/*asm("NOP")*/0x00 == 0x00)
#define PowerOnGdtu()			(/*asm("")*/0x00 == 0x00)
#define GprsSendByte(a)			Uart0SendByte(a,0)
#define GprsSendString(a)		Uart0SendString(a,0)
#define GprsReceiveByte(a)		Uart0RecByte(a,0,0)

#define GPRS_HEAD				0		//	GPRS 接收头
#define	GPRS_ADDR				1		//	GPRS 地址接收
#define	GPRS_PACKAGE			2		//	GPRS 包号接收
#define	GPRS_LENGHT				3		//	GPRS 长度接收
#define	GPRS_COMMAND			4		//	GPRS 命令接收
#define	GPRS_ARGU				5		//	GPRS 参数接收
#define	GPRS_DATA				6		//	GPRS 数据接收
#define	GPRS_CHECK				7		//	GPRS 校验字接收
#define	GPRS_END				8		//	GPRS 尾接收
#define GPRS_HANDLE				9		//	GPRS 数据处理


static OS_EVENT *server_communication_sem,*server_return_sem;
_server_communication_s gprs_rec_buffer,gprs_send_buffer;
static uint8 server_package;
const char dtuinfo[] = "AT*DtuInfo";
const char heartbeat[] = "AT#Heartbeat:";
const char DSC_IP[] = "AT#IP:0,120.195.217.32";
const char protocol[] = "AT^Protocol:0";
const char DSC_port[] = "AT^DSC_Port:0,22301";
const char DNS_EN[] = "AT^DNS_EN:0,0";





#define GetServerPackage()			server_package
#define ClearServerPackage()		(server_package = 0)

uint8 GetNextPackage(void)
{
	if ((++server_package) > 99) 
		server_package = 0;
	return server_package;
}


void ServerCommInit(void) {
	server_communication_sem = OSMboxCreate(NULL);
	while(server_communication_sem == NULL);		//	初始化失败停止。

	server_return_sem = OSSemCreate(0);
	while (server_return_sem == NULL);
}

//	请求向server 通信通道
void RequestServerCommunication(void)
{
	uint8 err;
	OSSemPend(server_communication_sem,0,&err);
}

//	释放与server 通信通道
void FreeServerCommunication(void)
{
	OSSemPost(server_communication_sem);
}


uint16 CrcString(uint16 crc_r,char *str)
{
	while (*str)
	{
		crc_r = CRCByte(crc_r,*(uint8 *)str);
		str++;
	}
	return crc_r;
}

uint8 GprsSendFrameToServer(_server_communication_s *command)
{
	char temp[6];
	uint8 i=0;
	command->check = 0;
	if (	(command->package_no < 100)		// 校验参数是否正确
		||	(command->data_lenght < 100)
		)
	{
		
	}
	else
	{
		return FALSE;
	}
	GprsSendByte(SERVER_COMMUNICATION_HEAD);	//	发送头
	memset(temp,0,sizeof(temp));
	memcpy(temp,GetDeviceAddr(),4);					//	取设备地址
	GprsSendString(temp);							//	发送设备地址
	command->check = CrcString(command->check,temp);	//	校验设备地址
		
	memset(temp,0,sizeof(temp));
	sprintf(temp,"%02d",command->package_no);
	GprsSendString(temp);							//	发送包号
	command->check = CrcString(command->check,temp);	//	校验

	memset(temp,0,sizeof(temp));
	sprintf(temp,"%02d",command->data_lenght);
	GprsSendString(temp);
	command->check = CrcString(command->check,temp);

	GprsSendByte(command->command[0]);
	command->check = CRCByte(command->check,command->command[0]);
	command->data_lenght--;
	GprsSendByte(command->command[1]);
	command->check = CRCByte(command->check,command->command[1]);
	command->data_lenght--;
	GprsSendByte(command->argument[0]);
	command->check = CRCByte(command->check,command->argument[0]);
	command->data_lenght--;
	GprsSendByte(command->argument[1]);
	command->check = CRCByte(command->check,command->argument[1]);
	command->data_lenght--;
	
	for (i = 0; i < command->data_lenght; i++)
	{
		GprsSendByte((uint8)command->data[i]);
		command->check = CRCByte(command->check,(uint8)command->data[i]);
	}
	//GprsSendByte((uint8)(command->check >> 8));		//	发送校验
	//GprsSendByte((uint8)command->check);
	memset(temp,0,sizeof(temp));
	sprintf(temp,"%04X",command->check);
	GprsSendString(temp);
	
	GprsSendByte(SERVER_COMMUNICATION_END);			//	发送尾
	return TRUE;
}


//	联机命令
uint8 ServerOnLine(void)
{
	uint8 err;
	RequestServerCommunication();
	gprs_send_buffer.package_no = server_package;
	memcpy(gprs_send_buffer.command,"00",2);
	memcpy(gprs_send_buffer.argument,"00",2);
	gprs_send_buffer.data_lenght = 4;
	GprsSendFrameToServer(&gprs_send_buffer);
	
	OSSemPend(server_return_sem,GPRS_MAX_TIME_DELAY*2,&err);
	if (err == GPRS_DATA_NO_ERR)
	{
		if (memcmp(gprs_rec_buffer.command,"OK",2) == 0)
		{
			err = GPRS_DATA_NO_ERR;
		}
		else
		{
			err = GPRS_DATA_RETURN_ERR;
		}
	}
	FreeServerCommunication();
	return err;
}

//	上传交易数据
uint8 ServerUploadTradeData(_trade_data_to_server_s *data)
{
	uint8 err;
	
	RequestServerCommunication();
	gprs_send_buffer.package_no = server_package;
	memcpy(gprs_send_buffer.command,"TD",2);
	memcpy(gprs_send_buffer.argument,"00",2);
	sprintf(gprs_send_buffer.data,"%04d%02d%02d%02d%02d%02d%05d%05d%05d%02d%02d%02d%01d%02d",
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
		sprintf(&gprs_send_buffer.data[38 + (err * 7)],"%02d%03d%02d",
			data->destination[err].destination_station,
			data->destination[err].price,
			data->destination[err].people_num);
	}
	
	gprs_send_buffer.data_lenght = 4 + strlen(gprs_send_buffer.data);
	GprsSendFrameToServer(&gprs_send_buffer);
	
	OSSemPend(server_return_sem,GPRS_MAX_TIME_DELAY*2,&err);
	if (err == GPRS_DATA_NO_ERR)
	{
		if (memcmp(gprs_rec_buffer.command,"OK",2) == 0)
		{
			err = GPRS_DATA_NO_ERR;
		}
		else
		{
			err = GPRS_DATA_RETURN_ERR;
		}
	}
	FreeServerCommunication();
	return err;
}

//	时间同步
uint8 ServerTimeSync(void)
{
	uint8 err;
	
	RequestServerCommunication();
	gprs_send_buffer.package_no = server_package;
	memcpy(gprs_send_buffer.command,"TS",2);
	memcpy(gprs_send_buffer.argument,"00",2);
	gprs_send_buffer.data_lenght = 4;
	GprsSendFrameToServer(&gprs_send_buffer);
	
	OSSemPend(server_return_sem,GPRS_MAX_TIME_DELAY*2,&err);
	if (err == GPRS_DATA_NO_ERR)
	{
		if (memcmp(gprs_rec_buffer.command,"OK",2) == 0)
		{
			err = GPRS_DATA_NO_ERR;
		}
		else
		{
			err = GPRS_DATA_RETURN_ERR;
		}
	}
	FreeServerCommunication();
	return err;
}

//	钱箱剩余钞票
uint8 ServerCashBoxBalance(uint16 cashbox1, uint16 cashbox2, uint16 cashbox3)
{
	uint8 err;
	
	RequestServerCommunication();
	gprs_send_buffer.package_no = server_package;
	memcpy(gprs_send_buffer.command,"MB",2);
	memcpy(gprs_send_buffer.argument,"00",2);
	sprintf(gprs_send_buffer.data, "%05d%05d%05d", cashbox1,cashbox2,cashbox3);
	gprs_send_buffer.data_lenght = 4 + 15;
	GprsSendFrameToServer(&gprs_send_buffer);
	
	OSSemPend(server_return_sem,GPRS_MAX_TIME_DELAY*2,&err);
	if (err == GPRS_DATA_NO_ERR)
	{
		if (memcmp(gprs_rec_buffer.command,"OK",2) == 0)
		{
			err = GPRS_DATA_NO_ERR;
		}
		else
		{
			err = GPRS_DATA_RETURN_ERR;
		}
	}
	FreeServerCommunication();
	return err;
}

uint8 ServerGPSData(uint8 flag,uint32 latitude, uint32 longitude, uint32 speed)
{
	uint8 err;
	
	RequestServerCommunication();
	gprs_send_buffer.package_no = server_package;
	memcpy(gprs_send_buffer.command,"GD",2);
	if (flag)
	{
		memcpy(gprs_send_buffer.argument,"01",2);
		sprintf(gprs_send_buffer.data, "%010d%010d%03d.%02d",longitude,latitude,speed/1000,(speed%1000)/10);
		gprs_send_buffer.data_lenght = 4+26;
	}
	else
	{
		memcpy(gprs_send_buffer.argument,"00",2);
		gprs_send_buffer.data_lenght = 4;
	}
	GprsSendFrameToServer(&gprs_send_buffer);
	
	OSSemPend(server_return_sem,GPRS_MAX_TIME_DELAY*2,&err);
	if (err == GPRS_DATA_NO_ERR)
	{
		if (memcmp(gprs_rec_buffer.command,"OK",2) == 0)
		{
			err = GPRS_DATA_NO_ERR;
		}
		else
		{
			err = GPRS_DATA_RETURN_ERR;
		}
	}
	FreeServerCommunication();
	return err;
}

uint8 ServerLogin(char *staffid)
{
	uint8 err;
	
	RequestServerCommunication();
	gprs_send_buffer.package_no = server_package;
	memcpy(gprs_send_buffer.command,"LI",2);
	memcpy(gprs_send_buffer.argument,"00",2);
	memset(gprs_send_buffer.data,0,9);
	sprintf(gprs_send_buffer.data,"%s",staffid);
	gprs_send_buffer.data_lenght = 4+8;
	GprsSendFrameToServer(&gprs_send_buffer);
	
	OSSemPend(server_return_sem,GPRS_MAX_TIME_DELAY*2,&err);
	if (err == GPRS_DATA_NO_ERR)
	{
		if (memcmp(gprs_rec_buffer.command,"OK",2) == 0)
		{
			err = GPRS_DATA_NO_ERR;
		}
		else
		{
			err = GPRS_DATA_RETURN_ERR;
		}
	}
	FreeServerCommunication();
	return err;
}

uint8 ServerLogout(char *staffid)
{
	uint8 err;
	
	RequestServerCommunication();
	gprs_send_buffer.package_no = server_package;
	memcpy(gprs_send_buffer.command,"LO",2);
	memcpy(gprs_send_buffer.argument,"00",2);
	memset(gprs_send_buffer.data,0,9);
	sprintf(gprs_send_buffer.data,"%s",staffid);
	gprs_send_buffer.data_lenght = 4+8;
	GprsSendFrameToServer(&gprs_send_buffer);
	
	OSSemPend(server_return_sem,GPRS_MAX_TIME_DELAY*2,&err);
	if (err == GPRS_DATA_NO_ERR)
	{
		if (memcmp(gprs_rec_buffer.command,"OK",2) == 0)
		{
			err =  GPRS_DATA_NO_ERR;
		}
		else
		{
			err = GPRS_DATA_RETURN_ERR;
		}
	}
	FreeServerCommunication();
	return err;
}



uint16 GetVersion(void *arg)
{
	sprintf(arg,"%s;%s;0",VERSION,PAD_version);
	return strlen(arg);
}

uint16 GetUartConfig(void *arg)
{
	sprintf(arg,"%d%06d;%d%06d;%d%06d;%d%06d;%d%06d;%d%06d;%d%06d;%d%06d;%d%06d;%d%06d;",
		GetRj45UartChannal(),GetRj45UartBps(),
		GetGprsUartChannal(),GetGprsUartBps(),
		GetGpsUartChannal(),GetGpsUartBps(),
		GetBillUartChannal(),GetBillUartBps(),
		GetCoinUartChannal(),GetCoinUartBps(),
		GetPrintUartChannal(),GetPrintUartBps(),
		GetVoiceUartChannal(),GetVoiceUartBps(),
		GetICMachineUartChannal(),GetICMachineUartBps(),
		0x00,0x00,
		0x00,0x00
		);
	return 80;
}

uint16 UartConfig(void *arg)
{
	SetRj45UartChannal(((_uart_config_from_server_s *)gprs_rec_buffer.data)[0].channal_num - '0');
	((_uart_config_from_server_s *)gprs_rec_buffer.data)[0].separator = 0x00;
	SetRj45UartBps(atoi(((_uart_config_from_server_s *)gprs_rec_buffer.data)[0].bps));
	
	SetGprsUartChannal(((_uart_config_from_server_s *)gprs_rec_buffer.data)[1].channal_num - '0');
	((_uart_config_from_server_s *)gprs_rec_buffer.data)[1].separator = 0x00;
	SetGprsUartBps(atoi(((_uart_config_from_server_s *)gprs_rec_buffer.data)[1].bps));
	
	SetGpsUartChannal(((_uart_config_from_server_s *)gprs_rec_buffer.data)[2].channal_num - '0');
	((_uart_config_from_server_s *)gprs_rec_buffer.data)[2].separator = 0x00;
	SetGpsUartBps(atoi(((_uart_config_from_server_s *)gprs_rec_buffer.data)[2].bps));
	
	SetBillUartChannal(((_uart_config_from_server_s *)gprs_rec_buffer.data)[3].channal_num - '0');
	((_uart_config_from_server_s *)gprs_rec_buffer.data)[3].separator = 0x00;
	SetBillUartBps(atoi(((_uart_config_from_server_s *)gprs_rec_buffer.data)[3].bps));
	
	SetCoinUartChannal(((_uart_config_from_server_s *)gprs_rec_buffer.data)[4].channal_num - '0');
	((_uart_config_from_server_s *)gprs_rec_buffer.data)[4].separator = 0x00;
	SetCoinUartBps(atoi(((_uart_config_from_server_s *)gprs_rec_buffer.data)[4].bps));
	
	SetPrintUartChannal(((_uart_config_from_server_s *)gprs_rec_buffer.data)[5].channal_num - '0');
	((_uart_config_from_server_s *)gprs_rec_buffer.data)[5].separator = 0x00;
	SetPrintUartBps(atoi(((_uart_config_from_server_s *)gprs_rec_buffer.data)[5].bps));
	
	SetICMachineUartChannal(((_uart_config_from_server_s *)gprs_rec_buffer.data)[7].channal_num - '0');
	((_uart_config_from_server_s *)gprs_rec_buffer.data)[6].separator = 0x00;
	SetICMachineUartBps(atoi(((_uart_config_from_server_s *)gprs_rec_buffer.data)[7].bps));
	return TRUE;
}


uint16 GetFuncConfig(void *arg)
{
	sprintf(arg,"%d%031d",GetLoginMod(),0x00);
	return 32;
}

uint16 FuncConfig(void *arg)
{
	SetLoginMod(gprs_rec_buffer.data[0] - '0');
	return TRUE;
}

uint16 GetCashboxConfig(void *arg)
{
	sprintf(arg,"%04d%04d%04d%04d%04d%04d",
		GetCashbox1Value(),
		GetCashbox2Value(),
		GetCashbox3Value(),
		GetCashbox1AlarmThreshold(),
		GetCashbox2AlarmThreshold(),
		GetCashbox3AlarmThreshold()
		);
	return 24;
}

uint16 CashboxConfig(void *arg)
{
	uint8 err;
	uint32 temp_32;
	_cashbox_config_from_server_s *temp = (_cashbox_config_from_server_s *)gprs_rec_buffer.data;
		
	temp_32 = stoi(10,4,temp->cashbox1_par_value,&err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox1Value((uint8)temp_32);
	
	temp_32 = stoi(10,4,temp->cashbox2_par_value,&err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox2Value((uint8)temp_32);
	
	temp_32 = stoi(10,4,temp->cashbox3_par_value,&err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox3Value((uint8)temp_32);
	
	temp_32 = stoi(10,4,temp->cashbox1_threshold,&err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox1AlarmThreshold((uint8)temp_32);
	
	temp_32 = stoi(10,4,temp->cashbox2_threshold,&err);
	if (err == FALSE)
	{
		return FALSE;
	}	
	SetCashbox2AlarmThreshold((uint8)temp_32);
	
	temp_32 = stoi(10,4,temp->cashbox3_threshold,&err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetCashbox3AlarmThreshold((uint8)temp_32);
	return TRUE;
}

uint16 GetSysPerformanceConfig(void *arg)
{
	sprintf(arg,"%04d%05d%05d%05d%05d",
		0x00,
		GetGprsAnswerResponseTime(),
		GetGpsSamplingTime(),
		GetGprsOfflineResponseTime(),
		GetLoginRemainTime()
		);
	memcpy(arg,GetDeviceAddr(),4);
	return 24;
}


uint16 SysPerformanceConfig(void *arg)
{
	uint8 err;
	_sys_performance_config_from_server_s *temp;
	temp = (_sys_performance_config_from_server_s *)gprs_rec_buffer.data;
	if (SetDeviceAddr(temp->device_addr) == FALSE)
	{
		return FALSE;
	}
	SetGprsAnswerResponseTime((uint16)stoi(10,5,temp->gprs_response_time,&err));
	if (err == FALSE)
	{
		return FALSE;
	}
	SetGpsSamplingTime((uint16)stoi(10,5,temp->gps_sampling_time,&err));
	if (err == FALSE)
	{
		return FALSE;
	}
	SetGprsOfflineResponseTime((uint16)stoi(10,5,temp->gprs_off_line_delay_time,&err));
	if (err == FALSE)
	{
		return FALSE;
	}
	SetLoginRemainTime((uint16)stoi(10,5,temp->user_logout_delay_time,&err));
	if (err == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

uint16 GetServerCommConfig(void *arg)
{
	sprintf(arg,"%03d%03d%03d%03d%05d",
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
	_server_communication_config_from_server *temp = (_server_communication_config_from_server *)gprs_rec_buffer.data;
	ip_temp[0] = (uint8)stoi(10,3,temp->ip[0],&err);
	if (err == FALSE)
	{
		return FALSE;
	}
	ip_temp[1] = (uint8)stoi(10,3,temp->ip[1],&err);
	if (err == FALSE)
	{
		return FALSE;
	}
	ip_temp[2] = (uint8)stoi(10,3,temp->ip[2],&err);
	if (err == FALSE)
	{
		return FALSE;
	}
	ip_temp[3] = (uint8)stoi(10,3,temp->ip[3],&err);
	if (err == FALSE)
	{
		return FALSE;
	}
	port = (uint16)stoi(10,5,temp->port,&err);
	if (err == FALSE)
	{
		return FALSE;
	}
	SetServerIp(ip_temp[0],ip_temp[1],ip_temp[2],ip_temp[3]);
	SetServerPort(port);
	return TRUE;
}

uint16 GetPrintConfig(void *arg)
{
	memcpy(arg,GetPrintCustomer(),16);
	
	return 16;
}


uint16 PrintConfig(void *arg)
{
	SetPrintCustomer(gprs_rec_buffer.data);
	return TRUE;
}

typedef struct
{
	char command[3];
	_pfunc *funcset;
	_pfunc *funcget;
}_config_corresponding_s;			//	命令匹配表

//	配置命令参数处理
const _config_corresponding_s config_corresponding_table[] =
{
	{"00",NULL,GetVersion},
	{"01",UartConfig,GetUartConfig},
	{"02",FuncConfig,GetFuncConfig},
	{"03",CashboxConfig,GetCashboxConfig},
	{"04",SysPerformanceConfig,GetSysPerformanceConfig},
	{"05",ServerCommConfig,GetServerCommConfig},
	{"06",PrintConfig,GetPrintConfig},
	{"FF",NULL,NULL}
};
uint8 ServerDownloadConfigData(void)
{
	uint8 i = 0;
	while (1)
	{
		if (memcmp(gprs_rec_buffer.argument,config_corresponding_table[i].command,2) == 0)
		{
			return config_corresponding_table[i].funcset(NULL);
			
		}
		if (memcmp(config_corresponding_table[++i].command,"FF",2) == 0)
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
		if (memcmp(gprs_rec_buffer.argument,config_corresponding_table[i].command,2) == 0)
		{
			return config_corresponding_table[i].funcget(arg);
			
		}
		if (memcmp(config_corresponding_table[++i].command,"FF",2) == 0)
		{
			return FALSE;
		}
	}
}


// OK命令
uint8 ReturnOK(char *arg,uint8 package_no)
{
	//uint8 err;
	
	_server_communication_s server_communication_temp;
	memcpy(server_communication_temp.command,"OK",2);
	memcpy(server_communication_temp.argument,arg,2);
	server_communication_temp.data_lenght = 4;
	server_communication_temp.package_no = package_no;
	GprsSendFrameToServer(&server_communication_temp);
	return GPRS_DATA_NO_ERR;
}
// ER命令
uint8 ReturnER(char *arg,uint8 package_no)
{
	//uint8 err;
	
	_server_communication_s server_communication_temp;
	memcpy(server_communication_temp.command,"ER",2);
	memcpy(server_communication_temp.argument,arg,2);
	server_communication_temp.data_lenght = 4;
	server_communication_temp.package_no = package_no;
	GprsSendFrameToServer(&server_communication_temp);
	return GPRS_DATA_NO_ERR;
}

uint8 ReturnConfig(char *arg,uint8 package_no)
{
	_server_communication_s server_communication_temp;
	memcpy(server_communication_temp.command,"RS",2);
	memcpy(server_communication_temp.argument,arg,2);
	server_communication_temp.data_lenght = ServerGetConfigData(server_communication_temp.data) + 4;
	server_communication_temp.package_no = package_no;
	GprsSendFrameToServer(&server_communication_temp);
	return GPRS_DATA_NO_ERR;
}



//	用于启动GDTU
void StartGdtu(void)
{
	uint8 temp;
	PowerOffGdtu();
	OSTimeDly(OS_TICKS_PER_SEC);
	PowerOnGdtu();
	while (1)
	{
		if ((GprsReceiveByte((uint8 *)temp) == TRUE) && (temp == ':'))	//	判断是否进入配置模式
		{
			break;
		}
	}
	GprsSendString("12345678\r\n");		//	输入密码进入配置模式
	OSTimeDly(OS_TICKS_PER_SEC);
	//	发送F4 退出配置模式
	GprsSendByte(0x1b);
	GprsSendByte(0x4f);
	GprsSendByte(0x53);
	OSTimeDly(OS_TICKS_PER_SEC/20);
}

//	用于检测GDTU 配置是否正确，若不正确，重新配置
void DetectedAndConfigGdtu(void)
{
	uint8 temp[32],i,flag=0;
	PowerOffGdtu();
	OSTimeDly(OS_TICKS_PER_SEC);
	PowerOnGdtu();
	// 	一直发'U' 初始化GDTU
	//	一直发''' 进入AT 命令模式
	Uart0SendByte('\'',2);		//	发送
	while (1)
	{
		if ((GprsReceiveByte(temp) == TRUE) && (temp[0] == '*'))	//	判断是否进入AT 命令模式
		{
			break;
		}
	}
	OSTimeDly(OS_TICKS_PER_SEC);
	memset(temp,0,32);
	GprsSendString("AT*DtuInfo\r\n");
	i = 0;
	while (1)
	{
		if (GprsReceiveByte((uint8 *)(temp+i)) == TRUE)
		{
			if ((temp[i] == '\n') && (temp[i-1] == '\r'))
			{
				temp[i-1] = 0;
				if (strstr((char *)temp,"Protocol") != NULL)
				{
					flag |= 0x01;
				}
				else if (strstr((char *)temp,"Max_DscNum") != NULL)
				{
					flag |= 0x02;
				}
			}
			else if (temp[0] == 'A')
			{
				
			}
		}
		else
		{
			OSTimeDly(2);
		}
	}
	memset(temp,0,32);
	GprsSendString("AT*DscInfo:0\r\n");
	i = 0;
	while (1)
	{
		if (GprsReceiveByte((uint8 *)(temp+i)) == TRUE)
		{
			if ((temp[i] == '\n') && (temp[i-1] == '\r'))
			{
				if (strstr((char *)temp,"IP") != NULL)
				{
					flag |= 0x04;
				}
				else if (strstr((char *)temp,"DSC Port") != NULL)
				{
					flag |= 0x08;
				}
			}
			else 
			{
				if ((++i) >= 32)
				{
					i--;
				}
			}
		}
		else
		{
			OSTimeDly(2);
		}
	}
}

uint8 GprsReceiveFrameFromServer(void)
{
	uint8 state = GPRS_HEAD,err;
	char device_addr[4];
	char check_temp[4];
	uint8 data_temp,data_num=0,data_amount=0;
	uint16 check_sun,timer_num=0;
	
	while (1)
	{
		if (GprsReceiveByte(&data_temp) == FALSE)
		{
			OSTimeDly(2);
			if ((++timer_num) > GPRS_MAX_TIME_DELAY)	return GPRS_TIME_OUT;		//	超时退出，返回错误
			else										continue;			//	继续下一次接收
		}
		else
		{
			timer_num = 0;			//	有数据，清零延时
		}
		if ((++data_amount) > GPRS_FRAME_MAX_LENGHT)		//	线路上是否有很多杂乱数据
		{
			return GPRS_UART_FAULT;
		}
		switch (state)
		{
			case GPRS_HEAD:
				if (data_temp == SERVER_COMMUNICATION_HEAD)
				{
					state = GPRS_ADDR;
					data_num = 0;
					check_sun = 0;
				}
				break;
				
			case GPRS_ADDR:
				if ((data_temp >= '0') && (data_temp <= '9'))
				{
					//	计算地址
					device_addr[data_num] = data_temp;
					check_sun = CRCByte(check_sun,data_temp);
				}
				else
				{
					state = GPRS_HEAD;			//	重新接收
					data_num = 0;
				}
				if ((++data_num) >= 4)
				{
					state = GPRS_PACKAGE;
					gprs_rec_buffer.package_no = 0;
					data_num = 0;
				}
				break;
				
			case GPRS_PACKAGE:
				if ((data_temp >= '0') && (data_temp <= '9'))
				{
					//
					gprs_rec_buffer.package_no = gprs_rec_buffer.package_no * 10 + (data_temp - '0');
					check_sun = CRCByte(check_sun,data_temp);
				}
				else
				{
					state = GPRS_HEAD;			//	重新接收
					data_num = 0;
				}
				if ((++data_num) >= 2)
				{
					state = GPRS_LENGHT;			//	准备接收长度
					gprs_rec_buffer.data_lenght = 0;
					data_num = 0;
				}
				break;

			case GPRS_LENGHT:
				if ((data_temp >= '0') && (data_temp <= '9'))
				{
					//	计算长度
					gprs_rec_buffer.data_lenght = gprs_rec_buffer.data_lenght * 10 + (data_temp - '0');	
					check_sun = CRCByte(check_sun,data_temp);
				}
				else
				{
					state = GPRS_HEAD;			//	重新接收
					data_num = 0;
				}
				if ((++data_num) >= 2)
				{
					if ((gprs_rec_buffer.data_lenght > (86+2+2)) || (gprs_rec_buffer.data_lenght < 4))		//	长度错误，重新接收
					{
						state = GPRS_HEAD;			//	重新接收
						data_num = 0;
					}
					else
					{
						state = GPRS_COMMAND;		//	准备接收命令
						data_num = 0;
					}
				}
				break;

			case GPRS_COMMAND:
				gprs_rec_buffer.command[data_num] = data_temp;
				check_sun = CRCByte(check_sun,data_temp);
				if ((++data_num) >= 2)
				{
					state = GPRS_ARGU;		//	准备接收命令
					data_num = 0;
				}
				break;

			case GPRS_ARGU:
				gprs_rec_buffer.argument[data_num] = data_temp;
				check_sun = CRCByte(check_sun,data_temp);
				if ((++data_num) >= 2)
				{
					if (gprs_rec_buffer.data_lenght == 4)
					{
						gprs_rec_buffer.data[0] = 0;
						state = GPRS_CHECK;
						gprs_rec_buffer.check = 0;
						data_num = 0;
					}
					else
					{
						state = GPRS_DATA;		//	准备接收命令
						data_num = 0;
					}
				}
				break;

			case GPRS_DATA:
				gprs_rec_buffer.data[data_num] = data_temp;
				check_sun = CRCByte(check_sun,data_temp);
				if ((gprs_rec_buffer.data_lenght - 4) <= (++data_num))
				{
					gprs_rec_buffer.data[data_num] = 0;
					state = GPRS_CHECK;
					data_num = 0;
					gprs_rec_buffer.check = 0;
				}
				break;

			case GPRS_CHECK:
				//gprs_rec_buffer.check = gprs_rec_buffer.check * 16 + data_temp - '0';
				check_temp[data_num] = data_temp;
				if ((++data_num) >= 4)
				{
					gprs_rec_buffer.check = (uint16)stoi(16,4,check_temp,&err);
					if (gprs_rec_buffer.check == check_sun)
					{
						state = GPRS_END;
						data_num = 0;
					}
					else
					{
						return GPRS_CHECKE_ERR;
					}
				}
				break;

			case GPRS_END:
				if (data_temp == SERVER_COMMUNICATION_END)		//	包尾是否正确
				{
					state = GPRS_HANDLE;
				}
				else
				{
					state = GPRS_HEAD;
				}
				break;

			default:
				state = GPRS_HEAD;
				break;
		}
		if (state == GPRS_HANDLE)
		{
			if (memcmp(GetDeviceAddr(),device_addr,4) != 0)
			{
				ReturnER("99",gprs_rec_buffer.package_no);
			}
			else if (memcmp(gprs_rec_buffer.command,"OK",2) == 0)
			{
				OSSemPost(server_return_sem);
			}
			else if (memcmp(gprs_rec_buffer.command,"ER",2) == 0)
			{
				OSSemPost(server_return_sem);
			}
			else if (memcmp(gprs_rec_buffer.command,"MS",2) == 0)
			{
				if (DisplayMessage(gprs_rec_buffer.data) == SYS_NO_ERR) 
				{
					ReturnOK("00",gprs_rec_buffer.package_no);
				}
				else
				{
					ReturnER("00",gprs_rec_buffer.package_no);
				}
			}
			else if (memcmp(gprs_rec_buffer.command,"WS",2) == 0)
			{
				ServerDownloadConfigData();
				ReturnOK(gprs_rec_buffer.argument,gprs_rec_buffer.package_no);
			}
			else if (memcmp(gprs_rec_buffer.command,"RS",2) == 0)
			{
				ReturnConfig(gprs_rec_buffer.argument,gprs_rec_buffer.package_no);
			}
			state = GPRS_HEAD;
		}
	}
}

void TaskServerHandle(void *pdata) {
	uint8 err;
	//_trade_data_to_server_s data;

	pdata = pdata;
	err=0;
	OSTimeDly(200);

	//ConfigInit();
	RequestUart0(GPRS,0);

	while (1)
	{

		//	读数据
		//OSTimeDly(200);
		if (GprsReceiveFrameFromServer() == GPRS_DATA_NO_ERR)
		{
		}
		//ServerGPSData(1,device_control.gps.gps_latitude,device_control.gps.gps_longitude,device_control.gps.gps_movingspeed);
#if 0
		RequestUart1(2,0);
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
		ServerCashBoxBalance(10,20,30);
			break;
			case 5:
		ServerGPSData(1,1234567890,123456789,60000);
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











