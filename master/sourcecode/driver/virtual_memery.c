#include "includes.h"

const _Memery_chip_manage_s	memery_chip[] =
{
	{CAT1025_ADDR	,	CAT1025_SIZE	,	0,		CAT1025_VIRTUAL_ADDR		},
	{FM24V10_1_0_ADDR,	FM24V10_1_0_SIZE,	0,		FM24V10_1_0_VIRTUAL_ADDR	},
	{FM24V10_1_1_ADDR,	FM24V10_1_1_SIZE,	0,		FM24V10_1_1_VIRTUAL_ADDR	},
	{FM24V10_2_0_ADDR,	FM24V10_2_0_SIZE,	0,		FM24V10_2_0_VIRTUAL_ADDR	},
	{FM24V10_2_1_ADDR,	FM24V10_2_1_SIZE,	0,		FM24V10_2_1_VIRTUAL_ADDR	},
	{FM24V10_3_0_ADDR,	FM24V10_3_0_SIZE,	0,		FM24V10_3_0_VIRTUAL_ADDR	},
	{FM24V10_3_1_ADDR,	FM24V10_3_1_SIZE,	0,		FM24V10_3_1_VIRTUAL_ADDR	},
};

/**************************************************************************
*	����ԭ��:	uint8 ReadExternMemery(void *data, uint32 addr, uint32 len)
*	��������:	��ȡ�ⲿ�洢���е����ݡ��˴��ĵ�ַ�������ַ��
*					�����ַ��һ�������ĵ�ַ����0x00000000��ʼ��
*	��������:	void *data	--	��ȡ�����ݴ�ŵĵ�ַ
*					uint32 addr	--	��ȡ���ݵĵ�ַ��Ϊ�����ַ
*					uint32 len	--	��ȡ���ݵĳ���
*	��������:	��ȡ���
*
***************************************************************************/
uint16 ReadExternMemery(void *data, uint32 addr, uint32 len)
{
	uint8 i;
	uint32 len_temp,addr_temp;
	addr = 0x00;
	for (i = 0; i < EXTERN_CHIP_WITH_IO_NUM; i++)
	{
		if (addr < memery_chip[i].virtual_start_addr)
		{
			i--;
			break;
		}
	}
	if (i == EXTERN_CHIP_WITH_IO_NUM)
	{
		return EXTERN_MEMERY_ADDR_OVERFLOW;
	}
	while (1)
	{
		//	������ʱ��ַ��оƬ�������ַ
		addr_temp = addr - memery_chip[i].virtual_start_addr + memery_chip[i].memery_start_addr;
		//	�洢���ݣ��жϴ洢���Ƿ���
		if (len > memery_chip[i].memery_size - addr_temp)
		{
			//	���ݲ��Ǵ洢�ڵ�һ�洢����
			len_temp = memery_chip[i].memery_size - addr_temp;
			I2c0WriteReadBytes(memery_chip[i].io_addr,(uint8 *)&addr,2,data,len_temp);
			i++;
		}
		else
		{
			//	���ݿ����ڵ�ǰ�洢����ȫ���洢
			len_temp = len;
			I2c0WriteReadBytes(memery_chip[i].io_addr,(uint8 *)&addr_temp,2,data,len_temp);
			return 0;
		}
		data = (void *)((uint32)data + len_temp);
		len = len - len_temp;
		addr += len_temp;
		i++;
	}
}

uint16 WriteExternMemery(void *data, uint32 addr, uint32 len)
{
	uint8 i;
	uint32 len_temp,addr_temp;
	for (i = 0; i < EXTERN_CHIP_WITH_IO_NUM; i++)
	{
		if (addr < memery_chip[i].virtual_start_addr)
		{
			i--;
			break;
		}
	}
	if (i == EXTERN_CHIP_WITH_IO_NUM)
	{
		return EXTERN_MEMERY_ADDR_OVERFLOW;
	}
	while (1)
	{
		addr_temp = addr - memery_chip[i].virtual_start_addr + memery_chip[i].memery_start_addr;
		if (len > memery_chip[i].memery_size - addr_temp)
		{
			//	���ݲ��Ǵ洢�ڵ�һ�洢����
			len_temp = memery_chip[i].memery_size - addr_temp;
			I2c0WriteMemery(memery_chip[i].io_addr,(uint8 *)&addr_temp,2,data,len_temp);
			i++;
		}
		else
		{
			//	���ݿ����ڵ�ǰ�洢����ȫ���洢
			len_temp = len;
			I2c0WriteMemery(memery_chip[i].io_addr,(uint8 *)&addr_temp,2,data,len_temp);
			return 0;
		}
		data = (void *)((uint32)data + len_temp);
		len = len - len_temp;
		addr += len_temp;
		i++;
	}
}

void InitConfigData (void) {
	return;
}

 




