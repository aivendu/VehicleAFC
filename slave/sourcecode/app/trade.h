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
	uint16 changemoney;			//	��Ҫ����Ľ��
	uint16 changenum;			//  �洢��ʽΪ(��������* 2000 +5Ԫ����*100 +1Ԫ����)
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
	uint8 trade_exe				:1;		//	���׽���״̬��1--�������ڽ���
	uint8 change_note			:1;		//	ֽ�һ�����״̬��1--������ֽ��
	uint8 change_coin			:1;		//	Ӳ�һ�����״̬��1--������ֽ��
	uint8 print_end				:1;		//	��ӡ����ӡ״̬��1--���ڴ�ӡ
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
