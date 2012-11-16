#ifndef _HEAD_H
#define _HEAD_H


typedef enum {
	PRIOSTART=4,
		
	TaskTestPrio,
	TaskDeviceCommandPrio,
	TaskPADRecHandlePrio,
	TaskServerHandlePrio,
	TaskChipCommPrio,
	TaskTradePrio,
	TaskDataUploadPrio,
	TaskSysConfigPrio,
	
	PRIOEND
} _task_prio_e;

#define STATE_LED				(1<<30)


extern const char completeversion[16];


#endif
