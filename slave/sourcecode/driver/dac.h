#ifndef _DAC_H
#define _DAC_H

#define DACR (*((volatile unsigned long *) 0xE006C000))		//	����DAC���ƼĴ���


extern void DacInit(void);
extern void DacOut(uint16 val);


#endif
