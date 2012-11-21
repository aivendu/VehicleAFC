#include "includes.h"

/*
*********************************************************************************************************
										PTRExeTask()
									 LCD控制函数的具体实现
*********************************************************************************************************
*/


char print_buffer[TEMPSIZE];				//	用于缓存打印命令数据

#define		INVERSE		(0x01<<1)
#define		LTALIC		(0x01<<2)
#define		BOLD		(0x01<<3)
#define		HIGH		(0x01<<4)
#define		WIGTH		(0x01<<5)
#define		UNDERLINE	(0x01<<7)

#define		LEFT		0x00
#define		CENTER		0x01
#define		RIGHT		0x02

#define		WaitPrintCommand()			(device_control.cmd.print.exe_st == CMD_NO_EXE)
#define		SetPrintEndFlag()			SetPrintCommand(EXE_WAIT)

struct memDot{
	uint16  x;
	uint16  y;
	uint16  sy;
	uint16  ey;
	uint8  data;
	uint8  datay;
}dot[32];


uint8  dotPtr;
uint8  PTRData[DISSIZE];//自己创建的队列

char *calString(char *string,char *buf);
#if 0
void initLCDM(void);
void SdData(uint8 DData);
void SdCmd(uint8 Command);
void initDrawfram(void);
void drawframe(uint16 x,uint16 y,uint16 enx,uint16 eny);
void memcyData(uint16 x,uint16 y,uint16 enx,uint16 eny);
void Draw_Line(uint16  x1,uint16 y1,uint16 x2,uint16 y2);
void Draw_Dot1(uint16  x,uint16  y);
void Draw_Dot2(uint16  x,uint16  y,uint16 Sx,uint16 Ex);
void PrintASCII_LM2088(uint16 x,uint16 y, uint8 *pstr);
void PrintGB(uint8 x,uint8 y, uint8 *pstr);
void ClearRAM(void);
void RePrintASCII(uint16 x,uint16 y, uint8 *pstr);
#endif

#define PrintSendByte(c)				Uart0SendByte(c,0)
#define PrintSendBytes(a,b)				Uart0SendBytes(a,b,0)

