#include "alarm.h"
#include "oled.h"
#include "temp.h"
#include "chao.h"
#include <math.h>

sbit key=P3^5;
sfr T2MOD=0xC9;     //寄存器T2MOD定义

uchar j,k;
uint S,S1,totle;
float V=342.0;
uint Sav[11];							  //连续测量时10次平均值数组
bit flag_1s,flag_10ms,flag_temp;
bit flag_ok;

u8 Buffer[9] = {0};      //从串口接收的数据
u8 index;   //Buffer的index
u8 clock_value;

void DisplayOLED();
void oled_displayInit();
void key_pros();
void delaySWPB(uint x); //毫秒级延时函数
void Conut(void); 				  //测量计算函数
void Alarm_range();
bit GetTemp();                 //取温度函数

void UartSendBytes(u8 *c);//串口发送数据
void inittimer2();


void main(void)    //主函数
{
	u8 nk=100;
	for(;nk>0;nk--){
		GetTemp();
		delaySWPB(1);
	}
	oled_displayInit();  //
	inittimer0();					
	delaySWPB(10);
	UartSendBytes("AT+CIPMUX=1\r\n");
	delaySWPB(10);
	UartSendBytes("AT+CIPSERVER=1,5000\r\n");
	delaySWPB(10);
	
	inittimer2();
	while(1)
	{
			startb();	 //开始发射超声波
			waitb();
			Conut();		 //计算	 
			DisplayOLED(); 	
			key_pros();
	}
}

void oled_displayInit()
{
		uchar i=12,j=4;
		OLED_Init();
		OLED_Clear();
		
	  do{
			OLED_ShowChar(12+8*i,2,'_',16);
		}while(i--);
		
		OLED_ShowCHinese(32,1,16);
		OLED_ShowChar(50,1,'|',16);
	  OLED_ShowChar(92,1,'m',16);
		OLED_ShowChar(100,1,'/',16);
		OLED_ShowChar(108,1,'s',16);
		
		OLED_ShowChar(47,6,'.',16);
		disp32(99,4,11);   //m
}


void DisplayOLED()				  //显示函数
{
//									 0123456789012
	uchar table[14]="t19 v340 d    ";
	uchar code ta[]="0123456789";
	
	if((S1>=4000)||(flag==1))    //超出测量范围显示“c”
	{	 
		flag=0;
		flag_ok=1;
//		disp32(30,2,8);  //个位
//		disp32(58,2,8); //小数的十位
//		disp32(76,2,8);   //小数的个位
		
		disp32(28,4,8);  //个位
		disp32(56,4,8); //小数的十位
		disp32(74,4,8);   //小数的个位
	}
	
	else{	
		flag_ok=0;
		disp32(28,4,S1/1000);  //个位
		disp32(56,4,S1/100%10); //小数的十位
		disp32(74,4,S1/10%10);   //小数的个位
		
	}
	
	if((flag_10ms==1)&&(!flag_ok)){
		flag_10ms=0;
		table[1]=ta[temp/10];
		table[2]=ta[temp%10];
		table[5]=ta[(uint)V/100];
		table[6]=ta[(uint)V/10%10];
		table[7]=ta[(uint)V%10];
		
		if(S1/1000){
			table[10]=ta[S1/1000];
			table[11]=ta[S1/100%10];
			table[12]=ta[S1/10%10];
		}
		else{
			if((S1/100%10)){
				table[10]=ta[S1/100%10];
				table[11]=ta[S1/10%10];
			}
			else{
				table[10]=ta[S1/10%10];
			}
		}
		if(flag_temp){
			table[1]='-';
		}
		
		clock_value++;
		if(clock_value>=3){
			clock_value=0;
			flag_temp=GetTemp();
			if(flag_temp)
			OLED_ShowChar(3,1,'-',16);
			UartSendBytes("AT+CIPSEND=0,14\r\n");
			delaySWPB(10);
			UartSendBytes(table);
			delaySWPB(10);
		}
		if(S1>Min1 && S1<Max1){
				delaySWPB(5);
				if(S1>Min1 && S1<Max1){
					flag_alarm=1;
				}
		}
		else if(S1>Min2 && S1<Max2) {
				delaySWPB(5);
				if(S1>Min2 && S1<Max2) {
					flag_alarm=2;
				}
		}
		else
				flag_alarm=0;
		
		if(temp/10)
			OLED_ShowChar(12,1,table[1],16);
		OLED_ShowChar(20,1,table[2],16);
		
		OLED_ShowChar(64,1,table[5],16);
		OLED_ShowChar(73,1,table[6],16);
		OLED_ShowChar(82,1,table[7],16);
		
		Alarm_range();	
	}			
}

