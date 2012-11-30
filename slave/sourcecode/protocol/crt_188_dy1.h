#ifndef _CRT_188_DY1_H
#define _CRT_188_DY1_H

#define CHECK_DATA_LENGHT			16
#define CRT188_MAX_DATA_LENGHT		135


#define CRT188_STX	0X02	//ͨѶ�ı���ʼ�ַ� 
#define CRT188_ETX	0X03	//ͨѶ�ı������ַ� 
#define CRT188_ENQ	0X05	//���������������->�������� 
#define CRT188_ACK	0X06	//�϶�Ӧ�𣨶�����->������ 
#define CRT188_NAK	0X15	//��Ӧ�𣨶�����->������ 
#define CRT188_EOT	0X04	//ȡ��ͨ�� 

#define CRT188_CMD_RESET			0x30		//��λ���������ؿ�������汾��Ϣ 
#define CRT188_CMD_READ_SERIAL		0x30		//��д���к�
#define CRT188_CMD_READ_STATE		0x31		//��״̬
#define CRT188_CMD_TEST_IC_TYPE		0x31		//�� IC ������
#define CRT188_CMD_IC_POWER_OP		0x33		//IC �����µ�
#define CRT188_CMD_SIM_POWER_OP		0x4A		//SIM ���µ�
#define CRT188_CMD_SET_UART_BPS		0x34		//���ô��ڲ�����

#define CRT188_ARV_RESET_1			0x30		//��λ���������ؿ�������汾��Ϣ 
#define CRT188_ARV_RESET_2			0x31		//��λ���������ؿ�������汾��Ϣ 
#define CRT188_ARV_RESET_3			0x32		//��λ���������ؿ�������汾��Ϣ
#define CRT188_ARV_SERIAL_READ		0x3A		//�����������к�  
#define CRT188_ARV_SERIAL_WRITE		0x3B		//д�����������к� 
#define CRT188_ARV_SERSOR_STATE		0x2F		//�鿨������������״̬��Ϣ  
#define CRT188_ARV_CARD_STATE		0x30		//�鿨��״̬�����������޿��ȣ���Ϣ
#define CRT188_ARV_IC_TYPE			0x31		//�� IC ������
#define CRT188_ARV_IC_POWER_ON		0x30		//IC ���ϵ�
#define CRT188_ARV_IC_POWER_OFF		0x31		//IC ���µ�
#define CRT188_ARV_SIM_POWER_OFF	0x31		//SIM ���µ� 
#define CRT188_ARV_1200_BPS			0x30		//UART=1200 BPS 
#define CRT188_ARV_2400_BPS			0x31		//UART=2400 BPS 
#define CRT188_ARV_4800_BPS			0x32		//UART=4800 BPS 
#define CRT188_ARV_9600_BPS			0x33		//UART=9600 BPS 
#define CRT188_ARV_19200_BPS		0x34		//UART=19200 BPS 
#define CRT188_ARV_38400_BPS		0x35		//UART=38400 BPS 

#define CARD_TYPE_NO_CARD			1
#define CARD_TYPE_UNKOWN_TYPE		2
#define CARD_TYPE_POS_ERR			3
#define CARD_TYPE_M1				4
#define CARD_TYPE_T0_CPU			5
#define CARD_TYPE_T1_CPU			6
#define CARD_TYPE_24C01				7
#define CARD_TYPE_24C02				8
#define CARD_TYPE_24C04				9
#define CARD_TYPE_24C08				10
#define CARD_TYPE_24C16				11
#define CARD_TYPE_24C32				12
#define CARD_TYPE_24C64				13
#define CARD_TYPE_SL4442			14
#define CARD_TYPE_SL4428			15
#define CARD_TYPE_AT88S102			16
#define CARD_TYPE_AT88S1604			17
#define CARD_TYPE_AT45D041			18
#define CARD_TYPE_AT88SC1608		19


typedef struct
{
	uint8  lenght_h;
	uint8  lenght_l;
	uint8  cmd;
	uint8  arg;
	uint8  dat_lenght;
	uint8  dat[CRT188_MAX_DATA_LENGHT];
} _crt188_cmd_s;

typedef struct
{
	uint8  		stx;
	_crt188_cmd_s	*data;
	uint8		ext;
	uint8		bcc;
} _crt188_s;

typedef struct
{
	uint8  stx;
	uint16 lenght;
	uint8  no;
	uint8  cmd;
	uint8  err_code;
	uint8  ext;
	uint8  bcc;
} _crt188_err_return_s;

typedef enum
{
    CRT188_CMD_ERR = 0x00,				//  �����ִ��󣬷��͵�ͨѶ�����в�����ͨѶЭ��涨�������� CM
    CRT188_ARG_ERR = 0x01,				//  ����������󣬷��͵�ͨѶ�����в�����ͨѶЭ��涨��������� PM
    CRT188_NO_EXE_ERR = 0x02,			//  ����ܱ�ִ�У����͵����������Ʋ���ִ�и����
    CRT188_DATA_ERR = 0x04				//  �������ݰ����󣬷��͵�ͨѶ�������ݰ������в�����ͨѶЭ��涨�����ݡ�
} _crt188_err_code_e;

typedef struct
{
	uint8 crt188_state;
	uint8 errcode;
	_card_plaintxt_u df_card;
} _card_read_s;
/*
typedef struct {

} _card_write_s;
*/
extern _card_plaintxt_u card_read;

extern void Crt188Init(void);
extern void TaskICCard(void *pdata);





#endif
