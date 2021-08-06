/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "Setting.h"
#include "Sensor.h"
#include "BasicCtrl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

//上传传感器仓位标志位
/* 0 控制
 * 1 电源
 * 2 控水
 * 4 电水
 * */
u8 SensorCabin = 0;

//上位仓接收完毕标志位
volatile u8 UpSideFinish = 0;
//传感器读取完成标志位
volatile u8 WT931Finish = 0;
volatile u8 GY39Finish = 0;

//上位仓缓存读写标志位
volatile u8 UpIO = 0;
//传感器缓存读写标志位
volatile u8 WT931IO = 0;
volatile u8 GY39IO = 0;

//上位仓接收缓存
u8 UpCache[UART3RXLen];
//传感器数据缓存
u8 WaterDetect = 0; //漏水检测标志位

u16 AccelerationCache[3] =
{ 0 }; //加速度值缓存
u16 RotSpeedCache[3] =
{ 0 }; //角度值缓存
u16 EulerAngleCache[3] =
{ 0 }; //角度值缓存
u16 MagnetisCache[3] =
{ 0 }; //磁场值缓存

u16 TemCache = 0; //温度值缓存
u16 BaroCache[2] =
{ 0 }; //大气压值缓存
u16 HumCache = 0; //湿度值缓存
//本仓没有水深水温传感传感器

//传感器数据长度
volatile u8 WT931Len = 0;
//int rx_len;
volatile u8 GY39Len = 0;
//作业起始标志
volatile u8 DoingEnable = 0;

//串口收发缓存
u8 UART1RXCache[UART1RXLen]; //温湿度大气压传感器数据串口缓存
u8 UART1TXCache[UART1TXLen] = {0xA5, 0x83, 0x28}; //温湿度大气压传感器指令串口缓存
//u8 UART1TXCache[UART1TXLen]; //温湿度大气压传感器指令串口缓存
u8 UART2RXCache[UART2RXLen]; //九轴传感器数据串口缓存
u8 UART3RXCache[UART3RXLen]; //下传指令串口缓存
u8 UART3TXCache[UART3TXLen]; //上传数据串口缓存

//PWM端口定向指针
//推进器PWM
volatile uint32_t *ThrusterPWM[6] =
{ &TIM4->CCR1, &TIM4->CCR3, &TIM4->CCR2, &TIM4->CCR4, &TIM2->CCR4, &TIM2->CCR3 };
//机械臂PWM
volatile uint32_t *ArmPWM[6] =
{ &TIM5->CCR4, &TIM5->CCR3, &TIM5->CCR2, &TIM3->CCR1, &TIM3->CCR4, &TIM3->CCR3 };
//灯光PWM
volatile uint32_t *LightPWM = &TIM2->CCR2;
//云台PWM
volatile uint32_t *THPWM = &TIM2->CCR1;
//传送带PWM
volatile uint32_t *TranspPWM = &TIM5->CCR1;
//保留PWM
volatile uint32_t *ReservePWM = &TIM3->CCR2;




/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

IWDG_HandleTypeDef hiwdg1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

osThreadId InitialTaskHandle;
osThreadId SensorTaskHandle;
osThreadId UpTaskHandle;
osThreadId CtrlTaskHandle;
osThreadId EmptyTaskHandle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_IWDG1_Init(void);
void InitialTaskF(void const * argument);
void SensorTaskF(void const * argument);
void UpTaskF(void const * argument);
void CtrlTaskF(void const * argument);
void EmptyTaskF(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//串口接收完成回调函数
/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		HAL_UART_Receive_IT(&huart1, UART1RXCache, GY39Len);
	}
	else if (huart->Instance == USART2)
	{
		HAL_UART_Receive_IT(&huart2, UART2RXCache, WT931Len);
	}
	else if (huart->Instance == USART3)
	{
		HAL_UART_Receive_IT(&huart3, UART3RXCache, UART3RXLen);
	}
}
*/

#ifdef ROV_DEBUG
//printf重定向
int fputc(int ch, FILE *f)
{
	uint32_t temp = ch;
	HAL_UART_Transmit_DMA(&huart3, (uint8_t*) &temp, 1);
	osDelay(2);
	return ch;
}

/*

//串口发送完成回调函数
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart1)
	{

	}
	else if (huart == &huart2)
	{

	}
	else if (huart == &huart3)
	{

	}
}
*/
#endif
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
#ifdef ROV_DEBUG
	u8 *MachineStart = "Machine Opening\r\n";
#endif
  /* USER CODE END 1 */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
	MX_DMA_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_IWDG1_Init();
  /* USER CODE BEGIN 2 */
#ifdef ROV_DEBUG
//发送开机指令
	HAL_UART_Transmit_DMA(&huart3, MachineStart, 17);
