#include "includes.h"

const sintable[] = {
	0x80,0x83,0x86,0x89,0x8c,0x8f,0x92,0x95,0x98,0x9c,0x9f,0xa2,0xa5,0xa8,0xab,0xae,
	0xb0,0xb3,0xb6,0xb9,0xbc,0xbf,0xc1,0xc4,0xc7,0xc9,0xcc,0xce,0xd1,0xd3,0xd5,0xd8,
	0xda,0xdc,0xde,0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xed,0xef,0xf0,0xf2,0xf3,0xf4,
	0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfc,0xfd,0xfe,0xfe,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xfe,0xfd,0xfc,0xfc,0xfb,0xfa,0xf9,0xf8,0xf7,
	0xf6,0xf5,0xf3,0xf2,0xf0,0xef,0xed,0xec,0xea,0xe8,0xe6,0xe4,0xe3,0xe1,0xde,0xdc,
	0xda,0xd8,0xd6,0xd3,0xd1,0xce,0xcc,0xc9,0xc7,0xc4,0xc1,0xbf,0xbc,0xb9,0xb6,0xb4,
	0xb1,0xae,0xab,0xa8,0xa5,0xa2,0x9f,0x9c,0x99,0x96,0x92,0x8f,0x8c,0x89,0x86,0x83,
	0x80,0x7d,0x79,0x76,0x73,0x70,0x6d,0x6a,0x67,0x64,0x61,0x5e,0x5b,0x58,0x55,0x52,
	0x4f,0x4c,0x49,0x46,0x43,0x41,0x3e,0x3b,0x39,0x36,0x33,0x31,0x2e,0x2c,0x2a,0x27,
	0x25,0x23,0x21,0x1f,0x1d,0x1b,0x19,0x17,0x15,0x14,0x12,0x10,0x0f,0x0d,0x0c,0x0b,
	0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x03,0x02,0x01,0x01,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x05,0x06,0x07,0x08,
	0x09,0x0a,0x0c,0x0d,0x0e,0x10,0x12,0x13,0x15,0x17,0x18,0x1a,0x1c,0x1e,0x20,0x23,
	0x25,0x27,0x29,0x2c,0x2e,0x30,0x33,0x35,0x38,0x3b,0x3d,0x40,0x43,0x46,0x48,0x4b,
	0x4e,0x51,0x54,0x57,0x5a,0x5d,0x60,0x63,0x66,0x69,0x6c,0x6f,0x73,0x76,0x79,0x7c
};






