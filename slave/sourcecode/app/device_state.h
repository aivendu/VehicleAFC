#ifndef _DEVICE_STATE_H
#define _DEVICE_STATE_H


/*	用户登录状态
0--无卡（没有卡插入且没有登录）；
1--有卡未登录（有卡插入设备，但是未登录，可以登录）；
2--用户名错误（登录验证时，用户名不存在）；
3--密码错误（登录验证时，密码与用户名不匹配）；
4--合法用户（登录成功）；
5--非法用户（有卡无密登录时，卡为非法用户）；
6--配置用户（该用户为可配置用户）；
*/
#define USER_NO_CARD						0
#define USER_HAVE_CARD_NO_LOGIN				1
#define USER_LOGIN_NAME_ERR					2
#define USER_LOGIN_PASSWORD_ERR				3
#define USER_VALIDATED						4
#define USER_ILLEGAL						5
#define USER_SUPERUSER						6

/*
PAD通讯状态
0--未联机；
1--正常已联机；
2--串口故障；
4--设备断电；
*/
#define PAD_COMMUNICATION_NOT_CONNECT		0
#define PAD_COMMUNICATION_ON_LINE			1
#define PAD_COMMUNICATION_UART_ERROR		2
#define PAD_COMMUNICATION_DEVICE_OUTAGE		3

/*
与server 通讯状态
0--未联机；
1--正常已联机；
2--串口故障；
4--设备断电；
*/
#define SERVER_COMMUNICATION_NOT_CONNECT		0
#define SERVER_COMMUNICATION_ON_LINE			1
#define SERVER_COMMUNICATION_UART_ERROR			2
#define SERVER_COMMUNICATION_DEVICE_OUTAGE		3

/*
IC 读卡器状态
0--正常未初始化；
1--正常已初始化状态（无卡）；
2--设备断电；
3--正常状态（有可识别卡）；
4--异常状态（有不可识别卡）；
5-设备异常
*/
#define IC_MACHINE_NO_INIT						0
#define IC_MACHINE_NO_CARD						1
#define IC_MACHINE_OUTAGE						2
#define IC_MACHINE_HAVE_CARD					3
#define IC_MACHINE_CARD_NOT_IDENTIFY			4
#define IC_MACHINE_ABNORMAL						5

/*
打印机状态
0--正常未初始化；
1--正常已初始化状态；
2--设备断电；
3--卡纸；
4--无纸；
5--正在打印；
6--设备异常
*/
#define PRINT_MACHINE_NO_INIT					0
#define PRINT_MACHINE_NORMAL					1
#define PRINT_MACHINE_OUTAGE					2
#define PRINT_MACHINE_PAPER_JAM					3
#define PRINT_MACHINE_NO_PAPER					4
#define PRINT_MACHINE_PRINT_RUNNING				5
#define PRINT_MACHINE_ABNORMAL					6

/*
纸币机状态
0--正常未初始化；
1--正常已初始化状态；
2--设备断电；
3--卡币状态；
4--第一钱箱无币；
5--第一钱箱币少；
6--第二钱箱无币；
7--第二钱箱币少；
8-正在找币；
9-通信异常；
10-设备异常
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
硬币机状态
0--正常未初始化；
1--正常已初始化状态；
2--设备断电；
3--卡币状态；
4--无币状态；
5-正在找币；
*/
#define COIN_MACHINE_NO_INIT					0
#define COIN_MACHINE_NORMAL						1
#define COIN_MACHINE_OUTAGE						2
#define COIN_MACHINE_JAM						3
#define COIN_MACHINE_NO_COIN					4
#define COIN_MACHINE_CHANGE_RUNNING				5

/*
GPS 设置状态
0--正常状态；
1--断电状态；
2--串口故障；
3--未定位；
*/
#define GPS_MODE_NO_POSITION					0
#define GPS_MODE_OUTAGE							1
#define GPS_MODE_UART_ERROR						2
#define GPS_MODE_NORMAL							3

/*
板子状态
0--正常未初始化；
1--正常状态；
*/


#endif

