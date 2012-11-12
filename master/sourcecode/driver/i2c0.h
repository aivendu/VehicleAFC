#ifndef _I2C_H
#define _I2C_H

#define MEMERY_ADDR_MODE	1				/*0 -- С��ģʽ��1 -- ���ģʽ*/	

#define I2C_WRITE_END       0x101           /* д���   */
#define I2C_READ_END        0x102           /* �����   */
#define I2C_NOT_GET_BUS     0x104           /* ��ʧ�ٲ� */
#define I2C_ACK_ERR         0x108           /* ����ACK���� */


extern void I2c0Init(uint32 rate);
extern uint16 I2c0ReadBytes(uint8 chip_addr,uint8 *rdat,int16 rnbyte);
extern uint16 I2c0WriteMemery(uint8 chip_addr, uint8 *addr, uint8 addr_len, void *wdat, uint16 wnbyte);
extern uint16 I2c0WriteBytes(uint8 chip_addr,uint8 *wdat,uint16 wnbyte);
extern uint16 I2c0WriteReadBytes(uint8 chip_addr, uint8 *wdat, uint8 wnbyte, void *rdat, uint16 rnbyte);
extern void I2c0_Exception(void);

#endif
