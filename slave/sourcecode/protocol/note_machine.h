#ifndef _NOTE_MACHINE_H
#define _NOTE_MACHINE_H


#define NOTE_MACHINE_RETRY_TIME		3		//	�������Դ���	
#define MONEY_LITTLE_WARM_NUM		10		//	Ǯ��������ֵ


#define SDM2000_STX				0x02
#define	SDM2000_ETX				0X03
#define	SDM2000_ACK				0x06
#define	SDM2000_NAK				0x15
#define	SDM2000_ENQ				0x05


#define SDM2000_RESET			0x30  //Reset #endif
#define SDM2000_READ_STATUS		0x31  //Read Status 
#define SDM2000_DIAGNOSITCS		0x32  //Diagnositcs 
#define SDM2000_LAST_STATUS		0x34  //Last Status 
#define SDM2000_CONFIG_STATUS	0x35  //Configuration Status 
#define SDM2000_SET_THICKNESS	0x36  //Set Bill Thickness 
#define SDM2000_GET_THICKNESS	0x37  //Get Bill Thickness 
#define SDM2000_SET_SIZE		0x38  //Set Bill Size 
#define SDM2000_GET_SIZE		0x39  //Get Bill Size 
#define SDM2000_DISPENSE_BILL	0x3A  //Multi Cassette Dispense Bill
#define SDM2000_LEARN_PARAMETER	0x40  //Learn Bill Parameter 
#define SDM2000_REPORT_REJECT	0x43  //Report Reject Reason 
#define SDM2000_SENSOR_READ		0x44  //Sensor Read 
#define SDM2000_CONUTER_R_C		0x46  //Total Counter Read & Clear 
#define SDM2000_RESET_FINISH	0x80	// power on and initialization is completed

#define	NOTE_BOX_QUANTITY	2
#define BILL_RADIX1         5	//Ǯ��1��5Ԫֽ��,ע��Ǯ��1�ŵ���ֵ��ֽ��
#define BILL_RADIX2			10	//Ǯ��2��10Ԫֽ��
#define ENQ_WAITOUT_TIME		//ENQ�ȴ���ʱʱ��
#define ACK_WAITOUT_TIME		//ACK�ȴ���ʱʱ��
#define SDM2000_COMM_DATA_LENGHT	80

#define SDM2000ERR_SEND_NO_ACK		1
#define SDM2000ERR_NO_ENQ			2
#define SDM2000ERR_ENQ_MESERR		3
#define SDM2000ERR_MES_HALF_BAKED	4

#define	SDM2000_SMD_MAX_LENGHT	38

//#prgram warning(disable:C2485W)
typedef struct {
	uint8	exit_right_sensor	:1;
	uint8	exit_left_sensor	:1;
	uint8	top_sensor			:1;
	uint8	reject_sensor		:1;
	uint8	middle_sensor		:1;
	uint8	thickness_sensor	:1;
	uint8	reserve0_1			:1;
	uint8	reserve0_2			:1;
	uint8	pickup1_sensor		:1;
	uint8	near_end1_sensor	:1;
	uint8	reserve1_1			:1;
	uint8	reserve1_2			:1;
	uint8	cassette1_switch_1	:1;
	uint8	cassette1_switch_2	:1;
	uint8	cassette1_switch_3	:1;
	uint8	cassette1_switch_4	:1;
	uint8	pickup2_sensor		:1;
	uint8	near_end2_sensor	:1;
	uint8	reserve_2_1			:1;
	uint8	reserve_2_2			:1;
	uint8	cassette2_switch_1	:1;
	uint8	cassette2_switch_2	:1;
	uint8	cassette2_switch_3	:1;
	uint8	cassette2_switch_4	:1;
	uint8	pickup3_sensor		:1;
	uint8	near_end3_sensor	:1;
	uint8	reserve_3_1			:1;
	uint8	reserve_3_2			:1;
	uint8	cassette3_switch_1	:1;
	uint8	cassette3_switch_2	:1;
	uint8	cassette3_switch_3	:1;
	uint8	cassette3_Switch_4	:1;
	uint8	pickup4_sensor		:1;
	uint8	near_end4_sensor	:1;
	uint8	reserve_4_1			:1;
	uint8	reserve_4_2			:1;
	uint8	cassette4_switch_1	:1;
	uint8	cassette4_switch_2	:1;
	uint8	cassette4_switch_3	:1;
	uint8	cassette4_switch_4	:1;
	uint8	pickup5_sensor		:1;
	uint8	near_end5_sensor	:1;
	uint8	reserve_5_1			:1;
	uint8	reserve_5_2			:1;
	uint8	cassette5_switch_1	:1;
	uint8	cassette5_switch_2	:1;
	uint8	cassette5_switch_3	:1;
	uint8	cassette5_switch_4	:1;
	uint8	pickup6_sensor		:1;
	uint8	near_end6_sensor	:1;
	uint8	reserve_6_1			:1;
	uint8	reserve_6_2			:1;
	uint8	cassette6_switch_1	:1;
	uint8	cassette6_switch_2	:1;
	uint8	cassette6_switch_3	:1;
	uint8	cassette6_switch_4	:1;
	uint8	pickup7_sensor		:1;
	uint8	near_end7_sensor	:1;
	uint8	reserve_7_1			:1;
	uint8	reserve_7_2			:1;
	uint8	cassette7_switch_1	:1;
	uint8	cassette7_switch_2	:1;
	uint8	cassette7_switch_3	:1;
	uint8	cassette7_switch_4	:1;
	uint8	pickup8_sensor		:1;
	uint8	near_end8_sensor	:1;
	uint8	reserve_8_1			:1;
	uint8	reserve_8_2			:1;
	uint8	cassette8_switch_1	:1;
	uint8	cassette8_switch_2	:1;
	uint8	cassette8_switch_3	:1;
	uint8	cassette8_Switch_4	:1;
}_sdm_state_t;
//#prgram warning(enable:C2485W)

