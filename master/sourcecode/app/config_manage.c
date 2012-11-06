#include "includes.h"

_config_s config_ram;

const _config_s config_init_from_rom = 
{
	{"cv-100"},
	{"�����������޹�˾"},
	{0x00,0x00,0x00},
	{10,5,10,10,10,5,0,0},
	{	{0,0,0,9600},		//	rj45;
		{1,0,0,9600},		//	gprs;
		{2,0,0,9600},		//	gps;
		{3,0,0,9600},		//	bill;
		{4,0,0,9600},		//	coin;
		{5,0,0,9600},		//	print;
		{6,0,0,9600},		//	voice;
		{7,0,0,9600},		//	icmachine;
		{8,0,0,9600},		//	δʹ��
		{9,0,0,9600},		//	δʹ��
	},
	{"9999",5,3,30,10,22301,120,195,217,32},
};

void ConfigInit(void)
{
	ReadExternMemery(&config_ram,0,sizeof(_config_s));		//	�ϵ��ʼ�����ò���
	//	�ж��Ƿ��г�ʼ����
	if ((config_ram.fc.config_flag == 0) 
		&& (memcmp(config_ram.config_version,config_init_from_rom.config_version,strlen(config_init_from_rom.config_version) == 0)))
	{
		config_ram = config_init_from_rom;
		WriteExternMemery(&config_ram,0,sizeof(_config_s));		//	���³�ʼ��
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















