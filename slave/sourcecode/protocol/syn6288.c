#include "includes.h"

#define Syn6288SendByte(c)			Uart1SendByte(c,0)
#define Syn6288RecByte(a)			Uart1RecByte(a,0,0)
#define RequestHardResource()		RequestUart(SPEAKER_UART1,0)
#define FreeHardResource()			FreeUart(SPEAKER_UART1)
#define Syn6288CheckBusy()			(((IO0PIN & SYN6288_BUSY) == SYN6288_BUSY))


void Syn6288Init(void) {
	uint8 err  = 0;
}

void Syn6288SendFrame(_syn6288_command_s *cmd) {
	uint16 i;
	uint8  bcc=0;
	Syn6288SendByte(0xFD);				//	����ͷ
	bcc = 0xFD;
	Syn6288SendByte((uint8)(cmd->len>>8));	//	���ͳ��ȸ��ֽ�
	bcc ^= (uint8)(cmd->len>>8);
	Syn6288SendByte((uint8)cmd->len);		//	���ͳ��ȵ��ֽ�
	bcc ^= (uint8)cmd->len;
	Syn6288SendByte(cmd->cmd);			//	��������
	bcc ^= cmd->cmd;
	if (cmd->len > 2) { 					//	�����Ƿ��в���
		Syn6288SendByte(cmd->arg);		//	�����в�������������
		bcc ^= cmd->arg;
	}
	if ((cmd->len > 3) && (cmd->dat != NULL)) {	//	�Ƿ�������
		for (i=0;i<(cmd->len-2);i++) {			//	�����ݣ�������������
			Syn6288SendByte(cmd->dat[i]);
			bcc ^= cmd->dat[i];
		}
	}
	Syn6288SendByte(bcc);				//	�������У�飬���ǰ����������
}

void Syn6288Test(void) {
	uint8 test_buf[]={0xFD,0x00,0x0b,0x01,0x00,0xd3,0xee,0xd2,0xf4, 0xcc, 0xec, 0xcf, 0xc2, 0xc1};
	uint8 i;
	RequestHardResource();
	for (i=0;i<sizeof(test_buf);i++) {
		Syn6288SendByte(test_buf[i]);
	}
	FreeHardResource();
}

void Syn6288Speak(char *voice, uint8 flag) {
	_syn6288_command_s cmd;

	while ((flag == 1) && (Syn6288CheckBusy())) {	//	�ȴ�оƬ����
		OSTimeDly(OS_TICKS_PER_SEC/2);
	}
	cmd.len = 2+strlen(voice)+1;
	cmd.cmd = 0x01;
	cmd.arg = (SYN6288_BACKMUSIC<<5)+WORDFORMAT_GB2312_SYN6288;
	cmd.dat = (uint8 *)voice;
	RequestHardResource();
	Syn6288SendFrame(&cmd);
	FreeHardResource();
}

void ChangeBaudrate(uint8 bps_no) {
	_syn6288_command_s cmd;

	cmd.len = 2+1;
	cmd.cmd = 0x31;
	cmd.arg = bps_no;
	cmd.dat = NULL;
	RequestHardResource();
	Syn6288SendFrame(&cmd);
	FreeHardResource();
}

void Syn6288Stop(void) {
	_syn6288_command_s cmd;

	cmd.len = 2;
	cmd.cmd = 0x02;
	cmd.dat = NULL;
	RequestHardResource();
	Syn6288SendFrame(&cmd);
	FreeHardResource();
}

void Syn6288Pause(void) {
	_syn6288_command_s cmd;

	cmd.len = 2;
	cmd.cmd = 0x03;
	cmd.dat = NULL;
	RequestHardResource();
	Syn6288SendFrame(&cmd);
	FreeHardResource();
}

void Syn6288Recover(void) {
	_syn6288_command_s cmd;

	cmd.len = 2;
	cmd.cmd = 0x04;
	cmd.dat = NULL;
	RequestHardResource();
	Syn6288SendFrame(&cmd);
	FreeHardResource();
}

void Syn6288ReadState(void ) {
	_syn6288_command_s cmd;

	cmd.len = 2;
	cmd.cmd = 0x21;
	cmd.dat = NULL;
	RequestHardResource();
	Syn6288SendFrame(&cmd);
	FreeHardResource();
}

void Syn6288PowerDown(void) {
	_syn6288_command_s cmd;

	cmd.len = 2;
	cmd.cmd = 0x88;
	cmd.dat = NULL;
	RequestHardResource();
	Syn6288SendFrame(&cmd);
	FreeHardResource();
}

