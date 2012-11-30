#include "includes.h"

/*
* 	log存储结构:
*	数据块
*	|
*	|---	最后一个log 的索引(起始)地址  --	数据块的起始地址
*	|---	第一个log 数据
*	|	|_ _ 	log 管理块
*	|	|	|_ _		数据类型
*	|	|	|_ _		数据长度
*	|	|	|_ _		上一个数据地址
*	|	|
*	|	|_ _ 	log数据块
*	|
*	|---	第二个数据块
*	|---	第3-n 个数据块
*/
OS_EVENT *store_log_sem;
_log_manage_s	log_index;			//	最后一个log 的管理块地址
_log_s	log_data;


void InitLog(void)
{
	_log_s *log_temp;
	store_log_sem = OSSemCreate(1);
	while (store_log_sem == NULL);

	ReadExternMemery(&log_index, LOG_START_ADDR, sizeof(_log_manage_s));
	//	如果还没有存入过log 或者数据错误
	if ((log_index.log_end <= LOG_START_ADDR) || (log_index.log_end >= (LOG_START_ADDR + LOG_MEMERY_LENGHT)))
	{
		log_index.log_end = LOG_START_ADDR;
		memset(&log_data, 0, sizeof(_log_s));			//	不需要读取log
	}
	else
	{
		ReadExternMemery(&log_data, log_index.log_end, sizeof(_log_s));	//	读取最后一个log 块头
		//log_temp = ReadLog(0,NULL,0,log_index.log_start);
		while (1)
		{
			if ((log_data.lenght + log_index.log_end + sizeof(_log_s)) >= (LOG_START_ADDR + LOG_MEMERY_LENGHT))
			{
				//	当前log 记录有越界
				if ((log_temp->last_log < ((log_data.lenght + log_index.log_end + sizeof(_log_s)) - (LOG_START_ADDR + LOG_MEMERY_LENGHT)))
				        || (log_temp->last_log >= log_index.log_end))
				{
					//	上一数据已被覆盖
					break;
				}
			}
			else
			{
				if ((log_temp->last_log < ((log_data.lenght + log_index.log_end + sizeof(_log_s))))
				        || (log_temp->last_log >= log_index.log_end))
				{
					//	上一数据已被覆盖
					break;
				}
			}
			if ((log_temp->state != 0) || (log_temp == NULL))
			{
				break;
			}
			//log_temp = ReadLog(0,NULL,0,log_temp->last_log);
		}
	}
}


