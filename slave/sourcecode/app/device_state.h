#ifndef _DEVICE_STATE_H
#define _DEVICE_STATE_H


/*	�û���¼״̬
0--�޿���û�п�������û�е�¼����
1--�п�δ��¼���п������豸������δ��¼�����Ե�¼����
2--�û������󣨵�¼��֤ʱ���û��������ڣ���
3--������󣨵�¼��֤ʱ���������û�����ƥ�䣩��
4--�Ϸ��û�����¼�ɹ�����
5--�Ƿ��û����п����ܵ�¼ʱ����Ϊ�Ƿ��û�����
6--�����û������û�Ϊ�������û�����
*/
#define USER_NO_CARD						0
#define USER_HAVE_CARD_NO_LOGIN				1
#define USER_LOGIN_NAME_ERR					2
#define USER_LOGIN_PASSWORD_ERR				3
#define USER_VALIDATED						4
#define USER_ILLEGAL						5
#define USER_SUPERUSER						6

/*
PADͨѶ״̬
0--δ������
1--������������
2--���ڹ��ϣ�
4--�豸�ϵ磻
*/
#define PAD_COMMUNICATION_NOT_CONNECT		0
#define PAD_COMMUNICATION_ON_LINE			1
#define PAD_COMMUNICATION_UART_ERROR		2
#define PAD_COMMUNICATION_DEVICE_OUTAGE		3

/*
��server ͨѶ״̬
0--δ������
1--������������
2--���ڹ��ϣ�
4--�豸�ϵ磻
*/
#define SERVER_COMMUNICATION_NOT_CONNECT		0
#define SERVER_COMMUNICATION_ON_LINE			1
#define SERVER_COMMUNICATION_UART_ERROR			2
#define SERVER_COMMUNICATION_DEVICE_OUTAGE		3

/*
IC ������״̬
0--����δ��ʼ����
1--�����ѳ�ʼ��״̬���޿�����
2--�豸�ϵ磻
3--����״̬���п�ʶ�𿨣���
4--�쳣״̬���в���ʶ�𿨣���
5-�豸�쳣
*/
#define IC_MACHINE_NO_INIT						0
#define IC_MACHINE_NO_CARD						1
#define IC_MACHINE_OUTAGE						2
#define IC_MACHINE_HAVE_CARD					3
#define IC_MACHINE_CARD_NOT_IDENTIFY			4
#define IC_MACHINE_ABNORMAL						5

/*
��ӡ��״̬
0--����δ��ʼ����
1--�����ѳ�ʼ��״̬��
2--�豸�ϵ磻
3--��ֽ��
4--��ֽ��
5--���ڴ�ӡ��
6--�豸�쳣
*/
#define PRINT_MACHINE_NO_INIT					0
#define PRINT_MACHINE_NORMAL					1
#define PRINT_MACHINE_OUTAGE					2
#define PRINT_MACHINE_PAPER_JAM					3
#define PRINT_MACHINE_NO_PAPER					4
#define PRINT_MACHINE_PRINT_RUNNING				5
#define PRINT_MACHINE_ABNORMAL					6

/*
ֽ�һ�״̬
0--����δ��ʼ����
1--�����ѳ�ʼ��״̬��
2--�豸�ϵ磻
3--����״̬��
4--��һǮ���ޱң�
5--��һǮ����٣�
6--�ڶ�Ǯ���ޱң�
7--�ڶ�Ǯ����٣�
8-�����ұң�
9-ͨ���쳣��
10-�豸�쳣
*/
#define NOTE_MACHINE_NO_INIT					0
#define NOTE_MACHINE_NORMAL						1
#define NOTE_MACHINE_OUTAGE						2
#define NOTE_MACHINE_NOTE_JAM					3
#define NOTE_MACHINE_CASHBOX1_NO_MONEY			4
#define NOTE_MACHINE_CASHBOX1_LITTLE			5
#define NOTE_MACHINE_CASHBOX2_NO_MONEY			6
#define NOTE_MACHINE_CASHBOX2_LITTLE			7
#define NOTE_MACHINE_CHANGE_RUNNING				8
#define NOTE_MACHINE_COMMUNICATION_ABNORMAL		9
#define NOTE_MACHINE_ABNORMAL					10
#define NOTE_MACHINE_CHANGE_ABNORMAL			11

/*
Ӳ�һ�״̬
0--����δ��ʼ����
1--�����ѳ�ʼ��״̬��
2--�豸�ϵ磻
3--����״̬��
4--�ޱ�״̬��
5-�����ұң�
*/
#define COIN_MACHINE_NO_INIT					0
#define COIN_MACHINE_NORMAL						1
#define COIN_MACHINE_OUTAGE						2
#define COIN_MACHINE_JAM						3
#define COIN_MACHINE_NO_COIN					4
#define COIN_MACHINE_CHANGE_RUNNING				5

/*
GPS ����״̬
0--����״̬��
1--�ϵ�״̬��
2--���ڹ��ϣ�
3--δ��λ��
*/
#define GPS_MODE_NO_POSITION					0
#define GPS_MODE_OUTAGE							1
#define GPS_MODE_UART_ERROR						2
#define GPS_MODE_NORMAL							3

/*
����״̬
0--����δ��ʼ����
1--����״̬��
*/


#endif

