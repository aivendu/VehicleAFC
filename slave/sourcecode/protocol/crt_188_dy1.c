#include "includes.h"


#define IDLE				0
#define COMMAND_RETRY		1
#define WAIT_ACK			2
#define ACK_TIMEOUT			3
#define ENQ_WAIT			4
#define ENQ_TIMEOUT			5
#define MES_HANDLE			6



static OS_EVENT *crt188_sem,*crt188_cmd_mbox,*crt188_return_mbox;
uint8 card_machine_state;
_crt188_cmd_s crt188_cmd_temp;



#define Crt188SendByte(data)				(Uart1SendByte(data,0))
#define Crt188SendString(data,lenght)   	(Uart1SendBytes(data,lenght,0))
#define Crt188RecData(a,b)					(Uart1RecByte(a,1,b))
#define	RequestHardResource()				(RequestUart(IC_MACHINE_UART1,0))	//	ic_reader ��������Ӳ����Դ
#define	FreeHardResource()					(FreeUart(IC_MACHINE_UART1))							//	ic_reader ��������Ӳ����Դ
#define Crt188OpenPower()					(IO0SET |= POWER_CONTROL_IC_MACHINE)
#define Crt188ClosePower()					(IO0CLR |= POWER_CONTROL_IC_MACHINE)




void Crt188Init(void) {
	uint8 err;
	
	crt188_cmd_mbox = OSMboxCreate((void *)1);
	if (crt188_cmd_mbox == (void *)0) {
		while(1);
	}
	OSMboxPend(crt188_cmd_mbox,0,&err);

	crt188_return_mbox = OSMboxCreate((void *)1);
	if (crt188_return_mbox == (void *)0) {
		while(1);
	}
	OSMboxPend(crt188_return_mbox,0,&err);
	
	crt188_sem = OSSemCreate(1);
	if (crt188_sem == (void *)0) {
		while(1);
	}
	OSSemPend(crt188_sem,0,&err);

}

void Crt188SendCmd(void *cmd) {
	_crt188_cmd_s *cmd_temp = (_crt188_cmd_s *)cmd;
	uint8 eor=0,i;
	
	Crt188SendByte(CRT188_STX);				// ����STX
	eor ^= CRT188_STX;
	Crt188SendByte(cmd_temp->lenght_h);
	eor ^= cmd_temp->lenght_h;
	Crt188SendByte(cmd_temp->lenght_l);
	eor ^= cmd_temp->lenght_l;
	Crt188SendByte(cmd_temp->cmd);
	eor ^= cmd_temp->cmd;
	Crt188SendByte(cmd_temp->arg);
	eor ^= cmd_temp->arg;
	for (i=0;i<cmd_temp->dat_lenght;i++) {
		Crt188SendByte(cmd_temp->dat[i]);
		eor ^= (cmd_temp->dat[i]);
	}
	Crt188SendByte(CRT188_ETX);
	eor ^= CRT188_ETX;
	Crt188SendByte(eor);
}

uint8 Crt188GetACK(uint8 *buf) {
	uint8 err,j=2;
	*buf = 0;
	while (j--) {
		err = Crt188RecData(buf,10);
		if (err != TRUE) {
			continue;			//û�еõ���ȷ���ݣ����»�ȡ����
		}
		if ((*buf == CRT188_ACK) || (*buf == CRT188_NAK)) {
			return 0;
		}
		else {
			j = 1;
		}
	}
	return OS_TIMEOUT;
}

uint8 Crt188GetENQ(uint8 *buf){
	uint8 *buf_temp = buf,j=30,err,eor=0;
	uint16 dat_len=0;

	*buf_temp = 0;
	while (j--) {
		err = Crt188RecData(buf_temp,10);
		if (err != TRUE) {
			continue;			//û�еõ���ȷ���ݣ����»�ȡ����
		}
		if (*buf != CRT188_STX) {
			continue;			//û��ȡ��ͷ���ݣ�������ǰ����
		}
		j = 1;
		if (dat_len > CRT188_MAX_DATA_LENGHT)
		{
			buf_temp = buf;
			dat_len = 0;
		}
		if ((buf_temp - buf) >= (dat_len + 4)) {
			if (dat_len) {
				if (*(buf_temp - 1) != CRT188_ETX) {
					memset(buf,0,(dat_len+5));		//���ݳ��Ȳ���ȷ
					return 1;
				}
				buf_temp = buf;
				do {
					eor ^= *buf_temp++;			//����У���
				} while ((buf_temp - buf) < (dat_len + 3));
				eor ^= CRT188_ETX;
				if (eor != (*(buf_temp+1))) {
					memset(buf,0,(dat_len+5));		// ����У�鲻��ȷ
					return 1;
				}
				return 0;
			}
			else {
				dat_len = buf[1];				//  �������ݳ���
				dat_len = (dat_len << 8) + buf[2];
				buf_temp ++;
			}
		}
		else {
			buf_temp ++;		//   ����ȡ����
			if ((buf_temp - buf) >= CRT188_MAX_DATA_LENGHT)
			{
				buf_temp = buf;
				dat_len = 0;
			}
		}
	}
	*buf = 0;
	return 1;
}