#if 0
void SdCmd(uint8 Command)
{
	//IO0CLR |= P010CS;
	OS_ENTER_CRITICAL();
	ECS;
	CRS;
	
	IO0CLR|=D0D7;
	IO1CLR|=D6;
	if((Command & 0x40)!=0)
	{
		IO1SET|=D6;
	}
	Command = Command & (~0x40);
	IO0SET|=Command<<15;
	//WRDATA(Command);
	EWR;
    delay1us(1);
	IWR;
	//IO0SET |= P010CS;
	ICS;
	OS_EXIT_CRITICAL();
	
}
//--------数据传送子程序------------------
void SdData(uint8 DData)
{
	OS_ENTER_CRITICAL();
	ECS;
	//IO0CLR |= P010CS;
	DRS;
//	WRDATA(DData);

	IO0CLR|=D0D7;
	IO1CLR|=D6;
	if((DData & 0x40)!=0)
	{
		IO1SET|=D6;
	}
	DData = DData & (~0x40);
	IO0SET|=DData<<15;
	EWR;
    delay1us(1);	
	IWR;
	//IO0SET |= P010CS;
	ICS;
	
	OS_EXIT_CRITICAL();
}
void FillRAM(uint16 x,uint16 y,uint16 ex,uint16 ey)//x ,ex为8的倍数
{
	uint8 m =0,n= 0;
	if(x == 0){
		m= 0;
	}else{
		m = x/8;
	}
	
	if(ex == 0){
		n= 0;
	}else{
		n = ex/8;
	}

//	uint8 n = ex/8;
	  SdCmd(0x40);SdData(m);//工作窗口左边界位置（第一字符，取值为1）
	  SdCmd(0x20);SdData(n);//工作窗口右边界位置（水平最大字符数，取值为320）
	  SdCmd(0x50);SdData(y);//工作窗口上边界位置（第一行，取值为1）
	  SdCmd(0x30);SdData(ey);//工作窗口下边界位置（垂直最大行，取值为240）
	  
	  SdCmd(0x12);SdData(0x33);// 同时写入2个显示RAM区域
	  SdCmd(0xe0);SdData(0x00);// 设置填充数据为00H
	  SdCmd(0xf0);SdData(0x88);// 选择填充功能，D3=1
	  OSTimeDly(10);             // 延迟时间，保证写入时间充分	
	  SdCmd(0x40);SdData(0x00);//工作窗口左边界位置（第一字符，取值为1）
	  SdCmd(0x20);SdData(0x27);//工作窗口右边界位置（水平最大字符数，取值为320）
	  SdCmd(0x50);SdData(0x00);//工作窗口上边界位置（第一行，取值为1）
	  SdCmd(0x30);SdData(0xef);//工作窗口下边界位置（垂直最大行，取值为240）  
}
void PrintASCII_LM2088(uint16 x,uint16 y, uint8 *pstr)
//坐标X为屏幕上水平位置，以像素为单位；Y为垂直位置，以像素行为单位
{
	uint8 tmp = 0;
	tmp = x/8;
	SdCmd(0x00);SdData(0x0c);
    SdCmd(0x00);SdData(0x0c); // 设置写入数据模式为字符模式
    SdCmd(0x12);SdData(0x31); // 打开文本RAM区域（第一显示区域）
    SdCmd(0x60);SdData(tmp);    // 设置光标X坐标地址，水平字符数
    SdCmd(0x70);SdData(y);    // 设置光标Y坐标地址，垂直像素行
    SdCmd(0xb0);
    while(*pstr>0)
    {
    	delay1us(150);
       SdData(*pstr++);      // 写入字符代码
       
    }
}
void REPrintASCII_LM2088(uint16 x,uint16 y, uint8 *pstr)
//坐标X为屏幕上水平位置，以像素为单位；Y为垂直位置，以像素行为单位
{
	uint8 tmp = 0;
	tmp = x/8;
	SdCmd(0x00);SdData(0x0c);
    SdCmd(0x10);SdData(0x20); // 设置写入数据模式为字符模式
    SdCmd(0x12);SdData(0x31); // 打开文本RAM区域（第一显示区域）
    SdCmd(0x60);SdData(tmp);    // 设置光标X坐标地址，水平字符数
    SdCmd(0x70);SdData(y);    // 设置光标Y坐标地址，垂直像素行
    SdCmd(0xb0);
    while(*pstr>0)
    {
    	delay1us(150);
       SdData(*pstr++);      // 写入字符代码
       
    }
     SdCmd(0x10);SdData(0x00);
}
//-------汉字串写入子程序---------------
void PrintGB(uint8 x,uint8 y, uint8 *pstr)
//坐标X为屏幕上水平位置，以字符为单位；Y为垂直位置，以像素行为单位
{  

    SdCmd(0x00);SdData(0x0c);  // 设置写入数据模式为字符模式
    SdCmd(0x12);SdData(0x31);  // 打开文本RAM区域（第一显示区域）?
    SdCmd(0x60);SdData(x);     // 设置光标X坐标地址，水平字符数
    SdCmd(0x70);SdData(y);     // 设置光标Y坐标地址，垂直像素行
    SdCmd(0xb0);
    while(*pstr>0)
      {
      SdData(*pstr++);       // 写入字符代码
      }
}
void RePrintASCII(uint16 x,uint16 y, uint8 *pstr)//
//坐标X为屏幕上水平位置，以像素为单位；Y为垂直位置，以像素行为单位
{
	uint8 tmp = 0;
	tmp = (x / 8);///转化为字符数
    SdCmd(0x00);SdData(0x0c); // 设置写入数据模式为字符模式
    SdCmd(0x10);SdData(0x26); // 设置
    SdCmd(0x12);SdData(0x31); // 打开文本RAM区域（第一显示区域）
    SdCmd(0x60);SdData(tmp);    // 设置光标X坐标地址，水平字符数
    SdCmd(0x70);SdData(y);    // 设置光标Y坐标地址，垂直像素行
    SdCmd(0xb0);
    while(*pstr>0)
    {
    	delay1us(15);
       SdData(*pstr++);      // 写入字符代码
       //delay1us(15);
    }
    SdCmd(0x10);SdData(0x00);
}
void ClearRAM()
{
  SdCmd(0x40);SdData(0x00);//工作窗口左边界位置（第一字符，取值为1）
  SdCmd(0x20);SdData(0x27);//工作窗口右边界位置（水平最大字符数，取值为320）
  SdCmd(0x50);SdData(0x00);//工作窗口上边界位置（第一行，取值为1）
  SdCmd(0x30);SdData(0xef);//工作窗口下边界位置（垂直最大行，取值为240）
  
  SdCmd(0x12);SdData(0x33);// 同时写入2个显示RAM区域
  SdCmd(0xe0);SdData(0x00);// 设置填充数据为00H
  SdCmd(0xf0);SdData(0x88);// 选择填充功能，D3=1
  delayms(10);             // 延迟时间，保证写入时间充分
}
void initLCDM(void)
{
	IO0DIR|=D0D7;											//IO口方向控制
	IO0DIR|=P011RS;
	IO0DIR|=P08WR;
	IO0DIR|=P010CS;
	IO0DIR|=P012RST;
	IO1DIR|=D6;
	
	IO0CLR|=P011RS;												//控制线为低
	IO0CLR|=P08WR;
	IO0CLR|=P010CS;

	ERST;
	OSTimeDly(4);
	IRST;
	OSTimeDly(160);
//初始化设置
	//SdCmd(0x00);
	//SdCmd(0xff);	
	
	 SdCmd(0x00);SdData(0x04);//设置电源为正常模式，软复位无效，写入图形模式，开显示，关闪烁与反显；

	 SdCmd(0x0f);SdData(0x00);//设置中断使能状态

	 SdCmd(0x10);SdData(0x00);//设置文本处理方式 光标开，闪烁，跟随字符变化
	 SdCmd(0x11);SdData(0xe0);//设置光标为块状形式
	 SdCmd(0x12);SdData(0x31);//设置RAM（显示）工作模式，两个显示区域逻辑或合成显示

	 //模块显示窗口分辨率设置

	 SdCmd(0x03);SdData(0x00);
	//-------Under OSC=6MHz   FR=66Hz --------------------------------
	 SdCmd(0x01);SdData(0x68);//设置BUSY信号高有效；
	 SdCmd(0x90);SdData(0x6a);//设置数据移位时钟参数   
	 //----------------------------------------------------------------
	 SdCmd(0x21);SdData(0x27);//显示窗口右边界位置, 水平最大字符数，取值为320   
	 SdCmd(0x31);SdData(0xef);//显示窗口下边界位置（垂直最大行，取值为240）

	 //模块工作显示窗口设置
	 SdCmd(0x40);SdData(0x00);//工作窗口左边界位置（第一字符，取值为1）
	 SdCmd(0x20);SdData(0x27);//工作窗口右边界位置（水平最大字符数，取值为320）
	 SdCmd(0x50);SdData(0x00);//工作窗口上边界位置（第一行，取值为1）
	 SdCmd(0x30);SdData(0xef);//工作窗口下边界位置（垂直最大行，取值为240）
	//光标设置
	 SdCmd(0x60);SdData(0x00);//设置光标位置X坐标，使用初始值
	 SdCmd(0x70);SdData(0x00);//设置光标位置Y坐标，使用初始值

	 SdCmd(0x80);SdData(0x33);//设置闪烁时间
	 SdCmd(0xf0);SdData(0x80);//设置字库，选择GB汉字库和ASCII块0字符库
	 SdCmd(0xf1);SdData(0x00);//设置字符原大尺寸
	ClearRAM();
}

