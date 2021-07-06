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
#include "Sensor.h"
#include "BasicCtrl.h"
#include "Setting.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/* STP库的惯用表示 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef long unsigned int u32;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
extern u8 UpSideFinish; 	//上位机接收完毕标志位
extern u8 DownSideFinish; 	//下位机接收完毕标志位
extern u8 WT931Len; 		//传感器数据长度

extern u8 DeepFinish; 		//水深传感器读取完成标志位
extern u8 GY39Finish; 		//温湿度大气压传感器读取完成标志位
extern u8 WT931Finish; 		//九轴读取完成标志位

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
#define UART1RXLen 30
#define UART1TXLen 41
#define UART2RXLen 15
#define UART2TXLen 3
#define UART3RXLen 60
#define UART4RXLen 4
#define UART4TXLen 20
#define UART8RXLen 41
#define UART8TXLen 30

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
