#include "includes.h"

_config_s config_ram;

char *GetDeviceAddr(void)
{
	uint8 err;
	uint32 temp_32;
	temp_32 = stoi(16, 4, config_ram.device_addr, &err);		//	�����ַ
	if ((temp_32 <= MAX_DEVICE_ADDR) && (temp_32 >= MIN_DEVICE_ADDR))		//	��ַ�Ƿ���Ϲ涨
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
	temp_32 = stoi(16, 4, arg, &err);		//	�����ַ
	if ((temp_32 <= MAX_DEVICE_ADDR) && (temp_32 >= MIN_DEVICE_ADDR))		//	��ַ�Ƿ���Ϲ涨
	{
		memcpy(config_ram.device_addr, arg, 4);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/*
*	uart ��Դ����
	GPS_UART1,				//	GPSͨ��
	NOTE_MACHINE_UART0,		//	ֽ�һ�ͨ��
	COINMACHINE_UART0,		//	Ӳ�һ�ͨ��
	PRINTER_UART0,			//	��ӡͨ��
	SPEAKER_UART1,			//	����ͨ��
	IC_MACHINE_UART1,		//	IC ��ͨ��
*/

uint8 RequestUart(uint8 ch, uint16 t)
{
	switch (config_ram.um.um_array[ch].channal)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		return RequestUart0(t, config_ram.um.um_array[ch].channal, config_ram.um.um_array[ch].bps);
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		return RequestUart1(t, (config_ram.um.um_array[ch].channal - 4), config_ram.um.um_array[ch].bps);
		break;
	default:
		return FALSE;
		break;
	}

}

void FreeUart(uint8 ch)
{
	switch (config_ram.um.um_array[ch].channal)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		FreeUart0();
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		FreeUart1();
		break;
	default:
		break;
	}
}


