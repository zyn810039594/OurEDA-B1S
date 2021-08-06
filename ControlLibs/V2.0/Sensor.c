#include "Sensor.h"

__attribute__((section(".RAM_D1")))  u8 WT931Receive[WT931_UART_RXLen] =
{ 0 };
__attribute__((section(".RAM_D1")))  u8 GY39Send[GY39_UART_TXLen] =
{ 0 };
__attribute__((section(".RAM_D1")))  u8 GY39Receive[GY39_UART_RXLen] =
{ 0 };
__attribute__((section(".RAM_D1")))  u8 DeepReceive[Deep_UART_RXLen] =
{ 0 };

void InitGY39(void)
{
	GY39Send[0] = 0xa5;
	GY39Send[1] = 0x82;
	GY39Send[2] = 0x27;
	HAL_UART_Transmit_DMA(&GY39_UART, GY39Send, GY39_UART_TXLen);
}

GY39Data ReceiveGY39(void)
{
	GY39Data RevGY39;
	RevGY39.Temperature = ((GY39Receive[3] << 8) | GY39Receive[2]);
	RevGY39.Baro = ((GY39Receive[7] << 24) | (GY39Receive[6] << 16)
			| (GY39Receive[5] << 8) | GY39Receive[4]);
	RevGY39.Hum = ((GY39Receive[9] << 8) | GY39Receive[8]);
	__HAL_UART_ENABLE_IT(&GY39_UART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&GY39_UART, GY39Receive, GY39_UART_RXLen);
	return RevGY39;
}

WT931Data ReceiveWT931(void)
{
	WT931Data RevWT931;
	RevWT931.AccNum[0] = ((WT931Receive[3] << 8) | WT931Receive[2]);
	RevWT931.AccNum[1] = ((WT931Receive[5] << 8) | WT931Receive[4]);
	RevWT931.AccNum[2] = ((WT931Receive[7] << 8) | WT931Receive[6]);
	RevWT931.RotNum[0] = ((WT931Receive[14] << 8) | WT931Receive[13]);
	RevWT931.RotNum[1] = ((WT931Receive[16] << 8) | WT931Receive[15]);
	RevWT931.RotNum[2] = ((WT931Receive[18] << 8) | WT931Receive[17]);
	RevWT931.EulNum[0] = ((WT931Receive[25] << 8) | WT931Receive[24]);
	RevWT931.EulNum[1] = ((WT931Receive[27] << 8) | WT931Receive[26]);
	RevWT931.EulNum[2] = ((WT931Receive[29] << 8) | WT931Receive[28]);
	RevWT931.MagNum[0] = ((WT931Receive[36] << 8) | WT931Receive[35]);
	RevWT931.MagNum[1] = ((WT931Receive[38] << 8) | WT931Receive[37]);
	RevWT931.MagNum[2] = ((WT931Receive[40] << 8) | WT931Receive[39]);
	__HAL_UART_ENABLE_IT(&WT931_UART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&WT931_UART, WT931Receive, WT931_UART_RXLen);
	return RevWT931;
}

DeepData ReceiveDeep(void)
{
	DeepData RevDeep;
	RevDeep.WaterDepth = ((WT931Receive[1] << 8) | WT931Receive[0]);
	RevDeep.WaterTemperature = ((WT931Receive[3] << 8) | WT931Receive[2]);
	__HAL_UART_ENABLE_IT(&Deep_UART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&Deep_UART, DeepReceive, Deep_UART_RXLen);
	return RevDeep;
}

