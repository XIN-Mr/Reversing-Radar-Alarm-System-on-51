uint temp=25;          //温度值
sbit DQ=P3^7;          //DS18B20单总线接口

void Delayus(uchar xus) //晶振为12MHz，延时时间为2i+5 us
{
    while(--xus);
}

bit Init_DS18B20(void)
{
	bit x;
	DQ=1;
	DQ=0;
	Delayus(250);
	DQ=1;
	Delayus(20);
	if(!DQ) x=0;
	else x=1;
	Delayus(250);
	DQ=1;
	return x;
}

//读DS18B20函数
uchar Read_DS18B20(void)
{
	uchar i=0,Dat=0;
	for(i=0;i<8;i++)
	{
		DQ=1;
		DQ=0;
		Dat>>=1;
		DQ=1;
		if(DQ) Dat |= 0x80;
		DQ=1;
		Delayus(30);
	}
	return Dat;
}


//写DS18B20函数
void Write_DS18B20(uchar Dat)
{
	uchar i=0;
	for(i=0;i<8;i++)//循环8次，写入一个字节
	{
		DQ=1;//未发送前的状态
		Dat >>= 1;//将要传送的最低位放入CY
		DQ=0;//将总线拉低，产生写时序
		DQ=CY;//将要传送的位状态送到总线上
		Delayus(30);//延时50us，即保持总线状态，待DS18B20采样
		DQ=1;//恢复期，总线置1
	}
}

bit GetTemp(void)       //获取温度函数
{
	bit Temp_Flag=0;         //正负温度标志：温度为正Temp_Flag=0，否则为1
	uchar a=0,b=0;
	Init_DS18B20();
	Write_DS18B20(0xcc); //跳过ROM
	Write_DS18B20(0x44); //开启温度转换
	Init_DS18B20();
	Write_DS18B20(0xcc); //跳过ROM
	Write_DS18B20(0xbe); //读暂存器
	a=Read_DS18B20();    //读取高速暂存字节0，温度低8位
	b=Read_DS18B20();    //读取高速暂存字节1，温度高8位
	temp=b;
	temp<<=8;
	temp=temp|a;         //将高、低位温度编码合在一起
	if(b>=8)             //判断温度值是否为负，如果温度高字节大于等于8说明温度值为负
	{
	    temp=~temp+1;      //将补码转换成原码
	    Temp_Flag=1;       //温度标志为1，表示温度为负
	}
	else
	{
	    Temp_Flag=0;       //温度标志为0，表示温度为正
	}
	temp=temp*0.0625+0.5;//得到整数（无小数）将温度编码转换成温度值,加0.5是为了四舍五入
	return Temp_Flag;
}

