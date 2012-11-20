#include "includes.h"


#define	DataSendToSDM2000(a,b,c)			Uart0SendBytes(a,b,c)				//	coin	�������ݽӿ�
#define	DataRecFromSDM2000(a,b,c)			Uart0RecByte(a,b,c)					//	coin	�������ݽӿ�
#define	RequestHardResource()				RequestUart(NOTE_MACHINE_UART0,0)	//	coin ��������Ӳ����Դ
#define	FreeHardResource()					FreeUart(NOTE_MACHINE_UART0)							//	coin ��������Ӳ����Դ
#define OpenNotePower()						(IO0SET |= POWER_CONTROL_NOTE_MACHINE)
#define CloseNotePower()					(IO0CLR |= POWER_CONTROL_NOTE_MACHINE)


OS_EVENT	*pHopBox1;			//Ӧ�ò��note ��������
OS_EVENT	*note_to_bill, *bill_to_note;	//Ӧ�ò�task��ֽ�һ�ͬ�������
OS_EVENT	*bill_signal;

_note_machine_cmd_s note_machine_cmd;

static _sdm2000_cmd_s cmd_temp;

uint32 cashbox0_coin_amount=300,cashbox1_note_amount=100,cashbox2_note_amount=20;



//		�����״̬
#define IDLE				0			//	������ִ�У��ȴ�����
#define COMMAND_RETRY		1			//	�����ط�
#define WAIT_ACK			2			//	�ȴ�ACK
#define ACK_TIMEOUT			3			//	ACK��ʱ
#define ENQ_WAIT			4			//	�ȴ�ENQ��Ӧ
#define ENQ_TIMEOUT			5			//	ENQ��ʱ
#define MES_HANDLE			6			//	��Ϣ����
#define WAIT_EXE			7			//	�ȴ�����ִ��




void SDM2000SendCommand(_sdm2000_cmd_s *cmd) {
	uint8 temp_8,bcc=0;
	temp_8 = SDM2000_STX;
	DataSendToSDM2000(&temp_8,1,2);			//	������շ��ͻ���
	DataSendToSDM2000(&cmd->len,1,0);
	bcc = cmd->len;
	DataSendToSDM2000(&cmd->cmd,1,0);
	bcc ^= cmd->cmd;
	if (cmd->len > 1) {
		for (temp_8=0; temp_8<(cmd->len-1); temp_8++) {
			DataSendToSDM2000(&cmd->dat[temp_8],1,0);
			bcc ^= cmd->dat[temp_8];
		}
	}
	temp_8 = SDM2000_ETX;
	DataSendToSDM2000(&temp_8,1,0);
	bcc ^= temp_8;
	DataSendToSDM2000(&bcc,1,0);
}

uint8 GetNoteACK(uint8 * ret) {
	
	uint16 j=20;
	
	while (j--) {
		if (DataRecFromSDM2000(ret,0,0) == FALSE) {
			OSTimeDly(2);
			continue;
		}
		if ((*ret == SDM2000_ACK) || (*ret == SDM2000_NAK)) {
			return OS_NO_ERR;
		}
		else {
			OSTimeDly(2);
		}
	}
	return OS_TIMEOUT;
}


uint8 GetNoteENQ(uint8 *dst){

	uint8	*buf=dst,dat_len=0,eor=0;
	uint16 j=20;
	//memset(buf,0,50);
	
	while (j) {
		if (DataRecFromSDM2000(buf,0,0) == FALSE) {
			OSTimeDly(2);
			j--;
			continue;
		}
		if (*(uint8 *)dst != SDM2000_STX) {
			continue;			//û��ȡ��ͷ���ݣ�������ǰ����
		}
		if (dat_len >= SDM2000_COMM_DATA_LENGHT)
		{
			buf = dst;
			dat_len = 0;
		}
		if ((buf - dst) >= (dat_len + 3)) {
			if (dat_len) {
				if (*(buf - 1) != SDM2000_ETX) {
					memset(dst,0,(dat_len+5));		//���ݳ��Ȳ���ȷ
					return 1;
				}
				buf = dst+1;
				do {
					eor ^= *buf++; 		//����У���
				} while ((buf - dst) < (dat_len + 2));
				eor ^= SDM2000_ETX;
				if (eor != (*(buf+1))) {
					memset(dst,0,(dat_len+5));		// ����У�鲻��ȷ
					return 1;
				}
				memcpy(dst,(dst+2),dat_len);		// ���ػظ�������
				return 0;
			}
			else {
				dat_len = dst[1];				//	�������ݳ���
				buf ++;
			}
		}
		else {
			buf ++;		//	 ����ȡ����
			if ((buf - dst) >= SDM2000_COMM_DATA_LENGHT)
			{
				buf = dst;
				dat_len = 0;
			}
		}
	}
	*buf = 0;
	return 1;

}

