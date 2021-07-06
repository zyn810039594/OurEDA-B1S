#ifndef __SENSOR_H_
#define __SENSOR_H_

#include "main.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef long unsigned int u32;

void DeepPoint(u8 *SerialData);
//void DeepRequestStr(u8* TXCache);
void DeepTake(u16 *DepthNum, u16 *WaterTempertureNum);
void WT931Point(u8 *SerialData);
void WT931Take(u16 *AccelerationNum, u16 *RotSpeedNum, u16 *EulerAngleNum,
		u16 *MagnetismNum);
void GY39InitStr(u8 TXCache[3]);
//void GY39RequestStr(u8 TXCache[3]);
void GY39Point(u8 *SerialData);
void GY39Take(u16 *TemNum, u16 BaroNum[2], u16 *HumNum);

#endif
