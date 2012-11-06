#ifndef _COIN_MACHINE_H
#define _COIN_MACHINE_H


typedef struct {
	_exe_s coin;
	uint8 changenum;
} _coin_machine_cmd_s;


extern _coin_machine_cmd_s coin_machine_cmd;

#define POLL		0x01
#define SELFCHECK	0X02
#define MODIFYCOIN	0X03
#define CLOSECOIN	0X04
#define RECOIN		0X05
#define MODIFYPATH	0X06
#define READSTATE   0x07

#define HOPPERPOLL			0x10
#define DISPENSECOIN		0x11
#define REQUESTDISACCOUNT	0x12
#define QUERYSENSOR			0x13
#define EMERGENCYSTOP		0x14
#define HOPPERRESET			0x15
#define HOPPERTEST			0x16
#define HOPPEREBALE			0X17
#define REQUESTSTATUS		0x18
#define REQUESTKEY			0X19

extern const uint8 hopperPoll[5];
extern const uint8 hopperEnable[6];
extern const uint8 hopperRequestStatus[5];
extern const uint8 hopperRequestKey[5];
extern uint8 hopperDispense[14];
extern const uint8 hopperDispenseAccount[5];
extern const uint8 hopperEmergencyStop[5];
extern const uint8 hopperReset[5];
extern const uint8 hopperTest[5];
extern const uint8 hopperQuerySensor[5];

extern const uint8 hopperPoll_bak[5];
extern const uint8 hopperEnable_bak[6];
extern const uint8 hopperRequestStatus_bak[5];
extern const uint8 hopperRequestKey_bak[5];
extern uint8 hopperDispense_bak[14];
extern const uint8 hopperDispenseAccount_bak[5];
extern const uint8 hopperEmergencyStop_bak[5];
extern const uint8 hopperReset_bak[5];
extern const uint8 hopperTest_bak[5];
extern const uint8 hopperQuerySensor_bak[5];
extern uint8 modifyCoinstyle[7];
extern  volatile    uint8 rcv_buf_Sr[32];

extern	OS_EVENT	*pHopBox;

extern uint16	clearCoinNum ;
extern volatile uint8 uartnum_Sr;
//extern	uint8 	sr5CheckData;
extern	uint8	hopperR1State;
extern	uint8	hopperR2State;
extern	uint8	hopperR1State2;
extern	uint8	hopperR2State2;
extern	uint8	coin_surplus;
extern	uint8	coin_paid;

extern uint8    hopperChannge;

extern void  TaskHopperExe(void *pdata);


#endif