#endif
	/* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of InitialTask */
  osThreadDef(InitialTask, InitialTaskF, osPriorityRealtime, 0, 512);
  InitialTaskHandle = osThreadCreate(osThread(InitialTask), NULL);

  /* definition and creation of SensorTask */
  osThreadDef(SensorTask, SensorTaskF, osPriorityNormal, 0, 512);
  SensorTaskHandle = osThreadCreate(osThread(SensorTask), NULL);

  /* definition and creation of UpTask */
  osThreadDef(UpTask, UpTaskF, osPriorityNormal, 0, 512);
  UpTaskHandle = osThreadCreate(osThread(UpTask), NULL);

  /* definition and creation of CtrlTask */
  osThreadDef(CtrlTask, CtrlTaskF, osPriorityNormal, 0, 512);
  CtrlTaskHandle = osThreadCreate(osThread(CtrlTask), NULL);

  /* definition and creation of EmptyTask */
  osThreadDef(EmptyTask, EmptyTaskF, osPriorityIdle, 0, 512);
  EmptyTaskHandle = osThreadCreate(osThread(EmptyTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_USART3;
  PeriphClkInitStruct.PLL3.PLL3M = 25;
  PeriphClkInitStruct.PLL3.PLL3N = 192;
  PeriphClkInitStruct.PLL3.PLL3P = 2;
  PeriphClkInitStruct.PLL3.PLL3Q = 2;
  PeriphClkInitStruct.PLL3.PLL3R = 2;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_PLL3;
  PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_PLL3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief IWDG1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG1_Init(void)
{

  /* USER CODE BEGIN IWDG1_Init 0 */

  /* USER CODE END IWDG1_Init 0 */

  /* USER CODE BEGIN IWDG1_Init 1 */

  /* USER CODE END IWDG1_Init 1 */
  hiwdg1.Instance = IWDG1;
  hiwdg1.Init.Prescaler = IWDG_PRESCALER_4;
  hiwdg1.Init.Window = 2047;
  hiwdg1.Init.Reload = 2047;
  if (HAL_IWDG_Init(&hiwdg1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG1_Init 2 */

  /* USER CODE END IWDG1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 200-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 50000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1500;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 200-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 50000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1500;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 199;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 50000-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1500;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 199;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 50000-1;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1500;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */
  HAL_TIM_MspPostInit(&htim5);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
	//__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	//HAL_UART_Receive_DMA(&huart1, UART1RXCache, UART1RXLen);
  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 921600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
	//__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	//HAL_UART_Receive_DMA(&huart2, UART2RXCache, UART2RXLen);
  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */
	//__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
	//HAL_UART_Receive_DMA(&huart3, UART3RXCache, UART3RXLen);
  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin : PD10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PD11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_InitialTaskF */
/**
 * @brief  Function implementing the InitialTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_InitialTaskF */
void InitialTaskF(void const * argument)
{
  /* USER CODE BEGIN 5 */
	//喂狗
	HAL_IWDG_Refresh(&hiwdg1);
	//挂起要执行的任务
	vTaskSuspend(SensorTaskHandle);
	vTaskSuspend(UpTaskHandle);
	vTaskSuspend(CtrlTaskHandle);
	//继电器闭合
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);
	
	/* 初始化一大堆PWM */
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);

	//HAL_UART_Receive_IT(&huart1, UART1RXCache, UART1RXLen);
	//HAL_UART_Receive_IT(&huart2, UART2RXCache, UART2RXLen);
	//HAL_UART_Receive_IT(&huart3, UART3RXCache, UART3RXLen);

	//HAL_UART_Transmit_DMA(&huart1, UART1TXCache, UART1TXLen);
/* 保存各控制位数据 */
/*
*StraightNum = *PDStraight;
		*RotateNum = *PDRotate;
		*VerticalNum = *PDVertical;
		*LightNum = *PDLight;
		*THNum = *PDTH;
		*TranspNum = *PDTransp;
		*ArmNum[0] = *PDArm[0];
		*ArmNum[1] = *PDArm[1];
		*ArmNum[2] = *PDArm[2];
		*ArmNum[3] = *PDArm[3];
		*ArmNum[4] = *PDArm[4];
		*ArmNum[5] = *PDArm[5];
		*ResPWM = *PDRes;
		*ModeNum = (*PDMode & 7);
		*RelaySW = (*PDMode & 8);

	PDBegin = UpCache; //起始位对准
	*/
	/* 其他各位对齐 */
	/*
	PDStraight = (UpCache + 1);
	PDRotate = (UpCache + 3);
	PDVertical = (UpCache + 5);
	PDLight = (UpCache + 7);
	PDTH = (UpCache + 9);
	PDTransp = (UpCache + 11);
	PDArm[0] = (UpCache + 13);
	PDArm[1] = (UpCache + 15);
	PDArm[2] = (UpCache + 17);
	PDArm[3] = (UpCache + 19);
	PDArm[4] = (UpCache + 21);
	PDArm[5] = (UpCache + 23);
	PDRes = (UpCache + 25);
	PDMode = (UpCache + 27);
	PDId = UpCache + 28;
	PDEnd = UpCache + 29; //结束位对准


	AccelerationCache = (UART1RXCache + 2);
	//PAccPos[1] = (u16*) (UART1RXCache + 4);
	//PAccPos[2] = (u16*) (UART1RXCache + 6);
	
	//PInsTemPos[0] = (u16*) (UART1RXCache + 8);
	
	RotSpeedCache = (UART1RXCache + 13);
	//PRotPos[1] = (u16*) (UART1RXCache + 15);
	//PRotPos[2] = (u16*) (UART1RXCache + 17);
	
	//PInsTemPos[1] = (u16*) (UART1RXCache + 19);
	
	EulerAngleCache = (UART1RXCache + 24);
	//PEulPos[1] = (u16*) (UART1RXCache + 26);
	//PEulPos[2] = (u16*) (UART1RXCache + 28);
	
	//PInsTemPos[2] = (u16*) (UART1RXCache + 30);
	
	MagnetisCache = (UART1RXCache + 35);
	//PMagPos[1] = (u16*) (UART1RXCache + 37);
	//PMagPos[2] = (u16*) (UART1RXCache + 39);
	
	//PInsTemPos[2] = (u16*) (UART1RXCache + 41);


	TemCache = (UART1RXCache + 13);
	BaroCache = (UART1RXCache + 15);
	HumCache = (UART1RXCache + 19);
*/
	
	//开启其他任务
	vTaskResume(SensorTaskHandle);
	vTaskResume(UpTaskHandle);
	vTaskResume(CtrlTaskHandle);
	
	//喂狗
	HAL_IWDG_Refresh(&hiwdg1);
	
	//挂起初始化任务不再执行
	vTaskSuspend(InitialTaskHandle);
	/* Infinite loop */
	for (;;)
	{
		HAL_IWDG_Refresh(&hiwdg1);
	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_SensorTaskF */
/**
 * @brief Function implementing the SensorTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_SensorTaskF */
void SensorTaskF(void const * argument)
{
  /* USER CODE BEGIN SensorTaskF */
	//SendBackPoint(UART3TXCache, NULL, &WT931IO, &GY39IO); //上传指针与标志位定向
	//DownDetectPoint(UpCache); //下传指令缓存指针定向
	//HAL_UART_Receive_DMA(&huart1, UART3RXCache, UART3RXLen); //初始化下传串口
	
	WT931Point(UART2RXCache); //九轴传感器读取指针定向
	GY39InitStr(UART1TXCache); //温湿度传感器初始化数据发送字符串初始化
	//向温湿度传感器发送初始化指令
	HAL_UART_Transmit_DMA(&huart1, UART1TXCache, UART1TXLen);
	GY39Point(UART1RXCache); //温湿度大气压传感器串口缓存区定向
	
	/* Infinite loop */
	for (;;)
	{
		WaterDetect = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10); //检测是否漏水

		if (WT931Finish == 1) //如果传感器读取完成
		{
			/*
			while(WT931IO == 1) //等待传感器读写
			{
				HAL_IWDG_Refresh(&hiwdg1);
				osDelay(1);
			}
			WT931IO = 1; //正在读写传感器
			*/
			if (WT931Len == 44)
			{
				WT931Take(AccelerationCache, RotSpeedCache, EulerAngleCache, MagnetisCache);
				//HAL_UART_Transmit_DMA(&huart3, UART2RXCache, UART2RXLen);
#ifdef ROV_DEBUG
				HAL_UART_Transmit_DMA(&huart3, UART2RXCache, UART2RXLen);
#endif
			}
			//memset(UART2RXCache, 0, WT931Len);
			WT931Len = 0;
			WT931Finish = 0; //已完成传感器数据取出

			HAL_UART_Receive_DMA(&huart2, UART2RXCache, UART2RXLen); //恢复串口接收
			__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
			//HAL_UART_Transmit_DMA(&huart3, UART2RXCache, UART2RXLen);
			
			WT931IO = 0;
		}
		
		if (GY39Finish == 1) //如果传感器读取完成
		{
			/*
			while (GY39IO == 1)
			{
				HAL_IWDG_Refresh(&hiwdg1);
				osDelay(1);
			}
			GY39IO = 1;
			*/
			if(GY39Len == UART1RXLen)
			{
				GY39Take(&TemCache, BaroCache, &HumCache);
				//HAL_UART_Transmit_DMA(&huart3, UART1RXCache, UART1RXLen);
#ifdef ROV_DEBUG
				HAL_UART_Transmit_DMA(&huart3, UART1RXCache, UART1RXLen);
#endif
			}
			//memset(UART1RXCache, 0, GY39Len);
			GY39Len = 0;
			GY39Finish = 0; //已完成传感器数据取出
			
			HAL_UART_Receive_DMA(&huart1, UART1RXCache, UART1RXLen); //恢复串口接收
			__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
			//HAL_UART_Transmit_DMA(&huart3, UART1RXCache, UART3TXLen);
			
			GY39IO = 0;
		}
		
		SendBack(WaterDetect, TemCache, BaroCache, HumCache, AccelerationCache,
					RotSpeedCache, EulerAngleCache, MagnetisCache, NULL, NULL);
		
		//HAL_UART_Transmit_DMA(&huart3, UART3TXCache, UART3TXLen);
		
		HAL_IWDG_Refresh(&hiwdg1);
		osDelay(1);
	}
  /* USER CODE END SensorTaskF */
}

/* USER CODE BEGIN Header_UpTaskF */
/**
 * @brief Function implementing the UpTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_UpTaskF */
void UpTaskF(void const * argument)
{
  /* USER CODE BEGIN UpTaskF */

	SendBackPoint(UART3TXCache, NULL, &WT931IO, &GY39IO); //上传指针与标志位定向
	DownDetectPoint(UpCache); //下传指令缓存指针定向
	HAL_UART_Receive_DMA(&huart1, UART3RXCache, UART3RXLen); //初始化下传串口

	/* Infinite loop */
	
	for (;;)
	{
		
		while (UpSideFinish == 0) //等待数据下传
		{
			osDelay(1);
		}

		if (IdTest(UART3RXCache, 0)) //下传指令数据校验
		{
			while(UpIO == 1)
			{
				osDelay(1);
			}
			UpIO = 1;

			for (u8 i = 0; i < UART3RXLen; ++i)
			{
				UpCache[i] = UART3RXCache[i];
			}
			//memset(UART3RXCache, 0, UART3RXLen);
			UpIO = 0; //收取结束
			UpSideFinish = 0;
		}
		DoingEnable = 1;

		//上传传感器数据
		SendBack(WaterDetect, TemCache, BaroCache, HumCache, AccelerationCache,
					RotSpeedCache, EulerAngleCache, MagnetisCache, NULL, NULL);
		
#ifdef ROV_DEBUG
		HAL_UART_Transmit_DMA(&huart3, UART2RXCache, UART2RXLen);
		HAL_UART_Transmit_DMA(&huart3, UART1RXCache, UART1RXLen);
#endif
		HAL_UART_Transmit_DMA(&huart3, UART3TXCache, UART3TXLen);

		__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
		HAL_UART_Receive_DMA(&huart3, UART3RXCache, UART3RXLen);

		HAL_IWDG_Refresh(&hiwdg1);
		osDelay(1);
	}
  /* USER CODE END UpTaskF */
}

/* USER CODE BEGIN Header_CtrlTaskF */
/**
 * @brief Function implementing the CtrlTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_CtrlTaskF */
void CtrlTaskF(void const * argument)
{
  /* USER CODE BEGIN CtrlTaskF */

	static u16 RStraightNum = 0;
	static u16 RRotateNum = 0;
	static u16 RVerticalNum = 0;
	static u8 RMode = 0;
	static u8 RRelay = 0;
	
	while (DoingEnable == 0) //等待允许作业的指令
	{
		HAL_IWDG_Refresh(&hiwdg1);
		osDelay(1);
	}
	
	/* Infinite loop */
	for (;;)
	{
		while (UpIO == 1) //不打扰上传或收取数据
		{
			HAL_IWDG_Refresh(&hiwdg1);
			osDelay(1);
		}
		UpIO = 1;
		/* 接收并处理解析数据 */
		DownDetectReceive(&RStraightNum, &RRotateNum, &RVerticalNum, LightPWM, 
							THPWM, TranspPWM, ArmPWM, ReservePWM, &RMode, &RRelay);
		
		//根据指令控制继电器
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, RRelay);

		//根据指令控制PWM操作推进器来处理机器人运动
		MoveControl(RStraightNum, RRotateNum, RVerticalNum, RMode, ThrusterPWM);
		
		HAL_IWDG_Refresh(&hiwdg1);
		//osDelay(1);
	}
  /* USER CODE END CtrlTaskF */
}

/* USER CODE BEGIN Header_EmptyTaskF */
/**
 * @brief Function implementing the EmptyTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_EmptyTaskF */
void EmptyTaskF(void const * argument)
{
  /* USER CODE BEGIN EmptyTaskF */
	/* Infinite loop */
	for (;;)
	{
		HAL_IWDG_Refresh(&hiwdg1); //喂狗
		osDelay(1);
	}
  /* USER CODE END EmptyTaskF */
}

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
