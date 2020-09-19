#include"chao.h"

bit flag=0;

void delay_20us(void)   //误差 0us
{
    unsigned char a,b;
    for(b=1;b>0;b--)
        for(a=7;a>0;a--);
}
void startb() //启动后测距模块
{
	trigb=0;
	trigb=1;
	delay_20us();
	trigb=0;
}

void inittimer0(void)
{
    TMOD = 0x21;
    SCON = 0x50;
    TH1 = 0xF3;
    TL1 = TH1;
		TH0=0;
		TL0=0;
    PCON |= 0x00;
    EA = 1;
    ES = 1;
    TR1 = 1;	
		TR0=0;
}




void waitb()//	 等待后的回波
{
	//echo=0;
	while(!echob);//此处可能会卡住
	TR0=1;
	while(echob);//此处可能会卡住
	TR0=0;
}

void chaoju() interrupt 1
{
	flag=1;
}





