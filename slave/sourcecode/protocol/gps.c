#include "includes.h"

#define GPS_MAX_SAMPLING_TIME		3	//单位s
#define GPS_SAMPLING_RATE			3	//单位s


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
*	原型:	uint32 ftoi(const char *data, uint8 d_n)
*	描述:	浮点数字符串转换为整形数，转换后的数为浮点数* 10的n次方
*
*	参数:	char *data	--		浮点数字符串
*			uint8 d_n		--		浮点数小数个数
*	输出:	转换后的uint32 类型的整形数
*
*	调用模块:	无
*	资源占用:	局部变量8byte
*
*	说明: 字符串的个数小于等于9
*
*	作者:	杜其俊
*	日期:	2012-4-12
*
**********************************************************/
uint32 ftoi(const char *data, uint8 d_n)
{
	uint32 temp_32 = 0, flag = 0;

	while (*data && d_n)  			//	字符结束或者已经达到小数位数退出
	{
		if ((*data <= '9') && (*data >= '0'))  		//	判断是否是数字
		{
			temp_32 = *data - '0' + temp_32 * 10;		//	转换为整数
		}
		else if (*data == '.')  					//	判断是否开始小数
		{
			flag = 1;								//	置起小数标志
		}
		else
		{
			temp_32 = 0xffffffff;					//	异常退出
			break;
		}
		if ((*data != '.') && (flag == 1))  							//	小数递减
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
		while (d_n--)  				//	补足计算小数部分
		{
			temp_32 *= 10;
		}
	}
	return temp_32;
}


/******************************************************************************
** 函数原型: 	uint8 CheckAndParseNMEAGpsData(char *)
** 功能描述:		读取和校验NEMAGPS数据
** 输　入:		读出数据是否成功
** 输　出: 		0 -- 读出正确数据，	1--没有读出或者读出不正确的数据
** 全局变量: 	无
** 调用模块:  	Uart0RecByte
** 资源占用:		局部变量占用3个byte
**
** 作　者:	 杜其俊
** 日　期: 	2012年4月12日
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
		if (Uart1RecByte((uint8 *)dat_temp, 0, 0) == FALSE)  		//	获取GPS数据
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
		if (*dat != '$')  					//	未收到头，丢掉当前数据
		{
			continue;
		}
		time_num_temp = timedelay;
		if ((dat_temp - dat) > MAX_GPS_DATA_LENGHT)  		//	接收数据不能超过设定的数据长度，超长的数据不是我们需要的
		{
			return FALSE;
		}
		if (((dat_temp - dat) > 4) && (*(dat_temp - 4) == '*'))  			//	接收到校验数据
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
	{255, 0							, NULL					},		//	结尾符
};
const _nmea_argu_resolve_s gll_argu[] =
{
	{1	, LATITUDE_LENGHT			, latitude_c				},
	{2	, LATITUDE_INDICATOR_LENGHT	, &latitude_indicator	},
	{3	, LONGITUDE_LENGHT			, longitude_c			},
	{4	, LONGITUDE_INDICATOR_LENGHT	, &longitude_indicator	},
	{5	, UTC_TIME_LENGHT			, time_c					},
	{255, 0							, NULL					},		//	结尾符
};
const _nmea_argu_resolve_s gsa_argu[] =
{
	{255, 0							, NULL					},		//	结尾符
};
const _nmea_argu_resolve_s gsv_argu[] =
{
	{255, 0							, NULL					},		//	结尾符
};
const _nmea_argu_resolve_s rmc_argu[] =
{
	{1	, UTC_TIME_LENGHT			, time_c					},
	{3	, LATITUDE_LENGHT			, latitude_c				},
	{4	, LATITUDE_INDICATOR_LENGHT	, &latitude_indicator	},
	{5	, LONGITUDE_LENGHT			, longitude_c			},
	{6	, LONGITUDE_INDICATOR_LENGHT	, &longitude_indicator	},
	{9	, DATE_LENGHT				, date_c					},
	{255, 0							, NULL					},		//	结尾符
};

const _nmea_argu_resolve_s vtg_argu[] =
{
	{7	, MOVE_SPEED_LENGHT	, move_speed_c	},
	{255, 0					, NULL			},		//	结尾符
};

const _nmea_argu_resolve_s zda_argu[] =
{
	{1	, UTC_TIME_LENGHT, time_c	},
	//{2	,DAY_LENGHT		,day_c	},
	//{3	,MONTH_LENGHT	,month_c},
	{4	, YEAR_LENGHT	, year_c	},
	{255, 0				, NULL	},		//	结尾符
};

/**************************************************************************************************
*
*	原型:	uint32 ParseNMEAGpsData(const char *data,_nmea_argu_resolve_s *index_table)
*	描述:	解析NMEA 格式的数据，从字符串中读出需要的数据
*
*	参数:	char *data	--		NMEA 格式的字符串
*			_nmea_argu_resolve_s *index_table		--	该条数据里有用的数据表
*	输出:	恒为TRUE -- 数据解析完成，			FALSE -- 没有完整解析数据
*
*	调用模块:	无
*	资源占用:	局部变量2byte
*
*	说明: 取数据时需指定数据
*
*	作者:	杜其俊
*	日期:	2012-4-12
*
****************************************************************************************************/
uint8 ParseNMEAGpsData(const char *data, const _nmea_argu_resolve_s *index_table)
{
	uint8 temp_8 = 0, index = 0;

	while((index_table[index].dat != NULL) && (*data != '*'))  						//	直到数据搜索完毕
	{
		if (temp_8 == index_table[index].argu_num)  				//	找到数据
		{
			if (*data == ',')  										//	如果该数据为空，解析失败
			{
				return FALSE;
			}
			memset(index_table[index].dat, 0, index_table[index].argu_max_len + 1);		//	清空数据
			memcpy(index_table[index].dat, data, index_table[index].argu_max_len);		//	保存数据
			index ++;				//	继续搜索下一条
			if (index_table[index].argu_max_len == 0) 				//	全部数据解析完
			{
				return TRUE;
			}
		}
		if (*data == ',')  		//	索引到下一数据
		{
			temp_8++;
		}
		data++;					//	需要对每个字符进行比较
	}
	return FALSE;
}

