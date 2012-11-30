#include "includes.h"


_sys_st_u sys_state;

_exe_s	coinchange_exe;
_exe_s	notechange_exe;
_exe_s	print_exe;
_exe_s  print_amount_exe;
_exe_s  print_record;



uint8 GetExeCommand(_exe_s *comm)
{
	if (comm->exe_flag == 0)
	{
		if (comm->exe_st <= EXE_INVALID)
			return comm->exe_st;
		else
			return EXE_INVALID;
	}
	else
	{
		return 0xff;
	}
}

uint8 SetExeCommand(_exe_s *comm, uint8 st, uint8 *err)
{
	if ((comm->exe_flag == 0) && (st < EXE_INVALID))
	{
		switch (comm->exe_st)
		{
		case EXE_WAIT:
			comm->exe_st = st;
			*err = SYS_NO_ERR;
			return TRUE;
			break;

		case EXE_WRITED:
			if (st == EXE_RUNNING)
			{
				comm->exe_st = st;
				*err = SYS_NO_ERR;
				return TRUE;
			}
			else if (st == EXE_WAIT)
			{
				comm->exe_st = st;
				*err = COMMAND_ERR_LOST;
				return FALSE;
			}
			else
			{
				*err = COMMAND_ERR_EXE_CLASH;
				return FALSE;
			}
			break;

		case EXE_RUNNING:
			if ((st == EXE_WAIT) || (st == EXE_RUN_ABORT))
			{
				comm->exe_st = st;
				*err = SYS_NO_ERR;
				return TRUE;
			}
			else
			{
				*err = COMMAND_ERR_EXE_CLASH;
				return FALSE;
			}

			break;

		case EXE_RUN_ABORT:
		case EXE_INVALID:
			if (st == EXE_WAIT)
			{
				comm->exe_st = st;
				*err = SYS_NO_ERR;
				return TRUE;
			}
			else
			{
				*err = COMMAND_ERR_EXE_CLASH;
				return FALSE;
			}
			break;

		default:
			comm->exe_st = EXE_WAIT;
			*err = COMMAND_ERR_EXE_CLASH;
			return FALSE;
			break;

		}
	}
	else if (comm->exe_flag != 0)
	{
		*err = COMMAND_ERR_CANNOT_CHANGE;
		return FALSE;
	}
	else
	{
		*err = COMMAND_ERR_ST_INVALID;
		return FALSE;
	}
}












