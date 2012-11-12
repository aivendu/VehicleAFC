#ifndef _TRADE_H
#define _TRADE_H

#include "sys_config.h"

#define	TRADE_DATA_START_ADDR			(256 + 256)
#define TRADE_DATA_SIZE					(0x30000 - 256)

typedef struct
{
	uint16	year;				//	当前记录日期的年数
	uint8	month;				//	当前记录日期的月份
	uint8	day;				//	当前记录日期的日数
	uint32	last_day_addr;		//	前一天数据地址
	uint32	in;					//	下一组数据存储位置
	uint32	out;				//	上传数据的位置
	uint32	realpay_amount;		//	当日前期实收金额的总额
	uint32	needpay_amount;		//	当日前期营收金额的总额
	uint16	coin_dis_amount;	//	当日前期硬币找零的总个数
	uint16	note_1_dis_amount;	//	当日前期纸币钱箱一找零的总个数
	uint16	note_2_dis_amount;	//	当日前期纸币钱箱二找零的总个数
	uint16	trade_num;			//	当日前期交易的总笔数
	uint16	people_amount;		//	当日前期的客流量
	uint16	unused;
} _trade_manage_data_s;


extern _trade_manage_data_s	trade_manage_data_temp;
extern _trade_data_to_server_s trade_data_temp;
extern uint32	current_trade_index;

extern void TaskTrade(void *pdata);


#endif