/**************************************************************************************************
*
*	原型:	uint32 ExtractNMEAGpsData(char *data, uint8 n, uint8 flag)
*	描述:	提取NMEA 格式的数据，从字符串中读出需要的一个数据
*
*	参数:	char *data	--		NMEA 格式的字符串
*			uint8 n		--		需要的数据位置，表示第几个
*			uint8 flag		--		0-返回字符串指针，1-返回整形数据
*	输出:	恒为TRUE -- 数据解析完成，			FALSE -- 没有完整解析数据
*
*	调用模块:	无
*	资源占用:	局部变量5 byte
*
*	说明: 取数据时需指定数据
*
*	作者:	杜其俊
*	日期:	2012-4-12
*
****************************************************************************************************/
uint32 ExtractNMEAGpsData(const char *data, uint8 n, uint8 flag)
{
	//uint8 temp_8=0;
	uint32 temp_32 = 0;

	while (n)  				//	索引需要的数据
	{
		if (*data == ',')   	//	索引到下一数据
		{
			n--;
		}
		data++; 				//	需要对每个字符进行比较
	}
	while ((*data != ',') && (*data != '*'))  		//	数据是否处理完成
	{
		if (flag == 0)
		{
			return (uint32)data;
		}
		else
		{
			if ((*data >= '0') && (*data <= '9'))   		//	只对数字进行处理
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
	if (*(uint8 *)ExtractNMEAGpsData(dat, 2, 0) == 'A')  		//	判断数据是否有效
	{
		return ParseNMEAGpsData(dat, rmc_argu);
	}
	return FALSE;
}

uint8 VtgHandleFunc (char *dat)
{
	//if (*(uint8 *)ExtractNMEAGpsData(dat,8,0) == 'K') {		//	判断数据是否有效
	return ParseNMEAGpsData(dat, vtg_argu);
	//}
	//return FALSE;

}
uint8 ZdaHandleFunc (char *dat)
{
	return ParseNMEAGpsData(dat, zda_argu);

}



//	组建地址和处理函数的hash 关系
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
*	原型:	void TaskGPS(void *pdata)
*	描述:	GPS数据处理函数，用于读取GPS模块中的数据
*
*	参数:	void *pdata	--		无意义
*	输出:	无
*
*	调用模块:
*	资源占用:
*
*	说明: 每隔3S 采集一次数据
*
*	作者:	杜其俊
*	日期:	2012-4-12
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
		while (gps_tick < (GetGpsSamplingTime() * 200 - 40))  				//	每隔一定时间 采集一次GPS 数据，可配置
		{
			OSTimeDly(4);						//	20ms一个单位
		}
		err = FALSE;
		if (RequestUart(GPS_UART1, 0) == OS_NO_ERR)  					//	申请uart资源
		{
			time_num = 50;
			memset(gps_buf, 0, sizeof(gps_buf));
			while (GetAndCheckNMEAGpsData(gps_buf, time_num) == TRUE)  	//	接收数据
			{
				time_num = 20;
				for (i = 0; i < MAX_GPS_ADDR; i++)  			//	轮询GPS数据地址
				{
					if (strncmp(&gps_buf[1], gpsaddr[i].addr, 5) == 0)  		//	判断是什么地址数据
					{
						err = gpsaddr[i].func(gps_buf);			//	解析地址数据
						if (err == FALSE) break;
						else if (i == 0)
						{

							gps_tick = 0;			//	第一个数据rmc 采样成功，重新开始计时
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
							//	定位成功，如果时间不对，更新芯片时间
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
							gps_data.gps_state = GPS_MODE_NORMAL;			//	状态正常
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
					gps_data.gps_state = GPS_MODE_NO_POSITION;			//	定位未成功
				}
				else
				{
					locator_failure_num ++;
				}
				failure_num = 0;
			}
			if (failure_num > 4)  			//	接收失效处理
			{
				gps_data.gps_state = GPS_MODE_UART_ERROR;	//	GPS 串口故障
			}
			else
			{
				failure_num ++;
			}
			FreeUart(GPS_UART1);			//	清零定时器，准备下一次接收
		}
		//	计算采样到的数据
		device_control.gps = gps_data;
		device_control.sys_device.gps_mode_state = device_control.gps.gps_state;
	}
}

