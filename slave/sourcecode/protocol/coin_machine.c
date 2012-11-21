#include "includes.h"


#define SNEDNUM	 0

#define	DataSendToCoin(a,b,c)				Uart0SendBytes(a,b,c)				//	coin	发送数据接口
#define	DataRecFromCoin(a,b,c)				Uart0RecByte(a,b,c)					//	coin	接收数据接口
#define	RequestHardResource()				RequestUart(COINMACHINE_UART0,0)	//	coin 发送请求硬件资源
#define	FreeHardResource()					FreeUart(COINMACHINE_UART0)							//	coin 发送请求硬件资源
#define OpenCoinPower()						(IO0SET |= POWER_CONTROL_COIN_MACHINE)
#define CloseCoinPower()					(IO0CLR |= POWER_CONTROL_COIN_MACHINE)

#define UartnumSr()						((uartnum_Sr<32)?(uartnum_Sr+1):0)

_coin_machine_cmd_s coin_machine_cmd;
OS_EVENT	*pHopBox;

	// UART0数据接收缓冲区
//volatile uint8 rcv_new_Sr;	// 接收新数据标志
volatile uint8 uartnum_Sr;
uint8    sr5CheckData = 0;
uint8    hopperR1State = 0;
uint8    hopperR2State = 0;
uint8    hopperR1State2 = 0;
uint8    hopperR2State2 = 0;

//uint8    lackCoin = 1;

uint8    hopperChannge = 0;
volatile uint8	stopDispense = 0;
uint16	clearCoinNum =0;
volatile float coinnum = 0;
volatile uint8 rcv_buf_Sr[32];


volatile uint8 coinStatecount = 0;

volatile uint32 RequestDisAccout = 0;
void UART0_Sr5i_Ini(void);

const uint8 sr5PollStr[5]={0x02,0x0,0x01,0xFE,0xFF};

const uint8 sr5Selfcheck[5]={0x02,0x0,0x01,0xE8,0x15};

//uint8 modifyCoinstyle[7]={0x02,0x02,0x1,0xE7,0XFF,0XFF,0X16};

uint8 modifyCoinstyle[7]={0x02,0x02,0x1,0xE7,0X02,0X00,0X12};

//uint8 modifyCoinstyle[7]={0x02,0x02,0x1,0xE7,0X01,0X00,0X13};

const uint8 colseCoinRec[7]={0x02,0x02,0x01,0xE7,0x00,0x00,0x14};

const uint8 recCoinPoll[5]={0x02,0x00,0x01,0xE5,0x18};

const uint8 readCoinState[5]={0x02,0x00,0x01,0xE6,0x17};

//uint8 modifyPath[7]={0x02,0x02,0x01,0xD2,0x02,0x04,0x23};
uint8 modifyPath[7]={0x02,0x02,0x01,0xD2,0x02,0x01,0x26};
//const uint8 modifyPath[7]={}


const uint8 hopperPoll[5]={0x04,0x00,0x01,0xFE,0xFD};
const uint8 hopperEnable[6]={0x04,0x01,0x01,0xA4,0xA5,0xB1};
const uint8 hopperRequestStatus[5]={0x04,0x00,0x01,0xA6,0x55};
const uint8 hopperRequestKey[5]={0x04,0x00,0x01,0xA0,0x5B};
uint8 hopperDispense[14]={0x04,0x09,0x01,0xA7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4B};
const uint8 hopperDispenseAccount[5]={0x04,0x00,0x01,0xA8,0x53};
const uint8 hopperEmergencyStop[5]={0x04,0x00,0x01,0xAC,0x4F};
const uint8 hopperReset[5]={0x04,0x00,0x01,0x01,0xFA};

const uint8 hopperTest[5]={0x04,0x00,0x01,0xA3,0x58};//////////test

const uint8 hopperQuerySensor[5]={0x04,0x00,0x01,0xD9,0x22};