typedef union {
	uint8	sdm_state_bye[sizeof(_sdm_state_t)];
	_sdm_state_t sdm_state_bit;
}_sdm_state_u;

typedef enum {
	SDM_NORMAL = 0x30,								//	����
	SDM_MOTOR_ERROR,								//	�������
	SDM_CASSETTE_1_EMPTY,							//	��1����û��ֽ��
	SDM_CASSETTE_2_EMPTY,							//	��2����û��ֽ��
	SDM_CASSETTE_3_EMPTY,							//	��3����û��ֽ��
	SDM_CASSETTE_4_EMPTY,							//	��4����û��ֽ��
	SDM_CASSETTE_5_EMPTY,							//	��5����û��ֽ��
	SDM_CASSETTE_6_EMPTY,							//	��6����û��ֽ��
	SDM_CASSETTE_7_EMPTY,							//	��7����û��ֽ��
	SDM_CASSETTE_8_EMPTY,							//	��8����û��ֽ��
	SDM_CASSETTE_1_NO_EXIST,						//	��1���䲻����
	SDM_CASSETTE_2_NO_EXIST,						//	��2���䲻����
	SDM_CASSETTE_3_NO_EXIST,						//	��3���䲻����
	SDM_CASSETTE_4_NO_EXIST,						//	��4���䲻����
	SDM_CASSETTE_5_NO_EXIST,						//	��5���䲻����
	SDM_CASSETTE_6_NO_EXIST,						//	��6���䲻����
	SDM_CASSETTE_7_NO_EXIST,						//	��7���䲻����
	SDM_CASSETTE_8_NO_EXIST,						//	��8���䲻����
	SDM_CASSETTE_1_NO_FEED_ERROR,					//	��1����δ���ң�δ���� near end��
	SDM_CASSETTE_2_NO_FEED_ERROR,					//	��2����δ���ң�δ���� near end��
	SDM_CASSETTE_3_NO_FEED_ERROR,					//	��3����δ���ң�δ���� near end��
	SDM_CASSETTE_4_NO_FEED_ERROR,					//	��4����δ���ң�δ���� near end��
	SDM_CASSETTE_5_NO_FEED_ERROR,					//	��5����δ���ң�δ���� near end��
	SDM_CASSETTE_6_NO_FEED_ERROR,					//	��6����δ���ң�δ���� near end��
	SDM_CASSETTE_7_NO_FEED_ERROR,					//	��7����δ���ң�δ���� near end��
	SDM_CASSETTE_8_NO_FEED_ERROR,					//	��8����δ���ң�δ���� near end��
	SDM_PICKUP1_SENSOR_TO_MIDDLE_SENSOR_JAM,		//	
	SDM_PICKUP2_SENSOR_TO_MIDDLE_SENSOR_JAM,		//	
	SDM_PICKUP3_SENSOR_TO_MIDDLE_SENSOR_JAM,		//	
	SDM_PICKUP4_SENSOR_TO_MIDDLE_SENSOR_JAM,		//	
	SDM_PICKUP5_SENSOR_TO_MIDDLE_SENSOR_JAM,		//	
	SDM_PICKUP6_SENSOR_TO_MIDDLE_SENSOR_JAM,		//	
	SDM_PICKUP7_SENSOR_TO_MIDDLE_SENSOR_JAM,		//	
	SDM_PICKUP8_SENSOR_TO_MIDDLE_SENSOR_JAM,		//	
	SDM_MIDDLE_SENSOR_TO_REJECT_SENSOR_JAM,			//	
	SDM_MIDDLE_SENSOR_TO_TOP_SENSOR_JAM,			//	
	SDM_TOP_SENSOR_TO_EXIT_SENSOR_JAM,
	SDM_EXIT_SENSOR_JAM,			//	Exit ������������
	SDM_TOP_SENSOR_JAM,				//	Top ������������
	SDM_REJECT_SENSOR_JAM,			//	Reject ������������
	SDM_EXIT_RIGHT_SENSOR_ERROR,	//	Exit Right ����������
	SDM_EXIT_LEFT_SENSOR_ERROR,		//	Exit Left ����������
	SDM_TOP_SENSOR_ERROR,			//	Top ����������
	SDM_REJECT_SENSOR_ERROR,		//	Reject ����������
	SDM_THICKNESS_SENSOR_ERROR,		//	Thickness ����������
	SDM_MIDDLE_SENSOR_ERROR,		//	Middle ����������
	SDM_CONTINUOUS_REJECT_ERROR,	//	������5�Σ��ܸ�����
	SDM_SUSPECT_EXIT_ACCOUNTANCY,	//	
	SDM_F_W_BOOT_START,				//	
	SDM_COMMUNICATION_TIME_OUT,		//	
	SDM_ILLEGAL_COMMAND,			//	�Ƿ�����
	SDM_SHUTDOWN_ERROR,				//	
	SDM_TOP_BILL_EXIST,				//	
	SDM_EXIT_BILL_EXIST,			//	
	SDM_BILL_COUNT_ERROR,			//	ֽ�Ҽ�������
	SDM_UNIT_TYPE_ERROR,			//	�������ʹ���
	SDM_CASSETTE1_NOT_BILL_INFORMATION,
	SDM_CASSETTE2_NOT_BILL_INFORMATION,
	SDM_CASSETTE3_NOT_BILL_INFORMATION,
	SDM_CASSETTE4_NOT_BILL_INFORMATION,
	SDM_CASSETTE5_NOT_BILL_INFORMATION,
	SDM_CASSETTE6_NOT_BILL_INFORMATION,
	SDM_CASSETTE7_NOT_BILL_INFORMATION,
	SDM_CASSETTE8_NOT_BILL_INFORMATION
		
}_sdm_err_num_e;

