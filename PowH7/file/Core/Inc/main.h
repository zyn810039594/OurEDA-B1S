/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Setting.h"
//#include "Sensor.h"
//#include "BasicCtrl.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef unsigned char u8;
typedef unsigned short u16;
typedef long unsigned int u32;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
//上位仓接收完毕标志位
extern volatile u8 UpSideFinish;
//传感器读取完成标志位
extern volatile u8 WT931Finish;
extern volatile u8 GY39Finish;

//上位仓缓存读写标志位
extern volatile u8 UpIO;
//传感器缓存读写标志位
extern volatile u8 WT931IO;
extern volatile u8 GY39IO;

extern volatile u8 WT931Len;
extern volatile u8 GY39Len;

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
#define UART1RXLen 24
#define UART1TXLen 3
#define UART2RXLen 60
#define UART3TXLen 30
#define UART3RXLen 41

extern u8 UART1RXCache[UART1RXLen]; //温湿度大气压传感器数据串口缓�?????
extern u8 UART1TXCache[UART1TXLen]; //温湿度大气压传感器指令串口缓�?????
extern u8 UART2RXCache[UART2RXLen]; //九轴传感器数据串口缓�?????
extern u8 UART3RXCache[UART3RXLen]; //下传指令串口缓存
extern u8 UART3TXCache[UART3TXLen]; //上传数据串口缓存
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
