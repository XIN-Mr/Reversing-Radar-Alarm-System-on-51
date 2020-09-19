uint temp=25;          //�¶�ֵ
sbit DQ=P3^7;          //DS18B20�����߽ӿ�

void Delayus(uchar xus) //����Ϊ12MHz����ʱʱ��Ϊ2i+5 us
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

//��DS18B20����
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


//дDS18B20����
void Write_DS18B20(uchar Dat)
{
	uchar i=0;
	for(i=0;i<8;i++)//ѭ��8�Σ�д��һ���ֽ�
	{
		DQ=1;//δ����ǰ��״̬
		Dat >>= 1;//��Ҫ���͵����λ����CY
		DQ=0;//���������ͣ�����дʱ��
		DQ=CY;//��Ҫ���͵�λ״̬�͵�������
		Delayus(30);//��ʱ50us������������״̬����DS18B20����
		DQ=1;//�ָ��ڣ�������1
	}
}

bit GetTemp(void)       //��ȡ�¶Ⱥ���
{
	bit Temp_Flag=0;         //�����¶ȱ�־���¶�Ϊ��Temp_Flag=0������Ϊ1
	uchar a=0,b=0;
	Init_DS18B20();
	Write_DS18B20(0xcc); //����ROM
	Write_DS18B20(0x44); //�����¶�ת��
	Init_DS18B20();
	Write_DS18B20(0xcc); //����ROM
	Write_DS18B20(0xbe); //���ݴ���
	a=Read_DS18B20();    //��ȡ�����ݴ��ֽ�0���¶ȵ�8λ
	b=Read_DS18B20();    //��ȡ�����ݴ��ֽ�1���¶ȸ�8λ
	temp=b;
	temp<<=8;
	temp=temp|a;         //���ߡ���λ�¶ȱ������һ��
	if(b>=8)             //�ж��¶�ֵ�Ƿ�Ϊ��������¶ȸ��ֽڴ��ڵ���8˵���¶�ֵΪ��
	{
	    temp=~temp+1;      //������ת����ԭ��
	    Temp_Flag=1;       //�¶ȱ�־Ϊ1����ʾ�¶�Ϊ��
	}
	else
	{
	    Temp_Flag=0;       //�¶ȱ�־Ϊ0����ʾ�¶�Ϊ��
	}
	temp=temp*0.0625+0.5;//�õ���������С�������¶ȱ���ת�����¶�ֵ,��0.5��Ϊ����������
	return Temp_Flag;
}