const uint8 hopperPoll_bak[5]={0x03,0x00,0x01,0xFE,0xFE};
const uint8 hopperEnable_bak[6]={0x03,0x01,0x01,0xA4,0xA5,0xB2};
const uint8 hopperRequestStatus_bak[5]={0x03,0x00,0x01,0xA6,0x56};
const uint8 hopperRequestKey_bak[5]={0x03,0x00,0x01,0xA0,0x5C};
uint8 hopperDispense_bak[14]={0x03,0x09,0x01,0xA7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4C};
const uint8 hopperDispenseAccount_bak[5]={0x03,0x00,0x01,0xA8,0x54};
const uint8 hopperEmergencyStop_bak[5]={0x03,0x00,0x01,0xAC,0x50};
const uint8 hopperReset_bak[5]={0x03,0x00,0x01,0x01,0xFB};

const uint8 hopperTest_bak[5]={0x03,0x00,0x01,0xA3,0x59};/////////test

const uint8 hopperQuerySensor_bak[5]={0x03,0x00,0x01,0xD9,0x23};


/*
*********************************************************************************************************
** 函数名称 ：IRQ_UART0()
** 函数功能 ：串口0接收中断服务程序
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************
*/

void sendCommand(uint8 flag){
	//
//	switchchannel(0);
	
	//OSSemPend(Uart0_Channel_Sem, 0, &errno);  //取得资源
	
	switch(flag){
		case POLL:
			DataSendToCoin((uint8 *)sr5PollStr,5,2);
			break;
		case SELFCHECK:
			DataSendToCoin((uint8 *)sr5Selfcheck,5,2);
			break;		
		case MODIFYCOIN:
			DataSendToCoin((uint8 *)modifyCoinstyle,7,2);
			break;		
		case CLOSECOIN:
			DataSendToCoin((uint8 *)colseCoinRec,7,2);
			break;		
		case READSTATE:
			DataSendToCoin((uint8 *)readCoinState,5,2);
		case RECOIN:
			DataSendToCoin((uint8 *)recCoinPoll,5,2);
			break;		
		case MODIFYPATH:
			DataSendToCoin((uint8 *)modifyPath,7,2);
			break;	
		case HOPPERPOLL:
			if(hopperChannge == 0){
				DataSendToCoin((uint8 *)hopperPoll,5,2);
			}else{
				DataSendToCoin((uint8 *)hopperPoll_bak,5,2);
			}
			break;
		case DISPENSECOIN:
			if(hopperChannge == 0){
				DataSendToCoin((uint8 *)hopperDispense,14,2);
			}else{
				DataSendToCoin((uint8 *)hopperDispense_bak,14,2);
			}
			break;
		case REQUESTDISACCOUNT:
			if(hopperChannge == 0){
			DataSendToCoin((uint8 *)hopperDispenseAccount,5,2);
			}else{
			DataSendToCoin((uint8 *)hopperDispenseAccount_bak,5,2);			
			}
			break;
		case QUERYSENSOR:
			if(hopperChannge == 0){
				DataSendToCoin((uint8 *)hopperQuerySensor,5,2);
			}else{
				DataSendToCoin((uint8 *)hopperQuerySensor_bak,5,2);			
			}
			break;
		case EMERGENCYSTOP:
			if(hopperChannge == 0){
				DataSendToCoin((uint8 *)hopperEmergencyStop,5,2);
			}else{
				DataSendToCoin((uint8 *)hopperEmergencyStop_bak,5,2);			
			}
			break;
		case HOPPERRESET:
			if(hopperChannge == 0){
				DataSendToCoin((uint8 *)hopperReset,5,2);
			}else{
				DataSendToCoin((uint8 *)hopperReset_bak,5,2);			
			}
			break;
		case HOPPERTEST:
			if(hopperChannge == 0){
				DataSendToCoin((uint8 *)hopperTest,5,2);
			}else{
				DataSendToCoin((uint8 *)hopperTest_bak,5,2);			
			}
			break;
		case HOPPEREBALE:
			if(hopperChannge == 0){
				DataSendToCoin((uint8 *)hopperEnable,6,2);
			}else{
				DataSendToCoin((uint8 *)hopperEnable_bak,6,2);			
			}
			break;	
		case REQUESTSTATUS:
			if(hopperChannge == 0){
				DataSendToCoin((uint8 *)hopperRequestStatus,5,2);
			}else{
				DataSendToCoin((uint8 *)hopperRequestStatus_bak,5,2);			
			}
			break;	
		case REQUESTKEY:
			if(hopperChannge == 0){
			DataSendToCoin((uint8 *)hopperRequestKey,5,2);
			}else{
			DataSendToCoin((uint8 *)hopperRequestKey_bak,5,2);			
			}
			
			break;
	}
	//OSSemPost(Uart0_Channel_Sem);
}
uint8 strLen(uint8 *buf){
	int8 a = 0;
	for(a=99;a>=0;a--){
		if(buf[a]!=0){
			return a+1;
		}
	}
	return 0;
}