uint8 Crt188CommandHandle(_crt188_cmd_s *cmd) {
	uint8 err;
	uint8 *rec_data = (uint8 *)cmd;
	uint8 current_state=IDLE;
	uint16 lenght;
	while (1) {
		switch (current_state){
			case IDLE:
				/*����״̬*/
				Crt188SendByte(CRT188_ACK);
				OSTimeDly(5);
				Crt188SendCmd(cmd);
				current_state = WAIT_ACK;
				
				break;
				
			case WAIT_ACK:
				/*�ȴ�Ӧ��*/
				err = Crt188GetACK(rec_data);
				if (err == OS_NO_ERR) {
					if (*rec_data == CRT188_ACK) {
						Crt188SendByte(CRT188_ENQ);		// �յ�Ӧ�𣬷���ѯ��
						current_state = ENQ_WAIT;
						break;
					}
					else if(*rec_data == CRT188_NAK) {
						return UART_COMMUNICATION_ERR_COMM_NACK;
					}
				}
				else {
					return UART_COMMUNICATION_ERR_COMM_TIMEOUT;
				}
				break;
			
			case ENQ_WAIT:
				/*�ȴ�ѯ�ʷ���*/
				err = Crt188GetENQ((uint8 *)cmd);
				if (err == 1) {
					Crt188SendByte(CRT188_ENQ);
					current_state = ENQ_TIMEOUT;
					break;
				}
				else {
				}
				Crt188SendByte(CRT188_ACK);

				lenght = ((uint8 *)rec_data)[1];
				lenght = ((lenght << 8) & 0xff00) + (((uint8 *)rec_data)[2]);
	
				memcpy(rec_data,&rec_data[3],lenght);		// �������ݵ����ػ�����
				
				return SYS_NO_ERR;

			case ENQ_TIMEOUT:
				err = Crt188GetENQ((uint8 *)cmd);
				if (err == 1) {
					return UART_COMMUNICATION_ERR_DATA_REC_TIMEOUT;
				}
				else {
				}
				Crt188SendByte(CRT188_ACK);

				lenght = ((uint8 *)rec_data)[1];
				lenght = ((lenght << 8) & 0xff00) + (((uint8 *)rec_data)[2]);
	
				memcpy(rec_data,&rec_data[3],lenght);		// �������ݵ����ػ�����
				
				return SYS_NO_ERR;
				
			default : break;
				
		}

	}
}

uint8 Crt188Reset(void){

	uint8 err;
	
	_crt188_cmd_s *cmd_temp=&crt188_cmd_temp;	
	uint8 *mess = (uint8 *)cmd_temp;
	
	cmd_temp->lenght_h = 0x00;
	cmd_temp->lenght_l = 0x02;
	cmd_temp->cmd = 0x30;
	cmd_temp->arg = 0x30;
	cmd_temp->dat_lenght = 0;
	//do {
	err = Crt188CommandHandle(cmd_temp);
	if (err == OS_NO_ERR) {
		if ((mess[0] == 0x30) && (mess[1] == 0x30)) {
			return err;
		}
		return UART_COMMUNICATION_ERR_RETURN_DATA_ERR;
	}
	return err;
	//} while (err == OS_TIMEOUT);
}

