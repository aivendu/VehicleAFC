#ifndef _I2C_H
#define _I2C_H



#define I2C_WRITE_END       1           /* д���   */
#define I2C_READ_END        2           /* �����   */
#define I2C_NOT_GET_BUS     4           /* ��ʧ�ٲ� */
#define I2C_ACK_ERR         8           /* ����ACK���� */


extern void I2c0Init(uint32 rate);
extern uint16 I2c0ReadBytes(uint8 chip_addr,uint8 *rdat,int16 rnbyte);
extern uint16 I2c0WriteBytes(uint8 chip_addr,uint8 *wdat,uint16 wnbyte);
extern uint16 I2c0WriteReadBytes(uint8 chip_addr, uint8 *wdat, uint8 wnbyte, uint8 *rdat, uint16 rnbyte);
extern void I2c0_Exception(void);

#endif
