#include "ms5837.h"

/* 声明全局变量 */
ms5837RxData_t ms5837data;
u8 DataGet[16];
u8 dataget[16];
u8 i=0;
uint8_t j = 0;
uint8_t cnt = 0;
void GetMs5837Data(u8 data)
{
	static u8 sta = 0;
	if((0==sta) && (data == 0x54) )
	{
		//'T'
//		printf("%x",data);
		dataget[0]=data;
		sta = 1;
	}
	else if((1 == sta) &&(data == 0x3D))
	{
		//'='
//		printf("%x",data);
		dataget[1]=data;
		sta = 2;
	}
	else if(2 == sta)
	{
		ms5837data.Ftemp = data;	//温度前半部分
		for(i=0;i<2;i++)
		{
			dataget[2+i]=data;		//dataget[2]和dataget[3]
		}
		sta = 3;
	}
	else if((3 == sta) && (0x2E == data))
	{
		//'.'
//		printf("%x",data);
		dataget[4] = data;
		sta = 4;
	}
	else if(4 == sta)		
	{
		//温度后半部分
		for(i=0;i<2;i++)
		{
			dataget[5+i]=data;		//dataget[5]和dataget[6]
		}
		sta = 5;
	}
	else if((5 == sta) && (0x44 == data))
	{
		//字符‘D’
//		printf("%x",data);
		dataget[7]=data;		
		sta = 6;
	}
	else if((6 == sta) && (0x3D == data))										
	{
		//字符'='
//		printf("%x",data);
		dataget[8]=data;
		sta = 7;
	}
	else if(7 == sta)
	{
		//深度前半部分
		for(i=0;i<2;i++)
		{
			dataget[9+i]=data;		//dataget[9]和dataget[10]
		}
		sta = 8;		
	}
	else if((8 == sta) && (0x2E == data))
	{
//		printf("%x",data);
		dataget[11]=data;
		sta = 9;
	}
	else if(9 == sta)
	{
		//深度计后半部分
		for(i=0;i<2;i++)
		{
			dataget[12+i]=data;		//dataget[12]和dataget[13]
		}
		sta = 10;
	}
	else if(10 == sta)
	{
		//‘\r’
		dataget[14]=data;
		sta = 11;
	}
	else if(11 == sta)
	{
		//'\n'
		dataget[15]=data;
		sta = 0;
	}
}

void  OutputMS5837(void)
{
	float depth;
	depth = ms5837data.Fpre + ms5837data.Bpre*0.01;
	printf("%.2f ",depth);
}

void pritdata(void)
{
	int j=0;
	for(j=0;j<15;j++)
	{
		printf("%c",dataget[j]);
	}
	printf("\r\n");
}

void ProcessMS5837Data(void)
{
	u8 Temperature_Ten;
	u8 Temperature_One;
	float Temperature_One_Dot;	
	float Temperature_Ten_Dot;
	float Temperature;
	
	u8 Depth_Ten;
	u8 Depth_One;
	float Depth_One_Dot;
	float Depth_The_Dot;
	float Depth;
	
	Temperature_Ten = (dataget[2] - '0') * 10;
	Temperature_One = (dataget[3] - '0');
	Temperature_One_Dot = (dataget[5] - '0') * 0.1;
	Temperature_Ten_Dot = (dataget[3] - '0') * 0.01;
	Temperature = Temperature_Ten + Temperature_One + Temperature_One_Dot + Temperature_Ten_Dot;
	

	Depth_Ten = (dataget[9] - '0') * 10;
	Depth_One = (dataget[10] - '0');
	Depth_One_Dot = (dataget[13] - '0') * 0.1;
	Depth_The_Dot = (dataget[14] - '0') * 0.01;
	Depth = Depth_Ten + Depth_One + Depth_One_Dot + Depth_The_Dot;
	
	printf("Temperature：%.2f\r\n", Temperature);
	printf("Depth：%.2f\r\n", Depth);
	printf("***************************\r\n");
}