uint8 testitem;
void TaskTest(void *pdata) {

	uint8 run_flag=1,i;
	char textdata[]="欢迎使用语音天下ANY6288";
	char rec_data[100];
	uint32 test_result = 0;;
	pdata = pdata;
	
	testitem = 1;
	while (1) {
		while (run_flag) {
switch (testitem)
{
	case 1:
			RequestUart0(PRINTER_UART0,0);
			memset(rec_data,0,sizeof(rec_data));
			Uart0SendString(textdata,1);
			for (i = 0; i < sizeof(rec_data); i++)
			{
				if (Uart0RecByte((uint8 *)&rec_data[i],1,OS_TICKS_PER_SEC/20) == FALSE)
				{
					break;
				}
				
			}
			if (strncmp(textdata,rec_data,sizeof(textdata)) == 0)
			{
				test_result |= (1<<1);
			}
			FreeUart0();
			testitem++;
	break;
	case 2:
			RequestUart0(COINMACHINE_UART0,0);
			memset(rec_data,0,sizeof(rec_data));
			Uart0SendString(textdata,1);
			for (i = 0; i < sizeof(rec_data); i++)
			{
				if (Uart0RecByte((uint8 *)&rec_data[i],1,OS_TICKS_PER_SEC/20) == FALSE)
				{
					break;
				}
				
			}
			if (strncmp(textdata,rec_data,sizeof(textdata)) == 0)
			{
				test_result |= (1<<2);
			}
			FreeUart0();
			testitem++;
	break;
	case 3:
			RequestUart0(NOTE_MACHINE_UART0,0);
			memset(rec_data,0,sizeof(rec_data));
			Uart0SendString(textdata,1);
			for (i = 0; i < sizeof(rec_data); i++)
			{
				if (Uart0RecByte((uint8 *)&rec_data[i],1,OS_TICKS_PER_SEC/20) == FALSE)
				{
					break;
				}
				
			}
			if (strncmp(textdata,rec_data,sizeof(textdata)) == 0)
			{
				test_result |= (1<<3);
			}
			FreeUart0();
			testitem++;
	break;
	case 4:
			RequestUart0(UART0_INIT,0);
			memset(rec_data,0,sizeof(rec_data));
			Uart0SendString(textdata,1);
			for (i = 0; i < sizeof(rec_data); i++)
			{
				if (Uart0RecByte((uint8 *)&rec_data[i],1,OS_TICKS_PER_SEC/20) == FALSE)
				{
					break;
				}
				
			}
			if (strncmp(textdata,rec_data,sizeof(textdata)) == 0)
			{
				test_result |= (1<<4);
			}
			FreeUart0();
			testitem++;
	break;
	case 5:
			RequestUart1(SPEAKER_UART1,0);
			memset(rec_data,0,sizeof(rec_data));
			Uart1SendString(textdata,1);
			for (i = 0; i < sizeof(rec_data); i++)
			{
				if (Uart1RecByte((uint8 *)&rec_data[i],1,OS_TICKS_PER_SEC/20) == FALSE)
				{
					break;
				}
				
			}
			if (strncmp(textdata,rec_data,sizeof(textdata)) == 0)
			{
				test_result |= (1<<5);
			}
			FreeUart1();
			testitem++;
	break;
	case 6:	
			RequestUart1(IC_MACHINE_UART1,0);
			memset(rec_data,0,sizeof(rec_data));
			Uart1SendString(textdata,1);
			for (i = 0; i < sizeof(rec_data); i++)
			{
				if (Uart1RecByte((uint8 *)&rec_data[i],1,OS_TICKS_PER_SEC/20) == FALSE)
				{
					break;
				}
				
			}
			if (strncmp(textdata,rec_data,sizeof(textdata)) == 0)
			{
				test_result |= (1<<0);
			}
			FreeUart1();
			testitem++;
	break;
	case 7:
			RequestUart1(GPS_UART1,6);
			memset(rec_data,0,sizeof(rec_data));
			Uart1SendString(textdata,1);
			for (i = 0; i < sizeof(rec_data); i++)
			{
				if (Uart1RecByte((uint8 *)&rec_data[i],1,OS_TICKS_PER_SEC/20) == FALSE)
				{
					break;
				}
				
			}
			if (strncmp(textdata,rec_data,sizeof(textdata)) == 0)
			{
				test_result |= (1<<7);
			}
			FreeUart1();
			testitem++;
	break;
	case 8:
			RequestUart1(UART1_INIT,0);
			memset(rec_data,0,sizeof(rec_data));
			Uart1SendString(textdata,1);
			for (i = 0; i < sizeof(rec_data); i++)
			{
				if (Uart1RecByte((uint8 *)&rec_data[i],1,OS_TICKS_PER_SEC/20) == FALSE)
				{
					break;
				}
				
			}
			if (strncmp(textdata,rec_data,sizeof(textdata)) == 0)
			{
				test_result |= (1<<8);
			}
			FreeUart1();
			testitem = 1;

	case 9:
		//	IO 口测试
		IO0CLR |= POWER_CONTROL_GPRS;			
		IO0CLR |= POWER_CONTROL_RESERVED;		
		IO0CLR |= POWER_CONTROL_PRINT_MACHINE;
		IO0CLR |= POWER_CONTROL_IC_MACHINE;	
		IO0CLR |= POWER_CONTROL_COIN_MACHINE;	
		IO0CLR |= POWER_CONTROL_GPS;			
		IO0CLR |= POWER_CONTROL_NOTE_MACHINE;	
		IO0CLR |= SYS_STATE_LEN;
		IO0SET |= POWER_CONTROL_GPRS;			
		IO0SET |= POWER_CONTROL_RESERVED;		
		IO0SET |= POWER_CONTROL_PRINT_MACHINE;
		IO0SET |= POWER_CONTROL_IC_MACHINE;	
		IO0SET |= POWER_CONTROL_COIN_MACHINE;	
		IO0SET |= POWER_CONTROL_GPS;			
		IO0SET |= POWER_CONTROL_NOTE_MACHINE;	
		IO0SET |= SYS_STATE_LEN;
		break;

	case 10:
		RequestUart1(PRINTER_UART0,0);
		Uart1SendByte(0x55,0);
		FreeUart1();
		break;

	case 11:
		RequestUart0(PRINTER_UART0,0);
		Uart0SendByte(0x55,0);
		FreeUart0();
		break;
		
	default:
		break;
	}
			
			//RequestUart0(SYN6288_CHNNEL);
			//testsyn6228(text);
			//run_flag = 0;
			//OSSemPost(Uart0_Channel_Sem);
			//OSTimeDly(OS_TICKS_PER_SEC*5);
		}
		OSTimeDly(20);
	}
}




