#include "includes.h"

_device_control_s device_control;	//	从芯片命令数据池，主芯片可以通过读写该数据控制相关设备
_line_mess_s curr_line;


void ChipCommInit(void) {
	
}

char speak[]={"欢迎使用无锡德飞科技的车载AFC"};

/*
*	原形:	void TaskChipComm(void *pdata)
*	描述:	这是一个处理芯片间通信的进程，用于处理芯片间通信的数据接收和发送
*
*	说明:	由于使用模拟通信方式，为保证响应速度，需要定义为最高优先级
*/
void TaskChipComm(void *pdata) {

	//uint8 err,bcc,temp_8;
	//uint8 rec_temp,*rec_p ;
	//uint16 op_addr,op_len,comm_num;
	//uint8 index;
	//uint8 chip_comm_state;
	//_chip_comm_form_s ccf_temp;
	//_ride_mess_s *node_temp;
	
	pdata = pdata;
	ENABLESPI0();
	while (1)			//	初始化
	{
		OSTimeDly(1);
		//	时间同步
		device_control.time.year = YEAR;
		device_control.time.month = MONTH;
		device_control.time.day = DOM;
		device_control.time.hour = HOUR;
		device_control.time.min = MIN;
		device_control.time.sec = SEC;
		if (spi0_tick > OS_TICKS_PER_SEC)
		{
#if (SPI0_MODE == 0)
			S0PCR  =(0 << 2) |				//	每帧数据8bit
					(0 << 3) |				// CPHA = 0, 数据在SCK 的第一个时钟沿采样
		 			(1 << 4) |				// CPOL = 1, SCK 为低有效
		 			(0 << 5) |				// MSTR = 0, SPI 处于从模式
		 			(1 << 6) |				// LSBF = 1, SPI 数据传输LSB (位0)在先
		 			(1 << 7);				// SPIE = 1, SPI 中断被使能
			S0PSR |= 0x00;
			S0PDR = 0x00;
			VICIntEnable = 1 << 9;
			spi0_tick = 0;
#endif
		}
		if (memcmp(GetConfigVersion(),"cv",2) == 0)
		{
			//	 配置项更新成功
			//OSTaskResume(TaskTestPrio);
			OSTaskResume(TaskCrt188Prio);
			OSTaskResume(TaskNoteMachinePrio);
			OSTaskResume(TaskPTRExePrio);
			OSTaskResume(TaskHopperExePrio);
			OSTaskResume(TaskGPSPrio);
				
			break;
		}
	}
	
	while(1) {
		OSTimeDly(1);
		device_control.time.year = YEAR;
		device_control.time.month = MONTH;
		device_control.time.day = DOM;
		device_control.time.hour = HOUR;
		device_control.time.min = MIN;
		device_control.time.sec = SEC;
		if (spi0_tick > OS_TICKS_PER_SEC)
		{
#if (SPI0_MODE == 0)
			S0PCR  =(0 << 2) |				//	每帧数据8bit
					(0 << 3) |				// CPHA = 0, 数据在SCK 的第一个时钟沿采样
		 			(1 << 4) |				// CPOL = 1, SCK 为低有效
		 			(0 << 5) |				// MSTR = 0, SPI 处于从模式
		 			(1 << 6) |				// LSBF = 1, SPI 数据传输LSB (位0)在先
		 			(1 << 7);				// SPIE = 1, SPI 中断被使能
			S0PSR |= 0x00;
			S0PDR = 0x00;
			VICIntEnable = 1 << 9;
			spi0_tick = 0;
#endif
		}
		if (strlen(GetConfigVersion()) > 0)
		{
			continue;
		}
		if (device_control.cmd.changemoney.exe_st == CMD_NO_EXE)	//	找零命令处理
		{
			device_control.cmd.changemoney.exe_st = CMD_RUNNING;
			memset(&device_control.trade.cr,0,sizeof(_change_result_s));	//	清零返回值
			if (device_control.trade.ts.trade_exe == 0) 
			{
				if (device_control.trade.tm.changemoney >= GetCashbox2Value()) {		//	判断是否需要找纸币
					note_machine_cmd.cass1 = (device_control.trade.tm.changemoney % GetCashbox3Value()) / GetCashbox2Value();
					note_machine_cmd.cass2 = device_control.trade.tm.changemoney / GetCashbox3Value();
					note_machine_cmd.note.exe_st = CMD_NO_EXE;		//	执行纸币找零
					device_control.trade.ts.change_note = 1;
					OSTimeDly(OS_TICKS_PER_SEC*2);					//	等待找纸币执行
				}
				else {
				}
				if (device_control.trade.tm.changemoney % GetCashbox2Value()) {		//	判断是否需要找硬币
					coin_machine_cmd.changenum = device_control.trade.tm.changemoney % GetCashbox2Value();
					coin_machine_cmd.coin.exe_st = CMD_NO_EXE;		//	执行硬币找零
					device_control.trade.ts.change_coin = 1;
				}
				else {
				}
				device_control.trade.ts.trade_exe = 1;
			}
			
		}
		else if (device_control.cmd.changemoney.exe_st == CMD_RUNNING)
		{
			if ((note_machine_cmd.note.exe_st == CMD_EXE_END) || (note_machine_cmd.note.exe_st == CMD_WITE))
			{
				note_machine_cmd.note.exe_st = CMD_WITE;
				device_control.trade.ts.change_note = 0;
			}
			if ((coin_machine_cmd.coin.exe_st == CMD_EXE_END) || (coin_machine_cmd.coin.exe_st == CMD_WITE))
			{
				coin_machine_cmd.coin.exe_st = CMD_WITE;
				device_control.trade.ts.change_coin = 0;
			}
			if ((note_machine_cmd.note.exe_st == CMD_WITE) && (coin_machine_cmd.coin.exe_st == CMD_WITE))
			{
				device_control.cmd.changemoney.exe_st = CMD_EXE_END;
#if 0
				device_control.cmd.changemoney.exe_st = CMD_NO_EXE;
				OSTimeDly(OS_TICKS_PER_SEC*4);
#endif
			}
		}
		
		if (device_control.cmd.speak.exe_st == CMD_NO_EXE)
		{
			device_control.cmd.speak.exe_st = CMD_EXE_END;
		}
		
		if (device_control.cmd.power_down.exe_st == CMD_NO_EXE)
		{
			device_control.cmd.power_down.exe_st = CMD_EXE_END;
		}
		
		if (device_control.trade.ts.trade_exe == 0)			//	交易命令处理
		{
			if ((device_control.trade.ts.change_coin) || (device_control.trade.ts.change_note)
				|| (device_control.trade.ts.print_end))
			{
				device_control.trade.ts.trade_exe = 1;
			}
		}
		else 
		{
			if ((device_control.trade.ts.change_coin == 0) && (device_control.trade.ts.change_note == 0)
				&& (device_control.trade.ts.print_end== 0))
			{
				device_control.trade.ts.trade_exe = 0;
			}
		}
			
		
		//Syn6288Test();
		//OSTimeDly(OS_TICKS_PER_SEC*3);
	}
}


uint8 GetTradeSt(void) {
	return device_control.cmd.changemoney.exe_st;
}





