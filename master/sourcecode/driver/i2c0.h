#ifndef _I2C_H
#define _I2C_H

#define MEMERY_ADDR_MODE	1				/*0 -- 小端模式，1 -- 大端模式*/	

#define I2C_WRITE_END       0x101           /* 写完成   */
#define I2C_READ_END        0x102           /* 读完成   */
#define I2C_NOT_GET_BUS     0x104           /* 丢失仲裁 */
#define I2C_ACK_ERR         0x108           /* 接收ACK错误 */


extern void I2c0Init(uint32 rate);
extern uint16 I2c0ReadBytes(uint8 chip_addr,uint8 *rdat,int16 rnbyte);
extern uint16 I2c0WriteMemery(uint8 chip_addr, uint8 *addr, uint8 addr_len, void *wdat, uint16 wnbyte);
extern uint16 I2c0WriteBytes(uint8 chip_addr,uint8 *wdat,uint16 wnbyte);
extern uint16 I2c0WriteReadBytes(uint8 chip_addr, uint8 *wdat, uint8 wnbyte, void *rdat, uint16 rnbyte);
extern void I2c0_Exception(void);

#endif
