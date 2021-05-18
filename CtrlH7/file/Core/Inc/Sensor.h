#ifndef _SENSOR_H_
#define _SENSOR_H_

extern const u8 TInitStr[3];
extern const u8 TSendStr[3];

void DeepAnalyze(u8* DataPosition,u16* PDepth,u16*PWaterTemperture,u8* PDN,u8* PWTN);

void WT931Point(u8* SerialData);

void WT931Take(u16* AttitudeNum,u16* AccelerationNum,u16* RotSpeedNum,u16* EulerAngleNum,u16* MagnetismNum);

void GY39Point(u8* SerialData);

void GY39Take(u16* TemNum,u16* BaroNum,u16* HumNum);

#endif
