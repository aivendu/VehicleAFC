#ifndef _TRADE_H
#define _TRADE_H

#include "sys_config.h"

#define	TRADE_DATA_START_ADDR			(256 + 256)
#define TRADE_DATA_SIZE					(0x30000 - 256)

typedef struct
{
	uint16	year;				//	��ǰ��¼���ڵ�����
	uint8	month;				//	��ǰ��¼���ڵ��·�
	uint8	day;				//	��ǰ��¼���ڵ�����
	uint32	last_day_addr;		//	ǰһ�����ݵ�ַ
	uint32	in;					//	��һ�����ݴ洢λ��
	uint32	out;				//	�ϴ����ݵ�λ��
	uint32	realpay_amount;		//	����ǰ��ʵ�ս����ܶ�
	uint32	needpay_amount;		//	����ǰ��Ӫ�ս����ܶ�
	uint16	coin_dis_amount;	//	����ǰ��Ӳ��������ܸ���
	uint16	note_1_dis_amount;	//	����ǰ��ֽ��Ǯ��һ������ܸ���
	uint16	note_2_dis_amount;	//	����ǰ��ֽ��Ǯ���������ܸ���
	uint16	trade_num;			//	����ǰ�ڽ��׵��ܱ���
	uint16	people_amount;		//	����ǰ�ڵĿ�����
	uint16	unused;
} _trade_manage_data_s;


extern _trade_manage_data_s	trade_manage_data_temp;
extern _trade_data_to_server_s trade_data_temp;
extern uint32	current_trade_index;

extern void TaskTrade(void *pdata);


#endif

