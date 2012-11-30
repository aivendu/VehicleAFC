#include "includes.h"

#define GPS_MAX_SAMPLING_TIME		3	//��λs
#define GPS_SAMPLING_RATE			3	//��λs


#define LATITUDE_LENGHT				10
#define LONGITUDE_LENGHT			11
#define LATITUDE_INDICATOR_LENGHT	1
#define LONGITUDE_INDICATOR_LENGHT	1
#define UTC_TIME_LENGHT				9
#define DAY_LENGHT					2
#define MONTH_LENGHT				2
#define YEAR_LENGHT					4
#define DATE_LENGHT					6
#define MOVE_SPEED_LENGHT			6

uint16 gps_tick;

char	move_speed_c[MOVE_SPEED_LENGHT + 1];
char	latitude_c[LATITUDE_LENGHT + 1], longitude_c[LONGITUDE_LENGHT + 1];
char	latitude_indicator, longitude_indicator;
char	date_c[DATE_LENGHT + 1], year_c[5], time_c[UTC_TIME_LENGHT + 1];

//static OS_EVENT *get_gps_sem,*get_zhandian_sem;
//static OS_EVENT *gps_mbox,*zhandian_mbox;
_time_s gps_time;
_gps_data_s gps_data;

/*********************************************************
*
*	ԭ��:	uint32 ftoi(const char *data, uint8 d_n)
*	����:	�������ַ���ת��Ϊ��������ת�������Ϊ������* 10��n�η�
*
*	����:	char *data	--		�������ַ���
*			uint8 d_n		--		������С������
*	���:	ת�����uint32 ���͵�������
*
*	����ģ��:	��
*	��Դռ��:	�ֲ�����8byte
*
*	˵��: �ַ����ĸ���С�ڵ���9
*
*	����:	���信
*	����:	2012-4-12
*
**********************************************************/
uint32 ftoi(const char *data, uint8 d_n)
{
	uint32 temp_32 = 0, flag = 0;

	while (*data && d_n)  			//	�ַ����������Ѿ��ﵽС��λ���˳�
	{
		if ((*data <= '9') && (*data >= '0'))  		//	�ж��Ƿ�������
		{
			temp_32 = *data - '0' + temp_32 * 10;		//	ת��Ϊ����
		}
		else if (*data == '.')  					//	�ж��Ƿ�ʼС��
		{
			flag = 1;								//	����С����־
		}
		else
		{
			temp_32 = 0xffffffff;					//	�쳣�˳�
			break;
		}
		if ((*data != '.') && (flag == 1))  							//	С���ݼ�
		{
			d_n --;
		}
		data ++;
	}
	if (temp_32 == 0xffffffff)
	{
	}
	else
	{
		while (d_n--)  				//	�������С������
		{
			temp_32 *= 10;
		}
	}
	return temp_32;
}


/******************************************************************************
** ����ԭ��: 	uint8 CheckAndParseNMEAGpsData(char *)
** ��������:		��ȡ��У��NEMAGPS����
** �䡡��:		���������Ƿ�ɹ�
** �䡡��: 		0 -- ������ȷ���ݣ�	1--û�ж������߶�������ȷ������
** ȫ�ֱ���: 	��
** ����ģ��:  	Uart0RecByte
** ��Դռ��:		�ֲ�����ռ��3��byte
**
** ������:	 ���信
** �ա���: 	2012��4��12��
*******************************************************************************/

uint8 GetAndCheckNMEAGpsData(char *const dat, uint32 timedelay)
{

	//uint8 err;
	uint8 eor;
	uint32 time_num_temp = timedelay;
	char *dat_temp = dat;
	*dat = 0;
	while (1)
	{
		if (Uart1RecByte((uint8 *)dat_temp, 0, 0) == FALSE)  		//	��ȡGPS����
		{
			OSTimeDly(1);
			if ((--time_num_temp) == 0)
			{
				return FALSE;
			}
			continue;
		}
		if (*dat_temp == '$')
		{
			*dat = '$';
			dat_temp = dat;
		}
		if (*dat != '$')  					//	δ�յ�ͷ��������ǰ����
		{
			continue;
		}
		time_num_temp = timedelay;
		if ((dat_temp - dat) > MAX_GPS_DATA_LENGHT)  		//	�������ݲ��ܳ����趨�����ݳ��ȣ����������ݲ���������Ҫ��
		{
			return FALSE;
		}
		if (((dat_temp - dat) > 4) && (*(dat_temp - 4) == '*'))  			//	���յ�У������
		{
			eor = 0;
			dat_temp = &dat[1];
			while (*dat_temp != '*')
			{
				eor ^= *dat_temp;
				if (*(++dat_temp) == '\r')
				{
					break;
				}
			}
			dat_temp[3] = '\0';
			if ((*dat_temp == '*') && (eor == strtol((char *)(dat_temp + 1), NULL, 16)))
			{
				dat_temp[3] = '\r';
				return TRUE;
			}
		}
		dat_temp++;
	}

}