/**************************************************************************************************
*
*	ԭ��:	uint8 SDM2000CommandHandle(_sdm2000_cmd_s *cmd)
*	����:	SDM2000������������ڴ��������߼�
*
*	����:	_sdm2000_cmd_s *cmd		--		��������
*	���:	ͨ��cmd ��������, �������ش�����
*
*	����ģ��:	SDM2000SendCommand
*	��Դռ��:	�ֲ�����8byte
*
*	˵��: 
*	
*	����:	���信
*	����:	2012-4-12
*
****************************************************************************************************/
uint8 SDM2000CommandHandle(_sdm2000_cmd_s *cmd) {
	uint8 err;
	uint8 current_state=IDLE;
	//uint16 lenght;
	uint8 *rec_data = (uint8 *)cmd;
	while (1) {
		switch (current_state){
			case IDLE:
				/*����״̬*/
				DISABLESPI0();
				err = CRT188_ACK;
				DataSendToSDM2000(&err,1,0);
				OSTimeDly(20);
				SDM2000SendCommand(cmd);			//	��������
				current_state = WAIT_ACK;
				OSTimeDly(10);
				ENABLESPI0();
				break;
				
			case WAIT_ACK:
				/*�ȴ�Ӧ��*/
				err = GetNoteACK(rec_data);
				if (err == OS_NO_ERR) {
					if (*rec_data == SDM2000_ACK) {
						current_state = WAIT_EXE;
						break;
					}
					else if(*rec_data == SDM2000_NAK) {
						return UART_COMMUNICATION_ERR_COMM_NACK;
					}
				}
				else {
					return UART_COMMUNICATION_ERR_COMM_TIMEOUT;
				}
				break;
				
			case WAIT_EXE:
				if (cmd->exe_time == 0xffff) {
					OSTimeDly(100);
				}
				else if(cmd->exe_time)
				{
					FreeHardResource();
					//OSTimeDly(cmd->exe_time);
					OSTimeDly(100);
					if (cmd->exe_time > 100)
						cmd->exe_time -= 100;
					else 
						cmd->exe_time = 0;
					RequestHardResource();
				}
				current_state = ENQ_WAIT;
				err = SDM2000_ENQ;
				DataSendToSDM2000(&err,1,0);		// �յ�Ӧ�𣬷���ѯ��
				break;
				
			case ENQ_WAIT:
				/*�ȴ�ѯ�ʷ���*/
				err = GetNoteENQ(rec_data);
				if (err == 1) {
					if (cmd->exe_time <= 30) {			//	ʱ��С��һ���շ�ʱ��ֱ������һ��
						err = SDM2000_ENQ;
						DataSendToSDM2000(&err,1,0);
						current_state = ENQ_TIMEOUT;
					}
					else {
						current_state = WAIT_EXE;		//	��ʱ����
					}
					break;
				}
				else {
				}
				err = SDM2000_ACK;
				DataSendToSDM2000(&err,1,0);

				//lenght = ((uint8 *)rec_data)[1];
				//lenght = ((lenght << 8) & 0xff00) + (((uint8 *)rec_data)[2]);
	
				//memcpy(cmd,rec_data,((uint8 *)rec_data)[1]);		// �������ݵ����ػ�����
				
				return SYS_NO_ERR;

			case ENQ_TIMEOUT:
				err = GetNoteENQ(rec_data);
				if (err == 1) {							//	�����Ƿ����
					return UART_COMMUNICATION_ERR_DATA_REC_TIMEOUT;
				}
				else {
				}
				err = SDM2000_ACK;
				DataSendToSDM2000(&err,1,0);

				//lenght = ((uint8 *)rec_data)[1];
				//lenght = ((lenght << 8) & 0xff00) + (((uint8 *)rec_data)[2]);
	
				//memcpy(cmd,rec_data,((uint8 *)rec_data)[1]);		// �������ݵ����ػ�����
				
				return SYS_NO_ERR;
				
			default : break;
				
		}

	}
}


