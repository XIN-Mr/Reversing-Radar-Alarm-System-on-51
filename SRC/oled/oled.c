#include "oled.h"
#include "oledfont.h"  	 

/**********************************************
//IIC Start
**********************************************/
void IIC_Start()
{
	OLED_SCLK_Set();
	OLED_SDIN_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()
{
	OLED_SCLK_Set();
//	OLED_SCLK_Clr();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
	
}

void IIC_Wait_Ack()
{
	//GPIOB->CRH &= 0XFFF0FFFF;	//����PB12Ϊ��������ģʽ
	//GPIOB->CRH |= 0x00080000;
//	OLED_SDA = 1;
//	delay_us(1);
	//OLED_SCL = 1;
	//delay_us(50000);
/*	while(1)
	{
		if(!OLED_SDA)				//�ж��Ƿ���յ�OLED Ӧ���ź�
		{
			//GPIOB->CRH &= 0XFFF0FFFF;	//����PB12Ϊͨ���������ģʽ
			//GPIOB->CRH |= 0x00030000;
			return;
		}
	}
*/
	OLED_SCLK_Set() ;
	OLED_SCLK_Clr();
}
/**********************************************
// IIC Write byte
**********************************************/

void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m,da;
	da=IIC_Byte;
	OLED_SCLK_Clr();
	for(i=0;i<8;i++)		
	{
		m=da;
		//	OLED_SCLK_Clr();
		m=m&0x80;
		if(m==0x80)
		{OLED_SDIN_Set();}
		else OLED_SDIN_Clr();
			da=da<<1;
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
		}


}
/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
   IIC_Start();
   Write_IIC_Byte(0x78);            //Slave address,SA0=0
	IIC_Wait_Ack();	
   Write_IIC_Byte(0x00);			//write command
	IIC_Wait_Ack();	
   Write_IIC_Byte(IIC_Command); 
	IIC_Wait_Ack();	
   IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
   IIC_Start();
   Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
	IIC_Wait_Ack();	
   Write_IIC_Byte(0x40);			//write data
	IIC_Wait_Ack();	
   Write_IIC_Byte(IIC_Data);
	IIC_Wait_Ack();	
   IIC_Stop();
}


void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
			{

   Write_IIC_Data(dat);
   
		}
	else {
   Write_IIC_Command(dat);
		
	}


}

//��������
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	
	OLED_WR_Byte(0xb0+y, OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10, OLED_CMD);
	OLED_WR_Byte((x&0x0f), OLED_CMD); 
}   
	  
//����OLED��ʾ    
//void OLED_Display_On(void)
//{
//	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
//	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
//	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
//}
////�ر�OLED��ʾ     
//void OLED_Display_Off(void)
//{
//	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
//	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
//	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
//}		   			 
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)
			OLED_WR_Byte(0,OLED_DATA); 
	} //������ʾ
}

//void OLED_On(void)  
//{  
//	u8 i,n;		    
//	for(i=0;i<8;i++)  
//	{  
//		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
//		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
//		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
//		for(n=0;n<128;n++)
//			OLED_WR_Byte(0x00, OLED_DATA); 
//	} //������ʾ
//}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{      	
	unsigned char c = 0, i = 0;
		
	c = chr - ' ';		//�õ�ƫ�ƺ��ֵ			
	if (x > Max_Column-1)
	{
		x = 0;
		y = y + 2;
	}
	if (Char_Size == 16)
	{
		OLED_Set_Pos(x, y);	
		for(i=0; i<8; i++)
			OLED_WR_Byte(F8X16[c*16+i], OLED_DATA);
		OLED_Set_Pos(x, y + 1);
		for(i=0; i<8; i++)
			OLED_WR_Byte(F8X16[c*16+i+8], OLED_DATA);
	}
	else 
	{	
		OLED_Set_Pos(x, y);
		for(i=0; i<6; i++)
			OLED_WR_Byte(F6x8[c][i], OLED_DATA);	
	}
}
				  

////��ʾһ���ַ��Ŵ�
//void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size)
//{
//	unsigned char j=0;
//	while (chr[j]!='\0')
//	{		OLED_ShowChar(x,y,chr[j],Char_Size);
//			x+=8;
//		if(x>120){x=0;y+=2;}
//			j++;
//	}
//}