void Draw_Dot1(uint16  x,uint16  y)
//坐标X为屏幕上水平位置，以像素点为单位；Y为垂直位置，以像素行为单位
{
    uint8 Data,m,i;
    SdCmd(0x00);SdData(0x04);  // 设置写入数据模式为图形模式
    SdCmd(0x12);SdData(0x32);  // 打开图形RAM区域（第二显示区域）
    SdCmd(0x60);SdData(x/8);   // 设置光标X坐标地址，按照字节（含8点）计算
    SdCmd(0x70);SdData(y);     // 设置光标Y坐标地址，垂直像素行
    m=x%8;                    // 计算显示位
    Data=0x80;
  
    Data=Data>>m;             // 求得显示点数据
	SdCmd(0xb1);//m = RdData();//m=RdData();   // 读该地址的数据
	for(i=0;i<dotPtr;i++)
	{
		if(dot[i].x/8 == x/8)
		{
			if(y >=dot[i].sy && y <= dot[i].ey)
			{
				if(y == dot[i].y)
				{
					Data =Data |dot[i].data;		
				}else{
					Data =Data |dot[i].datay;
					dot[i].datay = Data;
				}
				break;
			}
		}
	}
   // Data=Data|m;                // 合成点数据
    SdCmd(0x60);SdData(x/8);
    SdCmd(0x70);SdData(y);
	SdCmd(0xb0);SdData(Data);
}   
void Draw_Dot2(uint16  x,uint16  y,uint16 Sx,uint16 Ex)
//坐标X为屏幕上水平位置，以像素点为单位；Y为垂直位置，以像素行为单位
{
    uint8 Data,i;
    SdCmd(0x00);SdData(0x04);  // 设置写入数据模式为图形模式
    SdCmd(0x12);SdData(0x32);  // 打开图形RAM区域（第二显示区域）
    
    Data = 0xff;
 	for(i=0;i<dotPtr;i++)
	{
		
		if(dot[i].x/8  == x && y == dot[i].y)
		{
			
			Data = dot[i].data |dot[i].datay ;
		}
	}
    SdCmd(0x60);SdData(x);
    SdCmd(0x70);SdData(y);
	SdCmd(0xb0);SdData(Data);
}             
void flexMethod(uint8 x){
	if(x==1){
		SdCmd(0xf1);SdData(0x0F);		
	}else if(x == 2){
		SdCmd(0xf1);SdData(0x5F);
	}else if(x ==3){
		SdCmd(0xf1);SdData(0xAF);			
	}else if(x ==4){
		SdCmd(0xf1);SdData(0xFF);				
	}
}

