#include "includes.h"

/*
* 	log�洢�ṹ:
*	���ݿ�
*	|
*	|---	���һ��log ������(��ʼ)��ַ  --	���ݿ����ʼ��ַ
*	|---	��һ��log ����
*	|	|_ _ 	log �����
*	|	|	|_ _		��������
*	|	|	|_ _		���ݳ���
*	|	|	|_ _		��һ�����ݵ�ַ	
*	|	|
*	|	|_ _ 	log���ݿ�
*	|
*	|---	�ڶ������ݿ�
*	|---	��3-n �����ݿ�
*/
OS_EVENT *store_log_sem;
_log_manage_s	log_index;			//	���һ��log �Ĺ�����ַ
_log_s	log_data;


void InitLog(void)
{
	store_log_sem = OSSemCreate(0);
	while (store_log_sem == NULL);
	
	ReadExternMemery(&log_index,LOG_START_ADDR,sizeof(_log_manage_s));
	//	�����û�д����log �������ݴ���
	if ((log_index.log_end <= LOG_START_ADDR) || (log_index.log_end >= (LOG_START_ADDR + LOG_MEMERY_LENGHT)))
	{
		log_index.log_end = LOG_START_ADDR;
		memset(&log_data,0,sizeof(_log_s));			//	����Ҫ��ȡlog 
	}
	else
	{
		ReadExternMemery(&log_data,log_index.log_end,sizeof(_log_s));	//	��ȡ���һ��log ��ͷ
	}
}


/*
*	
*/
uint8 StoreLog(uint16 type, void *data, uint16 data_len)
{
	uint8 err;
	uint32 len_temp;
	OSSemPend(store_log_sem,0,&err);
	if (log_index.log_end == LOG_START_ADDR)		//	�����û�д洢��־
	{
		log_data.type = type;						//	�洢��һ����־
		log_data.lenght = data_len;
		log_data.last_log = NULL;
		log_index.log_end = LOG_START_ADDR + sizeof(_log_manage_s);
	}
	else
	{
		//	�洢
		log_index.log_end = log_index.log_end + sizeof(_log_s) + log_data.lenght;	//	�޸ĵ�ǰ��־����
		//	�޸���־����
		log_data.type = type;
		log_data.lenght = data_len;
		log_data.last_log = log_index.log_end;
	}
	//	���ʣ��洢�����Դ���־ͷ���֣���ӵ�ǰλ��д��־���������ݴ���־�洢����ʼд
	if ((log_index.log_end + sizeof(_log_s)) < (LOG_START_ADDR + LOG_MEMERY_LENGHT))
	{
		WriteExternMemery(&log_data, log_index.log_end, sizeof(_log_s));
		//	��֤����û��д���������ݻ���
		len_temp = log_data.last_log;		//	����������־
		memset(&log_data,0,sizeof(_log_s));
		ReadExternMemery(&log_data, log_index.log_end, sizeof(_log_s));		//	������
		if (len_temp != log_data.last_log)	//	�Ƚ����ݣ��Ƿ�д��
		{
			ReadExternMemery(&log_data, len_temp, sizeof(_log_s));		//	��ԭ��������
			log_index.log_end = len_temp;
			OSSemPost(store_log_sem);
			return FALSE;					//	д����
		}
	}
	else
	{
		log_index.log_end = LOG_START_ADDR + sizeof(_log_manage_s);
		WriteExternMemery(&log_data, log_index.log_end, sizeof(_log_s));
		//	��֤����û��д���������ݻ���
		len_temp = log_data.last_log;
		memset(&log_data,0,sizeof(_log_s));
		ReadExternMemery(&log_data, log_index.log_end, sizeof(_log_s));
		if (len_temp != log_data.last_log)
		{
			ReadExternMemery(&log_data, len_temp, sizeof(_log_s));		//	��ԭ��������
			log_index.log_end = len_temp;
			OSSemPost(store_log_sem);
			return FALSE;
		}
	}
	//	��������־ͷд��־����
	if	((log_index.log_end + sizeof(_log_s) + log_data.lenght) < (LOG_START_ADDR + LOG_MEMERY_LENGHT))
	{
		WriteExternMemery(data, (log_index.log_end + sizeof(_log_s)), data_len);
	}
	else
	{
		len_temp = LOG_START_ADDR + LOG_MEMERY_LENGHT - log_index.log_end - sizeof(_log_s);
		if (len_temp != 0)
		{
			WriteExternMemery(data, (log_index.log_end + sizeof(_log_s)),len_temp);
		}
		WriteExternMemery((void *)((uint32)data + len_temp), (LOG_START_ADDR + sizeof(_log_manage_s)), data_len - len_temp);
	}
	//	д��־��������
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
*	��log ����
*	����log ͷ���ݣ���ȡ��log �����data ��
*/
void * ReadLog(uint8 flag,void *data,uint16 data_lenght)
{
	uint8 err;
	uint32 len_temp;
	static uint32 log_read_index = 0;
	_log_s log_temp;
	if ((log_read_index == 0) || (log_read_index >= (LOG_START_ADDR + LOG_MEMERY_LENGHT)))
	{
		log_read_index = log_index.log_end;
	}
	OSSemPend(store_log_sem,0,&err);
	if (flag == 0)			//	����һ�����ݵĿ�ͷ
	{
		ReadExternMemery(&log_temp, log_read_index, sizeof(_log_s));	//	����ǰlog ��
		if ((log_temp.last_log > log_index.log_end) && (log_temp.last_log < (log_index.log_end + log_data.lenght)))
		{
			//	�Ѿ�����ǰ�����־���һ��־�ѱ�����
			log_read_index = log_index.log_end;								//	�л���ǰ��
		}
		else
		{
			log_read_index = log_temp.last_log;								//	ȡ��һ��LOG ������
		}
		ReadExternMemery(&log_temp, log_read_index, sizeof(_log_s));		//	����ǰ��ͷ
		OSSemPost(store_log_sem);
		return &log_temp;
	}
	else if (flag == 1)		//	����һ�����ݵĿ�ͷ
	{
		ReadExternMemery(&log_temp, log_read_index, sizeof(_log_s));	//	����ǰlog ��
		if (log_read_index != log_index.log_end)
		{
			log_read_index = log_temp.last_log;								//	ȡ��һ��LOG ������
		}
		ReadExternMemery(&log_temp, log_read_index, sizeof(_log_s));		//	����ǰ��ͷ
		OSSemPost(store_log_sem);
		return &log_temp;
	}
	else if (flag == 2)		//	������
	{
		ReadExternMemery(&log_temp, log_read_index, sizeof(_log_s));	//	����ǰlog ��
		if ((log_temp.lenght + log_read_index) < (LOG_START_ADDR + LOG_MEMERY_LENGHT))
		{
			ReadExternMemery(data,log_read_index + sizeof(_log_s),data_lenght);
		}
		else
		{
			len_temp = LOG_START_ADDR + LOG_MEMERY_LENGHT - log_temp.lenght - log_read_index;
			ReadExternMemery(data,log_read_index + sizeof(_log_s),len_temp);
			ReadExternMemery((void *)((uint32)data + len_temp),LOG_START_ADDR + sizeof(_log_manage_s), data_lenght - len_temp);
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














