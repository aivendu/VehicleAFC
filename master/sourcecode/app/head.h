#ifndef _HEAD_H
#define _HEAD_H


typedef enum
{
    PRIOSTART = 4,

    TaskTestPrio,				//	5
    TaskDeviceCommandPrio,		//	6
    TaskPADRecHandlePrio,		//	7
    TaskServerHandlePrio,		//	8
    TaskChipCommPrio,			//	9
    TaskTradePrio,				//	10
    TaskDataUploadPrio,			//	11
    TaskSysConfigPrio,			//	12

    PRIOEND
} _task_prio_e;

#define STATE_LED				(1<<30)


extern const char completeversion[16];


#endif