void Draw_Line(uint16  x1,uint16 y1,uint16 x2,uint16 y2)
// (x1,y1),(x2,y2)：直线起始和终止位置坐标，以像素点为单位
{
	uint16 tmp,tmp1,xt,xe;
	  if(x1 == x2){
	  		if(y1 >y2){
	  			return;
	  		}
	  		while(y1<y2+1){
	  			Draw_Dot1(x1,y1);
	  			y1++;
	  		}
	  		
	  }else if(y1 == y2){
	 		if(x1 >x2){
	  			return;
	  		}
	  		xt = x1/8;
	  		xe = x2/8;
	  		
	  		tmp = xt;
	  		tmp1 = y1;
	  		while(xt <= xe)
	  		{
	  			Draw_Dot2(xt,y1,tmp,tmp1);
	  			xt++;
	  		}
	  }

}

void memcyData(uint16 x,uint16 y,uint16 enx,uint16 eny){
				uint8 i;
				i = 0;
					dot[dotPtr].x = x;
					dot[dotPtr].y = y;	
					dot[dotPtr].sy = y;
					dot[dotPtr].ey = eny;
					dot[dotPtr].datay = 0x80 >> (x%8);
					for(i=0;i<dotPtr;i++){
						if(dot[i].x/8 == x/8 &&  y >= dot[i].sy && y <= dot[i].ey){
							dot[dotPtr].datay = 0x80 >> (x%8)|dot[i].datay;
						}
					}
					dot[dotPtr++].data = 0xff >> (x%8);
					
					dot[dotPtr].x = x;
					dot[dotPtr].y = eny;
					dot[dotPtr].sy = y;
					dot[dotPtr].ey = eny;
					dot[dotPtr].datay =  0x80 >> (x%8);			
					for(i=0;i<dotPtr;i++){
						if(dot[i].x/8 == x/8 && eny >= dot[i].sy && eny <= dot[i].ey){
							dot[dotPtr].datay = 0x80 >> (x%8)|dot[i].datay;
						}
					}
					
					dot[dotPtr++].data = 0xff >> (x%8);
					
					dot[dotPtr].x = enx;
					dot[dotPtr].y = y;	
					dot[dotPtr].sy = y;
					dot[dotPtr].ey = eny;
					dot[dotPtr].datay = 0x80 >> (enx%8);		
					for(i=0;i<dotPtr;i++){
						if(dot[i].x/8 == enx/8 && y >= dot[i].sy && y <= dot[i].ey){
							dot[dotPtr].datay = 0x80 >> (enx%8)|dot[i].datay;
						}
					}
						
					dot[dotPtr++].data = (0xff >>(8- enx%8))<<(8- enx%8);
					
					dot[dotPtr].x = enx;
					dot[dotPtr].y = eny;					
					dot[dotPtr].sy = y;
					dot[dotPtr].ey = eny;
					dot[dotPtr].datay =  0x80 >> (enx%8);
					for(i=0;i<dotPtr;i++){
						if(dot[i].x/8 == enx/8 && eny >= dot[i].sy && eny <= dot[i].ey){
							dot[dotPtr].datay = 0x80 >> (enx%8)|dot[i].datay;
						}
					}
					
					dot[dotPtr++].data = (0xff >>(8- enx%8))<<(8- enx%8);
}
void drawframe(uint16 x,uint16 y,uint16 enx,uint16 eny){
					memcyData(x,y,enx,eny);
					Draw_Line(x,y,x,eny);
					Draw_Line(x,y,enx,y);
					Draw_Line(enx,y,enx,eny);
					Draw_Line(x,eny,enx,eny);	
					
}
void initDrawfram()
{
					dotPtr = 0;					
					drawframe(8,10,ENDFRAME,230);
					delayms(1);
					drawframe(ENDFRAME+OFFSET,10,ENDFRAMEY,90);
					delayms(1);				
					drawframe(ENDFRAME+OFFSET,100,ENDFRAMEY,230);					
}
#endif

