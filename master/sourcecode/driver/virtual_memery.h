#ifndef _VIRTUAL_MEMERY_H
#define _VIRTUAL_MEMERY_H

#define EXTERN_CHIP_WITH_IO_NUM				7

#define CAT1025_ADDR				0xA0
#define FM24V10_1_0_ADDR			0xA4
#define FM24V10_1_1_ADDR			0xA5
#define FM24V10_2_0_ADDR			0xA8
#define FM24V10_2_1_ADDR			0xA9
#define FM24V10_3_0_ADDR			0xAC
#define FM24V10_3_1_ADDR			0xAD

#define CAT1025_SIZE				256
#define FM24V10_1_0_SIZE			65536
#define FM24V10_1_1_SIZE			65536
#define FM24V10_2_0_SIZE			65536
#define FM24V10_2_1_SIZE			65536
#define FM24V10_3_0_SIZE			65536
#define FM24V10_3_1_SIZE			65536

#define CAT1025_VIRTUAL_ADDR				0
#define FM24V10_1_0_VIRTUAL_ADDR			(CAT1025_VIRTUAL_ADDR + CAT1025_SIZE)
#define FM24V10_1_1_VIRTUAL_ADDR			(FM24V10_1_0_VIRTUAL_ADDR + FM24V10_1_0_SIZE)
#define FM24V10_2_0_VIRTUAL_ADDR			(FM24V10_1_1_VIRTUAL_ADDR + FM24V10_1_1_SIZE)
#define FM24V10_2_1_VIRTUAL_ADDR			(FM24V10_2_0_VIRTUAL_ADDR + FM24V10_2_0_SIZE)
#define FM24V10_3_0_VIRTUAL_ADDR			(FM24V10_2_1_VIRTUAL_ADDR + FM24V10_2_1_SIZE)
#define FM24V10_3_1_VIRTUAL_ADDR			(FM24V10_3_0_VIRTUAL_ADDR + FM24V10_3_0_SIZE)




#define EXTERN_MEMERY_ADDR_OVERFLOW			1





#define MAX_RIDE_NUMBER_OF_PEOPLE	5			//	每笔交易，到达的站点不能大于的个数

typedef	unsigned char IO_ADDR_T;				//	外部存储器地址类型定义


typedef struct
{
	IO_ADDR_T io_addr;
	uint8  memery_addr_len;
	uint32 memery_size;
	uint32 memery_start_addr;
	uint32 virtual_start_addr;
} _Memery_chip_manage_s;



typedef struct
{
	uint8 data[31];
	uint8 crc;
} _MemeryData_s;

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

//extern _trade_amount_s trade_amount;

extern uint16 ReadExternMemery(void *data, uint32 addr, uint32 len);
extern uint16 WriteExternMemery(void *data, uint32 addr, uint32 len);
extern void InitConfigData(void);



#endif
