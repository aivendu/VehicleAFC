#include "includes.h"

#define FM24V10_ADDR(addr,c)				(c)?(addr|0x02):(addr&~0x02)


//static _mem_eeprom_u mem_temp;
//static _trade_mess_s tm_temp;

_mem_manager_s pad_log_mem,server_comm_mem;

_sys_config_u sys_config_ram;
const _sys_config_u sys_config_rom = {0,0};



void WriteMem(uint32 st_addr,uint8 addr_len,uint8 *data,uint8 data_len) {
	I2c0WriteBytes(CAT1025_ADDR,data,data_len); 
}




void InitConfigData (void) {
	uint8 temp_8,bcc,temp_1;
	uint8 addr[2];
	uint8 data_temp[16];
	for (temp_8=0; temp_8<MAX_RIDE_NUMBER_OF_PEOPLE;temp_8++) {
		
	}
	//I2c0ReadBytes(CAT1025_ADDR,uint8 * rdat,int16 rnbyte);
	for (temp_1=0;temp_1<((sizeof(_sys_config_s))/16);temp_1++) {
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
	}
	
}


void TaskDataStore(void *pdata) {
	
}