uint8 Sdm2000Rest(void) {
	uint8 err;
	memset(&cmd_temp,0,sizeof(_sdm2000_cmd_s));
	cmd_temp.cmd = 0x30;
	cmd_temp.exe_time = 0xffff;
	cmd_temp.len = 1;
	err = SDM2000CommandHandle(&cmd_temp);
	if (err == SYS_NO_ERR) {
		if (((uint8 *)&cmd_temp)[0] == 0x80) {
			return SYS_NO_ERR;
		}
		else
		{
			return UART_COMMUNICATION_ERR_RETURN_DATA_ERR;
		}
	}
	return err;
}

uint8 Sdm2000ReadState(void) {
	uint8 err;
	memset(&cmd_temp,0,sizeof(_sdm2000_cmd_s));
	cmd_temp.cmd = 0x31;
	cmd_temp.exe_time = 0;
	cmd_temp.len = 1;
	err = SDM2000CommandHandle(&cmd_temp);
	return err;
}

uint8 Sdm2000Diagnostics(void) {
	uint8 err;
	memset(&cmd_temp,0,sizeof(_sdm2000_cmd_s));
	cmd_temp.cmd = 0x32;
	cmd_temp.exe_time = OS_TICKS_PER_SEC*5;
	cmd_temp.len = 1;
	err = SDM2000CommandHandle(&cmd_temp);
	return err;
}

uint8 Sdm2000LastState(void) {
	uint8 err;
	memset(&cmd_temp,0,sizeof(_sdm2000_cmd_s));
	cmd_temp.cmd = 0x34;
	cmd_temp.exe_time = 0;
	cmd_temp.len = 1;
	err = SDM2000CommandHandle(&cmd_temp);
	return err;
}


uint8 Sdm2000ConfigurationStatus() {
	return TRUE;
}

uint8 Sdm2000SetBillThickness() {
	return TRUE;
}

uint8 Sdm2000GetBillThickness() {
	return TRUE;
}

uint8 Sdm2000SetBillSize() {
	return TRUE;
}

uint8 Sdm2000GetBillSize(void) {
	return TRUE;
}

uint8 Sdm2000MultiCassetteDispense(uint8 cass1,uint8 cass2) {
	uint8 err;
	memset(&cmd_temp,0,sizeof(_sdm2000_cmd_s));
	cmd_temp.cmd = 0x3A;
	cmd_temp.exe_time = (12+cass1+cass2)*OS_TICKS_PER_SEC/2;
	cmd_temp.len = 1+8;
	cmd_temp.dat[0] = cass1;
	cmd_temp.dat[1] = cass2;
	err = SDM2000CommandHandle(&cmd_temp);
	return err;
}

uint8 Sdm2000LearnBillParameter(void) {
	return TRUE;
}

uint8 Sdm2000ReportRejectReason(void) {
	uint8 err;
	memset(&cmd_temp,0,sizeof(_sdm2000_cmd_s));
	cmd_temp.cmd = 0x43;
	cmd_temp.exe_time = 0;
	cmd_temp.len = 1;
	err = SDM2000CommandHandle(&cmd_temp);
	return err;
}

uint8 Sdm2000SensorRead(void) {
	return TRUE;
}

//	argu1: 0x30--����������� 0x31--��������
//	argu2: 0x30--��ȡ��0x31--����
//	
uint8 Sdm2000TotalCounterReadClear(uint8 argu1,uint8 argu2) {
	uint8 err;
	memset(&cmd_temp,0,sizeof(_sdm2000_cmd_s));
	cmd_temp.cmd = 0x46;
	cmd_temp.exe_time = 0;
	cmd_temp.len = 1+2;
	cmd_temp.dat[0] = argu1;
	cmd_temp.dat[1] = argu2;
	
	err = SDM2000CommandHandle(&cmd_temp);
	
	return err;
}

/*******************************************************************
*
*
*
*******************************************************************/
uint8 CassetteDispense(uint8 *const cashbox) {
	uint8 err,i;
	uint8 buf[NOTE_BOX_QUANTITY+1];
	uint8 *ret_dat;
	memset(buf,0,NOTE_BOX_QUANTITY+1);
	OSSemPend(bill_to_note,0,&err);
	if (err == OS_NO_ERR) {
		buf[0] = SDM2000_DISPENSE_BILL;
		memcpy((uint8 *)(buf+1),cashbox,NOTE_BOX_QUANTITY);
		OSMboxPost(pHopBox1,buf);				// ��Ʊ
		ret_dat = OSMboxPend(bill_signal,0,&err);
		for (i=0;i<NOTE_BOX_QUANTITY;i++) {
			cashbox[i] = (*(ret_dat+i+2)) - (*(ret_dat+i+10));
		}
		return 0;
	}
	return err;
}




