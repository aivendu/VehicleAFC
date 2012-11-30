/*
ֽ�һ�״̬
0--����δ��ʼ����
1--�����ѳ�ʼ��״̬��
2--�豸�ϵ磻
3--����״̬��
4--��һǮ���ޱң�
5--��һǮ����٣�
6--�ڶ�Ǯ���ޱң�
7--�ڶ�Ǯ����٣�
8-�����ұң�
9-ͨ���쳣��
10-�豸�쳣
11-�����쳣
*/
#define NOTE_MACHINE_NO_INIT					0
#define NOTE_MACHINE_NORMAL						1
#define NOTE_MACHINE_OUTAGE						2
#define NOTE_MACHINE_NOTE_JAM					3
#define NOTE_MACHINE_CASHBOX1_NO_MONEY			4
#define NOTE_MACHINE_CASHBOX1_LITTLE			5
#define NOTE_MACHINE_CASHBOX2_NO_MONEY			6
#define NOTE_MACHINE_CASHBOX2_LITTLE			7
#define NOTE_MACHINE_CHANGE_RUNNING				8
#define NOTE_MACHINE_COMMUNICATION_ABNORMAL		9
#define NOTE_MACHINE_ABNORMAL					10
#define NOTE_MACHINE_CHANGE_ABNORMAL			11

