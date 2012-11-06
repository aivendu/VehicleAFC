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
uint32	log_index;			//	最后一个log 的管理块地址
_log_s	log_manage;


void InitLog(void)
{
	store_log_sem = OSSemCreate(0);
	while (store_log_sem == NULL);
	
	ReadExternMemery(&log_index,LOG_START_ADDR,4);
	ReadExternMemery(&log_manage,log_index,sizeof(_log_s));
}

void StoreLog(uint16 type, void *data, uint16 data_len)
{
	uint8 err;
	OSSemPend(store_log_sem,0,&err);
	
	log_manage.type = type;
	log_manage.lenght = data_len;
	log_manage.last_log = log_index;
	log_index = log_index + sizeof(_log_s) + data_len;
	WriteExternMemery(&log_manage, log_index, sizeof(_log_s));
	WriteExternMemery(data, log_index + sizeof(_log_s), data_len);
	WriteExternMemery(&log_index, LOG_START_ADDR, 4);
	
	OSSemPost(store_log_sem);
}

void *ReadLog(uint8 flag,uint16 data_lenght)
{
	uint8 err;
	static uint32 log_read_index = 100;
	_log_s log_temp;
	OSSemPend(store_log_sem,0,&err);
	log_read_index = 200;
	//ReadExternMemery(,uint32 addr,uint32 len)
	OSSemPost(store_log_sem);
	return NULL;
}