uint8	print_mode(uint8 dat)//设置打印机模式
{
	PrintSendByte(0x1B);
	PrintSendByte(0x21);
	PrintSendByte(dat);
	return	TRUE;
}

uint8	freed(uint8	dat)//将纸向前推一段距离
{
	PrintSendByte(0x1B);
	PrintSendByte(0x4A);
	PrintSendByte(dat);
	return	TRUE;
}
uint8	back(uint8	dat)//将纸向反推一段距离 dat*0.125mm
{
	PrintSendByte(0x1B);
	PrintSendByte(0x6A);
	PrintSendByte(dat);
	return	TRUE;
}

uint8 CutALL(void){
	PrintSendByte(0x0a);
	PrintSendByte(0x1b);
	PrintSendByte(0x69);
	PrintSendByte(0x00);
	return TRUE;
}

uint8	cut(void)		//切纸
{
	#if 0
	PrintSendByte(0x1B);
	PrintSendByte(0x69);
	#endif
	
	//cut[]={0x0A,0x1D,0x56,0x42,0x00}
	
	PrintSendByte(0x0a);
	PrintSendByte(0x1D);
	PrintSendByte(0x56);
	PrintSendByte(0x42);
	PrintSendByte(0x00);
	
	return	TRUE;
}
uint8	get_point(void)		//找黑标点位置
{
	PrintSendByte(0x12);
	PrintSendByte(0x6d);
	PrintSendByte(0x00);
	PrintSendByte(0xff);
	PrintSendByte(0x7f);
	return	TRUE;
}
uint8	set_thick(uint8 dat)		//设定浓度  范围：70~200
{
	PrintSendByte(0x12);
	PrintSendByte(0x7e);
	PrintSendByte(dat);
	return	TRUE;
}
uint8	set_position(uint8	dat)		// 设定对其方式
{
	PrintSendByte(0x1B);
	PrintSendByte(0x61);
	PrintSendByte(dat);
	return	TRUE;
}

