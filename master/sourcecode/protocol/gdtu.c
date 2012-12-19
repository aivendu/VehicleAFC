#include "includes.h"

#define PowerOffGdtu()			(/*asm("NOP")*/0x00 == 0x00)
#define PowerOnGdtu()			(/*asm("")*/0x00 == 0x00)

#if 0
//	用于启动GDTU
void StartGdtu(void)
{
	uint8 temp;
	PowerOffGdtu();
	OSTimeDly(OS_TICKS_PER_SEC);
	PowerOnGdtu();
	while (1)
	{
		if ((ServerReceiveByte((uint8 *)temp) == TRUE) && (temp == ':'))	//	判断是否进入配置模式
		{
			break;
		}
	}
	ServerSendString("12345678\r\n");		//	输入密码进入配置模式
	OSTimeDly(OS_TICKS_PER_SEC);
	//	发送F4 退出配置模式
	ServerSendByte(0x1b);
	ServerSendByte(0x4f);
	ServerSendByte(0x53);
	OSTimeDly(OS_TICKS_PER_SEC / 20);
}

//	用于检测GDTU 配置是否正确，若不正确，重新配置
void DetectedAndConfigGdtu(void)
{
	uint8 temp[32], i, flag = 0;
	PowerOffGdtu();
	OSTimeDly(OS_TICKS_PER_SEC);
	PowerOnGdtu();
	// 	一直发'U' 初始化GDTU
	//	一直发''' 进入AT 命令模式
	Uart0SendByte('\'', 2);		//	发送
	while (1)
	{
		if ((ServerReceiveByte(temp) == TRUE) && (temp[0] == '*'))	//	判断是否进入AT 命令模式
		{
			break;
		}
	}
	OSTimeDly(OS_TICKS_PER_SEC);
	memset(temp, 0, 32);
	ServerSendString("AT*DtuInfo\r\n");
	i = 0;
	while (1)
	{
		if (ServerReceiveByte((uint8 *)(temp + i)) == TRUE)
		{
			if ((temp[i] == '\n') && (temp[i - 1] == '\r'))
			{
				temp[i - 1] = 0;
				if (strstr((char *)temp, "Protocol") != NULL)
				{
					flag |= 0x01;
				}
				else if (strstr((char *)temp, "Max_DscNum") != NULL)
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
	memset(temp, 0, 32);
	ServerSendString("AT*DscInfo:0\r\n");
	i = 0;
	while (1)
	{
		if (ServerReceiveByte((uint8 *)(temp + i)) == TRUE)
		{
			if ((temp[i] == '\n') && (temp[i - 1] == '\r'))
			{
				if (strstr((char *)temp, "IP") != NULL)
				{
					flag |= 0x04;
				}
				else if (strstr((char *)temp, "DSC Port") != NULL)
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




#endif