const _nmea_argu_resolve_s gga_argu[] =
{
	{1	, UTC_TIME_LENGHT			, time_c					},
	{2	, LATITUDE_LENGHT			, latitude_c				},
	{3	, LATITUDE_INDICATOR_LENGHT	, &latitude_indicator	},
	{4	, LONGITUDE_LENGHT			, longitude_c			},
	{5	, LONGITUDE_INDICATOR_LENGHT	, &longitude_indicator	},
	{255, 0							, NULL					},		//	��β��
};
const _nmea_argu_resolve_s gll_argu[] =
{
	{1	, LATITUDE_LENGHT			, latitude_c				},
	{2	, LATITUDE_INDICATOR_LENGHT	, &latitude_indicator	},
	{3	, LONGITUDE_LENGHT			, longitude_c			},
	{4	, LONGITUDE_INDICATOR_LENGHT	, &longitude_indicator	},
	{5	, UTC_TIME_LENGHT			, time_c					},
	{255, 0							, NULL					},		//	��β��
};
const _nmea_argu_resolve_s gsa_argu[] =
{
	{255, 0							, NULL					},		//	��β��
};
const _nmea_argu_resolve_s gsv_argu[] =
{
	{255, 0							, NULL					},		//	��β��
};
const _nmea_argu_resolve_s rmc_argu[] =
{
	{1	, UTC_TIME_LENGHT			, time_c					},
	{3	, LATITUDE_LENGHT			, latitude_c				},
	{4	, LATITUDE_INDICATOR_LENGHT	, &latitude_indicator	},
	{5	, LONGITUDE_LENGHT			, longitude_c			},
	{6	, LONGITUDE_INDICATOR_LENGHT	, &longitude_indicator	},
	{9	, DATE_LENGHT				, date_c					},
	{255, 0							, NULL					},		//	��β��
};

const _nmea_argu_resolve_s vtg_argu[] =
{
	{7	, MOVE_SPEED_LENGHT	, move_speed_c	},
	{255, 0					, NULL			},		//	��β��
};

const _nmea_argu_resolve_s zda_argu[] =
{
	{1	, UTC_TIME_LENGHT, time_c	},
	//{2	,DAY_LENGHT		,day_c	},
	//{3	,MONTH_LENGHT	,month_c},
	{4	, YEAR_LENGHT	, year_c	},
	{255, 0				, NULL	},		//	��β��
};

/**************************************************************************************************
*
*	ԭ��:	uint32 ParseNMEAGpsData(const char *data,_nmea_argu_resolve_s *index_table)
*	����:	����NMEA ��ʽ�����ݣ����ַ����ж�����Ҫ������
*
*	����:	char *data	--		NMEA ��ʽ���ַ���
*			_nmea_argu_resolve_s *index_table		--	�������������õ����ݱ�
*	���:	��ΪTRUE -- ���ݽ�����ɣ�			FALSE -- û��������������
*
*	����ģ��:	��
*	��Դռ��:	�ֲ�����2byte
*
*	˵��: ȡ����ʱ��ָ������
*
*	����:	���信
*	����:	2012-4-12
*
****************************************************************************************************/
uint8 ParseNMEAGpsData(const char *data, const _nmea_argu_resolve_s *index_table)
{
	uint8 temp_8 = 0, index = 0;

	while((index_table[index].dat != NULL) && (*data != '*'))  						//	ֱ�������������
	{
		if (temp_8 == index_table[index].argu_num)  				//	�ҵ�����
		{
			if (*data == ',')  										//	���������Ϊ�գ�����ʧ��
			{
				return FALSE;
			}
			memset(index_table[index].dat, 0, index_table[index].argu_max_len + 1);		//	�������
			memcpy(index_table[index].dat, data, index_table[index].argu_max_len);		//	��������
			index ++;				//	����������һ��
			if (index_table[index].argu_max_len == 0) 				//	ȫ�����ݽ�����
			{
				return TRUE;
			}
		}
		if (*data == ',')  		//	��������һ����
		{
			temp_8++;
		}
		data++;					//	��Ҫ��ÿ���ַ����бȽ�
	}
	return FALSE;
}

