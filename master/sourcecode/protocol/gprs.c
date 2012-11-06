#include "includes.h"

#define MAX_GPRS_AVG_BUF		30
#define MAX_GPRS_SEND_BUF		200

static OS_EVENT *gprs_send_mbox, *gdtu_sent_successfully_sem;


uint8 GdtuReturnDat(uint8 *buf);
void GPRSSendData(char * dat,uint16 ev_num);


void TaskGPRS(void *pdata){

	uint8 err,temp_8;
	uint8 gprs_buf[MAX_GPRS_AVG_BUF];
	uint8 gprs_data_buf[MAX_GPRS_SEND_BUF];
	_mbox_data_s *buf;
	//_mbox_data_s mbox_buf;
	uint32 temp_32;
	uint16 year_temp;
	uint8  mon_temp,day_temp,hour_temp,min_temp,sec_temp;
	uint16 event_num=0;
	_df_afc_trade_log_u *log_temp;
	pdata = pdata;

	while(1) {

		buf = OSMboxPend(gprs_send_mbox,0,&err);
		
		if (err == OS_NO_ERR) {
			if (buf->mod == GDTU_SEND_LOG) {
				temp_8 = TRADE_LOG_S_LENGTH;
			}
			gprs_buf[0] = GDTU_SEND_LOG;
			memcpy(&gprs_buf[2],(char *)(buf->dat),TRADE_LOG_S_LENGTH);
			OSSemPend(Uart0_Channel_Sem,0,&err);
			if (err == OS_NO_ERR) {
				flagcom = 2;
				UART0_Init(IC_CHANNEL2);
				switch (gprs_buf[0]) {
					case GDTU_SEND_LOG:
						event_num ++;				// 发送事件号递增。
						log_temp = (_df_afc_trade_log_u *)(&gprs_buf[2]);
						temp_32 = log_temp->trade_log.trade_time[0];
						temp_32 = (temp_32<<8)+log_temp->trade_log.trade_time[1];
						temp_32 = (temp_32<<8)+log_temp->trade_log.trade_time[2];

						sec_temp = temp_32 % 60;
						temp_32 = temp_32 / 60;
						min_temp = temp_32 % 60;
						temp_32 = temp_32 / 60;
						hour_temp = temp_32 % 24;
						day_temp = temp_32 / 24;
						mon_temp = MONTH;
						if (log_temp->trade_log.month > MONTH) {
							year_temp = YEAR-1;
						}
						else {
							year_temp = YEAR;
						}
						SetGprsUsedState(1);
						while (1) {
							sprintf((char *)gprs_data_buf,"Type=Trans;TransTime=%04d-%02d-%02d %02d:%02d:%02d;SN=%05d;SS=%s;ES=%s;Price=%03d.0;TN=;SPay=%03d.0;RPay=%03d.0;CM10=;CM5=%02d;CM1=%02d;Addr=FFFF",\
												year_temp,log_temp->trade_log.month,day_temp,hour_temp,min_temp,sec_temp,log_temp->trade_log.serail_num,\
												ADDRESS[log_temp->trade_log.trade_start_st-1],ADDRESS[log_temp->trade_log.trade_end_st-1],\
												log_temp->trade_log.trade_price/10,log_temp->trade_log.needpay/10,log_temp->trade_log.realpay/10,\
												log_temp->trade_log.changemoney/10,log_temp->trade_log.changemoney%10);
							GPRSSendData((char *)gprs_data_buf,event_num);
							
							if (GdtuReturnDat(gprs_data_buf) == OS_NO_ERR) {
								if (memcmp(calString((char *)gprs_data_buf,"Verdict"),"ACK",3)) {
									
								}
								else {
									OSSemPost(gdtu_sent_successfully_sem);
									break;			//  相等( 返回正确)跳出，
								}
							}
							OSSemPost(Uart0_Channel_Sem);
							SetGprsUsedState(0);
							if ((GetUserState() == USER_RUNNING) && (GetUpdataState() == 0)) {
								break;
							}
							//OSTaskSuspend(TaskGPRSPrio);
							OSTimeDly(100);
							OSSemPend(Uart0_Channel_Sem,0,&err);
							flagcom = 2;
							UART0_Init(IC_CHANNEL2);
							SetGprsUsedState(1);
						}
						SetGprsUsedState(0);
						break;

					default: break;
				}
			}
		}
		do {
			OSSemPend(Uart0_Channel_Sem,1,&err);
		} while (err != OS_TIMEOUT);
		OSSemPost(Uart0_Channel_Sem);
		
		//OSTimeDly(50);
	}
	while(1) OSTimeDly(10000);
}

