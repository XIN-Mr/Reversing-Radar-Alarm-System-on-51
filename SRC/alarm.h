#include "common.h"

//MM单位   ① 50~150CM；  ② 150~200CM；
#define Min1 120  //  500<=x && x<=1500;  即50cm--150cm
#define Max1 150

#define Min2 150	//  1500<=x && x<=2000;  即150cm--200cm
#define Max2 200

sbit beep=P3^4;
sbit led=P2^2;

//语音播报程序
sbit MUSIC_REST =P2^3;//语音芯片复位脚					
sbit MUSIC_DATA =P2^4;//语音芯片脉冲识别
sbit bus =P2^5;//语音芯片工作状态识别信号  

void MscAlarm();
	
void delaySWPB(uint x) //毫秒级延时函数
{
  uint i,j;
  for(i=x;i>0;i--)
     for(j=110;j>0;j--);
}

void delay_us(unsigned int us)
{
    while(us--)
    {
    	_nop_();
    	_nop_();
    	_nop_();
    	_nop_();
    }
} 

//void Alarm(u8 freq, u8 t) //蜂鸣器报警,间断持续t秒
//{
//	u8 i,j; 	
//	for(j=0;j<t;j++)
//	{ 
//		for(i=0;i<freq;i++) //间断频率 高频
//		{
//			beep=0;delay_us(50);
//			beep=1;delay_us(50);
//			led=~led;
//		}
//	}
//}
	
uchar mode=0,flag_alarm=0;

//void Alarm_range(u16 distance)
//{
//	if(distance>Min1 && distance<Max1){
//			flag_alarm=1;
//	}
//	else if(distance>Min2 && distance<Max2) 
//	{
//			flag_alarm=2;
//	}
//}

void Music(unsigned char music_count)
{
    MUSIC_REST=1;
    delay_us(200);
    MUSIC_REST=0;
    delay_us(200);

    while(music_count>0)
    {
        MUSIC_DATA=1;
        delay_us(100);
        MUSIC_DATA=0;
        delay_us(100);
				music_count--;
    }
} 

void MscAlarm()
{
	Music(23);					//播报：“危险”
	while(!bus);
}

void MscDistance(uint distance)
{
	uchar xm0,xm1,xm2;
	xm0=distance/1000;
	xm1=distance/100%10;
	xm2=distance/10%10;
  if((xm0+xm1+xm2)!=0)		   //当超出测量范围时不进行播报
	{
		Music(17);					//播报：“距离”
		while(!bus);
		if(xm0==0)
		{
   	  Music(2);			 //播报：“十位数具体值”	  0
			while(!bus);	
		}	            
   		else Music(xm0+2);			 //播报：“十位数具体值”
		while(!bus);
		if((xm1+xm2)!=0)
		{	  
			Music(14);					//播报：“点”
			while(!bus);
			Music(xm1+2);				//播报：“十分位的具体值” 
			while(!bus);	 
			if(xm2){
				Music(xm2+2);				//播报：“百分位的具体值”
				while(!bus);	 
			}
			Music(32);				//播报：“米”
			while(!bus);
		}
		
		else
		{
			Music(32);				//播报：“米”
			while(!bus);	
		}
	}	  
}

//void MscTemp(uint t)
//{
//	uchar t0,t1;
//	t0=t/10;
//	t1=t%10;
//	if((t0+t1)!=0)
//	{
//		Music(21);					//播报：“温度”
//		while(!bus);
//		if(t0==0)
//		{
//			Music(t1+2);				//播报：“整数十位数的具体值” 
//			while(!bus);	 
//		}	            
//		else
//		{
//			switch(t0)
//			{
//				case 1: Music(12);					//播报：“十”
//								while(!bus);
//								break;
//				default:Music(t0+2);			 //播报：“整数个位数具体值”
//								while(!bus);
//								Music(12);					//播报：“十”
//								while(!bus);
//								break;					
//			}
//			
//			switch(t1)
//			{
//				case 0: break;
//				default:Music(t1+2);				//播报：“整数十位数的具体值” 
//								while(!bus);
//								break;					
//			}
//		}
//		Music(29);				//播报：“度”
//		while(!bus);
//	}  
//}


////	static uchar bvalue;
//	if(flag_alarm==1){
//		flag_alarm = 0;
//		if(flag_100ms == 1)		  //100ms执行一次
//		{
//			flag_100ms = 0;	 
//			clock_value++;
//			if(clock_value <= 30)	 
//				beep = ~beep;	   //蜂鸣器叫3秒
//			else if(clock_value >= 30)
//			{
//				beep = 1;		    //蜂鸣器停2秒
//				if(clock_value > 20)
//					clock_value = 0;
//			}
//			beep=1;
////		1s 后自动关闭闹钟
////			bvalue ++;
////			if(bvalue >= 10)
////			{
////				bvalue = 0;
////				beep = 1;	
////				flag_alarm = 0;
////			}		
//			
//		}
//	}
//	else if(flag_alarm==2){
//		flag_alarm = 0;
//		if(flag_100ms == 1)		  //100ms执行一次
//		{
//			flag_100ms = 0;	 
//			clock_value++;
//			if(clock_value <= 10)	 
//				beep = ~beep;	   //蜂鸣器叫3秒
//			else if(clock_value >= 10)
//			{
//				beep = 1;		    //蜂鸣器停2秒
//				if(clock_value > 10)
//					clock_value = 0;
//			}
//			beep = 1;	
////		1s 后自动关闭闹钟
////			bvalue ++;
////			if(bvalue >= 1)
////			{
////				bvalue = 0;
////				beep = 1;	
////				flag_alarm = 0;
////			}		
//		}
//	}