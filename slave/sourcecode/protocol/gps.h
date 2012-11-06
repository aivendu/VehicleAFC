#ifndef _GPS_H
#define _GPS_H

typedef uint8 (*_pfunc)(char *);

#define MAX_GPS_DATA_LENGHT						80

typedef struct {
	uint8 argu_num;
	uint8 argu_max_len;
	void *dat;
} _nmea_argu_resolve_s;

typedef struct {
	char addr[6];
	_nmea_argu_resolve_s *argu_reso;
	_pfunc func;
} _nmea_data_s;


typedef struct {
	uint32 gps_state;
	uint32 gps_latitude;
	uint32 gps_longitude;
	uint32 gps_movingspeed;
	uint32 gps_time;
} _gps_data_s;

#define gps_data_write				NULL

extern _gps_data_s gps_data;

extern uint16 gps_tick;


extern uint8 ZhanDianDisUpdataFlag;

extern void gps_init(void);
extern uint8 GetZhanDian(void *zd);
extern uint8 GetGpsData(void *gps_dat);
extern void TaskGPS(void *pdata);


#endif
