#ifndef __BASICCTRL_H_
#define __BASICCTRL_H_

#include "main.h"

#ifdef CtrlSide
#define CabinNum 0
#define WaterNum 2
#endif

#ifdef PowerSide
#define CabinNum 1
#define WaterNum 4
#endif

#define MAX(a, b)			(a>b? a:b)
#define MIN(a, b)			(a<b? a:b)
#define RANGE(x, a, b)		(MIN(MAX(x, a), b))

/* 定义常用的数据类型 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef long unsigned int u32;

void DownDetectPoint(u8 *RXString);
void DownDetectReceive(u16 *StraightNum, u16 *RotateNum, u16 *VerticalNum,
		u32 *LightNum, u32 *THNum, u32 *TranspNum, u32 *ArmNum[6], u32 *ResPWM,
		u8 *ModeNum, u8 *RelaySW);
void MoveControl(u16 StraightNum, u16 RotateNum, u16 VerticalNum, u8 ModeNum,
		u32 *MoveThruster[6]);
u16 SpecialMovePID(u8 ModeType, u16 SetValue, u16 ActualValue);
u8 DownMoveDetect(void);
void SendBackPoint(u8 *TXString, u8 *DeepSIO, u8 *WT931SIO, u8 *GY39SIO);
void SendBack(u8 WaterDetect, u16 TempNum, u16 BaroNum[2], u16 HumNum,
		u16 AccNum[3], u16 RotNum[3], u16 EulNum[3], u16 MagNum[3],
		u16 WaterTNum, u16 WaterDNum);
u8 IdTest(u8 *String, u8 Format);
u8 XorCaculate(u8 *CacString, u8 CacStringSize);

#endif