uint8	SetLeftMargin(uint8 dat) {	// 设置左边距,单位mm
	
	PrintSendByte(0x1D);
	PrintSendByte(0x4C);
	PrintSendByte(dat);
	return	TRUE;
}

uint8	Ent(uint8	dat)
{
	uint8 i;
	
	for(i=dat;i>0;i--)
	{
		PrintSendByte(0x0D);
		PrintSendByte(0x0A);
	}
	return TRUE;
}

void PrintAmount(void)
{
	Ent(1);
	print_mode(BOLD|HIGH|WIGTH);
	set_position(CENTER);
	//	打印车票
	sprintf(print_buffer,"交易统计");
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));
	Ent(1);
	print_mode(0);
	set_position(LEFT);
	SetLeftMargin(10);
	Ent(1);
	sprintf(print_buffer,"===============================");
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	//	打印时间
	sprintf(print_buffer,"打印时间: %4d-%02d-%02d %02d:%02d:%02d",YEAR,MONTH,DOM,HOUR,MIN,SEC);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(2);
	OSTimeDly(PRINT_TIME_TICK);
	
	sprintf(print_buffer,"-------------------------------");
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	
	sprintf(print_buffer,"交易日期: %4d-%02d-%02d",device_control.trade_amount.year,
									device_control.trade_amount.month,device_control.trade_amount.day);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	
	device_control.trade_amount.driver_id[9] = 0;
	sprintf(print_buffer,"司机工号: %s",device_control.trade_amount.driver_id);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	
	sprintf(print_buffer,"-------------------------------");
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	sprintf(print_buffer,"实收总额: %d元",device_control.trade_amount.realpay_amount);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	sprintf(print_buffer,"营收总额: %d元",device_control.trade_amount.needpay_amount);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	sprintf(print_buffer,"硬币找零个数: %d个",device_control.trade_amount.coin_dis_amount);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	sprintf(print_buffer,"纸币钱箱1找零张数: %d张",device_control.trade_amount.note_1_dis_amount);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	sprintf(print_buffer,"纸币钱箱2找零张数: %d张",device_control.trade_amount.note_2_dis_amount);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	sprintf(print_buffer,"交易笔数: %d次",device_control.trade_amount.trade_num);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	sprintf(print_buffer,"上车人数: %d人",device_control.trade_amount.trade_people);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	Ent(5);
	CutALL();
}

void PrintReceipt(void)
{
	uint8 i;
	
	print_mode(0);
	set_position(LEFT);
	SetLeftMargin(10);
	Ent(1);
	sprintf(print_buffer,"===============================");
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	//	打印时间
	sprintf(print_buffer,"时  间: %4d-%02d-%02d %02d:%02d:%02d",device_control.trade.tm.year+2000,device_control.trade.tm.month,
					device_control.trade.tm.day,device_control.trade.tm.hour,device_control.trade.tm.min,device_control.trade.tm.sec);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);		
	//	打印流水号
	sprintf(print_buffer,"流水号: 223%010d%05d",TimeSec(device_control.trade.tm.year+2000,device_control.trade.tm.month,device_control.trade.tm.day,
					device_control.trade.tm.hour,device_control.trade.tm.min,device_control.trade.tm.sec),device_control.trade.tm.serail_num);		//	流水号格式"df"+"时间的秒数+"序列号"
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);		

	sprintf(print_buffer,"车牌号: %s",sys_config_ram.sc.license_plate);		//	打印车牌号
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);			

	sprintf(print_buffer,"总票价: %d元",device_control.trade.tm.needpay);			//	打印总票价
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);			

	sprintf(print_buffer,"付  款: %d元",device_control.trade.tm.realpay);			//	打印总票价
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);	

	sprintf(print_buffer,"找  零: %d元",(device_control.trade.cr.cass1_dis * 5 + device_control.trade.cr.cass2_dis * 10 + device_control.trade.cr.coin_dis));			//	打印总票价
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);	
	
	sprintf(print_buffer,"-------------------------------");
	 PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer)); 
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);

	sprintf(print_buffer,"起点站  终点站  票价(元) 人数");				//	打印标题
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	
	sprintf(print_buffer,"-------------------------------");
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer)); 
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
		
	for (i=0;i<device_control.trade.tm.des_num;i++) {		//	循环打印每个站点的信息
		sprintf(print_buffer,"%-8s%-8s%  -9d%  -2d",curr_line.station[device_control.trade.rm[i].trade_start_st-1].station_name,
			curr_line.station[device_control.trade.rm[i].trade_end_st-1].station_name,device_control.trade.rm[i].price,device_control.trade.rm[i].number_of_people);
		PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));
		Ent(1);
		OSTimeDly(PRINT_TIME_TICK);
	}
}