uint8 checkSum(uint8 a,uint8 e,uint8 *string)
{
	uint8 sum = 0;
	uint8 i=a;
	if(uartnum_Sr < e){
		return FALSE;
	}
	while(i<e)
	{
		sum += string[i++];
	}
	
	if(string[e]==(uint8)(0 - sum))
	{
		return TRUE;
	}else{
		return FALSE;
	}
}
#define COUNTNUM    21

int32 calSr5Method(uint8 flag){
	//uint8 Ri = 0;
	volatile uint8 i=0;
	uint8 tmp[32];
	//uint8 num;
	uint8 contemp = 0;//,errno;
	volatile uint8 temp[16];
#if 1
	memset((char *)rcv_buf_Sr,0,sizeof(rcv_buf_Sr));
	uartnum_Sr = 0;
	while (DataRecFromCoin((uint8 *)&rcv_buf_Sr[uartnum_Sr],1,10) == TRUE) {
		uartnum_Sr = UartnumSr();
	}
#endif
	//uartnum_Sr = Ri;
	memset(tmp,0,32);
	memset((char *)temp,0,32);	
	if(flag == POLL){
		while((i++)<COUNTNUM){
			if(uartnum_Sr >=9&&rcv_buf_Sr[7]==0x02){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;
				return TRUE;
			}
			OSTimeDly(2);
			/*
			while(QueueRead(&rcv_buf_Sr[uartnum_Sr],UT0DataA) == QUEUE_OK)
			{
				uartnum_Sr = UartnumSr();
			}*/	
			if( i%10 == SNEDNUM){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;			
				sendCommand(flag);
				OSTimeDly(2);
				while (DataRecFromCoin((uint8 *)&rcv_buf_Sr[uartnum_Sr],0,0) == TRUE) {
					uartnum_Sr = UartnumSr();
				}
			}			
		}
		memset((char *)rcv_buf_Sr,0,32);
		uartnum_Sr = 0;
		return FALSE;
	}
	if(flag == SELFCHECK){
		
	}	
	if(flag == READSTATE){
		while((i++)<COUNTNUM){
			if(strLen((uint8 *)rcv_buf_Sr)>=11&&rcv_buf_Sr[9]==0x02&&rcv_buf_Sr[11]==0xBC){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;
				return TRUE;
			}
			OSTimeDly(2);
			/*
			while(QueueRead(&rcv_buf_Sr[uartnum_Sr],UT0DataA) == QUEUE_OK)
			{
				uartnum_Sr = UartnumSr();
			}*/	
			if( i%10 == SNEDNUM){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;			
				sendCommand(flag);
				OSTimeDly(2);
				while (DataRecFromCoin((uint8 *)&rcv_buf_Sr[uartnum_Sr],0,0) == TRUE) {
					uartnum_Sr = UartnumSr();
				}
			}			
		}
		memset((char *)rcv_buf_Sr,0,32);
		uartnum_Sr = 0;		
		return FALSE;
	}	
	
	
	if(flag == MODIFYCOIN){
		while((i++)<COUNTNUM){
			if(strLen((uint8 *)rcv_buf_Sr)>=11&&rcv_buf_Sr[9]==0x02&&rcv_buf_Sr[10]==0x00){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;
				return TRUE;
			}
			OSTimeDly(2);
			if( i%10 == SNEDNUM){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;			
				sendCommand(flag);
				OSTimeDly(2);
				while (DataRecFromCoin((uint8 *)&rcv_buf_Sr[uartnum_Sr],0,0) == TRUE) {
					uartnum_Sr = UartnumSr();
				}
			}			
		}
		memset((char *)rcv_buf_Sr,0,32);
		uartnum_Sr = 0;		
		return FALSE;
	}	
	if(flag == RECOIN){
		
	}
	if(flag == CLOSECOIN){
		while((i++)<COUNTNUM){
			if(strLen((uint8 *)rcv_buf_Sr)>=11&&rcv_buf_Sr[9]==0x02){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;
				return TRUE;
			}
			OSTimeDly(2);
			if( i%10 == SNEDNUM){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;			
				sendCommand(flag);
				OSTimeDly(2);
				while (DataRecFromCoin((uint8 *)&rcv_buf_Sr[uartnum_Sr],0,0) == TRUE) {
					uartnum_Sr = UartnumSr();
				}
			}			
		}
		memset((char *)rcv_buf_Sr,0,32);
		uartnum_Sr = 0;		
		return FALSE;		
	}
	if(flag == HOPPERPOLL){
		while((i++)<COUNTNUM){
			if(uartnum_Sr>=9&&(rcv_buf_Sr[7]==0x04||rcv_buf_Sr[7]==0x03)){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;
				return TRUE;
			}
			OSTimeDly(2);
			/*
			while(QueueRead(&rcv_buf_Sr[uartnum_Sr],UT0DataA) == QUEUE_OK)
			{
				uartnum_Sr = UartnumSr();
			}*/	
			if( i%10 == SNEDNUM){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;			
				sendCommand(flag);
				OSTimeDly(2);
				while (DataRecFromCoin((uint8 *)&rcv_buf_Sr[uartnum_Sr],0,0) == TRUE) {
					uartnum_Sr = UartnumSr();
				}
			}			
		}
		memset((char *)rcv_buf_Sr,0,32);
		uartnum_Sr = 0;		
		return FALSE;		
	}
	if(flag == HOPPEREBALE)
	{
		while((i++)<COUNTNUM){
			if(uartnum_Sr >=10&&(rcv_buf_Sr[8]==0x04||rcv_buf_Sr[8]==0x03)){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;
				return TRUE;
			}
			OSTimeDly(2);
			/*
			while(QueueRead(&rcv_buf_Sr[uartnum_Sr],UT0DataA) == QUEUE_OK)
			{
				uartnum_Sr = UartnumSr();
			}*/	
			if( i%10 == SNEDNUM){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;			
				sendCommand(flag);
				OSTimeDly(2);
				while (DataRecFromCoin((uint8 *)&rcv_buf_Sr[uartnum_Sr],0,0) == TRUE) {
					uartnum_Sr = UartnumSr();
				}
			}			
		}
		memset((char *)rcv_buf_Sr,0,32);
		uartnum_Sr = 0;		
		return FALSE;		
	}
	if(flag == REQUESTSTATUS){
		while((i++)<COUNTNUM){
			if(checkSum(5,13,(uint8 *)rcv_buf_Sr)){
				memcpy((char *)temp,(char *)rcv_buf_Sr,14);
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;
				if((temp[11]+temp[12])== 0xff || temp[12]>0){
						clearCoinNum += temp[11];
				}
				contemp = temp[12];
				if(contemp > 0){
					stopDispense = contemp;
				}
				return (temp[11]+temp[12]);
			}
			OSTimeDly(2);
			/*
			while(QueueRead(&rcv_buf_Sr[uartnum_Sr],UT0DataA) == QUEUE_OK)
			{
				uartnum_Sr = UartnumSr();
			}*/	
			if( i%10 == SNEDNUM){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;			
				sendCommand(flag);
				OSTimeDly(2);
				while (DataRecFromCoin((uint8 *)&rcv_buf_Sr[uartnum_Sr],0,0) == TRUE) {
					uartnum_Sr = UartnumSr();
				}
			}			
		}
		memset((char *)rcv_buf_Sr,0,32);
		uartnum_Sr = 0;		
		return FALSE;		
	}
	if(flag == REQUESTKEY){
		while((i++)<COUNTNUM){
			if(uartnum_Sr >= 17 && checkSum(5,17,(uint8 *)rcv_buf_Sr) ){////||strLen((uint8 *)rcv_buf_Sr)==16
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;
				return TRUE;
			}
			OSTimeDly(2);
			/*
			while(QueueRead(&rcv_buf_Sr[uartnum_Sr],UT0DataA) == QUEUE_OK)
			{
				uartnum_Sr = UartnumSr();
			}*/	
			if( i%10 == SNEDNUM){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;
				sendCommand(flag);
				OSTimeDly(2);
				while (DataRecFromCoin((uint8 *)&rcv_buf_Sr[uartnum_Sr],0,0) == TRUE) {
					uartnum_Sr = UartnumSr();
				}
			}			
		}
		memset((char *)rcv_buf_Sr,0,32);
		uartnum_Sr = 0;		
		return FALSE;			
	}
	if(flag == DISPENSECOIN){
		while((i++)<COUNTNUM){
			if(uartnum_Sr >= 19 && checkSum(14,19,(uint8 *)rcv_buf_Sr)){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;
				return TRUE;
			}
			OSTimeDly(2);
			/*
			while(QueueRead(&rcv_buf_Sr[uartnum_Sr],UT0DataA) == QUEUE_OK)
			{
				uartnum_Sr = UartnumSr();
			}*/	
			if( i%10 == SNEDNUM){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;			
				//sendCommand(flag);
				OSTimeDly(2);
				while (DataRecFromCoin((uint8 *)&rcv_buf_Sr[uartnum_Sr],0,0) == TRUE) {
					uartnum_Sr = UartnumSr();
				}
			}			
		}
		memset((char *)rcv_buf_Sr,0,32);
		uartnum_Sr = 0;		
		return FALSE;			
	}
	if(flag == REQUESTDISACCOUNT){
		while((i++)<COUNTNUM){
			if(uartnum_Sr >= 12 && checkSum(5,12,(uint8 *)rcv_buf_Sr)){
				RequestDisAccout = rcv_buf_Sr[11]<<16|rcv_buf_Sr[10]<<8|rcv_buf_Sr[9];
				
				
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;
				return RequestDisAccout;
			}
			OSTimeDly(2);
			/*
			while(QueueRead(&rcv_buf_Sr[uartnum_Sr],UT0DataA) == QUEUE_OK)
			{
				uartnum_Sr = UartnumSr();
			}*/	
			if( i%10 == SNEDNUM){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;			
				sendCommand(flag);
				OSTimeDly(2);
				while (DataRecFromCoin((uint8 *)&rcv_buf_Sr[uartnum_Sr],0,0) == TRUE) {
					uartnum_Sr = UartnumSr();
				}
			}			
		}
		memset((char *)rcv_buf_Sr,0,32);
		uartnum_Sr = 0;		
		return FALSE;		
	}
	
	if(flag == EMERGENCYSTOP){
		while((i++)<COUNTNUM){
			if(uartnum_Sr >=10 &&(rcv_buf_Sr[7]==0x04||rcv_buf_Sr[7]==0x03)){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;
				return TRUE;
			}
			OSTimeDly(2);
			/*
			while(QueueRead(&rcv_buf_Sr[uartnum_Sr],UT0DataA) == QUEUE_OK)
			{
				uartnum_Sr = UartnumSr();
			}*/	
			if( i%10 == SNEDNUM){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;			
				sendCommand(flag);
				OSTimeDly(2);	
				while (DataRecFromCoin((uint8 *)&rcv_buf_Sr[uartnum_Sr],0,0) == TRUE) {
					uartnum_Sr = UartnumSr();
				}
			}			
		}
		memset((char *)rcv_buf_Sr,0,32);
		uartnum_Sr = 0;		
		return FALSE;			
	}
	
	if(flag == HOPPERRESET){
		while((i++)<COUNTNUM){
			if(uartnum_Sr>=9 &&(rcv_buf_Sr[7]==0x04||rcv_buf_Sr[7]==0x03)){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;
				return TRUE;
			}
			OSTimeDly(2);
			/*
			while(QueueRead(&rcv_buf_Sr[uartnum_Sr],UT0DataA) == QUEUE_OK)
			{
				uartnum_Sr = UartnumSr();
			}*/	
			if( i%10 == SNEDNUM){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;			
				sendCommand(flag);
				OSTimeDly(2);
				while (DataRecFromCoin((uint8 *)&rcv_buf_Sr[uartnum_Sr],0,0) == TRUE) {
					uartnum_Sr = UartnumSr();
				}
			}			
		}
		memset((char *)rcv_buf_Sr,0,32);
		uartnum_Sr = 0;		
		return FALSE;			
	}
	if(flag == QUERYSENSOR){
		while((i++)<COUNTNUM){
			if(uartnum_Sr >=10 &&(rcv_buf_Sr[7]==0x04||rcv_buf_Sr[7]==0x03)){
				if(rcv_buf_Sr[9]==0x11||rcv_buf_Sr[9]==0x31){
					//lackCoin = 0;
					//SendMoneyToPC((uint8 *)"CE",(uint8 *)"",0);
				}
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;
				return TRUE;
			}
			OSTimeDly(2);
			/*
			while(QueueRead(&rcv_buf_Sr[uartnum_Sr],UT0DataA) == QUEUE_OK)
			{
				uartnum_Sr = UartnumSr();
			}*/	
			if( i%10 == SNEDNUM){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;			
				sendCommand(flag);
				OSTimeDly(2);
				while (DataRecFromCoin((uint8 *)&rcv_buf_Sr[uartnum_Sr],0,0) == TRUE) {
					uartnum_Sr = UartnumSr();
				}
			}			
		}
		memset((char *)rcv_buf_Sr,0,32);
		uartnum_Sr = 0;		
		return FALSE;			
	}
	if(flag == HOPPERTEST){
		while((i++)<COUNTNUM){
			if(uartnum_Sr>=11 &&(rcv_buf_Sr[7]==0x04||rcv_buf_Sr[7]==0x03)){
				if(hopperChannge ==0){
					hopperR1State = rcv_buf_Sr[9];
					hopperR2State = rcv_buf_Sr[10];
				}else{
					hopperR1State2 = rcv_buf_Sr[9];
					hopperR2State2 = rcv_buf_Sr[10];
				}
				//i = uartnum_Sr;
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;
				return FALSE;
			}
			OSTimeDly(2);
			/*
			while(QueueRead(&rcv_buf_Sr[uartnum_Sr],UT0DataA) == QUEUE_OK)
			{
				uartnum_Sr = UartnumSr();
			}*/	
			if( i%10 == SNEDNUM){
				memset((char *)rcv_buf_Sr,0,32);
				uartnum_Sr = 0;			
				sendCommand(flag);
				OSTimeDly(2);
				while (DataRecFromCoin((uint8 *)&rcv_buf_Sr[uartnum_Sr],0,0) == TRUE) {
					uartnum_Sr = UartnumSr();
				}
			}			
		}
		memset((char *)rcv_buf_Sr,0,32);
		uartnum_Sr = 0;		

		return FALSE;			
	}

	return FALSE;
}

