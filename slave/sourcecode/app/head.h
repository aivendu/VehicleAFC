#ifndef _HEAD_H
#define _HEAD_H


typedef enum
{
    PRIOSTART = 4,

    TaskChipCommPrio,
    TaskTestPrio,
    TaskCrt188Prio,
    TaskNoteMachinePrio,
    TaskPTRExePrio,
    TaskGPSPrio,
    TaskHopperExePrio,
    PRIOEND
} _task_prio_e;

#define STATE_LED				(1<<30)


extern const char completeversion[16];


#endif
