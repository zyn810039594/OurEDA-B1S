#ifndef BASICCTRL_H
#define BASICCTRL_H
#include "main.h"
#include "Setup.h"

#define max(a, b)			(a>b? a:b)
#define min(a, b)			(a<b? a:b)
#define range(x, a, b)		(min(max(x, a), b))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef volatile uint8_t vu8;
typedef volatile uint16_t vu16;
typedef volatile uint32_t vu32;

typedef struct _DownDataDef
{
	u16 StraightNum;
	u16 RotateNum;
	u16 VerticalNum;
	vu32 LightPWM;
	vu32 THPWM;
	vu32 TranspPWM;
	vu32 ArmPWM[6];
	vu32 ResPWM;
	u8 Mode;
	u8 Relay;
	u8 IdTest;
} DownDataDef;

typedef struct _UpDataDef
{
	u8 CabinNum;
	u8 WaterDetect;
	u16 CabinTemperature;
	u32 CabinBaro;
	u16 CabinHum;
	u16 AccNum[3];
	u16 RotNum[3];
	u16 EulNum[3];
	u16 MagNum[3];
	u16 WaterTemperature;
	u16 WaterDepth;
	u8 IdTest;
} UpDataDef;

typedef struct _MoveThruster
{
	vu32 HorizontalThruster[4];
	vu32 VerticalThruster[2];
} MoveThruster;

DownDataDef CaptureDownData(void);
void SendDownData(DownDataDef SendData);
UpDataDef CaptureUpData(void);
void SendUpData(UpDataDef UpData);
MoveThruster MoveControl(u16 StraightNum, u16 RotateNum, u16 VerticalNum,
		u8 ModeNum);
u16 SpecialMovePID(u8 ModeType, u16 SetValue, u16 ActualValue);

#endif