void GPRSSendData(char *dat,uint16 ev_num) {
	char *send_temp = dat;
	uint16 data_len;
	uint16 crc_data = 0;
	uint8 temp_8;

	//  发送头字符
	temp_8 = '<';
	crc_data = crcByte(crc_data,temp_8);
	UART0Putch(temp_8);
	//  计算与发送长度
	data_len = strlen(dat)+2;
	while (*send_temp++){
		if ((*send_temp == '<') || (*send_temp == '>') ||(*send_temp == '\\')) {
			data_len ++;
		}
	}
	temp_8 = (uint8)(data_len >> 8);
	if ((temp_8 == '<') || (temp_8 == '>') ||(temp_8 == '\\')) {
		crc_data = crcByte(crc_data,'\\');
		UART0Putch('\\');
		crc_data = crcByte(crc_data,temp_8);
		UART0Putch(temp_8);
	}
	else {
		crc_data = crcByte(crc_data,temp_8);
		UART0Putch(temp_8);
	}
	temp_8 = (uint8)data_len;
	if ((temp_8 == '<') || (temp_8 == '>') ||(temp_8 == '\\')) {
		crc_data = crcByte(crc_data,'\\');
		UART0Putch('\\');
		crc_data = crcByte(crc_data,temp_8);
		UART0Putch(temp_8);
	}
	else {
		crc_data = crcByte(crc_data,temp_8);
		UART0Putch(temp_8);
	}
	// 发送事件号
	temp_8 = (uint8)(ev_num >> 8);
	if ((temp_8 == '<') || (temp_8 == '>') ||(temp_8 == '\\')) {
		crc_data = crcByte(crc_data,'\\');
		UART0Putch('\\');
		crc_data = crcByte(crc_data,temp_8);
		UART0Putch(temp_8);
	}
	else {
		crc_data = crcByte(crc_data,temp_8);
		UART0Putch(temp_8);
	}
	temp_8 = (uint8)ev_num;
	if ((temp_8 == '<') || (temp_8 == '>') ||(temp_8 == '\\')) {
		crc_data = crcByte(crc_data,'\\');
		UART0Putch('\\');
		crc_data = crcByte(crc_data,temp_8);
		UART0Putch(temp_8);
	}
	else {
		crc_data = crcByte(crc_data,temp_8);
		UART0Putch(temp_8);
	}
	//  发送数据
	send_temp = dat;
	while (*send_temp) {
		if ((*send_temp == '<') || (*send_temp == '>') ||(*send_temp == '\\')) {
			crc_data = crcByte(crc_data,'\\');
			UART0Putch('\\');
			crc_data = crcByte(crc_data,*send_temp);
			UART0Putch(*send_temp);
		}
		else {
			crc_data = crcByte(crc_data,*send_temp);
			UART0Putch(*send_temp);
		}
		send_temp++;
	}
	// 发送校验位
	temp_8 = (uint8)(crc_data >> 8);
	if ((temp_8 == '<') || (temp_8 == '>') ||(temp_8 == '\\')) {
		UART0Putch('\\');
		UART0Putch(temp_8);
	}
	else {
		UART0Putch(temp_8);
	}
	temp_8 = (uint8)crc_data;
	if ((temp_8 == '<') || (temp_8 == '>') ||(temp_8 == '\\')) {
		UART0Putch('\\');
		UART0Putch(temp_8);
	}
	else {
		UART0Putch(temp_8);
	}
	//  发送结尾字符
	UART0Putch('>');
}



