#ifndef _SYN6288_H
#define _SYN6288_H

typedef struct {
	uint16 len;		//	�������ݳ��ȣ�����"������","����","����","У��"
	uint8  cmd;		//	�����֣�ÿ�������
	uint8  arg;		//	������ĳЩ����û��
	uint8  *dat;	//	����
} _syn6288_command_s;



#define SYN6288_BACKMUSIC					0			//	���ñ�������

//	��������
#define WORDFORMAT_GB2312_SYN6288			0			//	����GB2312 ����
#define WORDFORMAT_GBK_SYN6288				1
#define WORDFORMAT_BIG5_SYN6288				2
#define WORDFORMAT_UNICODE_SYN6288			3

//	���ò�����
#define BAUDRATE_9600_SYN6288				0			//	���ò�����Ϊ9600
#define BAUDRATE_19200_SYN6288				1
#define BAUDRATE_38400_SYN6288				2

extern void Syn6288Init(void);
extern void Syn6288Speak(char *voice,uint8 flag);
extern void ChangeBaudrate(uint8 bps_no);
extern void Syn6288Stop(void);
extern void Syn6288Pause(void);
extern void Syn6288Recover(void);
extern void Syn6288ReadState(void);
extern void Syn6288PowerDown(void);

extern void Syn6288Test(void);




#endif

