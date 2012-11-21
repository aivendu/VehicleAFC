#include "includes.h"

/*
*********************************************************************************************************
										PTRExeTask()
									 LCD���ƺ����ľ���ʵ��
*********************************************************************************************************
*/


char print_buffer[TEMPSIZE];				//	���ڻ����ӡ��������

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
uint8  PTRData[DISSIZE];//�Լ������Ķ���

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
//--------���ݴ����ӳ���------------------
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
void FillRAM(uint16 x,uint16 y,uint16 ex,uint16 ey)//x ,exΪ8�ı���
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
	  SdCmd(0x40);SdData(m);//����������߽�λ�ã���һ�ַ���ȡֵΪ1��
	  SdCmd(0x20);SdData(n);//���������ұ߽�λ�ã�ˮƽ����ַ�����ȡֵΪ320��
	  SdCmd(0x50);SdData(y);//���������ϱ߽�λ�ã���һ�У�ȡֵΪ1��
	  SdCmd(0x30);SdData(ey);//���������±߽�λ�ã���ֱ����У�ȡֵΪ240��
	  
	  SdCmd(0x12);SdData(0x33);// ͬʱд��2����ʾRAM����
	  SdCmd(0xe0);SdData(0x00);// �����������Ϊ00H
	  SdCmd(0xf0);SdData(0x88);// ѡ����书�ܣ�D3=1
	  OSTimeDly(10);             // �ӳ�ʱ�䣬��֤д��ʱ����	
	  SdCmd(0x40);SdData(0x00);//����������߽�λ�ã���һ�ַ���ȡֵΪ1��
	  SdCmd(0x20);SdData(0x27);//���������ұ߽�λ�ã�ˮƽ����ַ�����ȡֵΪ320��
	  SdCmd(0x50);SdData(0x00);//���������ϱ߽�λ�ã���һ�У�ȡֵΪ1��
	  SdCmd(0x30);SdData(0xef);//���������±߽�λ�ã���ֱ����У�ȡֵΪ240��  
}
void PrintASCII_LM2088(uint16 x,uint16 y, uint8 *pstr)
//����XΪ��Ļ��ˮƽλ�ã�������Ϊ��λ��YΪ��ֱλ�ã���������Ϊ��λ
{
	uint8 tmp = 0;
	tmp = x/8;
	SdCmd(0x00);SdData(0x0c);
    SdCmd(0x00);SdData(0x0c); // ����д������ģʽΪ�ַ�ģʽ
    SdCmd(0x12);SdData(0x31); // ���ı�RAM���򣨵�һ��ʾ����
    SdCmd(0x60);SdData(tmp);    // ���ù��X�����ַ��ˮƽ�ַ���
    SdCmd(0x70);SdData(y);    // ���ù��Y�����ַ����ֱ������
    SdCmd(0xb0);
    while(*pstr>0)
    {
    	delay1us(150);
       SdData(*pstr++);      // д���ַ�����
       
    }
}
void REPrintASCII_LM2088(uint16 x,uint16 y, uint8 *pstr)
//����XΪ��Ļ��ˮƽλ�ã�������Ϊ��λ��YΪ��ֱλ�ã���������Ϊ��λ
{
	uint8 tmp = 0;
	tmp = x/8;
	SdCmd(0x00);SdData(0x0c);
    SdCmd(0x10);SdData(0x20); // ����д������ģʽΪ�ַ�ģʽ
    SdCmd(0x12);SdData(0x31); // ���ı�RAM���򣨵�һ��ʾ����
    SdCmd(0x60);SdData(tmp);    // ���ù��X�����ַ��ˮƽ�ַ���
    SdCmd(0x70);SdData(y);    // ���ù��Y�����ַ����ֱ������
    SdCmd(0xb0);
    while(*pstr>0)
    {
    	delay1us(150);
       SdData(*pstr++);      // д���ַ�����
       
    }
     SdCmd(0x10);SdData(0x00);
}
//-------���ִ�д���ӳ���---------------
void PrintGB(uint8 x,uint8 y, uint8 *pstr)
//����XΪ��Ļ��ˮƽλ�ã����ַ�Ϊ��λ��YΪ��ֱλ�ã���������Ϊ��λ
{  

    SdCmd(0x00);SdData(0x0c);  // ����д������ģʽΪ�ַ�ģʽ
    SdCmd(0x12);SdData(0x31);  // ���ı�RAM���򣨵�һ��ʾ����?
    SdCmd(0x60);SdData(x);     // ���ù��X�����ַ��ˮƽ�ַ���
    SdCmd(0x70);SdData(y);     // ���ù��Y�����ַ����ֱ������
    SdCmd(0xb0);
    while(*pstr>0)
      {
      SdData(*pstr++);       // д���ַ�����
      }
}
void RePrintASCII(uint16 x,uint16 y, uint8 *pstr)//
//����XΪ��Ļ��ˮƽλ�ã�������Ϊ��λ��YΪ��ֱλ�ã���������Ϊ��λ
{
	uint8 tmp = 0;
	tmp = (x / 8);///ת��Ϊ�ַ���
    SdCmd(0x00);SdData(0x0c); // ����д������ģʽΪ�ַ�ģʽ
    SdCmd(0x10);SdData(0x26); // ����
    SdCmd(0x12);SdData(0x31); // ���ı�RAM���򣨵�һ��ʾ����
    SdCmd(0x60);SdData(tmp);    // ���ù��X�����ַ��ˮƽ�ַ���
    SdCmd(0x70);SdData(y);    // ���ù��Y�����ַ����ֱ������
    SdCmd(0xb0);
    while(*pstr>0)
    {
    	delay1us(15);
       SdData(*pstr++);      // д���ַ�����
       //delay1us(15);
    }
    SdCmd(0x10);SdData(0x00);
}
void ClearRAM()
{
  SdCmd(0x40);SdData(0x00);//����������߽�λ�ã���һ�ַ���ȡֵΪ1��
  SdCmd(0x20);SdData(0x27);//���������ұ߽�λ�ã�ˮƽ����ַ�����ȡֵΪ320��
  SdCmd(0x50);SdData(0x00);//���������ϱ߽�λ�ã���һ�У�ȡֵΪ1��
  SdCmd(0x30);SdData(0xef);//���������±߽�λ�ã���ֱ����У�ȡֵΪ240��
  
  SdCmd(0x12);SdData(0x33);// ͬʱд��2����ʾRAM����
  SdCmd(0xe0);SdData(0x00);// �����������Ϊ00H
  SdCmd(0xf0);SdData(0x88);// ѡ����书�ܣ�D3=1
  delayms(10);             // �ӳ�ʱ�䣬��֤д��ʱ����
}
void initLCDM(void)
{
	IO0DIR|=D0D7;											//IO�ڷ������
	IO0DIR|=P011RS;
	IO0DIR|=P08WR;
	IO0DIR|=P010CS;
	IO0DIR|=P012RST;
	IO1DIR|=D6;
	
	IO0CLR|=P011RS;												//������Ϊ��
	IO0CLR|=P08WR;
	IO0CLR|=P010CS;

	ERST;
	OSTimeDly(4);
	IRST;
	OSTimeDly(160);
//��ʼ������
	//SdCmd(0x00);
	//SdCmd(0xff);	
	
	 SdCmd(0x00);SdData(0x04);//���õ�ԴΪ����ģʽ����λ��Ч��д��ͼ��ģʽ������ʾ������˸�뷴�ԣ�

	 SdCmd(0x0f);SdData(0x00);//�����ж�ʹ��״̬

	 SdCmd(0x10);SdData(0x00);//�����ı�����ʽ ��꿪����˸�������ַ��仯
	 SdCmd(0x11);SdData(0xe0);//���ù��Ϊ��״��ʽ
	 SdCmd(0x12);SdData(0x31);//����RAM����ʾ������ģʽ��������ʾ�����߼���ϳ���ʾ

	 //ģ����ʾ���ڷֱ�������

	 SdCmd(0x03);SdData(0x00);
	//-------Under OSC=6MHz   FR=66Hz --------------------------------
	 SdCmd(0x01);SdData(0x68);//����BUSY�źŸ���Ч��
	 SdCmd(0x90);SdData(0x6a);//����������λʱ�Ӳ���   
	 //----------------------------------------------------------------
	 SdCmd(0x21);SdData(0x27);//��ʾ�����ұ߽�λ��, ˮƽ����ַ�����ȡֵΪ320   
	 SdCmd(0x31);SdData(0xef);//��ʾ�����±߽�λ�ã���ֱ����У�ȡֵΪ240��

	 //ģ�鹤����ʾ��������
	 SdCmd(0x40);SdData(0x00);//����������߽�λ�ã���һ�ַ���ȡֵΪ1��
	 SdCmd(0x20);SdData(0x27);//���������ұ߽�λ�ã�ˮƽ����ַ�����ȡֵΪ320��
	 SdCmd(0x50);SdData(0x00);//���������ϱ߽�λ�ã���һ�У�ȡֵΪ1��
	 SdCmd(0x30);SdData(0xef);//���������±߽�λ�ã���ֱ����У�ȡֵΪ240��
	//�������
	 SdCmd(0x60);SdData(0x00);//���ù��λ��X���꣬ʹ�ó�ʼֵ
	 SdCmd(0x70);SdData(0x00);//���ù��λ��Y���꣬ʹ�ó�ʼֵ

	 SdCmd(0x80);SdData(0x33);//������˸ʱ��
	 SdCmd(0xf0);SdData(0x80);//�����ֿ⣬ѡ��GB���ֿ��ASCII��0�ַ���
	 SdCmd(0xf1);SdData(0x00);//�����ַ�ԭ��ߴ�
	ClearRAM();
}

