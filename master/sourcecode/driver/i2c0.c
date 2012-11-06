#include "includes.h"

static uint8 *i2c_rbuf;
static OS_EVENT *I2cSem;
static OS_EVENT *I2cMbox;
static int16 i2c_rnbyte;
static uint8 I2cAddr;
static uint8 i2c_mode;
static uint16 i2c_wnbyte;
static uint8 *i2c_wbuf;

/*********************************************************************************************************
** ��������: I2cInit
** ��������: ��ʼ��I2c����ģʽ��
** �䡡��: FI2c:I2c����Ƶ��
**
** �䡡��:TRUE  :�ɹ�
**        FALSE:ʧ��
** ȫ�ֱ���: I2cSem,I2cMbox
** ����ģ��: OSSemCreate
**
** ������: ������
** �ա���: 2003��7��8��
**-------------------------------------------------------------------------------------------------------
** �޸���: ������
** �ա���: 2003��7��10��
**-------------------------------------------------------------------------------------------------------
** �޸���: ������
** �ա���: 2003��7��21��
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
void I2c0Init(uint32 rate)

{
    VICIntEnClr = 1 << 9;                           /* ��ֹ��I2c�ж� */
    if (rate > 400000)
    {
    }
    PINSEL0 = (PINSEL0 & 0xffffff0f) | 0x50;    /* ѡ��ܽ�ΪI2c */
    I2CONCLR = 0x6C;                            /* ������ƼĴ��� */
    I2SCLH = (Fpclk / rate + 1) / 2;            /* ���øߵ�ƽʱ�� */
    I2SCLL = (Fpclk / rate) / 2;                /* ���õ͵�ƽʱ�� */
    I2cSem = OSSemCreate(1);                    /* �ź������ڻ���������� */
	if (I2cSem == NULL) {
		while(1);
	}
    I2cMbox = OSMboxCreate(NULL);               /* ��Ϣ���������ж������񴫵ݲ������ */
    if (I2cMbox == NULL)
    {
        while(1);
    }
}



/*********************************************************************************************************
** ��������: I2c0SendBytes
** ��������: ��I2C����������STOP�ź� 
** �䡡��: Addr:�ӻ���ַ
**        Data:��Ҫд������
**        д��������Ŀ
** �䡡��:���͵������ֽ���
**
** ȫ�ֱ���: I2cAddr,i2c_rnbyte,i2c_rbuf
** ����ģ��: OSMboxPend
**
** ������: ������
** �ա���: 2003��7��8��
**-------------------------------------------------------------------------------------------------------
** �޸���: ������
** �ա���: 2003��7��21��
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint16 I2c0WriteBytes(uint8 chip_addr, uint8 *wdat, uint16 wnbyte)
{
    uint8 err;
    unsigned int Rt;

	I2CONCLR = 0x6C;
	I2CONSET = 0x40;							/* ʹ��I2c */

	i2c_mode = 1;
    I2cAddr = chip_addr & 0xfe;                              /* �洢���͵�ַ */
    i2c_wnbyte = wnbyte;                                   /* �洢д�ֽ��� */
    i2c_wbuf = wdat;                                      /* �洢д�����ݵ�ָ�� */
    I2CONSET = 0x24;                                    /* ����Ϊ���������������� */
    VICIntEnable = 1 << 9;                      /* ʹ��I2c�ж� */
    Rt = (unsigned int) OSMboxPend(I2cMbox, 0, &err);   /* �ȴ��������� */
	VICIntEnClr = 1 << 9;					   /* ��ֹ��I2c�ж� */
    return Rt;
}

uint16 I2c0WriteMemery(uint8 chip_addr, uint8 *addr, uint8 addr_len, uint8 *wdat, uint16 wnbyte)
{
	uint16 err;
	err = I2c0WriteBytes(chip_addr,addr,addr_len);
	if (err != I2C_WRITE_END)
	{
		return err;
	}
	err = I2c0WriteBytes(chip_addr,wdat,wnbyte);
	return err;
}

/*********************************************************************************************************
** ��������: I2cRead
** ��������: ��I2c������������
** �䡡��: Addr:�ӻ���ַ
**        Ret:ָ�򷵻����ݴ洢λ�õ�ָ��
**        Eaddr:��չ��ַ�洢λ��
**        EaddrNByte:��չ��ַ�ֽ�����0Ϊ��
**        ReadNbyte:��Ҫ��ȡ���ֽ���Ŀ
** �䡡��:�Ѷ�ȡ���ֽ���
**
** ȫ�ֱ���: I2cSem,I2cAddr,i2c_rnbyte,i2c_rbuf
** ����ģ��: OSSemPend,__I2cWrite,OSMboxPend,OSSemPost
**
** ������: ������
** �ա���: 2003��7��8��
**-------------------------------------------------------------------------------------------------------
** �޸���: ������
** �ա���: 2003��7��21��
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
uint16 I2c0ReadBytes(uint8 chip_addr,uint8 *rdat,int16 rnbyte)
{
    uint8 err;
    
    //OSSemPend(I2cSem, 0, &err);

    I2CONCLR = 0x6C;
    I2CONSET = 0x40;                            /* ʹ��I2c */
	i2c_mode = 2;
    I2cAddr = chip_addr | 0x01;                  /* �洢���͵�ַ */
    i2c_rnbyte = rnbyte;                      	 /* �洢���ֽ��� */
    i2c_rbuf = rdat;                               /* �洢���������� */
    I2CONCLR = 0x28;
    I2CONSET = 0x24;                            /* ����Ϊ���������������� */
    VICIntEnable = 1 << 9;                      /* ʹ��I2c�ж� */

    OSMboxPend(I2cMbox, 0, &err);               /* �ȴ��������� */

    VICIntEnClr = 1 << 9;                       /* ��ֹ��I2c�ж� */
    //OSSemPost(I2cSem);
    return (rnbyte - i2c_rnbyte);
}