/*
*
*/
uint8 StoreLog(uint16 type, void *data, uint16 data_len)
{
	uint8 err;
	uint32 len_temp;
	OSSemPend(store_log_sem, 0, &err);
	if (log_index.log_end == LOG_START_ADDR)		//	如果还没有存储日志
	{
		log_data.type = type;						//	存储第一个日志
		log_data.lenght = data_len;
		log_data.last_log = LOG_START_ADDR;
		log_index.log_end = LOG_START_ADDR + sizeof(_log_manage_s);
	}
	else
	{
		//	存储
		log_data.last_log = log_index.log_end;
		log_index.log_end = log_index.log_end + sizeof(_log_s) + log_data.lenght;	//	修改当前日志索引
		//	修改日志数据
		log_data.type = type;
		log_data.lenght = data_len;
	}
	//	如果剩余存储器可以存日志头部分，则从当前位置写日志，否则数据从日志存储器开始写
	if ((log_index.log_end + sizeof(_log_s)) < (LOG_START_ADDR + LOG_MEMERY_LENGHT))
	{
		WriteExternMemery(&log_data, log_index.log_end, sizeof(_log_s));
		//	保证数据没有写错，导致数据混乱
		len_temp = log_data.last_log;		//	保存上条日志
		memset(&log_data, 0, sizeof(_log_s));
		ReadExternMemery(&log_data, log_index.log_end, sizeof(_log_s));		//	读数据
		if (len_temp != log_data.last_log)	//	比较数据，是否写错
		{
			ReadExternMemery(&log_data, len_temp, sizeof(_log_s));		//	还原数据数据
			log_index.log_end = len_temp;
			OSSemPost(store_log_sem);
			return FALSE;					//	写错返回
		}
	}
	else
	{
		log_index.log_end = LOG_START_ADDR + sizeof(_log_manage_s);
		WriteExternMemery(&log_data, log_index.log_end, sizeof(_log_s));
		//	保证数据没有写错，导致数据混乱
		len_temp = log_data.last_log;
		memset(&log_data, 0, sizeof(_log_s));
		ReadExternMemery(&log_data, log_index.log_end, sizeof(_log_s));
		if (len_temp != log_data.last_log)
		{
			ReadExternMemery(&log_data, len_temp, sizeof(_log_s));		//	还原数据数据
			log_index.log_end = len_temp;
			OSSemPost(store_log_sem);
			return FALSE;
		}
	}
	//	紧接着日志头写日志数据
	if	((log_index.log_end + sizeof(_log_s) + log_data.lenght) < (LOG_START_ADDR + LOG_MEMERY_LENGHT))
	{
		WriteExternMemery(data, (log_index.log_end + sizeof(_log_s)), data_len);
	}
	else
	{
		len_temp = LOG_START_ADDR + LOG_MEMERY_LENGHT - log_index.log_end - sizeof(_log_s);
		if (len_temp != 0)
		{
			WriteExternMemery(data, (log_index.log_end + sizeof(_log_s)), len_temp);
		}
		WriteExternMemery((void *)((uint32)data + len_temp), (LOG_START_ADDR + sizeof(_log_manage_s)), data_len - len_temp);
	}
	//	写日志管理数据
	WriteExternMemery(&log_index, LOG_START_ADDR, sizeof(_log_manage_s));
	len_temp = log_index.log_end;
	log_index.log_end = 0;
	ReadExternMemery(&log_index, LOG_START_ADDR, sizeof(_log_manage_s));
	if (len_temp != log_index.log_end)
	{

	}

	OSSemPost(store_log_sem);
	return TRUE;
}