uint8 Crt188ReadCardType(uint8 *card_type) {
	uint8 err;
	_crt188_cmd_s *cmd_temp=&crt188_cmd_temp;
	uint8 *mess = (uint8 *)cmd_temp;

	cmd_temp->lenght_h = 0x00;
	cmd_temp->lenght_l = 0x02;	
	cmd_temp->cmd = 0x31;
	cmd_temp->arg = 0x31;
	cmd_temp->dat_lenght = 0;
	err = Crt188CommandHandle(cmd_temp);
	if(err != OS_NO_ERR) {
#if ERR_EN == 1
		
#endif
		return err;
	}
	if (mess[2] == 'N') {				//	�������쳣
		if (mess[3] == '0') {
			*card_type = CARD_TYPE_NO_CARD;
		}
		else if (mess[3] == '1') {
			*card_type = CARD_TYPE_UNKOWN_TYPE;
		}
		else if (mess[3] == '2') {
			*card_type = CARD_TYPE_POS_ERR;
		}
		else {
			*card_type = 0;
		}
		return CRT188_COMMAND_EXE_ERR;			//	��ȡ�Ŀ�������
	} 
	else if (mess[2] == '0') {
		if (mess[3] == '0') {
			*card_type = CARD_TYPE_M1;
		}
		else {
			*card_type = 0;
			return UART_COMMUNICATION_ERR_RETURN_DATA_ERR;
		}

	}
	else if (mess[2] == '1') {
		if (mess[3] == '0') {
			*card_type = CARD_TYPE_T0_CPU;
		}
		else if (mess[3] == '1') {
			*card_type = CARD_TYPE_T1_CPU;
		}
		else {
			*card_type = 0;
			return UART_COMMUNICATION_ERR_RETURN_DATA_ERR;
		}

	}
	else if (mess[2] == '2') {
		if (mess[3] == '0') {
			*card_type = CARD_TYPE_24C01;
		}
		else if (mess[3] == '1') {
			*card_type = CARD_TYPE_24C02;
		}
		else if (mess[3] == '2') {
			*card_type = CARD_TYPE_24C04;
		}
		else if (mess[3] == '3') {
			*card_type = CARD_TYPE_24C08;
		}
		else if (mess[3] == '4') {
			*card_type = CARD_TYPE_24C16;
		}
		else if (mess[3] == '5') {
			*card_type = CARD_TYPE_24C32;
		}
		else if (mess[3] == '6') {
			*card_type = CARD_TYPE_24C64;
		}
		else {
			*card_type = 0;
			return UART_COMMUNICATION_ERR_RETURN_DATA_ERR;
		}
	}
	else if (mess[2] == '3') {
		if (mess[3] == '0') {
			*card_type = CARD_TYPE_SL4442;
		}
		else if (mess[3] == '1') {
			*card_type = CARD_TYPE_SL4428;
		}
		else {
			*card_type = 0;
			return UART_COMMUNICATION_ERR_RETURN_DATA_ERR;
		}
	}
	else if (mess[2] == '4') {
		if (mess[3] == '0') {
			*card_type = CARD_TYPE_AT88S102;
		}
		else if (mess[3] == '1') {
			*card_type = CARD_TYPE_AT88S1604;
		}
		else if (mess[3] == '2') {
			*card_type = CARD_TYPE_AT45D041;
		}
		else if (mess[3] == '3') {
			*card_type = CARD_TYPE_AT88SC1608;
		}
		else {
			*card_type = 0;
			return UART_COMMUNICATION_ERR_RETURN_DATA_ERR;
		}
	}
	else {
		return UART_COMMUNICATION_ERR_RETURN_DATA_ERR;
	}
	
	return SYS_NO_ERR;
}



uint8 Crt188ReadState(void){
	uint8 err;
	_crt188_cmd_s *cmd_temp=&crt188_cmd_temp;
	uint8 *mess=(uint8 *)cmd_temp;
	//uint8 card_type;
	
	cmd_temp->lenght_h = 0x00;
	cmd_temp->lenght_l = 0x02;	
	cmd_temp->cmd = 0x31;
	cmd_temp->arg = 0x30;
	cmd_temp->dat_lenght = 0;
	err = Crt188CommandHandle(cmd_temp);
	if(err != OS_NO_ERR) {
#if ERR_EN == 1
			
#endif
		return err;
	}
	if (mess[2] == 0x4B) {		//�п�
		return SYS_NO_ERR;
	}
	else if (mess[2] == 0x4E) {		//�޿�
		return CRT188_NO_CARD;
	}
	else {
		return UART_COMMUNICATION_ERR_RETURN_DATA_ERR;
	}
/*	
	if (mess[1] == 0x4a) {
		
	}
	else {

	}
	
	if (mess[2] == 0x4a) {

	}
	else if (mess[2] == 0x4e){

	}
	else {

	}
*/	

}

