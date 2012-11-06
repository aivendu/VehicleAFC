#include "includes.h"

_trade_mess_whole_s curr_trade_data;


void TaskTrade(void * pdata) {
	uint8 err;
	pdata = pdata;

	if (SetExeCommand(&coinchange_exe,EXE_RUNNING,&err) == TRUE) {
		
	}
}