/*
*	读log 函数
*	返回log 头数据，读取的log 存放在data 中
*/
_log_s *ReadLog(uint8 flag, void *data, uint16 data_lenght, uint32 read_index)
{
	uint8 err;
	uint32 len_temp;
	static uint32 log_read_index = 0;
	_log_s log_temp;
	OSSemPend(store_log_sem, 0, &err);

	if (flag == 0)
	{
		log_read_index = read_index;
	}
	else if ((log_read_index == 0) || (log_read_index >= (LOG_START_ADDR + LOG_MEMERY_LENGHT)))
	{
		//	上电后LOG 还没开始读过
		log_read_index = log_index.log_end;
	}
	if ((log_read_index + sizeof(_log_s)) >= (LOG_START_ADDR + LOG_MEMERY_LENGHT))
	{
		//	地址不正确
		return NULL;
	}
	if (flag == 0)
	{
		if (ReadExternMemery(&log_temp, log_read_index, sizeof(_log_s)) != 0)	//	读当前log 块
		{
			OSSemPost(store_log_sem);
			return NULL;
		}
		else
		{
			OSSemPost(store_log_sem);
			return &log_temp;
		}
	}
	else if (flag == 1)			//	读上一个数据的块头
	{
		ReadExternMemery(&log_temp, log_read_index, sizeof(_log_s));	//	读当前log 块
		if ((log_data.lenght + log_index.log_end + sizeof(_log_s)) >= (LOG_START_ADDR + LOG_MEMERY_LENGHT))
		{
			//	当前log 记录有越界
			if ((log_temp.last_log < ((log_data.lenght + log_index.log_end + sizeof(_log_s)) - (LOG_START_ADDR + LOG_MEMERY_LENGHT)))
			        || (log_temp.last_log >= log_index.log_end))
			{
				//	上一数据已被覆盖
				log_read_index = log_index.log_end;			//	切换当前块
			}
			else
			{
				log_read_index = log_temp.last_log;			//	取上一个LOG 的索引
			}
		}
		else
		{
			if ((log_temp.last_log < ((log_data.lenght + log_index.log_end + sizeof(_log_s))))
			        || (log_temp.last_log >= log_index.log_end))
			{
				//	上一数据已被覆盖
				log_read_index = log_index.log_end;								//	切换当前块
			}
			else
			{
				log_read_index = log_temp.last_log;			//	取上一个LOG 的索引
			}
		}
		ReadExternMemery(&log_temp, log_read_index, sizeof(_log_s));		//	读当前块头
		OSSemPost(store_log_sem);
		return &log_temp;
	}
	else if (flag == 2)		//	读下一个数据的块头
	{
		ReadExternMemery(&log_temp, log_read_index, sizeof(_log_s));	//	读当前log 块
		if (log_read_index != log_index.log_end)
		{
			//	取下一个LOG 的索引
			if ((log_read_index + sizeof(_log_s)) >= (LOG_START_ADDR + LOG_MEMERY_LENGHT))
			{
				//	下一个log 已越界
				log_read_index = LOG_START_ADDR + sizeof(_log_manage_s);
			}
			else
			{
				log_read_index = log_read_index + sizeof(_log_s);
			}
			ReadExternMemery(&log_temp, log_read_index, sizeof(_log_s));	//	读当前块头
		}
		OSSemPost(store_log_sem);
		return &log_temp;
	}
	else if (flag == 3)		//	读数据
	{
		ReadExternMemery(&log_temp, log_read_index, sizeof(_log_s));	//	读当前log 块
		if ((log_temp.lenght + log_read_index + sizeof(_log_s)) < (LOG_START_ADDR + LOG_MEMERY_LENGHT))
		{
			ReadExternMemery(data, log_read_index + sizeof(_log_s), data_lenght);
		}
		else
		{
			len_temp = LOG_START_ADDR + LOG_MEMERY_LENGHT - log_temp.lenght - log_read_index;
			ReadExternMemery(data, log_read_index + sizeof(_log_s), len_temp);
			ReadExternMemery((void *)((uint32)data + len_temp), LOG_START_ADDR + sizeof(_log_manage_s), data_lenght - len_temp);
		}
		OSSemPost(store_log_sem);
		return &log_temp;
	}
	else
	{
		OSSemPost(store_log_sem);
		return NULL;
	}
}


//	保存登录log
uint8 LogStoreLogin(void)
{
	_log_device_use_cmd_s temp;
	memset(&temp, 0, sizeof(_log_device_use_cmd_s));
	memcpy(temp.staffid, device_control.user.uinfo.staffid, 7);
	temp.year = YEAR;
	temp.month = MONTH;
	temp.day = DOM;
	temp.hour = HOUR;
	temp.min = MIN;
	temp.sec = SEC;
	//StoreLog(LOGIN_COMMAND,&temp,sizeof(_log_device_use_cmd_s));
	return TRUE;
}
//	保存注销log
uint8 LogStoreLogout(void)
{
	_log_device_use_cmd_s temp;
	memset(&temp, 0, sizeof(_log_device_use_cmd_s));
	memcpy(temp.staffid, device_control.user.uinfo.staffid, 7);
	temp.year = YEAR;
	temp.month = MONTH;
	temp.day = DOM;
	temp.hour = HOUR;
	temp.min = MIN;
	temp.sec = SEC;
	//StoreLog(LOGOUT_COMMAND,&temp,sizeof(_log_device_use_cmd_s));
	return TRUE;
}

//	保存存钱log
uint8 LogStoreDeposit(void)
{
	_log_deposit_cmd_s temp;
	memset(&temp, 0, sizeof(_log_deposit_cmd_s));
	memcpy(temp.staffid, device_control.user.uinfo.staffid, 7);
	temp.cashbox1_amount = GetCashbox1Deposit();
	temp.cashbox2_amount = GetCashbox2Deposit();
	temp.cashbox3_amount = GetCashbox3Deposit();
	temp.year = YEAR;
	temp.month = MONTH;
	temp.day = DOM;
	temp.hour = HOUR;
	temp.min = MIN;
	temp.sec = SEC;
	//StoreLog(LOGOUT_COMMAND,&temp,sizeof(_log_deposit_cmd_s));
	return TRUE;
}