/*
*	uint8 tmp	--	协议类型
*	uint8 *n	--	找零个数
*	返回:	通过uint8 *n 返回
*/
uint8 dispenseCoin(uint8 tmp,uint8 *n)
{
		uint32 ReturnValue = 0,ReTmp = 0;
		uint8  countExe = 0;
		//uint8  confail = 0;
		//uint8  errno;
		uint8 num=*n;
		uint8  i,tmp1,overNum,temp;
		stopDispense = 0;
//		temp1 = hopperChannge;
	//	changeFlag = 0;
		tmp1 = 0;
		hopperChannge = tmp;
//change:		
		if(tmp == 0){
			hopperDispense[12]=num;
			for(i=0;i<13;i++){
				tmp1 +=hopperDispense[i];
			}
			hopperDispense[13]= ((uint8)0 - tmp1);
			hopperChannge = 0;
		
		}else{
			hopperDispense_bak[12]=num;
			for(i=0;i<13;i++){
				tmp1 +=hopperDispense_bak[i];
			}
			hopperDispense_bak[13]= ((uint8)0 - tmp1);
			hopperChannge = 1;	
		}		
		/*
		switchchannel(SR5CHANNEL);
		*/
		//OSSemPend(pU0SEMCHANNEL, 0, &errno); 
		//Uart0ChangeChannal(CHANNAL_COIN); 
		
		
		sendCommand(HOPPERTEST);

		if(!calSr5Method(HOPPERTEST))
		{
				#if 0
				if (hopperR1State != 0 || hopperR2State !=0 || hopperR1State2 != 0 || hopperR2State2 !=0 ){
					if(hopperChannge == 0){
						printERRORCODE(hopperR1State,hopperR2State);
					}else{
						printERRORCODE(hopperR1State2,hopperR2State2);
			
					}
				}
				#endif
		}

		sendCommand(REQUESTDISACCOUNT);
		
		ReturnValue  = calSr5Method(REQUESTDISACCOUNT);
		if(ReturnValue == 0)
		{
			//SendMoneyToPC((uint8 *)"HR",(uint8 *)"",0);
		//	P05_RIGHT_REPLAY_CLOSE;
		//	P04_LEFT_REPLAY_CLOSE;
			//OSSemPost(pU0SEMCHANNEL);
		//	return 1;
		}
		 //OSTimeDly(10);
		
RETRY:		
		countExe++;
		if(countExe >= 4){
				
				//SendMoneyToPC(num);
				//OSSemPost(pU0SEMCHANNEL);
				return 2;
					
		}		
		sendCommand(QUERYSENSOR);
		if(!calSr5Method(QUERYSENSOR))
		{
			//P05_RIGHT_REPLAY_CLOSE;
			//P04_LEFT_REPLAY_CLOSE;
			//printERROR("E0007");	
		
			goto RETRY;
		}
	
		sendCommand(HOPPEREBALE);
		if(!calSr5Method(HOPPEREBALE))
		{
			//P05_RIGHT_REPLAY_CLOSE;
			//P04_LEFT_REPLAY_CLOSE;

			//printERROR("E0008");				
			goto RETRY;
		}	

		
		sendCommand(REQUESTKEY);
		if(!calSr5Method(REQUESTKEY))
		{
				OSTimeDly(30);
			sendCommand(HOPPERRESET);
			OSTimeDly(10);
			calSr5Method(HOPPERRESET);
			goto   RETRY;
		
		}	
		sendCommand(DISPENSECOIN);
		calSr5Method(DISPENSECOIN);
		OSTimeDly(40);
		overNum = 0;
		while(1){
				sendCommand(REQUESTSTATUS);
				temp = calSr5Method(REQUESTSTATUS);
				if(temp == num){
						OSTimeDly(10);
						sendCommand(EMERGENCYSTOP);
						calSr5Method(EMERGENCYSTOP);
						OSTimeDly(2);
						sendCommand(REQUESTDISACCOUNT);
						ReTmp = calSr5Method(REQUESTDISACCOUNT);
						if(ReTmp == 0)
						{
							//SendMoneyToPC((uint8 *)"HC",(uint8 *)"",ReturnValue);	
							//P05_RIGHT_REPLAY_OPEN;
							//P04_LEFT_REPLAY_OPEN;
							break;					
						}		
						ReturnValue  = ReTmp - ReturnValue;
						ReTmp = (num - ReturnValue);						
						//SendMoneyToPC((uint8 *)"NC",(uint8 *)"",ReTmp);
						//SendMoneyToPC(ReTmp);
						//OSTimeDly(10);//修改
					break;
				}
				OSTimeDly(40);
				overNum++;
				if(overNum  > 15){
					

					sendCommand(EMERGENCYSTOP);
					calSr5Method(EMERGENCYSTOP);
					OSTimeDly(40);
					sendCommand(REQUESTDISACCOUNT);
					ReTmp = calSr5Method(REQUESTDISACCOUNT);
					if(ReTmp == 0)
					{
					
							//SendMoneyToPC((uint8 *)"HC",(uint8 *)"",ReturnValue);		
							//P05_RIGHT_REPLAY_OPEN;
							//P04_LEFT_REPLAY_OPEN;
							break;					
										
					}
					
					ReturnValue  = ReTmp - ReturnValue;
					ReTmp = (num - ReturnValue);
					//SendMoneyToPC((uint8 *)"NC",(uint8 *)"",ReTmp);
					//SendMoneyToPC(ReTmp);
					break;
					
				}
		}
		if (ReTmp) {
			*n=ReTmp;
			return 3;
		}
		else {
			*n=0;
			return 0;
		}
		//OSSemPost(pU0SEMCHANNEL);
} 