void  TaskPTRExe(void *pdata)
{
	//uint8 i;
	
	memset((char *)print_buffer,0,TEMPSIZE);
	
	IO0SET |= POWER_CONTROL_PRINT_MACHINE;

	print_exe.exe_st = CMD_WITE;
	device_control.sys_device.print_machine_state = PRINT_MACHINE_NORMAL;
	
	for(;;)
	{
		//OSSemPend(pPTRSEM,0,&errno);
		if (WaitPrintCommand()) {
			//OSTimeDly(OS_TICKS_PER_SEC*5);
			device_control.sys_device.print_machine_state = PRINT_MACHINE_PRINT_RUNNING;
			device_control.cmd.print.exe_st = CMD_RUNNING;
			RequestUart(PRINTER_UART0,0);
			//	打印车票抬头
			Ent(1);
			print_mode(BOLD|HIGH|WIGTH);
			set_position(CENTER);
			sprintf(print_buffer,"乘 车 小 票");
			PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));
			Ent(1);
			//	打印交易数据
			PrintReceipt();
			//	切纸
			Ent(4);
			CutALL();
			FreeUart(PRINTER_UART0);
			device_control.cmd.print.exe_st = CMD_EXE_END;
			device_control.sys_device.print_machine_state = PRINT_MACHINE_NORMAL;
			SetPrintEndFlag();	//	任务结束，修改标志
		}
		else if (device_control.cmd.print_amount.exe_st == CMD_NO_EXE)
		{
			device_control.sys_device.print_machine_state = PRINT_MACHINE_PRINT_RUNNING;
			device_control.cmd.print_amount.exe_st = CMD_RUNNING;
			RequestUart(PRINTER_UART0,0);
			PrintAmount();
			FreeUart(PRINTER_UART0);
			device_control.cmd.print_amount.exe_st = CMD_EXE_END;
			device_control.sys_device.print_machine_state = PRINT_MACHINE_NORMAL;
		}
		else if(device_control.cmd.print_record.exe_st == CMD_NO_EXE)
		{
			device_control.sys_device.print_machine_state = PRINT_MACHINE_PRINT_RUNNING;
			//	打印车票抬头
			RequestUart(PRINTER_UART0,0);
			Ent(1);
			print_mode(BOLD|HIGH|WIGTH);
			set_position(CENTER);
			sprintf(print_buffer,"交易明细");
			PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));
			Ent(1);
			FreeUart(PRINTER_UART0);
			while (1)
			{
				if (device_control.cmd.print_record.exe_st == CMD_NO_EXE)
				{
					//	打印交易数据
					device_control.cmd.print_record.exe_st = CMD_RUNNING;
					RequestUart(PRINTER_UART0,0);
					PrintReceipt();
					FreeUart(PRINTER_UART0);
					device_control.cmd.print_record.exe_st = CMD_EXE_END;
				}
				else if (device_control.cmd.print_record.exe_st == CMD_WITE)
				{
					//	打印结束，切纸
					RequestUart(PRINTER_UART0,0);
					Ent(4);
					CutALL();
					FreeUart(PRINTER_UART0);
					break;	
				}
				OSTimeDly(1);
			}
			device_control.sys_device.print_machine_state = PRINT_MACHINE_NORMAL;
		}
		else
		{
			OSTimeDly(20);
		}
	}
}






