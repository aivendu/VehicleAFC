#include "includes.h"

#define FM24V10_ADDR(addr,c)				(c)?(addr|0x02):(addr&~0x02)


static _mem_eeprom_u mem_temp;
static _trade_mess_s tm_temp;
static _ride_mess_u mess_temp[MAX_RIDE_NUMBER_OF_PEOPLE];

_mem_manager_s pad_log_mem,server_comm_mem;

const _sys_config_u sys_config_ram = {};
_sys_config_u sys_config_rom;

_trade_amount_s trade_amount;


/**************************************************************************
*	函数原型:	uint8 ReadExternMemery(void *data, uint32 addr, uint32 len)
*	函数描述:	读取外部存储器中的数据。此处的地址是虚拟地址，
*					虚拟地址是一个连续的地址，从0x00000000开始。
*	函数参数:	void *data	--	读取的数据存放的地址
*					uint32 addr	--	读取数据的地址，为虚拟地址
*					uint32 len	--	读取数据的长度
*	函数返回:	读取的数据长度，0xFFFFFFFF为读取失败
*
***************************************************************************/
uint32 ReadExternMemery(void *data, uint32 addr, uint32 len)
{
	uint8 chip_addr;
	uint8 * rdat;
	int16 rnbyte;
	addr = 0x00;
	//I2c0ReadBytes()
}

uint32 WriteExternMemery(void *data, uint32 addr, uint32 len)
{
	return 0;
}





void WriteMem(uint32 st_addr,uint8 addr_len,const uint8 *data,uint8 data_len)
{
	//I2c0WriteBytes(CAT1025_ADDR,addr_len,wnbyte); 
}




void InitConfigData (void) {
	uint8 temp_8,bcc,temp_1;
	uint8 addr[2];
	uint8 data_temp[16];
	uint16 temp_16;
	for (temp_8=0; temp_8<MAX_RIDE_NUMBER_OF_PEOPLE;temp_8++) {
		//mess_temp[temp_8].next = NULL;
	}
	addr[0] = 0x00;
	addr[1] = 0x00;
	I2c0WriteReadBytes(FM24V10_1_ADDR,addr,2,(uint8 *)&trade_amount,sizeof(_trade_amount_s));
	//I2c0ReadBytes(CAT1025_ADDR,uint8 * rdat,int16 rnbyte);
	/*for (temp_1=0;temp_1<((sizeof(_sys_config_s))/16);temp_1++) {
		addr[0] = 0;
		I2c0WriteReadBytes(CAT1025_ADDR,addr,1,data_temp,16);
		bcc = 0;
		for (temp_8=0; temp_8<15; temp_8++) {
			bcc ^= data_temp[temp_8];
		}
		if (bcc == data_temp[15]) {
			memcpy(sys_config_ram.sc_b,data_temp,15);
		}
		else {
			memcpy(sys_config_ram.sc_b,sys_config_rom.sc_b,15);
		}
	}*/
	
}


void TaskDataStore(void *pdata) {

	uint8 data_buffer[18],temp;
	uint16 changenoteamount0,changenoteamount1,changecoinamount;
	uint16 tradeamount;
	uint16 realpayamount;

	pdata = pdata;
	InitConfigData();
	while(1)
	{
	#if 0
		memset(data_buffer,0,sizeof(data_buffer));
		I2c0WriteReadBytes(FM24V10_1_ADDR,data_buffer,2,data_buffer,16);
		OSTimeDly(2);
		sprintf((char *)data_buffer,"  1234567890");
		data_buffer[0] = 0x00;
		data_buffer[1] = 0x00;
		I2c0WriteBytes(FM24V10_1_ADDR,data_buffer,10);
	#endif
		if (sys_state.ss.st_cmd.se.store_trade_data.exe_st == EXE_WRITED)
		{
			data_buffer[0] = 0x00;
			data_buffer[1] = 0x00;
			I2c0WriteReadBytes(FM24V10_1_ADDR,data_buffer,2,(uint8 *)&trade_amount,sizeof(_trade_amount_s));
			//memcpy(trade_amount,data_buffer,sizeof(_trade_amount_s));
			changenoteamount0 = (data_buffer[0] << 8) + data_buffer[1];
			changenoteamount1 = (data_buffer[2] << 8) + data_buffer[3];
			changecoinamount = (data_buffer[4] << 8) + data_buffer[5];
			realpayamount = (data_buffer[6] << 8) + data_buffer[7];
			tradeamount = (data_buffer[8] << 8) + data_buffer[9];
			trade_amount.note_1_dis_amount += device_control.trade.cr.cass1_dis;
			trade_amount.note_2_dis_amount += device_control.trade.cr.cass2_dis;
			trade_amount.coin_dis_amount += device_control.trade.cr.coin_dis;
			trade_amount.realpay_amount += device_control.trade.tm.realpay;
			trade_amount.needpay_amount += device_control.trade.tm.needpay;
			trade_amount.trade_num ++;
			data_buffer[0] = 0x00;
			data_buffer[1] = 0x00;
			memcpy(&data_buffer[2],(uint8 *)&trade_amount,sizeof(_trade_amount_s));
			I2c0WriteBytes(FM24V10_1_ADDR,data_buffer,2+sizeof(_trade_amount_s));
			sys_state.ss.st_cmd.se.store_trade_data.exe_st = EXE_WAIT;
			memset(promptmess,0,sizeof(promptmess));
			sprintf(promptmess,"实找%d元: 1元 %d个; 5元 %d张; 10元 %d张",
				(device_control.trade.cr.coin_dis+device_control.trade.cr.cass1_dis*5+device_control.trade.cr.cass2_dis*10),
				device_control.trade.cr.coin_dis,device_control.trade.cr.cass1_dis,device_control.trade.cr.cass2_dis);
			temp = 0x33;
			DisplayMessage(&temp);
		}
		else if (sys_state.ss.st_cmd.se.printamount.exe_st == EXE_RUN_END)
		{
			memset(&trade_amount,0,sizeof(_trade_amount_s));
			data_buffer[0] = 0x00;
			data_buffer[1] = 0x00;
			memcpy(&data_buffer[2],(uint8 *)&trade_amount,sizeof(_trade_amount_s));
			I2c0WriteBytes(FM24V10_1_ADDR,data_buffer,2+sizeof(_trade_amount_s));
			sys_state.ss.st_cmd.se.printamount.exe_st = EXE_WAIT;
		}
		else
		{
			OSTimeDly(2);
		}
	}
}



