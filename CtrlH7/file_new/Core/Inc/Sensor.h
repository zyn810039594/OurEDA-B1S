#ifndef SENSOR_H
#define SENSOR_H
#include "main.h"
#include "Setup.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef volatile uint8_t vu8;
typedef volatile uint16_t vu16;
typedef volatile uint32_t vu32;

typedef struct _DeepData
{
	u16 WaterTemperature;
	u16 WaterDepth;
}DeepData;

typedef struct _WT931Data
{
	u16 AccNum[3];
	u16 RotNum[3];
	u16 EulNum[3];
	u16 MagNum[3];
}WT931Data;

typedef struct _GY39Data
{
	u16 Temperature;
	u32 Baro;
	u16	Hum;
}GY39Data;

void InitGY39(void);
GY39Data ReceiveGY39(void);
WT931Data ReceiveWT931(void);
DeepData ReceiveDeep(void);

#endif
