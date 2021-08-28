#ifndef __SENSOR_H
#define __SENSOR_H
#include <stdint.h>
#include "Setup.h"

typedef volatile uint8_t vu8;
typedef volatile uint16_t vu16;
typedef volatile uint32_t vu32;

struct _MySensorData
{
    char* SensorData_name; //数据位对应的名字，相同功能的数据位可以指向同一个名字
    uint8_t SensorData_content; //所收取数据位的内容
};
typedef struct _MySensorData* MySensorData;

struct _ROV_Sensor
{
    uint16_t Sensor_BufferLen; //传感器接收缓存区大小
    __attribute__((section(".RAM_D1"))) u8 Sensor_Buffer; //传感器接收缓存区指针
    void ROV_Sensor_Init(void); //传感器初始化回调函数
    MySensorData ROV_Sensor_Receive(void); //传感器数据接收回调函数
};
typedef struct _ROV_Sensor ROV_Sensor;




#endif