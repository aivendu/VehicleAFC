#ifndef _SYN6288_H
#define _SYN6288_H

typedef struct {
	uint16 len;		//	命令数据长度，包括"命令字","参数","数据","校验"
	uint8  cmd;		//	命令字，每条命令都有
	uint8  arg;		//	参数，某些命令没有
	uint8  *dat;	//	数据
} _syn6288_command_s;



#define SYN6288_BACKMUSIC					0			//	设置背景音乐

//	设置字体
#define WORDFORMAT_GB2312_SYN6288			0			//	设置GB2312 字体
#define WORDFORMAT_GBK_SYN6288				1
#define WORDFORMAT_BIG5_SYN6288				2
#define WORDFORMAT_UNICODE_SYN6288			3

//	设置波特率
#define BAUDRATE_9600_SYN6288				0			//	设置波特率为9600
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

