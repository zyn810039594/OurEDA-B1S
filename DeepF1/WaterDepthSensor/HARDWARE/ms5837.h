#ifndef __MS5837_H
#define __MS5837_H
#include "stm32f4xx.h" 
#include "usart.h"

typedef struct 
{
	int8_t T;			//字符T
	int8_t Fe;			//字符= F表示First e表示equal
	int16_t Ftemp;		//浮点数据温度值F表示小数点前半部
	int8_t fdot;		//小数点 f表示first，dot表示'.'
	int16_t Btemp;		//浮点数据温度值B表示小数点后半部
	int8_t D;			//字符D
	int8_t Se;			//字符= S表示Second e表示equal
	int16_t Fpre;		//浮点型数据深度值F表示小数点前半部
	int8_t sdot;		//s表示second 
	int16_t Bpre;		//浮点型数据深度值B表示小数点后半部
	int8_t r;			//\r
	int8_t n;			//\n
}ms5837RxData_t;
 





void GetMs5837Data(uint8_t data);
void  OutputMS5837(void);
void ProcessMS5837Data(void);
void pritdata(void);

#endif
