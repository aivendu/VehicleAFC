#ifndef _SPI_H
#define _SPI_H


#define SPI0_INT_EN			0


#define SPI0_SCK			(1<<4)
#define SPI0_MISO			(1<<5)
#define SPI0_MOSI			(1<<6)
#define SPI0_CS				(1<<7)

extern void Spi0Init(void);
extern uint8 Spi0TranceByte(uint8 dat);
extern void Spi0_Exception(void);



#endif
