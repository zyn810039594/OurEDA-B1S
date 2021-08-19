/**
 * @file BasicCtrl.h
 * @brief
 * @author Harold Zhao(zyn810039594) and RedlightASl (dddbbbdd@foxmail.com)
 * @version 1.0
 * @date 2021-08-19
 *
 * @copyright Copyright (c) 2021  RedlightASl
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date       <th>Version <th>Author  <th>Description
 * <tr><td>2021-08-19 <td>1.0     <td>wangh     <td>Content
 * </table>
 */
#ifndef __BASICCTRL_H
#define __BASICCTRL_H
#include "main.h"
#include "Setup.h"

#define BASICCTRL_MAX(x, y) ({\
							typeof(x) _x = (x);\
							typeof(y) _y = (y);\
							(void)(&_x == &_y);\
							_x > _y ? _x : _y;\
							})

#define BASICCTRL_MIN(x, y) ({\
							typeof(x) _x = (x);\
							typeof(y) _y = (y);\
							(void)(&_x == &_y);\
							_x < _y ? _x : _y;\
							})

#define BASICCTRL_RANGE(x, a, b) (BASICCTRL_MIN((BASICCTRL_MAX(x, a)), b))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef volatile uint8_t vu8;
typedef volatile uint16_t vu16;
typedef volatile uint32_t vu32;

/* 下传指令格式 */
struct _DownDataDef
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
};
typedef struct _DownDataDef DownDataDef;

/* 上传数据格式 */
struct _UpDataDef
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
	u16 Confidence;
	u32 DepthToBottom;
	u8 IdTest;
};
typedef struct _UpDataDef UpDataDef;

/* 推进器控制格式 */
struct _MoveThruster
{
	vu32 HorizontalThruster[4];
	vu32 VerticalThruster[2];
};
typedef struct _MoveThruster MoveThruster;

DownDataDef CaptureDownData(void);
void SendDownData(DownDataDef SendData);
UpDataDef CaptureUpData(void);
void SendUpData(UpDataDef SendData);
MoveThruster MoveControl(u16 StraightNum, u16 RotateNum, u16 VerticalNum,
		u8 ModeNum);
u16 SpecialMovePID(u8 ModeType, u16 SetValue, u16 ActualValue);
#endif
