#ifndef _COMMAND_H
#define _COMMAND_H


typedef struct
{
	uint8 command_flag		: 1;
	uint8 use_flag			: 1;
	uint8 unused			: 2;
	uint8 state				: 4;
	/*	状态说明
	*	0--该命令未使用，
	*	1--有命令还未执行，
	*	2--命令执行中，
	*	3--命令执行完毕，
	*	4--等待命令，
	*	15--执行异常终止，*/
} _Command_S;

#define COMMAND_NO_USERD			0
#define COMMAND_NO_EXE				1
#define COMMAND_RUNNING				2
#define COMMAND_EXE_END				3
#define COMMAND_WITE				4
#define COMMAND_ABORT				15

#define MAX_COMMANDS				10


//	创建一个命令
extern _Command_S *CurrentCommand(void);

//	发送一个命令
extern uint8 PostCommand(_Command_S *cmd, uint8 *err);

//	等待一个命令
extern uint8 PendCommand(_Command_S *cmd, uint32 time, uint8 *err);

//	读命令状态
extern uint8 ReadCommandState(_Command_S *cmd);

//	复位命令
extern uint8 ResetCommand(_Command_S *cmd);

//	删除命令
extern void DelectCommand(_Command_S *cmd);


#endif