#define NOTE_MACHINE_RETRY_TIME					3
#define MONEY_LITTLE_WARM_NUM					10
/*

*/
uint32 cashbox0_coin_amount = 300, cashbox1_note_amount = 100, cashbox2_note_amount = 20;
uint8 NoteStateHandle (uint8 state)
{
	uint8 err, err_code;
	uint8 opration_num = 0;

	state = device_control.sys_device.note_machine_state;
	switch (state)
	{
	case NOTE_MACHINE_NO_INIT:
		OSTimeDly(OS_TICKS_PER_SEC / 2);		//	��ʱ500 ms
		OpenNotePower();					//	�ϵ�
		OSTimeDly(OS_TICKS_PER_SEC * 2);		//	��ʱ2S
		err = Sdm2000Rest();				//	��״̬
		if (err == SYS_NO_ERR)  			//	ͨ������
		{
			opration_num = 0 ;
		}
		else
		{
			if ((++opration_num) > NOTE_MACHINE_RETRY_TIME)		//	����ͨ�Ų���
			{
				opration_num = 0;
				state = NOTE_MACHINE_COMMUNICATION_ABNORMAL;	//	ͨ���쳣
			}
			break;
		}
		err = Sdm2000Diagnostics();					//	���
		if (err == SYS_NO_ERR)
		{
			switch (cmd_temp[1])
			{
			case SDM_NORMAL:
				state = NOTE_MACHINE_NORMAL;
			case SDM_EXIT_SENSOR_JAM:
			case SDM_TOP_SENSOR_JAM:
			case SDM_REJECT_SENSOR_JAM:
				state = NOTE_MACHINE_NOTE_JAM;	//	����
				break;
			case SDM_CASSETTE_1_EMPTY:
				state = NOTE_MACHINE_CASHBOX1_NO_MONEY;		//	Ǯ��һ�ޱ�
				break;
			case SDM_CASSETTE_2_EMPTY:
				state = NOTE_MACHINE_CASHBOX2_NO_MONEY;		//	Ǯ����ޱ�
				break;
			default :
				state = NOTE_MACHINE_ABNORMAL;				//	�����쳣
				break;
			}
		}
		else
		{
			if ((++opration_num) > NOTE_MACHINE_RETRY_TIME)		//	����ͨ�Ų���
			{
				opration_num = 0;
				state = NOTE_MACHINE_COMMUNICATION_ABNORMAL;	//	ͨ���쳣
			}
		}
		break;
	case NOTE_MACHINE_NORMAL:
	case NOTE_MACHINE_NOTE_JAM:
	case NOTE_MACHINE_CASHBOX1_NO_MONEY:
	case NOTE_MACHINE_CASHBOX2_NO_MONEY:
	case NOTE_MACHINE_CASHBOX1_LITTLE:
	case NOTE_MACHINE_CASHBOX2_LITTLE:
		//if (state != NOTE_MACHINE_NO_INIT) {	//	δ��ʼ�������ܶ�״̬
		err = Sdm2000ReadState();		//	��״̬
		err_code = cmd_temp[1];		//	ȡ״̬
		//}
		//else {
		//	err = 0x00;				//	����״̬
		//}

		if (err != SYS_NO_ERR)  		//	ͨ���쳣
		{
			if ((++opration_num) > NOTE_MACHINE_RETRY_TIME)  	//	����ͨ���쳣
			{
				opration_num = 0;
				state = NOTE_MACHINE_NO_INIT;	//	���³�ʼ��
				CloseNotePower();				//	�ϵ�
			}
		}
		else if (err_code != SDM_NORMAL)  		//	�����쳣
		{
			switch (err_code)
			{
			case SDM_EXIT_SENSOR_JAM:
			case SDM_TOP_SENSOR_JAM:
			case SDM_REJECT_SENSOR_JAM:
				state = NOTE_MACHINE_NOTE_JAM;	//	����
				break;
			case SDM_CASSETTE_1_EMPTY:
				state = NOTE_MACHINE_CASHBOX1_NO_MONEY;		//	Ǯ��һ�ޱ�
				break;
			case SDM_CASSETTE_2_EMPTY:
				state = NOTE_MACHINE_CASHBOX2_NO_MONEY;		//	Ǯ����ޱ�
				break;
			default :
				state = NOTE_MACHINE_ABNORMAL;				//	�����쳣
				break;
			}
		}
		else if (note_machine_cmd.note.exe_st == CMD_NO_EXE)
		{
			state = NOTE_MACHINE_CHANGE_RUNNING;				//	��ʼ����
		}
		else if (cashbox1_note_amount < MONEY_LITTLE_WARM_NUM)
		{
			state = NOTE_MACHINE_CASHBOX1_LITTLE;				//	Ǯ��һ����
		}
		else if (cashbox2_note_amount < MONEY_LITTLE_WARM_NUM)
		{
			state = NOTE_MACHINE_CASHBOX2_LITTLE;				//	Ǯ�������
		}
		break;
	case NOTE_MACHINE_OUTAGE:		//	�ϵ�
		CloseNotePower();
		break;
	case NOTE_MACHINE_NOTE_JAM:		//	�������޸��ú��������ϵ�ָ�

		break;
	case NOTE_MACHINE_CHANGE_RUNNING:
		err = Sdm2000MultiCassetteDispense(note_machine_cmd.cass1, note_machine_cmd.cass2)
		      if (err == SYS_NO_ERR)
		{
			switch (cmd_temp[1])
			{
			case SDM_NORMAL:
				state = NOTE_MACHINE_NORMAL;
				break;
			case SDM_EXIT_SENSOR_JAM:
			case SDM_TOP_SENSOR_JAM:
			case SDM_REJECT_SENSOR_JAM:
				state = NOTE_MACHINE_NOTE_JAM;	//	����
				break;
			case SDM_CASSETTE_1_EMPTY:
				state = NOTE_MACHINE_CASHBOX1_NO_MONEY;		//	Ǯ��һ�ޱ�
				break;
			case SDM_CASSETTE_2_EMPTY:
				state = NOTE_MACHINE_CASHBOX2_NO_MONEY;		//	Ǯ����ޱ�
				break;
			default :
				state = NOTE_MACHINE_ABNORMAL;				//	�����쳣
				break;
			}
		}
		else if (err == UART_COMMUNICATION_ERR_COMM_NACK)
		{
			if ((++opration_num) > NOTE_MACHINE_RETRY_TIME)  	//	����ͨ���쳣
			{
				opration_num = 0;
				state = NOTE_MACHINE_CHANGE_ABNORMAL;	//	�����쳣
			}
		}
		else
		{
			state = NOTE_MACHINE_CHANGE_ABNORMAL;
		}
		break;
	case NOTE_MACHINE_COMMUNICATION_ABNORMAL:
		err = Sdm2000ReadState();				//	���
		if (err == SYS_NO_ERR)  				//	ͨ���޴���
		{
			if (cmd_temp[1] == SDM_NORMAL)		//	�����޴���
			{
				state = NOTE_MACHINE_NORMAL;	//	��������״̬
			}
			else
			{
				state = NOTE_MACHINE_NO_INIT;	//	���¿�ʼ����
			}
		}
		break;
	case NOTE_MACHINE_CHANGE_ABNORMAL:
		err = Sdm2000LastState();
		break;
	case NOTE_MACHINE_ABNORMAL:
		err = Sdm2000Diagnostics();			//	���
		if (err == OS_NO_ERR)
		{
			if (cmd_temp[1] == SDM_NORMAL)	//������ָ�����
			{
				state = NOTE_MACHINE_NO_INIT;
			}
		}
		break;
	default:
		state = NOTE_MACHINE_NO_INIT;
		break;
	}
	device_control.sys_device.note_machine_state = state;
	return state;
}

