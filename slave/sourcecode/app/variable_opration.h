#ifndef _VARIABLE_OPRATION_H
#define _VARIABLE_OPRATION_H




typedef struct {
	uint16 change_flag			:1;		//	1-�Ѹ��ģ�0-δ���ģ��޸�֮ǰ�жϣ���ͬ���㣬��֮��һ
	uint16 write_flag			:1;		//	1-����д��0-û��д����̬����
	uint16 read_flag			:1;		//	1-���ڶ���0-û�ж�����̬����
	uint16 read_allow			:2;		//	2-��������1-������0-��ֹ������̬����
	uint16 write_allow			:2;		//	2-��������1-д����0-��ֹд����̬����
	uint16 read_num				:9;		//	���޸ĺ���𣬸����ݱ���ȡ�Ĵ���
} _variable_flag_s;

typedef struct {
	_variable_flag_s vflag;
	void *data;
} _variable_s;



#define	V_CHANGE			(1<<0)
#define	V_WRITE				(1<<1)
#define	V_READ				(1<<2)
#define	V_READ_ALLOW(c)		(c<<3)
#define	V_WRITE_ALLOW(c)	(c<<5)

#define CONDITION			2
#define ALLOWABLE			1
#define PROHIBIT			0


extern _variable_s * CreateVariable(void *data);
extern uint8 StartReadVariable(_variable_s *var,void *data);	//	��ʼ��
extern uint8 StartWriteVariable(_variable_s *var,void *data);	//	��ʼд
extern void StopReadVariable(_variable_s *var);				//	ֹͣ��
extern void StopWriteVariable(_variable_s *var);				//	ֹͣд


#define writevariable(a,b)      (a=b)





#endif