uint16 I2c0WriteReadBytes(uint8 chip_addr, uint8 *wdat, uint8 wnbyte, uint8 *rdat, uint16 rnbyte){
	uint8 err;
	
	err = 0;
	I2CONCLR = 0x6C;
	I2CONSET = 0x40;							/* ʹ��I2c */
	
	i2c_mode = 3;
    I2cAddr = chip_addr & 0xfe;                              /* �洢���͵�ַ */
    i2c_rnbyte = rnbyte;                                   /* �洢д�ֽ��� */
    i2c_rbuf = rdat;                                      /* �洢д�����ݵ�ָ�� */
	i2c_wbuf = wdat;
	i2c_wnbyte = wnbyte;
	I2CONCLR = 0x28;
    I2CONSET = 0x24;                                    /* ����Ϊ���������������� */
	VICIntEnable = 1 << 9;                      /* ʹ��I2c�ж� */
	OSMboxPend(I2cMbox, 0, &err);				/* �ȴ��������� */
	
	VICIntEnClr = 1 << 9;					   /* ��ֹ��I2c�ж� */
	//OSSemPost(I2cSem);
	return (rnbyte - i2c_rnbyte);
	
}

/*********************************************************************************************************
** ��������: I2c_Exception
** ��������: I2c�жϷ������
** �䡡��: ��
**
** �䡡��: ��
**         
** ȫ�ֱ���: I2cAddr,i2c_rbuf,i2c_rnbyte,I2cMbox
** ����ģ��: OSMboxPost
**
** ������: ������
** �ա���: 2003��7��8��
**-------------------------------------------------------------------------------------------------------
** �޸���: ������
** �ա���: 2003��7��21��
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
        void I2c0_Exception(void)
{
    OS_ENTER_CRITICAL();
    switch(I2STAT & 0xf8)
    {
        case  0x08:                     /* �ѷ�����ʼ��������0x18��ͬ���� */
//            break;
        case  0x10:                     /* �ѷ����ظ���ʼ���� */
            I2DAT = I2cAddr;            /* ���͵�ַ */
            I2CONCLR = 0x28;            /* �����־ */
            break;
        case  0x18:                     /* �ѷ���SLA+W�����ѽ���Ӧ�� */
            I2DAT = *i2c_wbuf++;
            i2c_wnbyte--;
            I2CONCLR = 0x28;            /* �����־ */
            break;
        case  0x28:                     /* �ѷ���I2C���ݣ������յ�Ӧ�� */
			
            if (i2c_wnbyte > 0)
            {
                I2DAT = *i2c_wbuf++;
                i2c_wnbyte--;
                I2CONCLR = 0x28;        /* �����־ */
            }
            else
            {
	            if (i2c_mode == 3) {
					//I2DAT = I2cAddr | 0x01;
					I2cAddr = I2cAddr&(~0x01)|0x01;
					I2CONCLR = 0x28;
					I2CONSET = 0x24;
	            }
				else {
	                OSMboxPost(I2cMbox, (void *)I2C_WRITE_END);
	                VICIntEnClr = 1 << 9;   /* ��ֹ��I2c�ж� */
				}
			}
            break;
        case  0x20:                     /* �ѷ���SLA+W���ѽ��շ�ACK, ��0x48������ͬ */
//            break;                    
        case  0x30:                     /* �ѷ���I2DAT�е������ֽڣ��ѽ��շ�ACK, ��0x48������ͬ */
//            break;
        case  0x48:                     /* �ѷ���SLA+R���ѽ��շ�ACK */
            I2CONSET = 1 << 4;          /* ����ֹͣ�ź� */
            OSMboxPost(I2cMbox, (void *)I2C_ACK_ERR);
            I2CONCLR = 0x28;            /* �����־ */
            break;
        case  0x38:                     /* ��SLA+R/W�������ֽ��ж�ʧ�ٲ� */
            OSMboxPost(I2cMbox, (void *)I2C_NOT_GET_BUS);
            I2CONCLR = 0x28;            /* �����־ */
            break;
        case  0x40:                     /* �ѷ���SLA+R���ѽ���ACK */
            if (i2c_rnbyte <= 1)
            {
                I2CONCLR = 1 << 2;      /* �´η��ͷ�Ӧ���ź� */
            }
            else
            {
                I2CONSET= 1 << 2;       /* �´η���Ӧ���ź� */
            }
            I2CONCLR = 0x28;            /* �����־ */
            break;
        case  0x50:                     /* �ѽ��������ֽڣ��ѷ���ACK */
            *i2c_rbuf++ = I2DAT;          /* �������� */
            i2c_rnbyte--;
            if (i2c_rnbyte <= 1)
            {
                I2CONCLR = 1 << 2;      /* �´η��ͷ�Ӧ���ź� */
            }
            I2CONCLR = 0x28;            /* �����־ */
            break;
        case  0x58:                     /* �ѽ��������ֽڣ��ѷ�����ACK */
            *i2c_rbuf = I2DAT;            /* �������� */
            i2c_rnbyte--;
            I2CONSET= 1 << 4;           /* �������� */
            OSMboxPost(I2cMbox, (void *)I2C_READ_END);
            I2CONCLR = 0x28;            /* �����־ */
            break;
        default:
            I2CONCLR = 0x28;            /* �����־ */
            break;
    }

    VICVectAddr = 0;                    /* ֪ͨ�жϿ������жϽ��� */
    OS_EXIT_CRITICAL();
}

