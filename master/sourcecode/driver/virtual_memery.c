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
*	函数原型:	uint8 ReadExternMemery(void *data, uint32 addr, uint32 len)
*	函数描述:	读取外部存储器中的数据。此处的地址是虚拟地址，
*					虚拟地址是一个连续的地址，从0x00000000开始。
*	函数参数:	void *data	--	读取的数据存放的地址
*					uint32 addr	--	读取数据的地址，为虚拟地址
*					uint32 len	--	读取数据的长度
*	函数返回:	读取结果
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
		//	计算临时地址，芯片的物理地址
		addr_temp = addr - memery_chip[i].virtual_start_addr + memery_chip[i].memery_start_addr;
		//	存储数据，判断存储器是否够用
		if (len > memery_chip[i].memery_size - addr_temp)
		{
			//	数据不是存储在单一存储器里
			len_temp = memery_chip[i].memery_size - addr_temp;
			I2c0WriteReadBytes(memery_chip[i].io_addr,(uint8 *)&addr,2,data,len_temp);
			i++;
		}
		else
		{
			//	数据可以在当前存储器里全部存储
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
			//	数据不是存储在单一存储器里
			len_temp = memery_chip[i].memery_size - addr_temp;
			I2c0WriteMemery(memery_chip[i].io_addr,(uint8 *)&addr_temp,2,data,len_temp);
			i++;
		}
		else
		{
			//	数据可以在当前存储器里全部存储
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

 




