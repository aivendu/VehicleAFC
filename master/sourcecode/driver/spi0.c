#include "includes.h"

#define SPI0RATE			500000				//	5500000>SPI0RATE>50 000
#define SPI_DATA_BUFFER_LENGHT				100

static uint8 spisendbuffer[SPI_DATA_BUFFER_LENGHT];
static uint8 spirecbuffer[SPI_DATA_BUFFER_LENGHT];
static OS_EVENT *spi0_sem;

void Spi0Init(void) {

	/*S0PCCR = (uint8)(Fpclk/SPI0RATE);
	S0PCR  = (1 << 3)|				// CPHA = 1, ������SCK �ĵڶ���ʱ���ز���
 			(1 << 4) |				// CPOL = 1, SCK Ϊ����Ч
 			(1 << 5) |				// MSTR = 1, SPI ������ģʽ
 			(1 << 6) |				// LSBF = 1, SPI ���ݴ���MSB (λ0)����
 			(1 << 7) |				// SPIE = 1, SPI �жϱ���ֹ
 			(0 << 8);

	S0PSR = 0x00;
	S0PDR = 0x00;
	//S0PCCR = (uint8)(Fpclk/SPI0RATE);
	//S0PINT = 0x00;
	*/if (QueueCreate((void *)spisendbuffer,sizeof(spisendbuffer),NULL,NULL) != QUEUE_OK) {
		while(1);
	}
	if (QueueCreate((void *)spirecbuffer,sizeof(spirecbuffer),NULL,NULL) != QUEUE_OK) {
		while(1);
	}
	spi0_sem = OSSemCreate(0);
	if (spi0_sem == NULL) {
		while (1);	
	}
}

uint32 time_delay2=10;

uint8 Spi0TranceByte(uint8 dat) {
#if (SPI0_INT_EN)
	uint8 err;
	S0PDR = dat;
	OSSemPend(spi0_sem,100,&err);
	//while ((S0PSR & 0x80) == 0);
	//err = S0PDR;
	return err;
#else
	uint8	i;
	uint8	buf;
	//uint32  time1,time2,time3;
	IO0SET=IO0SET|SPI0_SCK;			//	
	OS_ENTER_CRITICAL();
	IO0CLR |= SPI0_CS;				//	Ƭѡ�͵�ƽ��Ч
	delayus(20);
	buf=0;
	for(i=0;i<8;i++){										//����һ���ֽ�����
		if(dat&0x01)			IO0SET=IO0SET|SPI0_MOSI;			//�������
		else					IO0CLR=IO0CLR|SPI0_MOSI;
		delayus(1);
		IO0CLR=IO0CLR|SPI0_SCK;									//ʱ������
		dat>>=1;											//���������λ
		buf>>=1;											//����������λ
		if(IO0PIN&SPI0_MISO)		buf|=0x80;
		else						buf&=(~0x80);
		delayus(1);
		IO0SET=IO0SET|SPI0_SCK;
	}
	//delayus(time_delay);
	IO0SET |= SPI0_CS;
	OS_EXIT_CRITICAL();
	delayus(13);			//	ÿ�����ݷ��ͺ���ʱ50us
	return(buf);
#endif
}



void Spi0_Exception(void) {
	uint8 data_temp;
	if (S0PSR) ;

	data_temp = S0PDR;
	S0PDR = 0x55;
	
	S0PINT = 0x01;
	VICIntEnClr |= (1<<10);
}






