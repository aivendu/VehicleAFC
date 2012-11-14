#include "includes.h"

_config_s config_ram;

#define CONFIG_STATE_INIT_VALUE			1
#define LOGIN_MOD_INTT_VALUE			0
#define FUNCTION_INIT_VALUE				(uint32)(CONFIG_STATE_INIT_VALUE + (LOGIN_MOD_INTT_VALUE << 2))


const _config_s config_init_from_rom = 
{
	{"cv-100"},
	{"无锡客运有限公司"},
	{FUNCTION_INIT_VALUE},
	{10,5,10,10,10,5,0,0},
	{	0,9600,		//	rj45;
		1,9600,		//	gprs;
		2,9600,		//	gps;
		3,9600,		//	bill;
		4,9600,		//	coin;
		5,9600,		//	print;
		6,9600,		//	voice;
		7,9600,		//	icmachine;
		8,9600,		//	未使用
		9,9600,		//	未使用
	},
	{'F','F','F','F',5,0,3,0,30,0,10,0,0x1d,0x57,120,195,217,32},
};

void ConfigInit(void)
{
#if 1
	ReadExternMemery(&config_ram,CONFIG_SAVE_START_ADDR,sizeof(_config_s));		//	上电初始化配置参数
	//	判断是否有初始化过
	if ((GetConfigState() == 0) 
		|| (memcmp(GetConfigVersion(),config_init_from_rom.config_version,strlen(config_init_from_rom.config_version) != 0)))
	{
		if (GetConfigState() == 0)
		{
			current_trade_index = TRADE_DATA_START_ADDR;
			log_index.log_start = LOG_START_ADDR;
			log_index.log_end = LOG_START_ADDR;
			WriteExternMemery(&current_trade_index,TRADE_DATA_START_ADDR,sizeof(current_trade_index));
			WriteExternMemery(&log_index,LOG_START_ADDR,sizeof(_log_manage_s));
		}
		config_ram = config_init_from_rom;
		WriteExternMemery(&config_ram,CONFIG_SAVE_START_ADDR,sizeof(_config_s));		//	重新初始化
	}
#endif
}

char * GetDeviceAddr(void)
{
	uint8 err;
	uint32 temp_32;
	temp_32 = stoi(16,4,config_ram.pa.pa.device_addr,&err);		//	计算地址
	if ((temp_32 <= MAX_DEVICE_ADDR) && (temp_32 >= MIN_DEVICE_ADDR))		//	地址是否符合规定
	{
		return config_ram.pa.pa.device_addr;
		
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
		memcpy(config_ram.pa.pa.device_addr,arg,4);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
void TaskConfig(void *pdata)
{
	pdata= pdata;

	if (sizeof(_function_config_s) > 4)
	{
		while(1);
	}
	
}