void Draw_Dot1(uint16  x,uint16  y)
//����XΪ��Ļ��ˮƽλ�ã������ص�Ϊ��λ��YΪ��ֱλ�ã���������Ϊ��λ
{
    uint8 Data,m,i;
    SdCmd(0x00);SdData(0x04);  // ����д������ģʽΪͼ��ģʽ
    SdCmd(0x12);SdData(0x32);  // ��ͼ��RAM���򣨵ڶ���ʾ����
    SdCmd(0x60);SdData(x/8);   // ���ù��X�����ַ�������ֽڣ���8�㣩����
    SdCmd(0x70);SdData(y);     // ���ù��Y�����ַ����ֱ������
    m=x%8;                    // ������ʾλ
    Data=0x80;
  
    Data=Data>>m;             // �����ʾ������
	SdCmd(0xb1);//m = RdData();//m=RdData();   // ���õ�ַ������
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
   // Data=Data|m;                // �ϳɵ�����
    SdCmd(0x60);SdData(x/8);
    SdCmd(0x70);SdData(y);
	SdCmd(0xb0);SdData(Data);
}   
void Draw_Dot2(uint16  x,uint16  y,uint16 Sx,uint16 Ex)
//����XΪ��Ļ��ˮƽλ�ã������ص�Ϊ��λ��YΪ��ֱλ�ã���������Ϊ��λ
{
    uint8 Data,i;
    SdCmd(0x00);SdData(0x04);  // ����д������ģʽΪͼ��ģʽ
    SdCmd(0x12);SdData(0x32);  // ��ͼ��RAM���򣨵ڶ���ʾ����
    
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
// (x1,y1),(x2,y2)��ֱ����ʼ����ֹλ�����꣬�����ص�Ϊ��λ
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

uint8	print_mode(uint8 dat)//���ô�ӡ��ģʽ
{
	PrintSendByte(0x1B);
	PrintSendByte(0x21);
	PrintSendByte(dat);
	return	TRUE;
}

uint8	freed(uint8	dat)//��ֽ��ǰ��һ�ξ���
{
	PrintSendByte(0x1B);
	PrintSendByte(0x4A);
	PrintSendByte(dat);
	return	TRUE;
}
uint8	back(uint8	dat)//��ֽ����һ�ξ��� dat*0.125mm
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

uint8	cut(void)		//��ֽ
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
uint8	get_point(void)		//�Һڱ��λ��
{
	PrintSendByte(0x12);
	PrintSendByte(0x6d);
	PrintSendByte(0x00);
	PrintSendByte(0xff);
	PrintSendByte(0x7f);
	return	TRUE;
}
uint8	set_thick(uint8 dat)		//�趨Ũ��  ��Χ��70~200
{
	PrintSendByte(0x12);
	PrintSendByte(0x7e);
	PrintSendByte(dat);
	return	TRUE;
}
uint8	set_position(uint8	dat)		// �趨���䷽ʽ
{
	PrintSendByte(0x1B);
	PrintSendByte(0x61);
	PrintSendByte(dat);
	return	TRUE;
}

uint8	SetLeftMargin(uint8 dat) {	// ������߾�,��λmm
	
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
	//	��ӡ��Ʊ
	sprintf(print_buffer,"����ͳ��");
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
	//	��ӡʱ��
	sprintf(print_buffer,"��ӡʱ��: %4d-%02d-%02d %02d:%02d:%02d",YEAR,MONTH,DOM,HOUR,MIN,SEC);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(2);
	OSTimeDly(PRINT_TIME_TICK);
	
	sprintf(print_buffer,"-------------------------------");
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	
	sprintf(print_buffer,"��������: %4d-%02d-%02d",device_control.trade_amount.year,
									device_control.trade_amount.month,device_control.trade_amount.day);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	
	device_control.trade_amount.driver_id[9] = 0;
	sprintf(print_buffer,"˾������: %s",device_control.trade_amount.driver_id);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	
	sprintf(print_buffer,"-------------------------------");
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	sprintf(print_buffer,"ʵ���ܶ�: %dԪ",device_control.trade_amount.realpay_amount);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	sprintf(print_buffer,"Ӫ���ܶ�: %dԪ",device_control.trade_amount.needpay_amount);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	sprintf(print_buffer,"Ӳ���������: %d��",device_control.trade_amount.coin_dis_amount);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	sprintf(print_buffer,"ֽ��Ǯ��1��������: %d��",device_control.trade_amount.note_1_dis_amount);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	sprintf(print_buffer,"ֽ��Ǯ��2��������: %d��",device_control.trade_amount.note_2_dis_amount);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	sprintf(print_buffer,"���ױ���: %d��",device_control.trade_amount.trade_num);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	sprintf(print_buffer,"�ϳ�����: %d��",device_control.trade_amount.trade_people);
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
	//	��ӡʱ��
	sprintf(print_buffer,"ʱ  ��: %4d-%02d-%02d %02d:%02d:%02d",device_control.trade.tm.year+2000,device_control.trade.tm.month,
					device_control.trade.tm.day,device_control.trade.tm.hour,device_control.trade.tm.min,device_control.trade.tm.sec);
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);		
	//	��ӡ��ˮ��
	sprintf(print_buffer,"��ˮ��: 223%010d%05d",TimeSec(device_control.trade.tm.year+2000,device_control.trade.tm.month,device_control.trade.tm.day,
					device_control.trade.tm.hour,device_control.trade.tm.min,device_control.trade.tm.sec),device_control.trade.tm.serail_num);		//	��ˮ�Ÿ�ʽ"df"+"ʱ�������+"���к�"
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));		
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);		

	sprintf(print_buffer,"���ƺ�: %s",sys_config_ram.sc.license_plate);		//	��ӡ���ƺ�
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);			

	sprintf(print_buffer,"��Ʊ��: %dԪ",device_control.trade.tm.needpay);			//	��ӡ��Ʊ��
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);			

	sprintf(print_buffer,"��  ��: %dԪ",device_control.trade.tm.realpay);			//	��ӡ��Ʊ��
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);	

	sprintf(print_buffer,"��  ��: %dԪ",(device_control.trade.cr.cass1_dis * 5 + device_control.trade.cr.cass2_dis * 10 + device_control.trade.cr.coin_dis));			//	��ӡ��Ʊ��
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));	
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);	
	
	sprintf(print_buffer,"-------------------------------");
	 PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer)); 
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);

	sprintf(print_buffer,"���վ  �յ�վ  Ʊ��(Ԫ) ����");				//	��ӡ����
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
	
	sprintf(print_buffer,"-------------------------------");
	PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer)); 
	Ent(1);
	OSTimeDly(PRINT_TIME_TICK);
		
	for (i=0;i<device_control.trade.tm.des_num;i++) {		//	ѭ����ӡÿ��վ�����Ϣ
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
			//	��ӡ��Ʊ̧ͷ
			Ent(1);
			print_mode(BOLD|HIGH|WIGTH);
			set_position(CENTER);
			sprintf(print_buffer,"�� �� С Ʊ");
			PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));
			Ent(1);
			//	��ӡ��������
			PrintReceipt();
			//	��ֽ
			Ent(4);
			CutALL();
			FreeUart(PRINTER_UART0);
			device_control.cmd.print.exe_st = CMD_EXE_END;
			device_control.sys_device.print_machine_state = PRINT_MACHINE_NORMAL;
			SetPrintEndFlag();	//	����������޸ı�־
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
			//	��ӡ��Ʊ̧ͷ
			RequestUart(PRINTER_UART0,0);
			Ent(1);
			print_mode(BOLD|HIGH|WIGTH);
			set_position(CENTER);
			sprintf(print_buffer,"������ϸ");
			PrintSendBytes((uint8 *)print_buffer,strlen(print_buffer));
			Ent(1);
			FreeUart(PRINTER_UART0);
			while (1)
			{
				if (device_control.cmd.print_record.exe_st == CMD_NO_EXE)
				{
					//	��ӡ��������
					device_control.cmd.print_record.exe_st = CMD_RUNNING;
					RequestUart(PRINTER_UART0,0);
					PrintReceipt();
					FreeUart(PRINTER_UART0);
					device_control.cmd.print_record.exe_st = CMD_EXE_END;
				}
				else if (device_control.cmd.print_record.exe_st == CMD_WITE)
				{
					//	��ӡ��������ֽ
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