void inittimer2()
{
	T2MOD = 0;      //---- --00  初始化模式寄存器  默认向上计数
	T2CON = 0;      //0000 0000  初始化控制寄存器 一个中断源，16位自动重装模式
	TL2  =  0x00;     //设置定时初值50ms
	TH2  =  0x4C;     //设置定时初值50ms
	RCAP2L = 0x00;      //设置定时重载值
	RCAP2H = 0x4C;      //设置定时重载值
	TR2 = 1;        //定时器2开始计时
	IE|=0XA0;        //1010 0000 打开全局中断，定时器2中断
}

void int2() interrupt 5
{
	static uchar count;   //定义变量，记录中断次数
	TF2=0;              		//软件清零定时器2溢出标志位
	count++;
	if(count>=3){
			count=0;
			flag_10ms=1;
	}
	if(count%20==0){
			flag_1s=1;
	}
}

void Alarm(u16 freq) //蜂鸣器报警,间断持续t秒
{
	u16 i; 	
	if(flag_1s==1){
		flag_1s=0;
		for(i=0;i<freq;i++) //间断频率 高频
		{
			beep=0;delay_us(50);
			beep=1;delay_us(50);
			led=~led;
		}
	}
}


void Alarm_range()
{
	if(flag_alarm==1){
		flag_alarm=0;
		Alarm(10); //低频报警
		MscAlarm();
	}
	if(flag_alarm==2){
		flag_alarm=0;
		Alarm(100); //低频报警
	}
}


void key_pros()
{	
	if(key==0)
	{
		delaySWPB(10);
		if(key==0)
		{
			mode++;
			while(!key);			//等待按键释放			
			switch(mode)
			{
				case 1:	mode=0;MscDistance(S1);		break;
				default :break;
			}
		}
	}
	
}

void Conut(void)			   //测量计算函数
{
		uchar m=0;
		uint S1tmp=S1,time=TH0*256+TL0;
		if(flag_temp)
			V=(331.4-temp*0.0607);   
		V=(331.4+temp*0.0607);        //由温度计算速度值，单位是m/s
    S=(uint)(time*V/2000);       //算出来是mm
		TH0=0;
    TL0=0;
		
    if(k<=10)
			k++;
		if(k>=11)
			k=1;
		Sav[k]=S;
		
		j++;
		if(j<10)
			S1=S;
		if(j>=10)
		{
				j=10;
				for(m=1;m<=10;m++)
					totle=totle+Sav[m];
				S1=(uint)(totle/10);	      //取10次的平均值
				totle=0;
		}
//		if(abs(S1tmp-S1)>=150)
//			MscDistance(S1);
}

void UartSendBytes(u8 *c)//串口发送数据
{
	EA = 0;
	while(*c != '\0')
	{
		SBUF = *c;
		while(!TI);
		TI = 0;
	   	c++;
	}
	EA = 1;
}

void UartIsr(void) interrupt 4
{
	EA = 0;
	if (RI == 1)   			//当硬件接收到一个数据时，RI会置位
	{
		RI = 0;
		if(flag == 1)		//初始化完8266后，此处才为真
		{
			Buffer[index] = SBUF;//将接收到的字符放入Buffer
			index++;
			if(SBUF == 0x0a)//遇到换行就回到Buffer的头部开始存
			{
				index = 0;
			}
		}
	}
	EA = 1;
}