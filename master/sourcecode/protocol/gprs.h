#ifndef _GPRS_H
#define _GPRS_H


#define GDTU_SEND_LOG				1

extern void TaskGPRS(void * pdata);
extern void GdtuSendUserData(void *dat, uint8 mod);
extern void GdtuInit(void);
extern char *calString(char *string,char *buf);
extern uint8 GdtuLastSendSt(void);



#endif
