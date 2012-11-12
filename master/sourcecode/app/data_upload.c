#include "includes.h"


void TaskDataUpload(void *pdata)
{
	uint8 err;
	pdata = pdata;

	
	OSTimeDly(OS_TICKS_PER_SEC);

	while (1)
	{
		err = ServerGPSData(1,device_control.gps.gps_latitude,device_control.gps.gps_longitude,device_control.gps.gps_movingspeed);
		if (err == GPRS_DATA_NO_ERR)
		{
			OSTimeDly(OS_TICKS_PER_SEC*5);
			//OSTaskSuspend(OS_PRIO_SELF);
		}
		else
		{
			
		}
	}
}



