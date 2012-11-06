#ifndef _EEPROM_H
#define _EEPROM_H

#define CAT1025_ADDR				0xA0
#define FM24V10_0_ADDR				0xA0
#define FM24V10_1_ADDR				0xA4
#define FM24V10_2_ADDR				0xA8
#define FM24V10_3_ADDR				0xAC


#define MAX_RIDE_NUMBER_OF_PEOPLE	5			//	每笔交易，到达的站点不能大于的个数

typedef	unsigned char IO_ADDR_T;				//	外部存储器地址类型定义


typedef struct
{
	IO_ADDR_T io_addr;
	uint32 memery_size;
	uint32 memery_start_addr;
	uint32 virtual_start_addr;
} _Memery_s;



typedef struct
{
	uint8 data[31];
	uint8 crc;
} _MemeryData_s;


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


/*
typedef struct {
	
} _log_to_pad_s;
*/


typedef struct {
	uint16 start;
	uint16 end;
	uint16 in;
	uint16 out;
	uint16 size;
} _mem_manager_s;


typedef struct
{
	uint32 realpay_amount;
	uint32 needpay_amount;
	uint16 coin_dis_amount;
	uint16 note_1_dis_amount;
	uint16 note_2_dis_amount;
	uint16 trade_num;
} _trade_amount_s;

typedef struct
{
	uint16 in;
	uint16 out;
	uint16 record_num;
} _trade_record_q_s;

/*typedef struct
{
	_trade_mess_u tm;
	_trade_mess_u *next;
} _trade_record_s;
*/

extern _trade_amount_s trade_amount;

extern uint32 ReadExternMemery(void *data, uint32 addr, uint32 len);
extern uint32 WriteExternMemery(void *data, uint32 addr, uint32 len);
extern void InitConfigData(void);
extern void TaskDataStore(void * pdata);



#endif
