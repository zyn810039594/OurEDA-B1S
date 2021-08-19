/**
 * @file Sensor.h
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
#ifndef __SENSOR_H
#define __SENSOR_H
#include "main.h"
#include "Setup.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef volatile uint8_t vu8;
typedef volatile uint16_t vu16;
typedef volatile uint32_t vu32;

struct _DeepData
{
	u16 WaterTemperature;
	u16 WaterDepth;
};
typedef struct _DeepData DeepData;

struct _WT931Data
{
	u16 AccNum[3];
	u16 RotNum[3];
	u16 EulNum[3];
	u16 MagNum[3];
};
typedef struct _WT931Data WT931Data;

struct _GY39Data
{
	u16 Temperature;
	u32 Baro;
	u16 Hum;
};
typedef struct _GY39Data GY39Data;

typedef struct _P30Data
{
	u32 DepthToBottom;
	u16 Confidence;
};
typedef struct _P30Data P30Data;

void InitGY39(void);
GY39Data ReceiveGY39(void);
WT931Data ReceiveWT931(void);
DeepData ReceiveDeep(void);
void InitP30(void);
P30Data ReceiveP30(void);
#endif