typedef enum {
	REJECT_NEAR			= 1,
	REJECT_DOUBLE		= 2,
	REJECT_LENGHT_SHORT	= 4,
	REJECT_LENGHT_LONG	= 8
}_sdm_reject_code_e;

typedef struct {
	_sdm_err_num_e sdm_err;
	_sdm_state_u sdm_state;
}_sdm_rede_state_rx_s;

typedef struct {
	uint8 sdm_last_command;
	_sdm_err_num_e sdm_last_err;
	uint8 sdm_last_vomit_currency;
	uint8 sdm_last_pick_counts[8];
	uint8 sdm_last_dispense_counts[8];
	uint8 sdm_last_reject_counts[8];
}_sdm_last_status_rx_s;

typedef struct {
	uint16	sdm_fireware_ver;
	uint16  sdm_cassette1_exist_flag	: 1;
	uint16  sdm_cassette2_exist_flag	: 1;
	uint16  sdm_cassette3_exist_flag	: 1;
	uint16  sdm_cassette4_exist_flag	: 1;
	uint16  sdm_cassette5_exist_flag	: 1;
	uint16  sdm_cassette6_exist_flag	: 1;
	uint16  sdm_cassette7_exist_flag	: 1;
	uint16  sdm_cassette8_exist_flag	: 1;
	uint16	sdm_fold_bank_note			: 1;
	uint16	sdm_spit_currency_in_front	: 1;
	uint16	sdm_spit_currency_in_rear	: 1;
	uint16	sdm_unused					: 5;
}_sdm_configuration_status_rx_s;	//��ȡ����״̬


typedef struct {
	_sdm_err_num_e sdm_err;
	uint8 sdm_note_thick;
}_sdm_set_bill_thickness_tx_s;	//	����ֽ�Һ��"




