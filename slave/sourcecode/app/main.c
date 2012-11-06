/****************************************Copyright (c)**************************************************
**                               Guangzou ZLG-MCU Development Co.,LTD.
**                                      graduate school
**                                 http://www.zlgmcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			main.c
** Last modified Date:  2004-09-16
** Last Version:		1.0
** Descriptions:		The main() function example template
**
**------------------------------------------------------------------------------------------------------
** Created by:			Chenmingji
** Created date:		2004-09-16
** Version:				1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			Chenxibing
** Modified date:		2005-03-07
** Version:
** Descriptions:		EasyARM2131上的第一个基于uC/OS-II的例子。
**						按一下按键KEY1，松开后蜂鸣器响2声。
********************************************************************************************************/
#include "includes.h"
#include "stdlib.h"


#define TaskTestSize			128
#define	TaskCrt188Size			256
#define	TaskNoteMachineSize		256
#define	TaskPTRExeSize			256
#define	TaskGPSSize				256
#define	TaskHopperExeSize		256
#define	TaskChipCommSize		256




OS_STK	TaskTestStk[TaskTestSize];
OS_STK	TaskCrt188Stk[TaskCrt188Size];
OS_STK	TaskNoteMachineStk[TaskNoteMachineSize];
OS_STK	TaskPTRExeStk[TaskPTRExeSize];
OS_STK	TaskGPSStk[TaskGPSSize];
OS_STK	TaskHopperExeStk[TaskHopperExeSize];
OS_STK	TaskChipCommStk[TaskChipCommSize];


/* 函数声明 */


/*
*********************************************************************************************************
** 函数名称 ：main()
** 函数功能 ：uC/OS-II主函数，启动多任务环境。
*********************************************************************************************************
*/
int main (void)
{
    OSInit();	
    
    TargetInit();
    //OSTaskCreate (TaskTest, (void *)0, &TaskTestStk[TaskTestSize - 1], TaskTestPrio);
	OSTaskCreate(TaskICCard, (void *)0, &TaskCrt188Stk[TaskCrt188Size - 1], TaskCrt188Prio);
	OSTaskCreate(TaskNoteMachine, (void *)0, &TaskNoteMachineStk[TaskNoteMachineSize - 1], TaskNoteMachinePrio);
    OSTaskCreate (TaskPTRExe, (void *)0, &TaskPTRExeStk[TaskPTRExeSize - 1], TaskPTRExePrio);
	OSTaskCreate (TaskHopperExe, (void *)0, &TaskHopperExeStk[TaskHopperExeSize - 1], TaskHopperExePrio);
    OSTaskCreate (TaskGPS, (void *)0, &TaskGPSStk[TaskGPSSize - 1], TaskGPSPrio);
    OSTaskCreate (TaskChipComm, (void *)0, &TaskChipCommStk[TaskChipCommSize - 1], TaskChipCommPrio);
    //OSTaskCreate (TaskTest, (void *)0, &TaskTestStk[TaskTestSize - 1], TaskTestPrio);

	


	//OSTaskSuspend(TaskTestPrio);
	//OSTaskSuspend(TaskCrt188Prio);
	//OSTaskSuspend(TaskNoteMachinePrio);
	//OSTaskSuspend(TaskPTRExePrio);
	//OSTaskSuspend(TaskHopperExePrio);
	//OSTaskSuspend(TaskGPSPrio);
	//OSTaskSuspend(TaskChipCommPrio);

	
    OSStart();	
    															//(6)
    return 0;																//(7)
}



   
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
