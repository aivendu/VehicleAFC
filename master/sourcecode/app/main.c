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
** Descriptions:		EasyARM2131�ϵĵ�һ������uC/OS-II�����ӡ�
**						��һ�°���KEY1���ɿ����������2����
********************************************************************************************************/
#include "includes.h"
#include "stdlib.h"


#define TaskTestSize			256
#define TaskPADRecHandleSize	256
#define TaskDeviceCommandSize	256
#define TaskServerHandleSize	256
#define TaskChipCommSize		256
#define TaskDataStoreSize		256


const char completeversion[16]={VERSION};
OS_STK TaskTestStk[TaskTestSize];
OS_STK TaskPADRecHandleStk[TaskPADRecHandleSize];
OS_STK TaskDeviceCommandStk[TaskDeviceCommandSize];
OS_STK TaskServerHandleStk[TaskServerHandleSize];
OS_STK TaskChipCommStk[TaskChipCommSize];
OS_STK TaskDataStoreStk[TaskDataStoreSize];


/* �������� */


/*
*********************************************************************************************************
** �������� ��main()
** �������� ��uC/OS-II�����������������񻷾���
*********************************************************************************************************
*/
int main (void)
{
    OSInit();	
    
    TargetInit();
	
    OSTaskCreate (TaskTest, (void *)0, &TaskTestStk[TaskTestSize - 1], TaskTestPrio);
	OSTaskCreate (TaskPADRecHandle, (void *)0, &TaskPADRecHandleStk[TaskPADRecHandleSize - 1], TaskPADRecHandlePrio);
	OSTaskCreate (TaskDeviceCommand, (void *)0, &TaskDeviceCommandStk[TaskDeviceCommandSize - 1], TaskDeviceCommandPrio);
	OSTaskCreate (TaskChipComm, (void *)0, &TaskChipCommStk[TaskChipCommSize - 1], TaskChipCommPrio);
	OSTaskCreate (TaskServerHandle, (void *)0, &TaskServerHandleStk[TaskServerHandleSize - 1], TaskServerHandlePrio);
	OSTaskCreate (TaskDataStore, (void *)0, &TaskDataStoreStk[TaskDataStoreSize - 1], TaskDataStorePrio);


	OSTaskSuspend(TaskTestPrio);
	//OSTaskSuspend(TaskPADRecHandlePrio);
	//OSTaskSuspend(TaskDeviceCommandPrio);
	//OSTaskSuspend(TaskChipCommPrio);
	//OSTaskSuspend(TaskServerHandlePrio);

	
    OSStart();	
    															//(6)
    return 0;																//(7)
}



   
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