typedef struct {
	_sdm_err_num_e sdm_err;
	uint8 sdm_note_thick;
}_sdm_get_bill_thickness_rx_s;	//	��ȡֽ�Һ��"




typedef struct {
	_sdm_err_num_e sdm_err;
	uint8 sdm_note_size;
}_sdm_set_bill_size_tx_s;	//	����ֽ�ҳߴ�"




typedef struct {
	_sdm_err_num_e sdm_err;
	uint8 sdm_note_size;
}_sdm_get_bill_size_rx_s;	//	��ȡֽ�ҳߴ�"




typedef struct {
	_sdm_err_num_e sdm_err;
	uint8 sdm_cassettes_pick_up_notes[8];
	uint8 sdm_cassettes_dispense_notes[8];
	uint8 sdm_cassettes_cassettes_reject_notes[8];	
}_sdm_multi_cassette_dispense_rx_s;	//	���Ǯ�����"




typedef struct {
	_sdm_err_num_e sdm_err;
	uint8 sdm_paper_average_size;
	uint8 sdm_paper_average_thick;
}_sdm_learn_bill_parameter_rx_s;	//	ѧϰֽ�Ҳ���"




typedef struct {
	_sdm_err_num_e sdm_err;
	uint8				sdm_cassette1_reject_total;
	_sdm_reject_code_e	sdm_cassette1_reject_cause;
	uint8 				sdm_cassette2_reject_total;
	_sdm_reject_code_e	sdm_cassette2_reject_cause;
	uint8 				sdm_cassette3_reject_total;
	_sdm_reject_code_e	sdm_cassette3_reject_cause;
	uint8				sdm_cassette4_reject_total;
	_sdm_reject_code_e	sdm_cassette4_reject_cause;
	uint8				sdm_cassette5_reject_total;
	_sdm_reject_code_e	sdm_cassette5_reject_cause;
	uint8				sdm_cassette6_reject_total;
	_sdm_reject_code_e	sdm_cassette6_reject_cause;
	uint8				sdm_cassette7_reject_total;
	_sdm_reject_code_e	sdm_cassette7_reject_cause;
	uint8				sdm_cassette8_reject_total;
	_sdm_reject_code_e	sdm_cassette8_reject_cause;	
}_sdm_report_reject_reason_rx_s;	//	�ܸ�ԭ��"




typedef struct {
	_sdm_err_num_e sdm_err;
	uint8 sdm_exit_right_sensor_value;
	uint8 sdm_exit_left_sensor_value;
	uint8 sdm_top_sensor_value;
	uint8 sdm_reject_sensor_value;
	uint8 sdm_thickness_sensor_value;
	uint8 sdm_middle_sensor_value;
	uint8 pickup_sensor_value[8];
}_sdm_sensor_voltage_read_rx_s;	//	��������ѹֵ"




typedef struct {
	uint8 sdm_set_item;
	uint8 sdm_set_type;
}_sdm_total_counter_read_clear_tx_s;	//	��ȡ����������"


typedef struct {
	_sdm_err_num_e sdm_err;
	uint32	sdm_cassette1_note_amount;		//���ݸ��ֽ���ǰ��
	uint32	sdm_cassette2_note_amount;
	uint32	sdm_cassette3_note_amount;
	uint32	sdm_cassette4_note_amount;
	uint32	sdm_cassette5_note_amount;
	uint32	sdm_cassette6_note_amount;
	uint32	sdm_cassette7_note_amount;
	uint32	sdm_cassette8_note_amount;
}_sdm_total_counter_read_clear_rx_s;	//	��ȡ����������"


typedef struct {
	uint8 len;
	uint8 cmd;
	uint16 exe_time;
	uint8 dat[SDM2000_COMM_DATA_LENGHT];
} _sdm2000_cmd_s;

typedef struct {
	_exe_s note;
	uint8 cass1;
	uint8 cass2;
} _note_machine_cmd_s;


extern	_note_machine_cmd_s note_machine_cmd;


extern	_sdm_state_u	sys_sdm_state;
extern	_sdm_err_num_e	sys_sdm_err;

extern	OS_EVENT	*pHopBox1;
extern	OS_EVENT 	*bill_signal;
extern	OS_EVENT 	*bill_to_note;
extern	OS_EVENT 	*note_to_bill;



extern void TaskNoteMachine(void *pdata);
extern void NoteMachineInit(void);
extern void NoteDispenseBill(uint8 *dst);
//extern uint8 analyze_data(uint8 *data, uint8 cmd);
extern uint8 CassetteDispense(uint8 *const cashbox);



#endif