uint8 Crt188SetType24CXX(uint8 type) {
	uint8 err;
	_crt188_cmd_s *cmd_temp=&crt188_cmd_temp;
	uint8 *buf = (uint8 *)cmd_temp;
	//uint8 dat_buf[2];
	//Crt188ReadCardType();
	switch (type) {
		case CARD_TYPE_24C01: cmd_temp->dat[0] = 0x30; break;
		case CARD_TYPE_24C02: cmd_temp->dat[0] = 0x31; break;
		case CARD_TYPE_24C04: cmd_temp->dat[0] = 0x32; break;
		case CARD_TYPE_24C08: cmd_temp->dat[0] = 0x33; break;
		case CARD_TYPE_24C16: cmd_temp->dat[0] = 0x34; break;
		case CARD_TYPE_24C32: cmd_temp->dat[0] = 0x35; break;
		case CARD_TYPE_24C64: cmd_temp->dat[0] = 0x36; break;
		default :cmd_temp->dat[0] = 0x30; break;
	}

	cmd_temp->lenght_h = 0x00;
	cmd_temp->lenght_l = 0x03;
	cmd_temp->cmd = 0x36;
	cmd_temp->arg = 0x30; 
	cmd_temp->dat_lenght = 1;
	
	err = Crt188CommandHandle(cmd_temp);
	if (err != SYS_NO_ERR) {
#if ERR_EN == 1

#endif
		return err;
	}
	if (((uint8 *)buf)[3] == 'N') {			//���ÿ����ɹ�
		return CRT188_COMMAND_EXE_ERR;
	}
	else if (((uint8 *)buf)[3] == 'Y') {		//���ÿ��ɹ�
		return SYS_NO_ERR;
	}
	else if (((uint8 *)buf)[3] == 'E') {		//�����޿�
		
	}
	else if (((uint8 *)buf)[3] == 'W') {		//���������������λ����
		
	}
	else {
		
	}
	return err;
	
}

uint8 Crt188Read24CXX(uint8 type, uint16 addr, uint8 len, void *buf) {
	uint8 err;
	_crt188_cmd_s *cmd_temp=&crt188_cmd_temp;
	uint8 *buf_temp = (uint8 *)cmd_temp;

	cmd_temp->lenght_h = 0x00;
	cmd_temp->lenght_l = 0x06;
	cmd_temp->cmd	= 0x36;
	cmd_temp->arg	= 0x31;
	cmd_temp->dat_lenght = 0x04;
	switch (type) {
		case CARD_TYPE_24C01: cmd_temp->dat[0] = 0x30; break;
		case CARD_TYPE_24C02: cmd_temp->dat[0] = 0x31; break;
		case CARD_TYPE_24C04: cmd_temp->dat[0] = 0x32; break;
		case CARD_TYPE_24C08: cmd_temp->dat[0] = 0x33; break;
		case CARD_TYPE_24C16: cmd_temp->dat[0] = 0x34; break;
		case CARD_TYPE_24C32: cmd_temp->dat[0] = 0x35; break;
		case CARD_TYPE_24C64: cmd_temp->dat[0] = 0x36; break;
		default :cmd_temp->dat[0] = 0x30; break;
	}
	cmd_temp->dat[1] = (uint8)addr;
	cmd_temp->dat[2] = (uint8)(addr >> 8);
	cmd_temp->dat[3] = len;
	
	err = Crt188CommandHandle(cmd_temp);
	if (err == OS_TIMEOUT) {
#if ERR_EN == 1

#endif
		return err;
	}
	if (buf_temp[3] == 'Y') {		//���ÿ��ɹ�
		memcpy(buf,&((uint8 *)buf_temp)[7],len);
		return SYS_NO_ERR;
	}
	else if (buf_temp[3] == 'N') {		//���ÿ����ɹ�
		((uint8 *)buf)[0] = 0;
	}
	else if (buf_temp[3] == 'E') {		//�����޿�
		((uint8 *)buf)[0] = 0;
	}
	else if (buf_temp[3] == 'W') {		//���������������λ����
		((uint8 *)buf)[0] = 0;
	}
	else {
		((uint8 *)buf)[0] = 0;
		return UART_COMMUNICATION_ERR_DATA_REC_TIMEOUT;
	}
	return CRT188_COMMAND_EXE_ERR;
	
}

