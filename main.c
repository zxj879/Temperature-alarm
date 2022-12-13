#include <REGX52.H>
#include "AT24C02.h"
#include "Buzzer.h"
#include "Delay.h"
#include "Ds18B20.h"
#include "Key.h"
#include "LCD1602.h"
#include "Timer0.h"
float T,TShow;
unsigned char KeyNum;
char THigh,TLow;//温度上下限制

void main()
{
	DS18B20_ConvertT();//上电先转换一次温度，防止第一次读数据错误
	Delay(1000);
	THigh=AT24C02_ReadByte(0);//读取温度阈值数据
	TLow=AT24C02_ReadByte(1);
	if(THigh>100 || TLow<-50 || THigh<=TLow)//如果阈值非法，则设为默认值
	{
		THigh=22;
		TLow=16;
	}
	LCD_Init();
	LCD_ShowString(1,1,"T:");
	LCD_ShowString(2,1,"TH:");
	LCD_ShowString(2,9,"TL:");
	LCD_ShowSignedNum(2,4,THigh,3);
	LCD_ShowSignedNum(2,12,TLow,3);
	Timer0_Init();
	while(1)
	{
		KeyNum=Key();
		DS18B20_ConvertT();//转换温度
		T=DS18B20_ReadT();//读取温度
		if(T<0)
		{
			LCD_ShowChar(1,3,'-');
			TShow=-T;
		}
		else
		{
			LCD_ShowChar(1,3,'+');
			TShow=T;
		}
		LCD_ShowNum(1,4,TShow,3);//显示温度整数部分
		LCD_ShowChar(1,7,'.');
		LCD_ShowNum(1,8,(unsigned long)(TShow*100)%100,2);//显示温度小数部分

	//阈值判断
	
	if(KeyNum)
	{
		if(KeyNum==1)
		{
			THigh++;
			if(THigh>100){THigh==100;}
		}
		if(KeyNum==2)
		{
			THigh--;
			if(THigh<=TLow){THigh++;}
		}
		if(KeyNum==3)
		{
			TLow++;
			if(TLow>=THigh){TLow--;}
		}
		if(KeyNum==4)
		{
			TLow--;
			if(TLow<-50){TLow==-50;}
		}
		LCD_ShowSignedNum(2,4,THigh,3);//显示阈值数据
		LCD_ShowSignedNum(2,12,TLow,3);
		AT24C02_WriteByte(0,THigh);//写入到At24C02中保存
		AT24C02_WriteByte(1,TLow);
	}
	//越界判断
	if(T>THigh)
	{
		LCD_ShowString(1,12,"OV:H");
		Buzzer_Time(100);
	}
	else if(T<TLow)
	{
		LCD_ShowString(1,12,"OV:L");
		Buzzer_Time(100);
	}
	else
	{
		LCD_ShowString(1,12,"    ");
	}
	}
}
void Timer0_Routine() interrupt 1
{
	static unsigned int T0Count;
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	T0Count++;
	if(T0Count>=1)
	{
		T0Count=0;//每1ms调用一次按键驱动函数
		Key_Loop();
	}
}