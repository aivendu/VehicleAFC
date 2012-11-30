/*
纸币机状态
0--正常未初始化；
1--正常已初始化状态；
2--设备断电；
3--卡币状态；
4--第一钱箱无币；
5--第一钱箱币少；
6--第二钱箱无币；
7--第二钱箱币少；
8-正在找币；
9-通信异常；
10-设备异常
11-找零异常
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
		OSTimeDly(OS_TICKS_PER_SEC / 2);		//	延时500 ms
		OpenNotePower();					//	上电
		OSTimeDly(OS_TICKS_PER_SEC * 2);		//	延时2S
		err = Sdm2000Rest();				//	读状态
		if (err == SYS_NO_ERR)  			//	通信正常
		{
			opration_num = 0 ;
		}
		else
		{
			if ((++opration_num) > NOTE_MACHINE_RETRY_TIME)		//	连续通信不上
			{
				opration_num = 0;
				state = NOTE_MACHINE_COMMUNICATION_ABNORMAL;	//	通信异常
			}
			break;
		}
		err = Sdm2000Diagnostics();					//	诊断
		if (err == SYS_NO_ERR)
		{
			switch (cmd_temp[1])
			{
			case SDM_NORMAL:
				state = NOTE_MACHINE_NORMAL;
			case SDM_EXIT_SENSOR_JAM:
			case SDM_TOP_SENSOR_JAM:
			case SDM_REJECT_SENSOR_JAM:
				state = NOTE_MACHINE_NOTE_JAM;	//	卡钞
				break;
			case SDM_CASSETTE_1_EMPTY:
				state = NOTE_MACHINE_CASHBOX1_NO_MONEY;		//	钱箱一无币
				break;
			case SDM_CASSETTE_2_EMPTY:
				state = NOTE_MACHINE_CASHBOX2_NO_MONEY;		//	钱箱二无币
				break;
			default :
				state = NOTE_MACHINE_ABNORMAL;				//	机器异常
				break;
			}
		}
		else
		{
			if ((++opration_num) > NOTE_MACHINE_RETRY_TIME)		//	连续通信不上
			{
				opration_num = 0;
				state = NOTE_MACHINE_COMMUNICATION_ABNORMAL;	//	通信异常
			}
		}
		break;
	case NOTE_MACHINE_NORMAL:
	case NOTE_MACHINE_NOTE_JAM:
	case NOTE_MACHINE_CASHBOX1_NO_MONEY:
	case NOTE_MACHINE_CASHBOX2_NO_MONEY:
	case NOTE_MACHINE_CASHBOX1_LITTLE:
	case NOTE_MACHINE_CASHBOX2_LITTLE:
		//if (state != NOTE_MACHINE_NO_INIT) {	//	未初始化，不能读状态
		err = Sdm2000ReadState();		//	读状态
		err_code = cmd_temp[1];		//	取状态
		//}
		//else {
		//	err = 0x00;				//	清零状态
		//}

		if (err != SYS_NO_ERR)  		//	通信异常
		{
			if ((++opration_num) > NOTE_MACHINE_RETRY_TIME)  	//	连续通信异常
			{
				opration_num = 0;
				state = NOTE_MACHINE_NO_INIT;	//	重新初始化
				CloseNotePower();				//	断电
			}
		}
		else if (err_code != SDM_NORMAL)  		//	机器异常
		{
			switch (err_code)
			{
			case SDM_EXIT_SENSOR_JAM:
			case SDM_TOP_SENSOR_JAM:
			case SDM_REJECT_SENSOR_JAM:
				state = NOTE_MACHINE_NOTE_JAM;	//	卡钞
				break;
			case SDM_CASSETTE_1_EMPTY:
				state = NOTE_MACHINE_CASHBOX1_NO_MONEY;		//	钱箱一无币
				break;
			case SDM_CASSETTE_2_EMPTY:
				state = NOTE_MACHINE_CASHBOX2_NO_MONEY;		//	钱箱二无币
				break;
			default :
				state = NOTE_MACHINE_ABNORMAL;				//	机器异常
				break;
			}
		}
		else if (note_machine_cmd.note.exe_st == CMD_NO_EXE)
		{
			state = NOTE_MACHINE_CHANGE_RUNNING;				//	开始找零
		}
		else if (cashbox1_note_amount < MONEY_LITTLE_WARM_NUM)
		{
			state = NOTE_MACHINE_CASHBOX1_LITTLE;				//	钱箱一币少
		}
		else if (cashbox2_note_amount < MONEY_LITTLE_WARM_NUM)
		{
			state = NOTE_MACHINE_CASHBOX2_LITTLE;				//	钱箱二币少
		}
		break;
	case NOTE_MACHINE_OUTAGE:		//	断电
		CloseNotePower();
		break;
	case NOTE_MACHINE_NOTE_JAM:		//	卡钞，修复好后需重新上电恢复

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
				state = NOTE_MACHINE_NOTE_JAM;	//	卡钞
				break;
			case SDM_CASSETTE_1_EMPTY:
				state = NOTE_MACHINE_CASHBOX1_NO_MONEY;		//	钱箱一无币
				break;
			case SDM_CASSETTE_2_EMPTY:
				state = NOTE_MACHINE_CASHBOX2_NO_MONEY;		//	钱箱二无币
				break;
			default :
				state = NOTE_MACHINE_ABNORMAL;				//	机器异常
				break;
			}
		}
		else if (err == UART_COMMUNICATION_ERR_COMM_NACK)
		{
			if ((++opration_num) > NOTE_MACHINE_RETRY_TIME)  	//	连续通信异常
			{
				opration_num = 0;
				state = NOTE_MACHINE_CHANGE_ABNORMAL;	//	找零异常
			}
		}
		else
		{
			state = NOTE_MACHINE_CHANGE_ABNORMAL;
		}
		break;
	case NOTE_MACHINE_COMMUNICATION_ABNORMAL:
		err = Sdm2000ReadState();				//	诊断
		if (err == SYS_NO_ERR)  				//	通信无错误
		{
			if (cmd_temp[1] == SDM_NORMAL)		//	机器无错误
			{
				state = NOTE_MACHINE_NORMAL;	//	返回正常状态
			}
			else
			{
				state = NOTE_MACHINE_NO_INIT;	//	重新开始运行
			}
		}
		break;
	case NOTE_MACHINE_CHANGE_ABNORMAL:
		err = Sdm2000LastState();
		break;
	case NOTE_MACHINE_ABNORMAL:
		err = Sdm2000Diagnostics();			//	诊断
		if (err == OS_NO_ERR)
		{
			if (cmd_temp[1] == SDM_NORMAL)	//找零机恢复正常
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