uint8 Crt188Write24CXX(uint8 type,uint16 addr,uint8 len,void * buf) {
	uint8 err;
	_crt188_cmd_s *cmd_temp=&crt188_cmd_temp;
	uint8 *buf_temp = (uint8 *)cmd_temp;

	cmd_temp->lenght_h = 0x00;
	cmd_temp->lenght_l = 0x06+len;
	cmd_temp->cmd	= 0x36;
	cmd_temp->arg	= 0x32;
	cmd_temp->dat_lenght = 0x04+len;
	switch (type) {
		case CARD_TYPE_24C01: cmd_temp->dat[0] = 0x30; break;
		case CARD_TYPE_24C02: cmd_temp->dat[0] = 0x31; break;
		case CARD_TYPE_24C04: cmd_temp->dat[0] = 0x32; break;
		case CARD_TYPE_24C08: cmd_temp->dat[0] = 0x33; break;
		case CARD_TYPE_24C16: cmd_temp->dat[0] = 0x34; break;
		case CARD_TYPE_24C32: cmd_temp->dat[0] = 0x35; break;
		case CARD_TYPE_24C64: cmd_temp->dat[0] = 0x36; break;
		default :cmd_temp->dat[0] = 0x30; break;
	}
	cmd_temp->dat[1] = (uint8)addr;
	cmd_temp->dat[2] = (uint8)(addr >> 8);
	cmd_temp->dat[3] = len;
	memcpy(&cmd_temp->dat[4],buf,len);
	err = Crt188CommandHandle(cmd_temp);
	if (err == OS_TIMEOUT) {
#if ERR_EN == 1

#endif
		return err;
	}
	if (buf_temp[3] == 'Y') {		//д���ɹ�
		//memcpy(buf,&((uint8 *)buf_temp)[7],len);
		return SYS_NO_ERR;
	}
	else if (buf_temp[3] == 'N') {		//д�����ɹ�
		((uint8 *)buf)[0] = 0;
	}
	else if (buf_temp[3] == 'E') {		//�����޿�
		((uint8 *)buf)[0] = 0;
	}
	else if (buf_temp[3] == 'W') {		//���������������λ����
		((uint8 *)buf)[0] = 0;
	}
	else {
		((uint8 *)buf)[0] = 0;
		return UART_COMMUNICATION_ERR_DATA_REC_TIMEOUT;
	}
	return CRT188_COMMAND_EXE_ERR;
	
}

uint8 Crt188ICPower(uint8 flag) {
	uint8 err;
	_crt188_cmd_s *cmd_temp=&crt188_cmd_temp;
	uint8 *buf_temp = (uint8 *)cmd_temp;
	
	cmd_temp->lenght_h = 0x00;
	cmd_temp->lenght_l = 0x02;
	cmd_temp->cmd = 0x33;
	if (flag) {
		cmd_temp->arg = 0x30;			//	�ϵ�
	}
	else {
		cmd_temp->arg = 0x31;			//	�µ�
	}
	cmd_temp->dat_lenght = 0;
	
	err = Crt188CommandHandle(cmd_temp);
	if (err != SYS_NO_ERR){
#if ERR_EN == 1
		
#endif
		return err;
	}
	if (buf_temp[2] == 'Y') {
		return SYS_NO_ERR;
	}
	return CRT188_COMMAND_EXE_ERR;
	
}

uint8 Crt188CPUCardColdReset(void){
	uint8 err;
	_crt188_cmd_s *cmd_temp=&crt188_cmd_temp;
	uint8 *buf_temp = (uint8 *)cmd_temp;

	cmd_temp->lenght_h = 0x00;
	cmd_temp->lenght_l = 0x02;
	cmd_temp->cmd = 0x37;
	cmd_temp->arg = 0x30;
	cmd_temp->dat_lenght = 0;
	
	err = Crt188CommandHandle(cmd_temp);
	if (err != SYS_NO_ERR){
#if ERR_EN == 1
		
#endif
		return err;
	}
	if (((uint8 *)buf_temp)[2] == 'Y') {
		
	}
	else if (((uint8 *)buf_temp)[2] == 'Z') {

	}
	else if (((uint8 *)buf_temp)[2] == 'N') {

	}
	else if (((uint8 *)buf_temp)[2] == 'E') {

	}
	else if (((uint8 *)buf_temp)[2] == 'W') {

	}
	else {

	}
	return err;
}

