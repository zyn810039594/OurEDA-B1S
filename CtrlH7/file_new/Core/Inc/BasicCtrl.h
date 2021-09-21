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
	u8 HeadOfData; //指令最前面的数据 0x25
	u16 StraightNum; //前进
	u16 RotateNum; //转向
	u16 VerticalNum; //垂直升降
	vu32 LightPWM; //灯光
	vu32 THPWM; //云台
	vu32 TranspPWM; //传送带
	vu32 ArmPWM[6]; //机械臂
	vu32 ResPWM; //保留位PWM
	u8 Mode; //模式控制
	u8 Relay; //继电器
	u8 IdTest; //奇偶校验
	u8 EndOfData; //指令最后的数据 0x21
};
typedef struct _DownDataDef DownDataDef;

/* 上传数据格式 */
struct _UpDataDef
{
	u8 CabinNum; //仓位
	u8 WaterDetect; //漏水检测
	u16 CabinTemperature; //舱内温度
	u32 CabinBaro; //舱内气压
	u16 CabinHum; //舱内湿度
	u16 AccNum[3]; //加速度
	u16 RotNum[3]; //角速度
	u16 EulNum[3]; //欧拉角
	u16 MagNum[3]; //磁场
	u16 WaterTemperature; //水温
	u16 WaterDepth; //当前所在水深
	u16 Confidence; //声呐数据确信度
	u32 DepthToBottom; //声呐测得当前所在水位高度
	u8 IdTest; //奇偶校验
};
typedef struct _UpDataDef UpDataDef;

/* 推进器控制格式 */
struct _MoveThruster
{
	vu32 HorizontalThruster[4]; //四个水平方向推进器
	vu32 VerticalThruster[2]; //两个垂直方向推进器
};
typedef struct _MoveThruster MoveThruster;

UpDataDef CaptureUpData(void);
DownDataDef CaptureDownData(void);
void SendUpData(UpDataDef SendData);
#ifdef CtrlSide
void LocalSendUpData(UpDataDef SendData)
void SendDownData(DownDataDef SendData);
#endif
#ifdef PowerSide
MoveThruster MoveControl(u16 StraightNum, u16 RotateNum, u16 VerticalNum, u8 ModeNum);
#endif
u16 SpecialMovePID(u8 ModeType, u16 SetValue, u16 ActualValue);
#endif
