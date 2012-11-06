#include "includes.h"


void TaskTest(void *pdata) {

	uint8 run_flag=1;
	//char text[]="欢迎使用语音天下ANY6288";
	
	char test[]={"www.deferotech.com"},temp;
	
	pdata = pdata;
	
	while (1) {
		//OutSine();
		//OutSquareWave();
		while (run_flag) {
			//RequestUart0(SYN6288_CHNNEL,1);
			//OSSemPost(Uart0_Channel_Sem);
			//OSTimeDly(OS_TICKS_PER_SEC*5);
			Uart1RecByte((uint8 *)&temp,1,2);
			Uart1SendString(test,0);
			OSTimeDly(10);
			Uart1SendByte(temp,0);
		}
		OSTimeDly(20);
	}
}




