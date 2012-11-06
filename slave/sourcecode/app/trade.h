#ifndef _TRADE_H
#define _TRADE_H

typedef struct {
	uint8 trade_start_st;
	uint8 trade_end_st;
	uint8 price;
	uint8 number_of_people;
} _ride_mess_s;

typedef union {
	uint8 rm_b[sizeof(_ride_mess_s)];
	_ride_mess_s rm;
} _ride_mess_u;


typedef struct {
	uint8 year;
	uint8 month;
	uint8 day;
	uint8 hour;
	uint8 min;
	uint8 sec;
	uint16 serail_num;
	uint16 needpay;
	uint16 realpay;
	uint16 changemoney;			//	需要找零的金额
	uint16 changenum;			//  存储方式为(第三钞箱* 2000 +5元个数*100 +1元个数)
	uint8  des_num;
} _trade_mess_s;

typedef struct {
	uint8 note_errcode;
	uint8 cass1_dis;
	uint8 cass2_dis;
	uint8 cass1_reject;
	uint8 cass2_reject;
	uint8 coin_dis;
	uint8 coin_reject;
} _change_result_s;

typedef struct {
	uint8 trade_exe				:1;		//	交易进行状态，1--交易正在进行
	uint8 change_note			:1;		//	纸币机找零状态，1--正在找纸币
	uint8 change_coin			:1;		//	硬币机找零状态，1--正在找纸币
	uint8 print_end				:1;		//	打印机打印状态，1--正在打印
	uint8 unused 				:4;
} _trade_state_s;

typedef struct {
	_trade_mess_s tm;
	_ride_mess_s  rm[MAX_RIDE_NUMBER_OF_PEOPLE];
	_trade_state_s ts;
	_change_result_s cr;
} _trade_mess_whole_s;



extern _trade_mess_whole_s curr_trade_data;


#endif
