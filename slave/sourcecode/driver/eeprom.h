#ifndef _EEPROM_H
#define _EEPROM_H

#include "comm.h"

#define CAT1025_ADDR				0xA0
#define FM24V10_0_ADDR				0xA0
#define FM24V10_1_ADDR				0xA4
#define FM24V10_2_ADDR				0xA8
#define FM24V10_3_ADDR				0xAC


#define MAX_RIDE_NUMBER_OF_PEOPLE	5			//	每笔交易，到达的站点不能大于的个数

typedef struct {
	uint32 login_mod				:2;
	uint32 unused					:30;
} _sys_config_flag_s;



typedef struct {
	_variable_flag_s vf_sc;
	_sys_config_flag_s scf;
	_uart_config_s uc_rj45;
	_uart_config_s uc_gprs;
	_uart_config_s uc_gps;
	_uart_config_s uc_bill;
	_uart_config_s uc_coin;
	_uart_config_s uc_print;
	_uart_config_s uc_voice;
	_uart_config_s uc_icmachine;
	_uart_config_s unused1;
	_uart_config_s unused2;
	uint8 max_ride_people;
	char license_plate[9];
} _sys_config_s;

typedef union {
	uint8 sc_b[sizeof(_sys_config_s)];
	_sys_config_s sc;
} _sys_config_u;


typedef struct {
	uint8 data[14];
	uint8 flag;
	uint8 crc;
} _mem_eeprom_s;

typedef union {
	uint8 mem_b[sizeof(_mem_eeprom_s)];
	_mem_eeprom_s mem;
} _mem_eeprom_u;


typedef struct {
	uint16 start;
	uint16 end;
	uint16 in;
	uint16 out;
	uint16 size;
} _mem_manager_s;



extern _sys_config_u sys_config_ram;
extern const _sys_config_u sys_config_rom;


extern void InitConfigData(void);





#endif
