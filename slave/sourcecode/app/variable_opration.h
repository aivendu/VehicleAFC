#ifndef _VARIABLE_OPRATION_H
#define _VARIABLE_OPRATION_H




typedef struct {
	uint16 change_flag			:1;		//	1-已更改，0-未更改，修改之前判断，相同清零，反之置一
	uint16 write_flag			:1;		//	1-正在写，0-没有写，动态属性
	uint16 read_flag			:1;		//	1-正在读，0-没有读，动态属性
	uint16 read_allow			:2;		//	2-条件读，1-读允许，0-禁止读。静态属性
	uint16 write_allow			:2;		//	2-条件读，1-写允许，0-禁止写，静态属性
	uint16 read_num				:9;		//	从修改后记起，该数据被读取的次数
} _variable_flag_s;

typedef struct {
	_variable_flag_s vflag;
	void *data;
} _variable_s;



#define	V_CHANGE			(1<<0)
#define	V_WRITE				(1<<1)
#define	V_READ				(1<<2)
#define	V_READ_ALLOW(c)		(c<<3)
#define	V_WRITE_ALLOW(c)	(c<<5)

#define CONDITION			2
#define ALLOWABLE			1
#define PROHIBIT			0


extern _variable_s * CreateVariable(void *data);
extern uint8 StartReadVariable(_variable_s *var,void *data);	//	开始读
extern uint8 StartWriteVariable(_variable_s *var,void *data);	//	开始写
extern void StopReadVariable(_variable_s *var);				//	停止读
extern void StopWriteVariable(_variable_s *var);				//	停止写


#define writevariable(a,b)      (a=b)





#endif