uint8 Crt188CPUCardHotReset(void) {

	uint8 err;
	_crt188_cmd_s *cmd_temp=&crt188_cmd_temp;
	uint8 *buf_temp = (uint8 *)cmd_temp;

	cmd_temp->lenght_h = 0x00;
	cmd_temp->lenght_l = 0x02;
	cmd_temp->cmd = 0x37;
	cmd_temp->arg = 0x2F;
	cmd_temp->dat_lenght = 0;
	
	err = Crt188CommandHandle(cmd_temp);
	if (err != SYS_NO_ERR){
#if ERR_EN == 1
		
#endif
		return err;
	}
	if (((uint8 *)buf_temp)[0] == 'Y') {
		
	}
	else if (((uint8 *)buf_temp)[0] == 'Z') {

	}
	else if (((uint8 *)buf_temp)[0] == 'N') {

	}
	else if (((uint8 *)buf_temp)[0] == 'E') {

	}
	else if (((uint8 *)buf_temp)[0] == 'W') {

	}
	else {

	}
	return err;
}

uint8 Crt188CPUT0CardOp(void *buf,uint8 len){

	uint8 err;
#if 0
	_crt188_cmd_s *cmd_temp=&crt188_cmd_temp;
	uint8 *buf_temp = (uint8 *)cmd_temp;
	uint16 len_temp;
	uint8 dat_buf[CHECK_DATA_LENGHT + 2] = {0x00,CHECK_DATA_LENGHT};				//cpu opration package

	len_temp = CHECK_DATA_LENGHT;
	len_temp += 4;
	cmd_temp->lenght_h = (uint8)(len_temp >> 8);
	cmd_temp->lenght_l = (uint8)len_temp;
	cmd_temp->cmd = 0x37;
	cmd_temp->arg = 0x31;
	cmd_temp->dat_lenght = CHECK_DATA_LENGHT + 2;
	cmd_temp->dat = dat_buf;
	err = Crt188CommandHandle(cmd_temp);
	if (err != SYS_NO_ERR){
		return err;
	}
	if (((uint8 *)buf_temp)[0] == 'Y') {
		memcpy(buf,&((uint8 *)buf_temp)[3],CHECK_DATA_LENGHT);
		return SYS_NO_ERR;
	}
	else if (((uint8 *)buf_temp)[0] == 'N') {
		((uint8 *)buf)[0] = 0;
	}
	else if (((uint8 *)buf_temp)[0] == 'E') {
		((uint8 *)buf)[0] = 0;
	}
	else if (((uint8 *)buf_temp)[0] == 'W') {
		((uint8 *)buf)[0] = 0;
	}
	else {
		((uint8 *)buf)[0] = 0;
		return UART_COMMUNICATION_ERR_RETURN_DATA_ERR;
	}
	return CRT188_COMMAND_EXE_ERR;
#else
	err = SYS_NO_ERR;
	return err;
#endif
}




uint8 Crt188CPUT1CardOp(void *buf){

	uint8 err;
#if 0
	_crt188_cmd_s *cmd_temp=&crt188_cmd_temp;
	void *buf_temp;
	uint16 len_temp;
	uint8 dat_buf[CHECK_DATA_LENGHT + 2] = {0x00,CHECK_DATA_LENGHT};				//cpu opration package

	len_temp = CHECK_DATA_LENGHT;
	len_temp += 4;
	cmd_temp->lenght_h = (uint8)(len_temp >> 8);
	cmd_temp->lenght_l = (uint8)len_temp;
	cmd_temp->cmd = 0x37;
	cmd_temp->arg = 0x32;
	cmd_temp->dat_lenght = CHECK_DATA_LENGHT + 2;
	cmd_temp->dat = dat_buf;
	err = Crt188CommandHandle(cmd_temp);
	if (err != SYS_NO_ERR){
#if ERR_EN == 1
		
#endif
		return err;
	}
	if (((uint8 *)buf_temp)[0] == 'Y') {
		memcpy(buf,&((uint8 *)buf_temp)[3],CHECK_DATA_LENGHT);
		return SYS_NO_ERR;
	}
	else if (((uint8 *)buf_temp)[0] == 'N') {
		((uint8 *)buf)[0] = 0;
	}
	else if (((uint8 *)buf_temp)[0] == 'E') {
		((uint8 *)buf)[0] = 0;
	}
	else if (((uint8 *)buf_temp)[0] == 'W') {
		((uint8 *)buf)[0] = 0;
	}
	else {
		((uint8 *)buf)[0] = 0;
		return UART_COMMUNICATION_ERR_RETURN_DATA_ERR;
	}
	return CRT188_COMMAND_EXE_ERR;
#else
	err = SYS_NO_ERR;
	return err;
#endif
}