char *calString(char *string,char *buf){
	uint8 i = 0,m = 0;
	char ret[64];
	
	char *tmp = strstr(string,buf);
	if(!tmp){
		return NULL;
	}
	memset(ret,0,64);
	while(1){
		if(tmp[i] == '=')
		{
			i++;
			while(tmp[i] != ';'&& tmp[i]!='>'&&tmp[i]!='\0'){
				ret[m++]=tmp[i];
				i++;				
			}		
			ret[m]='\0';
			break;
		}
		i++;
	}
	return ret;
}

uint8 GdtuReturnDat(uint8 *buf){
	uint8 err,esc_flag=0;
	uint16 j = 100,crc_data=0;
	uint8 *buf_temp=buf;
	uint16 data_len=0;
	while (j--) {
		err = UART0Getch(buf_temp);
		if (err == 0) {
			if ((buf_temp - buf) > (data_len + 2)) {
				if (data_len == 0) {
					crc_data = crcByte(crc_data,*buf_temp);
				}
				else {
					crcByte(0,0);
				}
				data_len = buf[1]*256 + buf[2];
			}
			else {
				crc_data = crcByte(crc_data,*buf_temp);
			}
			if (*(char *)buf_temp == '\\') {
				if(esc_flag == 0) {
					esc_flag = 1;
				}
				
				else {
					esc_flag = 0;
				}
			}
			else if ((*(char *)buf_temp == '<')) {
				if (esc_flag == 0){ 
					*(char *)buf = '<';
					buf_temp = buf+1;
					j = 200;
					crc_data = 0;
					crc_data = crcByte(crc_data,'<');
				}
				else if ((*(char *)buf == '<') && (buf_temp != buf)) {
					buf_temp ++;
					esc_flag = 0;
				}
				else {
					esc_flag = 0;
				}
			}
			else if (*(char *)buf_temp == '>') {
				if (esc_flag == 0) {
					*(buf_temp+1) = '\0';
					if (crc_data == (*(buf_temp-2)*256+*(buf_temp-1))){
						for (j =0;j<(data_len-2);j++) {	//  (data_len-2) 长度去除事件号的两个字节
							buf[j] = buf[j+5];			//  取出接收的数据，去除头和尾
							
						}
						buf[j] = '\0';
						return OS_NO_ERR;
					}
					else {
						return 1;
					}
				}
				else if (*(char *)buf == '<'){
					buf_temp ++;
					esc_flag = 0;
				}
			}
			else if (esc_flag){
				esc_flag = 0;
			}
			else {
				if (*(char *)buf == '<') {
					buf_temp ++;
				}
			}
		}
	}
	return OS_TIMEOUT;
}

void GdtuSendUserData(void *dat, uint8 mod) {
	uint8 err;
	_mbox_data_s mbox_temp;
	
	mbox_temp.mod = mod;
	mbox_temp.dat = dat;
	//OSSemPend(gdtu_sent_successfully_sem,1,&err);
	do {
		err = OSMboxPost(gprs_send_mbox,&mbox_temp);
		OSTimeDly(10);
	}while (err != OS_NO_ERR);
	//return err;
}
uint8 GdtuLastSendSt(void) {
	uint8 err;
	OSSemPend(gdtu_sent_successfully_sem,1,&err);
	//if (err == OS_NO_ERR) OSSemPost(gdtu_sent_successfully_sem);
	return err;
}

void GdtuInit(void) {
	uint8 err;

	gprs_send_mbox = OSMboxCreate(NULL);
	if (gprs_send_mbox == NULL) {
		while(1);
	}
	//OSMboxPend(gprs_send_mbox,0,&err);
	
	gdtu_sent_successfully_sem = OSSemCreate(1);
	if (gdtu_sent_successfully_sem == NULL) {
		while(1);
	}
	OSSemPend(gdtu_sent_successfully_sem,0,&err);
}

