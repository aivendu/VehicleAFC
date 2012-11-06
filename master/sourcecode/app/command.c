#include "command.h"

_Command_S command_table[MAX_COMMANDS];


//	����һ������
_Command_S *CurrentCommand(void)
{
	uint8 i;
	for (i = 0; i < MAX_COMMANDS; i++)
	{
		if (command_table[i].use_flag == 0)
		{
			command_table[i].use_flag = 1;
			command_table[i].command_flag = 0;
			command_table[i].state = COMMAND_WAIT;
			return &command_table[i];
		}
	}
	return NULL;
}

//	����һ������
uint8 PostCommand(_Command_S *cmd,uint8 *err)
{
}

//	�ȴ�һ������
uint8 PendCommand(_Command_S *cmd,uint32 time,uint8 *err)
{
}

//	������״̬
uint8 ReadCommandState(_Command_S *cmd)
{
}

//	��λ����
uint8 ResetCommand(_Command_S *cmd)
{
}

//	ɾ������
void DelectCommand(_Command_S *cmd)
{
}












