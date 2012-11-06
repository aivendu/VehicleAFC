#ifndef _SPI_H
#define _SPI_H

#define SPI0_MODE			0		//	1--模拟模式，0--中断模式

#define SPI0_COMM_CONFIG	(CPOL+LSBF)


#define SPI0_SCK			(1<<4)
#define SPI0_MISO			(1<<5)
#define SPI0_MOSI			(1<<6)
#define SPI0_CS				(1<<7)

#if (SPI0_MODE == 0)
#define ENABLESPI0()			(S0PCR |= (1<<7))
#define DISABLESPI0()			//(S0PCR &= (~(1<<7)))
#else
#define ENABLESPI0()			(VICIntEnable = 1 << 16)
#define DISABLESPI0()			(VICIntEnClr  = 1 << 16)
#endif

typedef struct {
	void *fun_data_addr;
	uint16 len;
} _data_pond_s;


extern uint32 spi0_tick;

extern uint8 spi0_send_data,spi0_rec_data;
extern uint8 spi0_state;
extern OS_EVENT *spi0_mbox;


extern _data_pond_s data_pond[8];


extern void Spi0Init(void);
extern uint8 SpiDataHandle(uint8 rec_temp);
extern void Spi0_Exception(void);



#endif
