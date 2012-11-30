#ifndef _COMMAND_H
#define _COMMAND_H


typedef struct
{
	uint8 command_flag		: 1;
	uint8 use_flag			: 1;
	uint8 unused			: 2;
	uint8 state				: 4;
	/*	״̬˵��
	*	0--������δʹ�ã�
	*	1--�����δִ�У�
	*	2--����ִ���У�
	*	3--����ִ����ϣ�
	*	4--�ȴ����
	*	15--ִ���쳣��ֹ��*/
} _Command_S;

#define COMMAND_NO_USERD			0
#define COMMAND_NO_EXE				1
#define COMMAND_RUNNING				2
#define COMMAND_EXE_END				3
#define COMMAND_WITE				4
#define COMMAND_ABORT				15

#define MAX_COMMANDS				10


//	����һ������
extern _Command_S *CurrentCommand(void);

//	����һ������
extern uint8 PostCommand(_Command_S *cmd, uint8 *err);

//	�ȴ�һ������
extern uint8 PendCommand(_Command_S *cmd, uint32 time, uint8 *err);

//	������״̬
extern uint8 ReadCommandState(_Command_S *cmd);

//	��λ����
extern uint8 ResetCommand(_Command_S *cmd);

//	ɾ������
extern void DelectCommand(_Command_S *cmd);


#endif

