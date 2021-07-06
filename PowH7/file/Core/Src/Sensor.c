#include "Sensor.h"
/* 这里原来用的是main.h，感觉不太对劲改成了Sensor.h */

/*
 温度：Byte2~Byte3
 T=(高 8 位<<8)|低 8 位
 T=T/100 单位℃
 气压：Byte4~Byte7
 P=(前高 8 位<<24) | (前低 8 位<<16) | (后高 8 位<<8) | 后低 8 位
 P=P/100 单位 pa
 湿度：Byte8~Byte9
 Hum=(高 8 位<<8)|低 8 位
 Hum=Hum/100 百分制
 */

//传感器预置指针
//水深指针
static u16 *DDepthPos = 0; 				//水深数据
static u16 *DWaterTemperturePos = 0; 	//水温数据

//WT931指针
static u16 *PAccPos[3] =
{ 0 }; 									//加速度x、y、z
static u16 *PRotPos[3] =
{ 0 }; 									//角速度x、y、z
static u16 *PEulPos[3] =
{ 0 }; 									//角度x、y、z
static u16 *PMagPos[3] =
{ 0 }; 									//磁场x、y、z
static u16 *PInsTemPos[4] =
{ 0 }; 									//

//GY39指针
static u16 *TTemPos = 0; 				//温度数据
static u16 *TBaroPos[2] =
{ 0 }; 									//气压数据
static u16 *THumPos = 0; 				//湿度数据

//水深初始化字符串
const u8 DSendStr[3];					//水深数据

//GY39初始化字符串
const u8 TInitStr[3] =
{ 0xA5, 0x80, 0x25 };
//GY39请求温湿度气压字符串
const u8 TSendStr[3] =
{ 0xA5, 0x52, 0xF7 };

//水深数据指针定向
void DeepPoint(u8 *SerialData)
{
	DDepthPos = (u16*) (SerialData);
	DWaterTemperturePos = (u16*) (SerialData + 2);
}

////水深数据请求
//void DeepRequestStr(u8 *TXCache)
//{
//
//}

//水深数据输出
void DeepTake(u16 *DepthNum, u16 *WaterTempertureNum)
{
	*DepthNum = *DDepthPos;
	*WaterTempertureNum = *DWaterTemperturePos;
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

//WT931数据取出
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