/*******************************************************************
*	����ԭ��:	void NoteMachineInit(void)
*	����:  ֽ�ҳ�ʼ����������Ӧ�ò��ṩֽ�ҳ�ʼ������
*	����:  ��
*			
*	����:	��
*
********************************************************************/

void NoteMachineInit(void) {
	uint8 err;
	uint8 buf[9];
	memset(buf,0,9);
	
reinit0:
	OSSemPend(bill_to_note, 0, &err);
	if (err == OS_NO_ERR) {
		buf[0] = SDM2000_DIAGNOSITCS;
		OSMboxPost(pHopBox1,buf);				//���
		//OSSemPost(pHopBox1)
		OSMboxPend(bill_signal,0,&err);
	}
	else {
		goto reinit0;
	}
	
reinit1:	
	OSSemPend(bill_to_note, 0, &err);
	//err_temp = OSMboxPend(note_err_box, 0, &err);
	if (err == OS_NO_ERR) {
		buf[0] = SDM2000_RESET;
		OSMboxPost(pHopBox1,buf);				//��λ
		OSMboxPend(bill_signal,0,&err);
	}
	else {
		goto reinit1;
	}
}


/*******************************************************************
*	����ԭ��:	void NoteDispenseBill(uint8 *dst) 
*	����:  ���Һ�������Ӧ�ò��ṩ���ҷ���
*	����: 	dst			���ҵ�������8��
*			
*	����:	��
*
********************************************************************/

void NoteDispenseBill(uint8 *dst) {
	uint8 err;
	uint8 buf[9];
	uint8 ret_buf[48],*ret_dat;
	memset(buf,0,9);
	/*
	OSSemPend(bill_to_note,0,&err);
	if (err == OS_NO_ERR) {
		buf[0] = SDM2000_DIAGNOSITCS;
		OSMboxPost(pHopBox1,buf);				//���
		ret_dat = OSMboxPend(bill_signal,0,&err);
		//OSSemPost(pHopBox1)
	}
	*/
	OSSemPend(bill_to_note,0,&err);
	if (err == OS_NO_ERR) {
		buf[0] = SDM2000_DISPENSE_BILL;
		memcpy((buf+1),dst,NOTE_BOX_QUANTITY);
		OSMboxPost(pHopBox1,buf);				// ��Ʊ
		ret_dat = OSMboxPend(bill_signal,0,&err);
	}
	memcpy(ret_buf,ret_dat,26);
	/*
	OSSemPend(bill_to_note,0,&err);
	if (err == OS_NO_ERR) {
		buf[0] = SDM2000_LAST_STATUS;
		OSMboxPost(pHopBox1,buf);				// ��ѯ��Ʊ���
		ret_dat = OSMboxPend(bill_signal,0,&err);
		err = 0;
		
	}*/
	//sprintf((char *)ret_buf,"<type=Separate;BillNum=%02d;CoinNum=%2d>",ret_dat[22],err);
	//UART1SendMtd((uint8 *)ret_buf,strlen((char *)ret_buf));	
}

static void SetNoteMachineState(uint8 st) {
	if (device_control.sys_device.note_machine_state != st) {
		device_control.sys_device.note_machine_state = st;
	}
}

