/**
 * @file Sensor.c
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
#include "Sensor.h"

__attribute__((section(".RAM_D1")))	      u8 WT931Receive[WT931_UART_RXLen] =
{ 0 };
__attribute__((section(".RAM_D1")))	      u8 GY39Send[GY39_UART_TXLen] =
{ 0 };
__attribute__((section(".RAM_D1")))	      u8 GY39Receive[GY39_UART_RXLen] =
{ 0 };
__attribute__((section(".RAM_D1")))	      u8 DeepReceive[Deep_UART_RXLen] =
{ 0 };
__attribute__((section(".RAM_D1")))       u8 P30Send[P30_UART_TXLen] =
{ 0 };
__attribute__((section(".RAM_D1")))	      u8 P30Receive[P30_UART_RXLen] =
{ 0 };

/**
 * @brief GY39温湿度大气压传感器初始化
 */
void InitGY39(void)
{
	GY39Send[0] = 0xa5;
	GY39Send[1] = 0x82;
	GY39Send[2] = 0x27;

	HAL_UART_Transmit_DMA(&GY39_UART, GY39Send, GY39_UART_TXLen);
}

/**
 * @brief 接收GY39温湿度大气压传感器数据
 * @return GY39Data GY39传感器数据接收结构体
 */
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

/**
 * @brief 接收WT931九轴传感器数据
 * @return WT931Data WT931九轴传感器数据结构体
 */
WT931Data ReceiveWT931(void)
{
	WT931Data RevWT931;

	if ((WT931Receive[0] == 0x55) && (WT931Receive[1] == 0x51))
	{
		RevWT931.AccNum[0] = ((WT931Receive[3] << 8) | WT931Receive[2]);
		RevWT931.AccNum[1] = ((WT931Receive[5] << 8) | WT931Receive[4]);
		RevWT931.AccNum[2] = ((WT931Receive[7] << 8) | WT931Receive[6]);
	}

	if ((WT931Receive[11] == 0x55) && (WT931Receive[12] == 0x52))
	{
		RevWT931.RotNum[0] = ((WT931Receive[14] << 8) | WT931Receive[13]);
		RevWT931.RotNum[1] = ((WT931Receive[16] << 8) | WT931Receive[15]);
		RevWT931.RotNum[2] = ((WT931Receive[18] << 8) | WT931Receive[17]);
	}

	if ((WT931Receive[22] == 0x55) && (WT931Receive[23] == 0x53))
	{
		RevWT931.EulNum[0] = ((WT931Receive[25] << 8) | WT931Receive[24]);
		RevWT931.EulNum[1] = ((WT931Receive[27] << 8) | WT931Receive[26]);
		RevWT931.EulNum[2] = ((WT931Receive[29] << 8) | WT931Receive[28]);
	}

	if ((WT931Receive[33] == 0x55) && (WT931Receive[34] == 0x54))
	{
		RevWT931.MagNum[0] = ((WT931Receive[36] << 8) | WT931Receive[35]);
		RevWT931.MagNum[1] = ((WT931Receive[38] << 8) | WT931Receive[37]);
		RevWT931.MagNum[2] = ((WT931Receive[40] << 8) | WT931Receive[39]);
	}

	__HAL_UART_ENABLE_IT(&WT931_UART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&WT931_UART, WT931Receive, WT931_UART_RXLen);

	return RevWT931;
}

/**
 * @brief 接收水深水温传感器数据
 * @return DeepData 水深水温传感器数据结构体
 */
DeepData ReceiveDeep(void)
{
	DeepData RevDeep;

	RevDeep.WaterDepth = ((DeepReceive[1] << 8) | DeepReceive[0]);
	RevDeep.WaterTemperature = ((DeepReceive[3] << 8) | DeepReceive[2]);

	__HAL_UART_ENABLE_IT(&Deep_UART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&Deep_UART, DeepReceive, Deep_UART_RXLen);
	return RevDeep;
}

/**
 * @brief 声呐传感器初始化
 */
void InitP30(void)
{
	//42 52 02 00 78 05 00 00 BB 04 D2 01
	P30Send[0] = 0x42;
	P30Send[1] = 0x52;
	P30Send[2] = 0x02;
	P30Send[3] = 0x00;
	P30Send[4] = 0x05;
	P30Send[5] = 0x00;
	P30Send[6] = 0x00;
	P30Send[7] = 0x00;
	P30Send[8] = 0xBB;
	P30Send[9] = 0x04;
	P30Send[10] = 0xD2;
	P30Send[11] = 0x01;

	HAL_UART_Transmit_DMA(&GP30_UART, P30Send, P30_UART_TXLen);
}

/**
 * @brief 接收声呐传感器数据
 * @return P30Data 声呐传感器数据结构体
 */
P30Data ReceiveP30(void)
{
	P30Data RevP30;

	uint8_t FrameState = 0;
	uint8_t Bytenum = 0;
	uint8_t CheckSum = 0;
	uint8_t datahex[11];

	for (uint8_t i = 0; i < 33; i++)
	{
		if (FrameState == 0)
		{
			if ((P30Receive[i] == 0x42) && (Bytenum == 0))
			{
				CheckSum = P30Receive[i];
				Bytenum = 1;
				continue;
			}
			else if ((P30Receive[i] == 0x52) && (Bytenum == 1))
			{
				CheckSum += P30Receive[i];
				Bytenum = 2;
				FrameState = 1;
				continue;
			}
		}
		else if (FrameState == 1)
		{
			if (Bytenum < 13)
			{
				datahex[Bytenum - 2] = P30Receive[i];
				CheckSum += P30Receive[i];
				Bytenum++;
			}
			else
			{
				if (P30Receive[i] == (CheckSum & 0xFF))
				{
					RevP30.Confidence = datahex[10];
					RevP30.DepthToBottom = (float) ((datahex[6] | datahex[7]
							| datahex[8] | datahex[9]) / 1000);
				}
				CheckSum = 0;
				Bytenum = 0;
				FrameState = 0;
			}
		}
	}

	__HAL_UART_ENABLE_IT(&GP30_UART, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&GP30_UART, P30Receive, P30_UART_RXLen);

	return RevP30;
}
