#ifndef _IC_CARD_H
#define _IC_CARD_H


#define CARD_CAPACITY			256		//	使用的卡片容量
#define IC_CARD_DATA_LEN		sizeof(_card_plaintxt_u)


typedef struct
{
	uint8 encryption_type;		//	 加密类型；0--未加密
	uint8 unused[3];
	uint8 secretkey[16];		//	密钥
	uint8 unused1[16];
} _ecrypted_sms_s;

typedef struct
{
	uint8 company_code;			//	公司卡类型标志，车载AFC卡，恒为0xAC
	uint8 user_id[3];			//	客户代号
	uint8 card_id[4];			//	卡号
	uint8 card_create_time[4];	//	办卡时间，从2000年开始计算的秒数，
} _company_flag_s;

typedef struct
{
	uint8 regist_time[4];		//	打卡时间
	uint8 user_role;			//	用户角色，用于判别用户的功能权限
	char staffid[7];			//	工号，支持最多7个字符
	char driver_name[8];		//	司机名字，支持最多4个字的名字
	uint8 ID_card[8];			//	司机身份证号，X用0表示，整形数
} _user_info_s;

typedef struct
{
	uint8 guid[16];				//	GUID, 路线唯一编号
	uint8 vehicle_plate[8];		//	车牌号
	uint16 routenum;			//	路线编号
} _route_info_s;


typedef struct
{
	_company_flag_s cflag;
	_user_info_s uinfo;
	_route_info_s rinfo;
} _card_plaintxt_s;		//	卡片上的明文信息

typedef union
{
	uint8 card_b[sizeof(_card_plaintxt_s)];
	_card_plaintxt_s card;
} _card_plaintxt_u;		//	卡片上的明文信息

typedef struct
{
	_ecrypted_sms_s esms;
	uint8 data[IC_CARD_DATA_LEN];
} _card_data_s;			//	卡片上的实际数据


#if (CARD_CAPACITY < 236)
#error	"Capacity of ID Card is too little !"
#endif


extern _card_data_s df_card_data;
extern _card_plaintxt_u *df_card;

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