/*
*	ԭ��:	void TaskNoteMachine(void * pdata)
*	����:	����������״̬����
*	
*/
void TaskNoteMachine(void * pdata) {

	uint8 err,err_code,state=NOTE_MACHINE_NO_INIT;
	uint8 opration_num = 0;
	uint8 *rec_data = (uint8 *)&cmd_temp;
	
	pdata = pdata;
	
init:

	while (1) {
		OSTimeDly(10);			//	��ʱ50 ms
		state = device_control.sys_device.note_machine_state;
		switch (state) {
			case NOTE_MACHINE_NO_INIT:
				OSTimeDly(OS_TICKS_PER_SEC/2);		//	��ʱ500 ms
				OpenNotePower();					//	�ϵ�
				OSTimeDly(OS_TICKS_PER_SEC*2);		//	��ʱ2S		
				RequestHardResource();
				err = Sdm2000Rest();				//	��״̬
				FreeHardResource();
				if (err == SYS_NO_ERR) {			//	ͨ������
					opration_num = 0 ;
				}
				else
				{
					if ((++opration_num) > NOTE_MACHINE_RETRY_TIME)		//	����ͨ�Ų���
					{
						opration_num = 0;
						state = NOTE_MACHINE_COMMUNICATION_ABNORMAL;	//	ͨ���쳣
					}
					break;
				}
				RequestHardResource();
				err = Sdm2000Diagnostics();					//	���
				FreeHardResource();
				if (err == SYS_NO_ERR)
				{
					switch (rec_data[1]) {
						case SDM_NORMAL:
							state = NOTE_MACHINE_NORMAL;
							break;
						case SDM_EXIT_SENSOR_JAM:
						case SDM_TOP_SENSOR_JAM:
						case SDM_REJECT_SENSOR_JAM:
							state = NOTE_MACHINE_NOTE_JAM;	//	����
							break;
						case SDM_CASSETTE_1_EMPTY:
							state = NOTE_MACHINE_CASHBOX1_NO_MONEY;		//	Ǯ��һ�ޱ�
							break;
						case SDM_CASSETTE_2_EMPTY:
							state = NOTE_MACHINE_CASHBOX2_NO_MONEY;		//	Ǯ����ޱ�
							break;
						default :
							state = NOTE_MACHINE_ABNORMAL;				//	�����쳣
							break;
					}
				}
				else
				{
					if ((++opration_num) > NOTE_MACHINE_RETRY_TIME)		//	����ͨ�Ų���
					{
						opration_num = 0;
						state = NOTE_MACHINE_COMMUNICATION_ABNORMAL;	//	ͨ���쳣
					}
				}
				break;
			case NOTE_MACHINE_NORMAL:
			case NOTE_MACHINE_CASHBOX1_NO_MONEY:
			case NOTE_MACHINE_CASHBOX2_NO_MONEY:
			case NOTE_MACHINE_CASHBOX1_LITTLE:
			case NOTE_MACHINE_CASHBOX2_LITTLE:
				//if (state != NOTE_MACHINE_NO_INIT) {	//	δ��ʼ�������ܶ�״̬
					RequestHardResource();
					err = Sdm2000ReadState();		//	��״̬
					FreeHardResource();
					err_code = rec_data[1];		//	ȡ״̬
				//}
				//else {
				//	err = 0x00;				//	����״̬
				//}
				
				if (err != SYS_NO_ERR) {		//	ͨ���쳣
					if ((++opration_num) > NOTE_MACHINE_RETRY_TIME) {	//	����ͨ���쳣
						opration_num = 0;
						state = NOTE_MACHINE_NO_INIT;	//	���³�ʼ��
						CloseNotePower();				//	�ϵ�
					}
				}
				else if (err_code != SDM_NORMAL) {		//	�����쳣
					switch (err_code) {
						case SDM_EXIT_SENSOR_JAM:
						case SDM_TOP_SENSOR_JAM:
						case SDM_REJECT_SENSOR_JAM:
							state = NOTE_MACHINE_NOTE_JAM;	//	����
							break;
						case SDM_CASSETTE_1_EMPTY:
							state = NOTE_MACHINE_CASHBOX1_NO_MONEY;		//	Ǯ��һ�ޱ�
							break;
						case SDM_CASSETTE_2_EMPTY:
							state = NOTE_MACHINE_CASHBOX2_NO_MONEY;		//	Ǯ����ޱ�
							break;
						default :
							state = NOTE_MACHINE_ABNORMAL;				//	�����쳣
							break;
					}
				}
				else if (note_machine_cmd.note.exe_st == CMD_NO_EXE) {
					note_machine_cmd.note.exe_st = CMD_RUNNING;
					state = NOTE_MACHINE_CHANGE_RUNNING;				//	��ʼ����
				}
				else if (cashbox1_note_amount < MONEY_LITTLE_WARM_NUM) {
					state = NOTE_MACHINE_CASHBOX1_LITTLE;				//	Ǯ��һ����
				}
				else if (cashbox2_note_amount < MONEY_LITTLE_WARM_NUM) {
					state = NOTE_MACHINE_CASHBOX2_LITTLE;				//	Ǯ�������
				}
				break;
			case NOTE_MACHINE_OUTAGE:		//	�ϵ�
				CloseNotePower();
				break;
			case NOTE_MACHINE_NOTE_JAM:		//	�������޸��ú��������ϵ�ָ�
				RequestHardResource();
				err = Sdm2000Diagnostics();
				FreeHardResource();
				if (err == SYS_NO_ERR)
				{
					if (rec_data[1] == SDM_NORMAL) 
					{
						state = NOTE_MACHINE_NO_INIT;
					}
				}
				break;
			case NOTE_MACHINE_CHANGE_RUNNING:
				RequestHardResource();
				err = Sdm2000TotalCounterReadClear(0x31,0x31);
				if (err != SYS_NO_ERR)
				{
					state = NOTE_MACHINE_ABNORMAL;
					break;
				}
				err = Sdm2000MultiCassetteDispense(note_machine_cmd.cass1,note_machine_cmd.cass2);
				if (err == SYS_NO_ERR)
				{
					device_control.trade.cr.note_errcode = rec_data[1];
					device_control.trade.cr.cass1_dis = rec_data[10];		//	����������
					device_control.trade.cr.cass2_dis = rec_data[11];
					device_control.trade.cr.cass1_reject = rec_data[18];
					device_control.trade.cr.cass2_reject = rec_data[19];
					
					err = Sdm2000TotalCounterReadClear(0x31,0x30);
					if (err == SYS_NO_ERR)
					{
						device_control.trade.cr.cass1_dis = rec_data[5];
						device_control.trade.cr.cass2_dis = rec_data[9];
					}
					err = SYS_NO_ERR;
				}
				FreeHardResource();
				note_machine_cmd.note.exe_st = CMD_EXE_END;
				if (err == SYS_NO_ERR)
				{
					switch (rec_data[1]) {
						case SDM_NORMAL:
							state = NOTE_MACHINE_NORMAL;
							break;
						case SDM_EXIT_SENSOR_JAM:
						case SDM_TOP_SENSOR_JAM:
						case SDM_REJECT_SENSOR_JAM:
							state = NOTE_MACHINE_NOTE_JAM;	//	����
							break;
						case SDM_CASSETTE_1_EMPTY:
							state = NOTE_MACHINE_CASHBOX1_NO_MONEY;		//	Ǯ��һ�ޱ�
							break;
						case SDM_CASSETTE_2_EMPTY:
							state = NOTE_MACHINE_CASHBOX2_NO_MONEY;		//	Ǯ����ޱ�
							break;
						default :
							state = NOTE_MACHINE_ABNORMAL;				//	�����쳣
							break;
					}
				}
				else if (err == UART_COMMUNICATION_ERR_COMM_NACK)
				{
					if ((++opration_num) > NOTE_MACHINE_RETRY_TIME) {	//	����ͨ���쳣
						opration_num = 0;
						state = NOTE_MACHINE_CHANGE_ABNORMAL;	//	�����쳣
					}
				}
				else
				{
					state = NOTE_MACHINE_CHANGE_ABNORMAL;
				}
				break;
			case NOTE_MACHINE_COMMUNICATION_ABNORMAL:
				RequestHardResource();
				err = Sdm2000ReadState();				//	���
				FreeHardResource();
				if (err == SYS_NO_ERR) {				//	ͨ���޴���
					if (rec_data[1] == SDM_NORMAL)		//	�����޴���
					{
						state = NOTE_MACHINE_NORMAL;	//	��������״̬
					}
					else
					{
						state = NOTE_MACHINE_NO_INIT;	//	���¿�ʼ����
					}
				}
				break;
			case NOTE_MACHINE_CHANGE_ABNORMAL:
				RequestHardResource();
				err = Sdm2000LastState();
				FreeHardResource();
				break;
			case NOTE_MACHINE_ABNORMAL:
				RequestHardResource();
				err = Sdm2000Diagnostics();			//	���
				FreeHardResource();
				if (err == OS_NO_ERR)
				{
					if (rec_data[1] == SDM_NORMAL)	//������ָ�����
					{
						state = NOTE_MACHINE_NO_INIT;
					}
				}
				break;
			default:
				state = NOTE_MACHINE_NO_INIT;
				break;
		}
		device_control.sys_device.note_machine_state = state;
	}
	
}