/*
*********************************************************************************************************
** 						任务1－Task1()
**      				Hopper找零功能
*********************************************************************************************************
*/
void  TaskHopperExe(void *pdata)
{
	//uint8   err=1;
	uint8	temp;
	
    pdata = pdata;                            	/* 避免编译警告 */
	
	OpenCoinPower();			//	给硬币机上电

	device_control.sys_device.coin_machine_state = COIN_MACHINE_NO_INIT;

	//CloseCoinPower();
#if 0
	while (1) {			//	测试串口
		OSTimeDly(2);
		while (err) {
			OSTimeDly(2);
			if (err == 1) {
				RequestUart(change_num_temp,0);
				if (Uart0RecByte(&temp,1,1) == FALSE);
				else	Uart0SendByte(temp,0);
				FreeUart(COINMACHINE_UART0);
			}
			else if (err == 2) {
				RequestUart(change_num_temp,0);
				if (Uart1RecByte(&temp,1,1)== FALSE);
				else	Uart1SendByte(temp,0);
				FreeUart(COINMACHINE_UART0);
			}
		}
	}
#endif
	
	OSTimeDly(OS_TICKS_PER_SEC*2);
	device_control.sys_device.coin_machine_state = COIN_MACHINE_NORMAL;
	coin_machine_cmd.coin.exe_flag = CMD_WITE;
	coin_machine_cmd.changenum = 0;
    for (;;)																		//(1)
    {
    	OSTimeDly(1);					//	每200ms 查询一次
    	//coin_machine_cmd.coin.exe_st = CMD_NO_EXE;
    	if (coin_machine_cmd.coin.exe_st == CMD_NO_EXE) {
	    	temp = coin_machine_cmd.changenum;
	    	//temp = 1;
			coin_machine_cmd.coin.exe_st = CMD_RUNNING;
			device_control.sys_device.coin_machine_state = COIN_MACHINE_CHANGE_RUNNING;
			RequestHardResource();
			DISABLESPI0();
			dispenseCoin(1,&temp);
			ENABLESPI0();
			FreeHardResource();
			//device_control.trade.cr.coin_reject = temp;
			device_control.trade.cr.coin_dis = coin_machine_cmd.changenum - temp;
			if (temp == 0) {
				device_control.sys_device.coin_machine_state = COIN_MACHINE_NORMAL;
			}
			else
			{
				device_control.sys_device.coin_machine_state = COIN_MACHINE_NO_COIN;
			}
			coin_machine_cmd.coin.exe_st = CMD_EXE_END;
		}
    }
}


