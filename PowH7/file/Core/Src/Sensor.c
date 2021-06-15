#include "main.h"

//传感器预置指针
//水深指针
static u16 *DDepthPos = 0;
static u16 *DWaterTemperturePos = 0;
//WT931指针
static u16 *PAccPos[3] =
{ 0 };
static u16 *PRotPos[3] =
{ 0 };
static u16 *PEulPos[3] =
{ 0 };
static u16 *PMagPos[3] =
{ 0 };
static u16 *PInsTemPos[4] =
{ 0 };
//GY39指针
static u16 *TTemPos = 0;
static u16 *TBaroPos[2] =
{ 0 };
static u16 *THumPos = 0;

//水深初始化字符串
const u8 DSendStr[3];

//GY39初始化字符串
const u8 TInitStr[3] =
{ 0xA5, 0x80, 0x25 };
//GY39请求温湿度气压字符串
const u8 TSendStr[3] =
{ 0xA5, 0x52, 0xF7 };

//水深数据指针定向
void DeepPoint(u8 *SerialData)
{

}

////水深数据请求
//void DeepRequestStr(u8 *TXCache)
//{
//
//}

//水深数据输出
void DeepTake(u16 *DepthNum, u16 *WaterTempertureNum)
{

}

//WT931数据指针定向
void WT931Point(u8 *SerialData)
{
	PAccPos[0] = (u16*) (SerialData + 2);
	PAccPos[1] = (u16*) (SerialData + 4);
	PAccPos[2] = (u16*) (SerialData + 6);
	PInsTemPos[0] = (u16*) (SerialData + 8);
	PRotPos[0] = (u16*) (SerialData + 13);
	PRotPos[1] = (u16*) (SerialData + 15);
	PRotPos[2] = (u16*) (SerialData + 17);
	PInsTemPos[1] = (u16*) (SerialData + 19);
	PEulPos[0] = (u16*) (SerialData + 24);
	PEulPos[1] = (u16*) (SerialData + 26);
	PEulPos[2] = (u16*) (SerialData + 28);
	PInsTemPos[2] = (u16*) (SerialData + 30);
	PMagPos[0] = (u16*) (SerialData + 35);
	PMagPos[1] = (u16*) (SerialData + 37);
	PMagPos[2] = (u16*) (SerialData + 39);
	PInsTemPos[2] = (u16*) (SerialData + 41);
}

//WT931数据输出
void WT931Take(u16 *AccelerationNum, u16 *RotSpeedNum, u16 *EulerAngleNum,
		u16 *MagnetismNum)
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
}

//读取GY39初始化指令
void GY39InitStr(u8 TXCache[3])
{
	TXCache[0] = TInitStr[0];
	TXCache[1] = TInitStr[1];
	TXCache[2] = TInitStr[2];
}

////读取GY39请求指令
//void GY39RequestStr(u8 TXCache[3])
//{
//	TXCache[0]=TSendStr[0];
//	TXCache[1]=TSendStr[1];
//	TXCache[2]=TSendStr[2];
//}

//GY39数据指针定向
void GY39Point(u8 *SerialData)
{
	TTemPos = (u16*) (SerialData + 2);
	TBaroPos[0] = (u16*) (SerialData + 4);
	TBaroPos[1] = (u16*) (SerialData + 6);
	THumPos = (u16*) (SerialData + 8);
}

//GY39数据输出
void GY39Take(u16 *TemNum, u16 BaroNum[2], u16 *HumNum)
{
	*TemNum = *TTemPos;
	BaroNum[0] = *TBaroPos[0];
	BaroNum[1] = *TBaroPos[1];
	*HumNum = *THumPos;
}