//��ʾ����
void OLED_ShowCHinese(u8 x, u8 y, u8 no)
{      			    
	u8 t, adder = 0;

	OLED_Set_Pos(x, y);	
  for(t=0;t<16;t++)
	{
		OLED_WR_Byte(Hzk[no][t], OLED_DATA);
		adder += 1;
	}	

	OLED_Set_Pos(x, y+1);	
  for(t=0; t<16; t++)
	{	
		OLED_WR_Byte(Hzk[no][t+16], OLED_DATA);
		adder += 1;
	}					
}

//��ʼ��SSD1306					    
void OLED_Init(void)
{ 	
	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
	OLED_WR_Byte(0x81,OLED_CMD); // contract control
	OLED_WR_Byte(0xFF,OLED_CMD);//--128   
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//
	
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD);//
	
	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD);//
	
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);//
	
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);//
	
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//
	
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD);//
	
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
}  


////��ʾ48*48�ĺ���
//void OLED_ShowCHinese48(u8 x,u8 y,u8 no)
//{                                  
//  u8 t, i;
//  for (i = 0; i < 6; i++)   //6��ʲô��˼��48���ַ��߶�Ϊ48���㣬OLED����Ϊ8ҳ������8�У���ÿҳ8���㣬����48/8=6
//  {
//    OLED_Set_Pos(x, y + i);
//    for(t = 0; t < 48; t++)  //48��ʲô���ַ��Ŀ��Ϊ48���㡣
//    {
//      OLED_WR_Byte(Hzk48[no][48 * i + t],OLED_DATA);
//    }
//  }
//}

