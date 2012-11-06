#ifndef _COMM_H
#define _COMM_H


typedef struct {
	uint8 change_flag			:1;		//	1-�Ѹ��ģ�0-δ���ģ��޸�֮ǰ�жϣ���ͬ���㣬��֮��һ
	uint8 write_flag			:1;		//	1-��д����0-��ʼ��״̬
	uint8 read_flag				:1;		//	1-�Ѷ������ڶ���ʱ����1 ���ڷ����ı�ʱ���㡣
	uint8 read_allow			:2;		//	2-��������1-������0-��ֹ����
	uint8 write_allow			:2;		//	2-��������1-д����0-��ֹд��
	uint8 invalid_flag			:1;
} _variable_flag_s;

#define	V_CHANGE			(1<<0)
#define	V_WRITE				(1<<1)
#define	V_READ				(1<<2)
#define	V_READ_ALLOW(c)		(c<<3)
#define	V_WRITE_ALLOW(c)	(c<<5)

#define CONDITION			2
#define ALLOWABLE			1
#define PROHIBIT			0

extern uint32 delay10us(uint32 m);
extern unsigned int CRCByte(unsigned int crc_data, unsigned char b);
extern uint32 TimeSec(uint16 yearnow,uint8 monthnow, uint8 daynow, uint8 hournow, uint8 minnow ,uint8 secnow);
extern void GetDateTimeFromSecond(uint32 lSec, uint8 *tTime);

#endif
