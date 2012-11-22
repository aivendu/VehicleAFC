#include "includes.h"

_config_s config_ram;

#define CONFIG_STATE_INIT_VALUE			1
#define LOGIN_MOD_INTT_VALUE			0
#define FUNCTION_INIT_VALUE				(uint32)(CONFIG_STATE_INIT_VALUE + (LOGIN_MOD_INTT_VALUE << 2))


#if 0
 
static _uart_config_s const uart0_config[] = {
 	{0,	9600		},		//	0- init
 	{3,	38400		},		//	1- print machine
	{2,	9600		},		//	2- coin machine
 	{1,	9600		},		//	3- note machine
 };

 
static _uart_config_s const uart1_config[] = {
 	{0,	9600		},		//	0- init
	{1,	9600		},		//	1- speaker
 	{2,	19200		},		//	2- IC Card machine
 	{3,	9600		},		//	3- GPS module
 };

uc_rj45;
uc_gprs;
uc_gps;
uc_bill;
uc_coin;
uc_print;
uc_voice;
uc_icmachine;
unused1;
unused2;

#endif

const _config_s config_init_from_rom = 
{
	{"无锡客运有限公司"},
	{1,0,0,0},
	{1,5,10,10,10,5,0,0,0,200,20,50,0,0},
	{	8,9600,		//	rj45;
		9,9600,		//	gprs;
		7,9600,		//	gps;
		1,9600,		//	bill;
		2,9600,		//	coin;
		3,38400,	//	print;
		5,9600,		//	voice;
		6,19200,		//	icmachine;
		0,9600,		//	未使用
		4,9600,		//	未使用
	},
	{5,3,30,10,22301,120,195,217,32},
	{"FFFF"},
	{"苏BU2956"},
	{"tv-10"},
	{"cv-101"},
};

void ConfigInit(void)
{
#if 1
	ReadExternMemery(&config_ram,CONFIG_SAVE_START_ADDR,sizeof(_config_s));		//	上电初始化配置参数
	//	初始化交易数据
	if ((GetConfigState() == 0) 
		|| (memcmp(GetTradeDataVersion(),config_init_from_rom.trade_form_version,strlen(config_init_from_rom.trade_form_version) != 0)))
	{
		current_trade_index = TRADE_DATA_START_ADDR;
		log_index.log_start = LOG_START_ADDR;
		log_index.log_end = LOG_START_ADDR;
		WriteExternMemery(&current_trade_index,TRADE_DATA_START_ADDR,sizeof(current_trade_index));
		WriteExternMemery(&log_index,LOG_START_ADDR,sizeof(_log_manage_s));
	}
	//	判断是否有初始化过配置数据
	if ((GetConfigState() == 0) 
		|| (memcmp(GetConfigVersion(),config_init_from_rom.config_version,strlen(config_init_from_rom.config_version) != 0)))
	{
		config_ram = config_init_from_rom;
		WriteExternMemery(&config_ram,CONFIG_SAVE_START_ADDR,sizeof(_config_s));		//	重新初始化
	}
	//	更新从芯片配置选项
	while (ChipDataUpload(CHIP_WRITE,0x06,0,sizeof(_config_s),&config_ram) != TRUE)
	{
		OSTimeDly(2);
	}
#endif
}

char * GetDeviceAddr(void)
{
	uint8 err;
	uint32 temp_32;
	temp_32 = stoi(16,4,config_ram.device_addr,&err);		//	计算地址
	if ((temp_32 <= MAX_DEVICE_ADDR) && (temp_32 >= MIN_DEVICE_ADDR))		//	地址是否符合规定
	{
		return config_ram.device_addr;
		
	}
	else
	{
		return "FFFF";
	}
}

uint8 SetDeviceAddr(char *arg)
{
	uint8 err;
	uint32 temp_32;
	temp_32 = stoi(16,4,arg,&err);		//	计算地址
	if ((temp_32 <= MAX_DEVICE_ADDR) && (temp_32 >= MIN_DEVICE_ADDR))		//	地址是否符合规定
	{
		memcpy(config_ram.device_addr,arg,4);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
void TaskSysConfig(void *pdata)
{
	uint8 temp[10];
	pdata= pdata;
	
	while (1)
	{
		OSTimeDly(10);
		if (GetSaveConfig() == EXE_WRITED)
		{
			SetSaveConfig(EXE_RUNNING);
			//	存储系统配置
			WriteExternMemery(&config_ram,CONFIG_SAVE_START_ADDR,sizeof(_config_s));
			//	更新从芯片配置选项
		}
		else
		{
			SetSaveConfig(EXE_WAIT);
		}
		memset(temp,0,sizeof(temp));
		if (ChipDataUpload(CHIP_READ,0x06,CONFIG_VERSION_INDEX_ADDR,sizeof(config_ram.config_version),temp) == TRUE)
		{
			if (memcmp(config_ram.config_version,temp,4) != 0)
			{
				//	从芯片的配置信息不对，重新配置
				while (ChipDataUpload(CHIP_WRITE,0x06,0,sizeof(_config_s),&config_ram) != TRUE)
				{
					OSTimeDly(2);
				}
			}
		}
	}
	
}















