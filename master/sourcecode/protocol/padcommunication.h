#ifndef _PAD_COMMUNICATION_H
#define _PAD_COMMUNICATION_H
#include "sys_state.h"


/*
typedef union {
	_exe_s shutdown;			//	ִ���µ����
	_exe_s makechange;			//	ִ������
	_exe_s print;				//	ִ�д�ӡ
	_exe_s config_match;		//	ִ������ƥ��
	_exe_s logout;				//	ִ��ע��
} _sys_exe_st_s;

typedef union {
	uint8 se_b[sizeof(_sys_exe_st_s)];
	_sys_exe_st_s se;
} _sys_exe_st_u;
*/

typedef struct {
	uint8 len;				//	�������ݵĳ���, ����"����","�����","������","����","����"
	uint8 backage_num;		//	�������ݵİ���
	uint16 err_no;			//	����PAD ����ʱ�Ĵ����
	uint8 cmd;				//	���͵�������
	uint8 arg;				//	���͵Ĳ���
	uint8 *dat;				//	���͵����ݣ��Ӳ�����һ���ֽڼ���
}_df_device_and_pad_comm;


typedef struct {
	_variable_flag_s change_flag;	//	����ı���ı��
	uint8 old_package_num;	//	��һ�����ݵİ���
	uint8 package_num;		//	�յ������ݵİ���
	uint8 comm_task_prio;	//	ͨ�ŵ�Task ���ȼ�
	uint16 err_no;		//	�յ�PAD�������ݵĴ����
	uint8 cmd;			//	�յ������ݵ�������
	uint8 arg;			//	�յ������ݵĲ���
	uint8 len;			//	���յ������ݵĳ��ȣ��Ӳ�����һ���ֽڼ���
	uint8 data[248];	//	�յ������ݣ��Ӳ�����һ���ֽڼ���
} _pad_com_task;


typedef struct {
	uint8 cmd;			//	������
	_pfunc *func;		//	���������ֵĴ�����
} _rj45_command_s;


#define PAD_ACK			0x06
#define PAD_NACK		0x15

#define RETRY_TIME					3
#define RJ45_COMM_NUMBER			11

#define RJ45_CTS					(1<<15)
#define RJ45_RTS					(1<<16)
#define RJ45_RST					(1<<17)

//	PAD ͨ�Ŵ������
/*
1.	0x0000���޴�
2.	0x0101����ͨ����
3.	0x0102����ǰ���ܽ��ף����ȵ�¼
4.	0x0103��û�н�����Ϣ����������
5.	0x0104���������㣬��������Ч
6.	0x0105���豸�쳣
7.	0x0181����Ч����
8.	0x0182������·�߱�ź�ԭ�����ͬ,����������������ᴥ��
9.	0x0183������·�߱�ź�ԭ��Ų�ͬ,����������������ᴥ��,   ��¼���潫�Ժ�һ��·�߱��Ϊ׼
10.	0x0184���Ҳ���GUID��Ӧ���ļ�
11.	0x0185��û�н��յ�GUID
*/
#define PAD_COMMUNICATION_NO_ERROR					0X0000
#define PAD_COMMUNICATION_GENERAL_ERROR				0x0101
#define PAD_COMMUNICATION_NEED_LOGIN_ERROR			0x0102
#define PAD_COMMUNICATION_NO_RIDE_MESSEGE_ERROR		0x0103
#define PAD_COMMUNICATION_IS_CHANGE_ERROR			0x0104
#define PAD_COMMUNICATION_DEVICE_ERROR				0x0105
#define PAD_COMMUNICATION_INVAILD_COMMAND			0x0181
#define PAD_COMMUNICATION_SAME_GUID					0x0182
#define PAD_COMMUNICATION_DIFFERENT_GUID			0x0183
#define PAD_COMMUNICATION_NO_FILE_WITH_GUID			0x0184
#define PAD_COMMUNICATION_NO_GUID					0x0185
#define PAD_COMMUNICATION_FILE_FORM_ERROR			0x0186


















extern uint8 old_station;
extern uint8 curr_station;
extern uint8 run_direction;
extern uint8 changesite_flag;

extern void PadCommInit(void);
extern void TaskDeviceCommand(void *pdata);
extern void TaskPADRecHandle(void *pdata);

#endif
