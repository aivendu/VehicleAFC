#ifndef _IC_CARD_H
#define _IC_CARD_H

typedef struct {
	uint8 company_code;			//	恒为0xDF
	uint8 card_id[7];
	uint8 unused[8];
} _company_flag_s;

typedef struct {
	_company_flag_s company_flag;
	char staffid[8];			//	工号
	char driver_name[8];		//	司机名字
	char secretkey[16];			//	密钥
	uint8 guid[16];				//	GUID, 路线唯一编号
	uint16 routenum;			//	路线编号
} _ic_data_s;

typedef union {
	uint8 icdata_b[sizeof(_ic_data_s)];
	_ic_data_s icdata;
} _ic_data_u;

extern _ic_data_u df_card;

#define GetCompanyCode()			(df_card.icdata.company_flag.company_code)
#define SetCompanyCode(c)			(df_card.icdata.company_flag.company_code=c)
#define GetCardID()					(df_card.icdata.company_flag.card_id)
#define SetCardID(c)				(memcpy(df_card.icdata.company_flag.card_id,c,7))
#define GetStaffid()				(df_card.icdata.staffid)
#define SetStaffid(c)				(memcpy(df_card.icdata.staffid,c,8))
#define GetDriverName()				(df_card.icdata.driver_name)
#define SetDriverName(c)			(memcpy(df_card.icdata.driver_name,c,8))
#define GetSecretKey()				(df_card.icdata.secretkey)
#define SetSecretKey(c)				(memcpy(df_card.icdata.secretkey,c,16))
#define GetGUID()					(df_card.icdata.guid)
#define SetGUID(c)					(memcpy(df_card.icdata.guid,c,16))
#define GetRouteNum()				(df_card.icdata.routenum)
#define SetRouteNum(c)				(df_card.icdata.routenum=c)
#define ClsIcData()					(memcpy(df_card.icdata_b,0xff,sizeof(_ic_data_s)))
#define SetIcData(c)				(memcpy(df_card.icdata_b,c,sizeof(_ic_data_s)))

#endif
