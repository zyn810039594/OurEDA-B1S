#include "main.h"

typedef unsigned char u8;
typedef unsigned short u16;

//传感器预置指针
//WT931指针
static u16* PAccPos[3];
static u16* PRotPos[3];
static u16* PEulPos[3];
static u16* PMagPos[3];
static u16* PInsTemPos[4];
//GY39指针
static u16* TTemPos;
static u16* TBaroPos[2];
static u16* THumPos;

//GY39初始化字符串
const u8 TInitStr[3]={0xA5,0x80,0x25};
//GY39请求温湿度气压字符串
const u8 TSendStr[3]={0xA5,0x52,0xF7};

//水深数据分析
void DeepAnalyze(u8* DataPosition,u16* PDepth,u16*PWaterTemperture,u8* PDN,u8* PWTN)
{
	u8 i = 0;
	PDN[3] = 10;
	PWTN[2] = 10;
	if (DataPosition[4] == '.')
	{
		PWTN[0] = (DataPosition[2] - '0');
		PWTN[1] = (DataPosition[3] - '0');
		PWTN[3] = (DataPosition[5] - '0');
		PWTN[4] = (DataPosition[6] - '0');
		*PWaterTemperture = PWTN[0] * 1000 + PWTN[1] * 100 + PWTN[3] * 10 + PWTN[4];
		if (DataPosition[9] == '-')
		{
			*PDepth = 0;
			PDN[0] = 0;
			PDN[1] = 0;
			PDN[2] = 0;
			PDN[4] = 0;
			PDN[5] = 0;
		}
		else
		{
			if (DataPosition[10] == '.')
			{
				PDN[0] = 0;
				PDN[1] = 0;
				PDN[2] = DataPosition[9] - '0';
				PDN[4] = DataPosition[11] - '0';
				PDN[5] = DataPosition[12] - '0';
				*PDepth = PDN[2] * 100 + PDN[4] * 10 + PDN[5];
			}
			else if (DataPosition[11] == '.')
			{
				PDN[0] = 0;
				PDN[1] = DataPosition[9] - '0';
				PDN[2] = DataPosition[10] - '0';
				PDN[4] = DataPosition[12] - '0';
				PDN[5] = DataPosition[13] - '0';
				*PDepth = PDN[1] * 1000 + PDN[2] * 100 + PDN[4] * 10 + PDN[5];
			}
			else if (DataPosition[12] == '.')
			{
				PDN[0] = DataPosition[9] - '0';
				PDN[1] = DataPosition[10] - '0';
				PDN[2] = DataPosition[12] - '0';
				PDN[4] = DataPosition[13] - '0';
				PDN[5] = DataPosition[14] - '0';
				*PDepth = PDN[0] * 10000 + PDN[1] * 1000 + PDN[2] * 100 + PDN[4] * 10 + PDN[5];
			}
		}
	}
	else if (DataPosition[3] == '.')
	{
		PWTN[0] = 0;
		PWTN[1] = (DataPosition[2] - '0');
		PWTN[3] = (DataPosition[4] - '0');
		PWTN[4] = (DataPosition[5] - '0');
		*PWaterTemperture = PWTN[1] * 100 + PWTN[3] * 10 + PWTN[4];
		if (DataPosition[8] == '-')
		{
			*PDepth = 0;
			PDN[0] = 0;
			PDN[1] = 0;
			PDN[2] = 0;
			PDN[4] = 0;
			PDN[5] = 0;
		}
		else
		{
			if (DataPosition[9] == '.')
			{
				PDN[0] = 0;
				PDN[1] = 0;
				PDN[2] = DataPosition[8] - '0';
				PDN[4] = DataPosition[10] - '0';
				PDN[5] = DataPosition[11] - '0';
				*PDepth = PDN[2] * 100 + PDN[4] * 10 + PDN[5];

			}
			else if (DataPosition[10] == '.')
			{
				PDN[0] = 0;
				PDN[1] = DataPosition[8] - '0';
				PDN[2] = DataPosition[9] - '0';
				PDN[4] = DataPosition[11] - '0';
				PDN[5] = DataPosition[12] - '0';
				*PDepth = PDN[1] * 1000 + PDN[2] * 100 + PDN[4] * 10 + PDN[5];

			}
			else if (DataPosition[11] == '.')
			{
				PDN[0] = DataPosition[8] - '0';
				PDN[1] = DataPosition[9] - '0';
				PDN[2] = DataPosition[10] - '0';
				PDN[4] = DataPosition[12] - '0';
				PDN[5] = DataPosition[13] - '0';
				*PDepth = PDN[0] * 10000 + PDN[1] * 1000 + PDN[2] * 100 + PDN[4] * 10 + PDN[5];
			}
		}
	}
}

//WT931数据指针定向
void WT931Point(u8* SerialData)
{
	PAccPos[0] = (u16*)(SerialData + 2);
	PAccPos[1] = (u16*)(SerialData + 4);
	PAccPos[2] = (u16*)(SerialData + 6);
	PInsTemPos[0] = (u16*)(SerialData + 8);
	PRotPos[0] = (u16*)(SerialData + 13);
	PRotPos[1] = (u16*)(SerialData + 15);
	PRotPos[2] = (u16*)(SerialData + 17);
	PInsTemPos[1] = (u16*)(SerialData + 19);
	PEulPos[0] = (u16*)(SerialData + 24);
	PEulPos[1] = (u16*)(SerialData + 26);
	PEulPos[2] = (u16*)(SerialData + 28);
	PInsTemPos[2] = (u16*)(SerialData + 30);
	PMagPos[0] = (u16*)(SerialData + 35);
	PMagPos[1] = (u16*)(SerialData + 37);
	PMagPos[2] = (u16*)(SerialData + 39);
	PInsTemPos[2] = (u16*)(SerialData + 41);
}

//WT931数据输出
void WT931Take(u16* AttitudeNum,u16* AccelerationNum,u16* RotSpeedNum,u16* EulerAngleNum,u16* MagnetismNum)
{
	
	AccelerationNum[0] = *PAccPos[0];
	AccelerationNum[1] = *PAccPos[1];
	AccelerationNum[2] = *PAccPos[2];
	RotSpeedNum[0] = *PRotPos[0];
	RotSpeedNum[1] = *PRotPos[1];
	RotSpeedNum[2] = *PRotPos[2];
	EulerAngleNum[0] = *PEulPos[0];
	EulerAngleNum[1] = *PEulPos[1];
	EulerAngleNum[2] = *PEulPos[2];
	MagnetismNum[0] = *PMagPos[0];
	MagnetismNum[1] = *PMagPos[1];
	MagnetismNum[2] = *PMagPos[2];
	AttitudeNum[0] = (u16)(((float)EulerAngleNum[0] / 32768) * 1800);
	AttitudeNum[1] = (u16)(((float)EulerAngleNum[1] / 32768) * 1800);
	AttitudeNum[2] = (u16)(((float)EulerAngleNum[2] / 32768) * 1800);
}

//GY39数据指针定向
void GY39Point(u8* SerialData)
{
	TTemPos=(u16*)(SerialData+2);
	TBaroPos[0]=(u16*)(SerialData+4);
	TBaroPos[1]=(u16*)(SerialData+6);
	THumPos=(u16*)(SerialData+8);
}

//GY39数据输出
void GY39Take(u16* TemNum,u16* BaroNum,u16* HumNum)
{
	*TemNum=*TTemPos;
	*BaroNum[0]=*TBaroPos[0];
	*BaroNum[1]=*TBaroPos[1];
	*HumNum=*THumPos;
}
