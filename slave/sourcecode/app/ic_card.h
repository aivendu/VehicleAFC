#ifndef _IC_CARD_H
#define _IC_CARD_H


#define CARD_CAPACITY			256		//	ʹ�õĿ�Ƭ����
#define IC_CARD_DATA_LEN		sizeof(_card_plaintxt_u)


typedef struct
{
	uint8 encryption_type;		//	 �������ͣ�0--δ����
	uint8 unused[3];
	uint8 secretkey[16];		//	��Կ
	uint8 unused1[16];
} _ecrypted_sms_s;

typedef struct
{
	uint8 company_code;			//	��˾�����ͱ�־������AFC������Ϊ0xAC
	uint8 user_id[3];			//	�ͻ�����
	uint8 card_id[4];			//	����
	uint8 card_create_time[4];	//	�쿨ʱ�䣬��2000�꿪ʼ�����������
} _company_flag_s;

typedef struct
{
	uint8 regist_time[4];		//	��ʱ��
	uint8 user_role;			//	�û���ɫ�������б��û��Ĺ���Ȩ��
	char staffid[7];			//	���ţ�֧�����7���ַ�
	char driver_name[8];		//	˾�����֣�֧�����4���ֵ�����
	uint8 ID_card[8];			//	˾�����֤�ţ�X��0��ʾ��������
} _user_info_s;

typedef struct
{
	uint8 guid[16];				//	GUID, ·��Ψһ���
	uint8 vehicle_plate[8];		//	���ƺ�
	uint16 routenum;			//	·�߱��
} _route_info_s;


typedef struct
{
	_company_flag_s cflag;
	_user_info_s uinfo;
	_route_info_s rinfo;
} _card_plaintxt_s;		//	��Ƭ�ϵ�������Ϣ

typedef union
{
	uint8 card_b[sizeof(_card_plaintxt_s)];
	_card_plaintxt_s card;
} _card_plaintxt_u;		//	��Ƭ�ϵ�������Ϣ

typedef struct
{
	_ecrypted_sms_s esms;
	uint8 data[IC_CARD_DATA_LEN];
} _card_data_s;			//	��Ƭ�ϵ�ʵ������


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