/**************************************************************************************************
*
*	ԭ��:	uint32 ExtractNMEAGpsData(char *data, uint8 n, uint8 flag)
*	����:	��ȡNMEA ��ʽ�����ݣ����ַ����ж�����Ҫ��һ������
*
*	����:	char *data	--		NMEA ��ʽ���ַ���
*			uint8 n		--		��Ҫ������λ�ã���ʾ�ڼ���
*			uint8 flag		--		0-�����ַ���ָ�룬1-������������
*	���:	��ΪTRUE -- ���ݽ�����ɣ�			FALSE -- û��������������
*
*	����ģ��:	��
*	��Դռ��:	�ֲ�����5 byte
*
*	˵��: ȡ����ʱ��ָ������
*
*	����:	���信
*	����:	2012-4-12
*
****************************************************************************************************/
uint32 ExtractNMEAGpsData(const char *data, uint8 n, uint8 flag)
{
	//uint8 temp_8=0;
	uint32 temp_32 = 0;

	while (n)  				//	������Ҫ������
	{
		if (*data == ',')   	//	��������һ����
		{
			n--;
		}
		data++; 				//	��Ҫ��ÿ���ַ����бȽ�
	}
	while ((*data != ',') && (*data != '*'))  		//	�����Ƿ������
	{
		if (flag == 0)
		{
			return (uint32)data;
		}
		else
		{
			if ((*data >= '0') && (*data <= '9'))   		//	ֻ�����ֽ��д���
			{
				temp_32 = temp_32 * 10 + (*data - '0');
			}
		}
		data++;
	}
	return temp_32;
}


uint8 GgaHandleFunc (char *dat)
{
	return ParseNMEAGpsData(dat, gga_argu);
}
uint8 GllHandleFunc (char *dat)
{
	return ParseNMEAGpsData(dat, gll_argu);
}

uint8 GsaHandleFunc (char *dat)
{
	return ParseNMEAGpsData(dat, gsa_argu);
}
uint8 GsvHandleFunc (char *dat)
{
	return ParseNMEAGpsData(dat, gsv_argu);
}
uint8 RmcHandleFunc (char *dat)
{
	if (*(uint8 *)ExtractNMEAGpsData(dat, 2, 0) == 'A')  		//	�ж������Ƿ���Ч
	{
		return ParseNMEAGpsData(dat, rmc_argu);
	}
	return FALSE;
}

uint8 VtgHandleFunc (char *dat)
{
	//if (*(uint8 *)ExtractNMEAGpsData(dat,8,0) == 'K') {		//	�ж������Ƿ���Ч
	return ParseNMEAGpsData(dat, vtg_argu);
	//}
	//return FALSE;

}
uint8 ZdaHandleFunc (char *dat)
{
	return ParseNMEAGpsData(dat, zda_argu);

}



//	�齨��ַ�ʹ�������hash ��ϵ
#define MAX_GPS_ADDR			((sizeof(gpsaddr))/(sizeof(_nmea_data_s)))
const _nmea_data_s gpsaddr[] =
{
	//{"GPGGA", (_nmea_argu_resolve_s *)gga_argu, GgaHandleFunc},
	//{"GPGLL", (_nmea_argu_resolve_s *)gll_argu, GllHandleFunc},
	//{"GPGSA", (_nmea_argu_resolve_s *)gsa_argu, GsaHandleFunc},
	//{"GPGSV", (_nmea_argu_resolve_s *)gsv_argu, GsvHandleFunc},
	{"GPRMC", (_nmea_argu_resolve_s *)rmc_argu, RmcHandleFunc},
	{"GPVTG", (_nmea_argu_resolve_s *)vtg_argu, VtgHandleFunc},
	//{"GPZDA", (_nmea_argu_resolve_s *)zda_argu, ZdaHandleFunc }
};

void gps_init(void)
{

	uint8 err;

	err = err;

}