/*
uint8 CheckCard(uint32 password) {		//�жϿ��Ƿ���Ч���Ϳ���Ȩ���ж�
	uint8 err,card_type;
	uint8 buf[CHECK_DATA_LENGHT];

	while (Crt188Reset() == 0) {
		
	}
	if (Crt188ReadState() == 1) {  //ע����󷵻�0XFF
		//return 1;			// �������п�
	}
	else {
		return 0;			// �������޿�
	}
	
	err = Crt188ReadCardType(&card_type);
	if (card_type != CARD_TYPE_24C01) {		//	�������ǵĿ����ͣ��˳�
		return 0;
	}
	if	(	(card_type == CARD_TYPE_24C01) || (card_type == CARD_TYPE_24C02) 	// 24CXX������
		||	(card_type == CARD_TYPE_24C04) || (card_type == CARD_TYPE_24C08)
		||	(card_type == CARD_TYPE_24C16) || (card_type == CARD_TYPE_24C32)
		||	(card_type == CARD_TYPE_24C64)
		){
		//Crt188SetType24CXX(card_type);
		if (Crt188ICPower(1) == 0) {
			return 0;
		}
		if (Crt188Read24CXX(card_type,0,sizeof(_ic_data_s),(uint8 *)&df_card) != 0) {
			return 0;
		}
		if (Crt188ICPower(0) == 0) {
			return 0;
		}
	}
	else if (card_type == CARD_TYPE_T0_CPU) {	// CPU ������
		Crt188CPUCardHotReset();
		if (Crt188CPUT0CardOp(buf) != 0) {
			return 0;
		}
	}
	else if (card_type == CARD_TYPE_T1_CPU) {	//CPU ������
		Crt188CPUCardHotReset();
		if (Crt188CPUT1CardOp(buf) != 0) {
			return 0;
		}
	}
	else if (card_type == CARD_TYPE_SL4442) {			// SL4442������
		return 0;
	}
	else if (card_type == CARD_TYPE_SL4428) {			//SL4428������
		return 0;
	}
	else if (card_type == CARD_TYPE_AT88S102) {			//AT88S102������
		return 0;
	}
	else if (card_type == CARD_TYPE_AT88S1604) {		//AT88S1604������
		return 0;
	}
	else if (card_type == CARD_TYPE_AT45D041) {			//AT45D041������
		return 0;
	}
	else if (card_type == CARD_TYPE_AT88SC1608) {		//AT88S1608������
		return 0;
	}
	else {
		return 0;
	}
	return 1;
		
}
*/
//	�ú�������IC �������ݶ�д��IC ������״̬��ѯ
#define	CARD_MACHINE_FAULT_CHECK_TIME						10
#define NO_CARD_STATE_CHECK_TIME							3
void TaskICCard(void *pdata) {
	uint8 err;
	uint8 card_type;
	uint8 state_time,failure_time,fault_time;
	
	pdata = pdata;
	
UART_COMMUNICATION_ERR_HANDLE:
	Crt188ClosePower();
	OSTimeDly(100);
	Crt188OpenPower();
	failure_time = 0;
	state_time = 0;
	while (1) {
		OSTimeDly(20);
		RequestHardResource();		//	����Ӳ����Դ
		if (Crt188Reset() != SYS_NO_ERR) {
			//	��λʧ��
			if ((++failure_time) > CARD_MACHINE_FAULT_CHECK_TIME) {
				card_machine_state = SYS_ERR_CRT188_SERIAL_PORT_FAULT;			//	��������
				device_control.sys_device.ic_machine_state = IC_MACHINE_ABNORMAL;
			}
		}
		else {
			FreeHardResource();
			break;
		}
		FreeHardResource();
	}
	failure_time = 0;
	state_time = 0;
	fault_time = 0;
	while (1) {
		OSTimeDly(OS_TICKS_PER_SEC/10);
		RequestHardResource();
		err = Crt188ReadState();			//	����״̬
		if (err == CRT188_NO_CARD) {
			//	�������޿�
			if ((++state_time) > (GetLoginRemainTime() / 5 / (OS_TICKS_PER_SEC / 10))) {	//	�����˿��˲�
				card_machine_state = CRT188_NO_CARD;				//	ȷʵ���˿�
				device_control.sys_device.ic_machine_state = IC_MACHINE_NO_CARD;
			}
			FreeHardResource();
			continue;
		}
		else if (err == SYS_NO_ERR) {
			//	�п�����
			state_time = 0;
		}
		else if ((err == UART_COMMUNICATION_ERR_COMM_NACK) || (err == CRT188_COMMAND_EXE_ERR) || (err == UART_COMMUNICATION_ERR_RETURN_DATA_ERR)) {
			//	ͨ�����쳣
			if ((++fault_time) > CARD_MACHINE_FAULT_CHECK_TIME) {
				card_machine_state = SYS_ERR_CRT188_MACHINE_FAULT;
				device_control.sys_device.ic_machine_state = IC_MACHINE_ABNORMAL;
			}
			FreeHardResource();
			continue;
		}
		else {
			FreeHardResource();
			goto UART_COMMUNICATION_ERR_HANDLE;
		}

		if (card_machine_state == CRT188_HAVE_CARD) {	//	�������һ�ο�����Ҫ�ٶ������ǿ����γ�
			FreeHardResource();
			continue;
		}
		err = Crt188ReadCardType(&card_type);			//	��������
		if (err == SYS_NO_ERR) {
			
		}
		else if ((err == UART_COMMUNICATION_ERR_COMM_NACK) || (err == CRT188_COMMAND_EXE_ERR) || (err == UART_COMMUNICATION_ERR_RETURN_DATA_ERR)) {
			if ((++fault_time) > CARD_MACHINE_FAULT_CHECK_TIME) {
				card_machine_state = SYS_ERR_CRT188_MACHINE_FAULT;
				device_control.sys_device.ic_machine_state = IC_MACHINE_ABNORMAL;
			}
			FreeHardResource();
			continue;
		}
		else {
			FreeHardResource();
			goto UART_COMMUNICATION_ERR_HANDLE;
		}

		err = Crt188ICPower(0x30);			//	�ϵ�
		if (err == SYS_NO_ERR) {
			
		}
		else if ((err == UART_COMMUNICATION_ERR_COMM_NACK) || (err == CRT188_COMMAND_EXE_ERR) || (err == UART_COMMUNICATION_ERR_RETURN_DATA_ERR)) {
			if ((++fault_time) > CARD_MACHINE_FAULT_CHECK_TIME) {
				card_machine_state = SYS_ERR_CRT188_MACHINE_FAULT;
				device_control.sys_device.ic_machine_state = IC_MACHINE_ABNORMAL;
			}
			FreeHardResource();
			continue;
		}
		else {
			FreeHardResource();
			goto UART_COMMUNICATION_ERR_HANDLE;
		}

		if ((card_type >= CARD_TYPE_24C01) && (card_type <= CARD_TYPE_24C64)) {		//	�ж��Ƿ���ϸ����͵Ŀ�
			err = Crt188Read24CXX(card_type,0,sizeof(_card_data_s),&df_card_data);			//	��IC ������
			if (err == SYS_NO_ERR) {
				card_machine_state = CRT188_HAVE_CARD;
				device_control.sys_device.ic_machine_state = IC_MACHINE_HAVE_CARD;
				device_control.user.uinfo = df_card->card.uinfo;
				device_control.user.rinfo = df_card->card.rinfo;
				//Crt188Write24CXX(card_type,0,sizeof(_card_data_s),df_card);
			}
			else if ((err == UART_COMMUNICATION_ERR_COMM_NACK) || (err == CRT188_COMMAND_EXE_ERR) || (err == UART_COMMUNICATION_ERR_RETURN_DATA_ERR)) {
				if ((++fault_time) > CARD_MACHINE_FAULT_CHECK_TIME) {
					card_machine_state = SYS_ERR_CRT188_MACHINE_FAULT;
					device_control.sys_device.ic_machine_state = IC_MACHINE_ABNORMAL;
				}
				FreeHardResource();
				continue;
			}
			else {
				FreeHardResource();
				goto UART_COMMUNICATION_ERR_HANDLE;
			}
		}
		else {
			device_control.sys_device.ic_machine_state = IC_MACHINE_CARD_NOT_IDENTIFY;
		}
		
		err = Crt188ICPower(0x31);			//	�µ�
		if (err == SYS_NO_ERR) {
			
		}
		else if ((err == UART_COMMUNICATION_ERR_COMM_NACK) || (err == CRT188_COMMAND_EXE_ERR) || (err == UART_COMMUNICATION_ERR_RETURN_DATA_ERR)) {
			if ((++fault_time) > CARD_MACHINE_FAULT_CHECK_TIME) {
				card_machine_state = SYS_ERR_CRT188_MACHINE_FAULT;
				device_control.sys_device.ic_machine_state = IC_MACHINE_ABNORMAL;
			}
			FreeHardResource();
			continue;
		}
		else {
			FreeHardResource();
			goto UART_COMMUNICATION_ERR_HANDLE;
		}
		FreeHardResource();
		fault_time = 0;
	}
	
}