void disp32(u8 x,u8 y,u8 numsdai)
{
   u8 r;
   switch(numsdai)
   {
     case 0:   OLED_Set_Pos(x,y);for(r=0;r<15;r++){OLED_WR_Byte(Sdai0[r],1);}OLED_Set_Pos(x,y+1);for(r=16;r<31;r++){OLED_WR_Byte(Sdai0[r],1);}
               OLED_Set_Pos(x,y+2);for(r=32;r<47;r++){OLED_WR_Byte(Sdai0[r],1);}OLED_Set_Pos(x,y+3);for(r=48;r<63;r++){OLED_WR_Byte(Sdai0[r],1);}
							 break;
   	 case 1:   OLED_Set_Pos(x,y);for(r=0;r<15;r++){OLED_WR_Byte(Sdai1[r],1);}OLED_Set_Pos(x,y+1);for(r=16;r<31;r++){OLED_WR_Byte(Sdai1[r],1);}
               OLED_Set_Pos(x,y+2);for(r=32;r<47;r++){OLED_WR_Byte(Sdai1[r],1);}OLED_Set_Pos(x,y+3);for(r=48;r<63;r++){OLED_WR_Byte(Sdai1[r],1);}
							 break;
	   case 2:   OLED_Set_Pos(x,y);for(r=0;r<15;r++){OLED_WR_Byte(Sdai2[r],1);}OLED_Set_Pos(x,y+1);for(r=16;r<31;r++){OLED_WR_Byte(Sdai2[r],1);}
               OLED_Set_Pos(x,y+2);for(r=32;r<47;r++){OLED_WR_Byte(Sdai2[r],1);}OLED_Set_Pos(x,y+3);for(r=48;r<63;r++){OLED_WR_Byte(Sdai2[r],1);}
							 break;
	   case 3:   OLED_Set_Pos(x,y);for(r=0;r<15;r++){OLED_WR_Byte(Sdai3[r],1);}OLED_Set_Pos(x,y+1);for(r=16;r<31;r++){OLED_WR_Byte(Sdai3[r],1);}
               OLED_Set_Pos(x,y+2);for(r=32;r<47;r++){OLED_WR_Byte(Sdai3[r],1);}OLED_Set_Pos(x,y+3);for(r=48;r<63;r++){OLED_WR_Byte(Sdai3[r],1);}
							 break;
	   case 4:   OLED_Set_Pos(x,y);for(r=0;r<15;r++){OLED_WR_Byte(Sdai4[r],1);}OLED_Set_Pos(x,y+1);for(r=16;r<31;r++){OLED_WR_Byte(Sdai4[r],1);}
               OLED_Set_Pos(x,y+2);for(r=32;r<47;r++){OLED_WR_Byte(Sdai4[r],1);}OLED_Set_Pos(x,y+3);for(r=48;r<63;r++){OLED_WR_Byte(Sdai4[r],1);}
							 break;
	   case 5:   OLED_Set_Pos(x,y);for(r=0;r<15;r++){OLED_WR_Byte(Sdai5[r],1);}OLED_Set_Pos(x,y+1);for(r=16;r<31;r++){OLED_WR_Byte(Sdai5[r],1);}
               OLED_Set_Pos(x,y+2);for(r=32;r<47;r++){OLED_WR_Byte(Sdai5[r],1);}OLED_Set_Pos(x,y+3);for(r=48;r<63;r++){OLED_WR_Byte(Sdai5[r],1);}
							 break;
	   case 6:   OLED_Set_Pos(x,y);for(r=0;r<15;r++){OLED_WR_Byte(Sdai6[r],1);}OLED_Set_Pos(x,y+1);for(r=16;r<31;r++){OLED_WR_Byte(Sdai6[r],1);}
               OLED_Set_Pos(x,y+2);for(r=32;r<47;r++){OLED_WR_Byte(Sdai6[r],1);}OLED_Set_Pos(x,y+3);for(r=48;r<63;r++){OLED_WR_Byte(Sdai6[r],1);}
							 break;
	   case 7:   OLED_Set_Pos(x,y);for(r=0;r<15;r++){OLED_WR_Byte(Sdai7[r],1);}OLED_Set_Pos(x,y+1);for(r=16;r<31;r++){OLED_WR_Byte(Sdai7[r],1);}
               OLED_Set_Pos(x,y+2);for(r=32;r<47;r++){OLED_WR_Byte(Sdai7[r],1);}OLED_Set_Pos(x,y+3);for(r=48;r<63;r++){OLED_WR_Byte(Sdai7[r],1);}
							 break;
     case 8:   OLED_Set_Pos(x,y);for(r=0;r<15;r++){OLED_WR_Byte(Sdai8[r],1);}OLED_Set_Pos(x,y+1);for(r=16;r<31;r++){OLED_WR_Byte(Sdai8[r],1);}
               OLED_Set_Pos(x,y+2);for(r=32;r<47;r++){OLED_WR_Byte(Sdai8[r],1);}OLED_Set_Pos(x,y+3);for(r=48;r<63;r++){OLED_WR_Byte(Sdai8[r],1);}
							 break;
	   case 9:   OLED_Set_Pos(x,y);for(r=0;r<15;r++){OLED_WR_Byte(Sdai9[r],1);}OLED_Set_Pos(x,y+1);for(r=16;r<31;r++){OLED_WR_Byte(Sdai9[r],1);}
               OLED_Set_Pos(x,y+2);for(r=32;r<47;r++){OLED_WR_Byte(Sdai9[r],1);}OLED_Set_Pos(x,y+3);for(r=48;r<63;r++){OLED_WR_Byte(Sdai9[r],1);}
							 break;
//		 case 10:   OLED_Set_Pos(x,y);for(r=0;r<15;r++){OLED_WR_Byte(Sdai10[r],1);}OLED_Set_Pos(x,y+1);for(r=16;r<31;r++){OLED_WR_Byte(Sdai10[r],1);}
//               OLED_Set_Pos(x,y+2);for(r=32;r<47;r++){OLED_WR_Byte(Sdai10[r],1);}OLED_Set_Pos(x,y+3);for(r=48;r<63;r++){OLED_WR_Byte(Sdai10[r],1);}
//							 break;
		 case 11:   OLED_Set_Pos(x,y);for(r=0;r<15;r++){OLED_WR_Byte(Sdai11[r],1);}OLED_Set_Pos(x,y+1);for(r=16;r<31;r++){OLED_WR_Byte(Sdai11[r],1);}
               OLED_Set_Pos(x,y+2);for(r=32;r<47;r++){OLED_WR_Byte(Sdai11[r],1);}OLED_Set_Pos(x,y+3);for(r=48;r<63;r++){OLED_WR_Byte(Sdai11[r],1);}
							 break;
	   default:  break;
   }     
}