/**************************************************************************************************
*
*	ԭ��:	void TaskGPS(void *pdata)
*	����:	GPS���ݴ����������ڶ�ȡGPSģ���е�����
*
*	����:	void *pdata	--		������
*	���:	��
*
*	����ģ��:
*	��Դռ��:
*
*	˵��: ÿ��3S �ɼ�һ������
*
*	����:	���信
*	����:	2012-4-12
*
****************************************************************************************************/
void TaskGPS(void *pdata)
{

	uint8 err, i;
	char gps_buf[MAX_GPS_DATA_LENGHT];
	uint32 time_num;
	uint8 failure_num, locator_failure_num;

	pdata = pdata;
	IO0SET |= POWER_CONTROL_GPS;

	while(1)
	{
		while (gps_tick < (GetGpsSamplingTime() * 200 - 40))  				//	ÿ��һ��ʱ�� �ɼ�һ��GPS ���ݣ�������
		{
			OSTimeDly(4);						//	20msһ����λ
		}
		err = FALSE;
		if (RequestUart(GPS_UART1, 0) == OS_NO_ERR)  					//	����uart��Դ
		{
			time_num = 50;
			memset(gps_buf, 0, sizeof(gps_buf));
			while (GetAndCheckNMEAGpsData(gps_buf, time_num) == TRUE)  	//	��������
			{
				time_num = 20;
				for (i = 0; i < MAX_GPS_ADDR; i++)  			//	��ѯGPS���ݵ�ַ
				{
					if (strncmp(&gps_buf[1], gpsaddr[i].addr, 5) == 0)  		//	�ж���ʲô��ַ����
					{
						err = gpsaddr[i].func(gps_buf);			//	������ַ����
						if (err == FALSE) break;
						else if (i == 0)
						{

							gps_tick = 0;			//	��һ������rmc �����ɹ������¿�ʼ��ʱ
							gps_time.year = 2000 + (date_c[4] - '0') * 10 + (date_c[5] - '0');
							gps_time.month = (date_c[2] - '0') * 10 + (date_c[3] - '0');
							gps_time.day = (date_c[0] - '0') * 10 + (date_c[1] - '0');
							gps_time.hour = (time_c[0] - '0') * 10 + (time_c[1] - '0');
							gps_time.min = (time_c[2] - '0') * 10 + (time_c[3] - '0');
							gps_time.sec = (time_c[4] - '0') * 10 + (time_c[5] - '0');
							gps_data.gps_latitude = ftoi(latitude_c, 5);
							gps_data.gps_longitude = ftoi(longitude_c, 5);
							gps_data.gps_time = TimeSec(gps_time.year, gps_time.month, gps_time.day,
							                            gps_time.hour, gps_time.min, gps_time.sec) + 8 * 60 * 60;
							GetDateTimeFromSecond(gps_data.gps_time, (uint8 *)((uint32)&gps_time + 1));
							gps_time.year = ((uint8 *)&gps_time)[1] + 2000;
							//	��λ�ɹ������ʱ�䲻�ԣ�����оƬʱ��
							if ((gps_time.min > (MIN + 1)) || (gps_time.min < (MIN - 1)) || (gps_time.hour != HOUR)
							        || (gps_time.day != DOM) || (gps_time.month != MONTH) || (gps_time.year != YEAR))
							{
								SEC = gps_time.sec + 1;
								MIN = gps_time.min;
								HOUR = gps_time.hour;
								DOM = gps_time.day;
								MONTH = gps_time.month;
								YEAR = gps_time.year;
							}
							locator_failure_num = 0;
							gps_data.gps_state = GPS_MODE_NORMAL;			//	״̬����
						}
						else if (i == 1)
						{
							gps_data.gps_movingspeed = ftoi(move_speed_c, 3);
						}
					}
				}
				if (err == TRUE)
				{
				}
				else if (locator_failure_num > 2)
				{
					gps_data.gps_state = GPS_MODE_NO_POSITION;			//	��λδ�ɹ�
				}
				else
				{
					locator_failure_num ++;
				}
				failure_num = 0;
			}
			if (failure_num > 4)  			//	����ʧЧ����
			{
				gps_data.gps_state = GPS_MODE_UART_ERROR;	//	GPS ���ڹ���
			}
			else
			{
				failure_num ++;
			}
			FreeUart(GPS_UART1);			//	���㶨ʱ����׼����һ�ν���
		}
		//	���������������
		device_control.gps = gps_data;
		device_control.sys_device.gps_mode_state = device_control.gps.gps_state;
	}
}

