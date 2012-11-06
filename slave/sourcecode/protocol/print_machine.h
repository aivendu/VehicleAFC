#ifndef _PRINT_MACHINE_H
#define _PRINT_MACHINE_H





#define  TIMEX			32
#define  TIMEY			16
/*
#define P08WR			(1 << 12)

#define P09RD			(1 << 9)
#define P010CS			(1 << 13)
#define P011RS			(1 << 11)
#define P012RST			(1 << 23)

#define P021BUSY		(1 << 21)
	

#define DB0		(1<<15)
#define DB1		(1<<16)
#define DB2		(1<<17)
#define DB3		(1<<18)

#define DB4		(1<<19)
#define DB5		(1<<20)
#define DB6		(1<<21)
#define DB7		(1<<22)

//#define IWR			IO0SET	|= P08WR

//#define EWR			IO0CLR	|= P08WR

//#define IRD			IO0SET	|= P09RD
//#define ERD			IO0CLR	|= P09RD

//#define ICS			IO0SET |= P010CS
//#define ECS			IO0CLR |= P010CS

//#define CRS			IO0SET |= P011RS

//#define DRS			IO0CLR |= P011RS

//#define IRST		IO0SET |= P012RST
//#define ERST		IO0CLR |= P012RST



//#define	D0D7					(0xBF<<15)
//#define D6						(1<<20)
*/
#define ENDFRAME		184//168//184
#define ENDFRAMEY		318//313
#define OFFSET			6
#define TEMPSIZE		64
#define	DISBUFSIZE		256
#define DISSIZE			256

extern	uint8		PTRData[DISSIZE];
extern	OS_EVENT	*pPTRSEM;
extern	OS_EVENT	*pU0SEMCHANNEL;


extern char print_buffer[TEMPSIZE];

extern void initLCDM(void);
//extern uint8 LCDDisData[DISSIZE];
extern uint8    dotPtr;
extern void  TaskPTRExe(void *pdata);







#endif

