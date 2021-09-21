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
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BasicCtrl.h"
#include "Sensor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

IWDG_HandleTypeDef hiwdg1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

osThreadId CtrlTaskHandle;
osThreadId SensorTaskHandle;
osThreadId BasetoUpTaskHandle;
osThreadId EmptyTaskHandle;
osMutexId UptoBaseDataRWFlagHandle;
osMutexId BasetoUpDataRWFlagHandle;
osMutexId WT931SensorDataRWFlagHandle;
osMutexId GY39SensorDataRWFlagHandle;
osSemaphoreId UptoBaseTransFinishHandle;
osSemaphoreId WT931TransFinishHandle;
osSemaphoreId GY39TransFinishHandle;
/* USER CODE BEGIN PV */
WT931Data WT931SensorData;
GY39Data GY39SensorData;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_IWDG1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM7_Init(void);
void CtrlTaskF(void const *argument);
void SensorTaskF(void const *argument);
void BtUF(void const *argument);
void EmptyTaskF(void const *argument);

/* USER CODE BEGIN PFP */
//void MotorAdjusterDelay(void);
///**
// * 软件PWM，可用于控制灯光PB3
// * 只开启时基单元即�????????
// * 频率�????????1kHz
// */
//void SoftwarePwm_Init(TIM_HandleTypeDef *htimx, uint16_t psr, uint16_t arr)
//{
//	/* 设置定时�???????? */
//	/* 调节定时器分频系数和预装载寄存器�???????? */
////	htimx->Instance->PSC = psr;
////	htimx->Instance->ARR = arr;
//	/* 设置软件比较�???????? */
//	SoftwarePwm_Control();
//
//	/* �????????启定时器及中�???????? */
////	HAL_TIM_PWM_Start();
//}
/**
 *	在主程序中轮询本函数来实现PWM输出
 */
void SoftwarePwm_Control(volatile uint32_t count)
{
	/* 判断计时值和设定阈�?�的大小 */
	if (htim7.Instance->CNT > count)
	{
		/* GPIO有输�???????? */
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
	}
	else
	{
		/* GPIO无输�???????? */
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
	}
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

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
	MX_IWDG1_Init();
	MX_TIM2_Init();
	MX_TIM3_Init();
	MX_TIM4_Init();
	MX_TIM5_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	MX_TIM1_Init();
	MX_TIM7_Init();
	/* USER CODE BEGIN 2 */
	HAL_IWDG_Refresh(&hiwdg1);
	//继电器初始化
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);
	//PWM初始�?????????????
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
//	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	__HAL_TIM_ENABLE(&htim7);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);

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

	HAL_IWDG_Refresh(&hiwdg1);
	/* USER CODE END 2 */

	/* Create the mutex(es) */
	/* definition and creation of UptoBaseDataRWFlag */
	osMutexDef(UptoBaseDataRWFlag);
	UptoBaseDataRWFlagHandle = osMutexCreate(osMutex(UptoBaseDataRWFlag));

	/* definition and creation of BasetoUpDataRWFlag */
	osMutexDef(BasetoUpDataRWFlag);
	BasetoUpDataRWFlagHandle = osMutexCreate(osMutex(BasetoUpDataRWFlag));

	/* definition and creation of WT931SensorDataRWFlag */
	osMutexDef(WT931SensorDataRWFlag);
	WT931SensorDataRWFlagHandle = osMutexCreate(osMutex(WT931SensorDataRWFlag));

	/* definition and creation of GY39SensorDataRWFlag */
	osMutexDef(GY39SensorDataRWFlag);
	GY39SensorDataRWFlagHandle = osMutexCreate(osMutex(GY39SensorDataRWFlag));

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* Create the semaphores(s) */
	/* definition and creation of UptoBaseTransFinish */
	osSemaphoreDef(UptoBaseTransFinish);
	UptoBaseTransFinishHandle = osSemaphoreCreate(
			osSemaphore(UptoBaseTransFinish), 1);

	/* definition and creation of WT931TransFinish */
	osSemaphoreDef(WT931TransFinish);
	WT931TransFinishHandle = osSemaphoreCreate(osSemaphore(WT931TransFinish),
			1);

	/* definition and creation of GY39TransFinish */
	osSemaphoreDef(GY39TransFinish);
	GY39TransFinishHandle = osSemaphoreCreate(osSemaphore(GY39TransFinish), 1);

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
	/* definition and creation of CtrlTask */
	osThreadDef(CtrlTask, CtrlTaskF, osPriorityNormal, 0, 128);
	CtrlTaskHandle = osThreadCreate(osThread(CtrlTask), NULL);

	/* definition and creation of SensorTask */
	osThreadDef(SensorTask, SensorTaskF, osPriorityNormal, 0, 128);
	SensorTaskHandle = osThreadCreate(osThread(SensorTask), NULL);

	/* definition and creation of BasetoUpTask */
	osThreadDef(BasetoUpTask, BtUF, osPriorityNormal, 0, 128);
	BasetoUpTaskHandle = osThreadCreate(osThread(BasetoUpTask), NULL);

	/* definition and creation of EmptyTask */
	osThreadDef(EmptyTask, EmptyTaskF, osPriorityIdle, 0, 128);
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
	RCC_OscInitTypeDef RCC_OscInitStruct =
	{ 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct =
	{ 0 };

	/** Supply configuration update enable
	 */
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
	{
	}
	/** Macro to configure the PLL clock source
	 */
	__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI
			| RCC_OSCILLATORTYPE_HSE;
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
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1
			| RCC_CLOCKTYPE_D1PCLK1;
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
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct =
	{ 0 };

	/** Initializes the peripherals clock
	 */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1
			| RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_USART3;
	PeriphClkInitStruct.PLL3.PLL3M = 25;
	PeriphClkInitStruct.PLL3.PLL3N = 192;
	PeriphClkInitStruct.PLL3.PLL3P = 2;
	PeriphClkInitStruct.PLL3.PLL3Q = 2;
	PeriphClkInitStruct.PLL3.PLL3R = 2;
	PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
	PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
	PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
	PeriphClkInitStruct.Usart234578ClockSelection =
	RCC_USART234578CLKSOURCE_PLL3;
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
	hiwdg1.Init.Window = 4095;
	hiwdg1.Init.Reload = 4095;
	if (HAL_IWDG_Init(&hiwdg1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN IWDG1_Init 2 */

	/* USER CODE END IWDG1_Init 2 */

}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void)
{

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig =
	{ 0 };
	TIM_MasterConfigTypeDef sMasterConfig =
	{ 0 };
	TIM_OC_InitTypeDef sConfigOC =
	{ 0 };
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig =
	{ 0 };

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 199;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 50000;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 1500;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.BreakFilter = 0;
	sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
	sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
	sBreakDeadTimeConfig.Break2Filter = 0;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */
	HAL_TIM_MspPostInit(&htim1);

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

	TIM_ClockConfigTypeDef sClockSourceConfig =
	{ 0 };
	TIM_MasterConfigTypeDef sMasterConfig =
	{ 0 };
	TIM_OC_InitTypeDef sConfigOC =
	{ 0 };

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 199;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 50000;
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

	TIM_ClockConfigTypeDef sClockSourceConfig =
	{ 0 };
	TIM_MasterConfigTypeDef sMasterConfig =
	{ 0 };
	TIM_OC_InitTypeDef sConfigOC =
	{ 0 };

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 199;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 50000;
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

	TIM_ClockConfigTypeDef sClockSourceConfig =
	{ 0 };
	TIM_MasterConfigTypeDef sMasterConfig =
	{ 0 };
	TIM_OC_InitTypeDef sConfigOC =
	{ 0 };

	/* USER CODE BEGIN TIM4_Init 1 */

	/* USER CODE END TIM4_Init 1 */
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 199;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 50000;
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

	TIM_ClockConfigTypeDef sClockSourceConfig =
	{ 0 };
	TIM_MasterConfigTypeDef sMasterConfig =
	{ 0 };
	TIM_OC_InitTypeDef sConfigOC =
	{ 0 };

	/* USER CODE BEGIN TIM5_Init 1 */

	/* USER CODE END TIM5_Init 1 */
	htim5.Instance = TIM5;
	htim5.Init.Prescaler = 199;
	htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim5.Init.Period = 50000;
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
 * @brief TIM7 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM7_Init(void)
{

	/* USER CODE BEGIN TIM7_Init 0 */

	/* USER CODE END TIM7_Init 0 */

	TIM_MasterConfigTypeDef sMasterConfig =
	{ 0 };

	/* USER CODE BEGIN TIM7_Init 1 */

	/* USER CODE END TIM7_Init 1 */
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = 399;
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = 50000;
	htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM7_Init 2 */

	/* USER CODE END TIM7_Init 2 */

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
	huart1.Init.BaudRate = 9600;
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
	if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8)
			!= HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8)
			!= HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART1_Init 2 */

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
	if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8)
			!= HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8)
			!= HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

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
	if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8)
			!= HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8)
			!= HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART3_Init 2 */

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
	HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
	/* DMA1_Stream1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
	/* DMA1_Stream2_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
	/* DMA1_Stream3_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	/* DMA1_Stream4_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct =
	{ 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);

	/*Configure GPIO pin : PC13 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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

	/*Configure GPIO pin : PB3 */
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_CtrlTaskF */
/**
 * @brief  Function implementing the CtrlTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_CtrlTaskF */
void CtrlTaskF(void const *argument)
{
	/* USER CODE BEGIN 5 */
	DownDataDef UptoBaseData =
	{ 0 };
	static MoveThruster Thurster;
	u8 Relay_filter = 0;
	u8 Relay_temp = 0;

	CaptureDownData();
	osDelay(200);
	//打开串口接收
	UptoBaseData = CaptureDownData();
	xSemaphoreGive(UptoBaseDataRWFlagHandle);
	osDelay(1);

	/* Infinite loop */
	for (;;)
	{
		if ((UptoBaseData.HeadOfData == 0x25)
				&& (UptoBaseData.EndOfData == 0x21))
		{
			if (Relay_filter >= 2) //滤波
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, UptoBaseData.Relay); //PD11 继电�?????????
			}

			//根据数据进行控制
			Thurster = MoveControl(UptoBaseData.StraightNum,
					UptoBaseData.RotateNum, UptoBaseData.VerticalNum,
					UptoBaseData.Mode);

			SoftwarePwm_Control(UptoBaseData.LightPWM); //软PWM灯光

			if (CheckPwmValue(Thurster.HorizontalThruster[3]))
			{
				TIM4->CCR4 = Thurster.HorizontalThruster[3]; //PD15 J6
			}
			if (CheckPwmValue(Thurster.VerticalThruster[0]))
			{
				TIM2->CCR4 = Thurster.VerticalThruster[0]; //PB11 J8
			}
			if (CheckPwmValue(Thurster.HorizontalThruster[2]))
			{
				TIM2->CCR3 = Thurster.HorizontalThruster[2]; //PB10 J9
			}
			if (CheckPwmValue(Thurster.HorizontalThruster[0]))
			{
				TIM4->CCR1 = Thurster.HorizontalThruster[0]; //PD12 J2
			}
			if (CheckPwmValue(Thurster.VerticalThruster[1]))
			{
				TIM4->CCR3 = Thurster.VerticalThruster[1]; //PD14 J3
			}
			if (CheckPwmValue(Thurster.HorizontalThruster[1]))
			{
				TIM4->CCR2 = Thurster.HorizontalThruster[1]; //PD13 J5
			}

			if (CheckPwmValue(UptoBaseData.ArmPWM[5]))
			{
				TIM2->CCR1 = UptoBaseData.ArmPWM[5]; //PA5机械臂6（预留）
			}
			if (CheckPwmValue(UptoBaseData.ArmPWM[0]))
			{
				TIM1->CCR1 = UptoBaseData.ArmPWM[0]; //PA8机械臂1
			}
			if (CheckPwmValue(UptoBaseData.ArmPWM[1]))
			{
				TIM3->CCR4 = UptoBaseData.ArmPWM[1]; //PB1机械臂2
			}
			if (CheckPwmValue(UptoBaseData.ArmPWM[2]))
			{
				TIM3->CCR3 = UptoBaseData.ArmPWM[2]; //PB0机械臂3
			}
			if (CheckPwmValue(UptoBaseData.THPWM))
			{
				TIM5->CCR1 = UptoBaseData.THPWM; //PA0云台
			}
			if (CheckPwmValue(UptoBaseData.ArmPWM[3]))
			{
				TIM3->CCR2 = UptoBaseData.ArmPWM[3]; //PA7机械臂4
			}
			if (CheckPwmValue(UptoBaseData.TranspPWM))
			{
				TIM5->CCR4 = UptoBaseData.TranspPWM; //PA3传送带
			}
			if (CheckPwmValue(UptoBaseData.ArmPWM[4]))
			{
				TIM5->CCR3 = UptoBaseData.ArmPWM[4]; //PA2机械臂5
			}
			if (CheckPwmValue(UptoBaseData.ResPWM))
			{
				TIM5->CCR2 = UptoBaseData.ResPWM; //PA1预留PWM
			}
		}

		if (xSemaphoreTake(UptoBaseTransFinishHandle,
				0) == pdTRUE)
		{
			if (xSemaphoreTake(UptoBaseDataRWFlagHandle,
					portTICK_PERIOD_MS) == pdTRUE)
			{
				//接收下传数据
				UptoBaseData = CaptureDownData();
				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

				if (UptoBaseData.Relay == Relay_temp) //继电器保持原来的值
				{
					Relay_filter++; //累计变化
					if (Relay_filter >= 2)
					{
						Relay_filter = 2;
					}
				}
				else //继电器变化
				{
					Relay_filter = 0;
					Relay_temp = UptoBaseData.Relay; //暂存新值
				}

				xSemaphoreGive(UptoBaseDataRWFlagHandle);
			}
		}

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
void SensorTaskF(void const *argument)
{
	/* USER CODE BEGIN SensorTaskF */
	//Finish:二进制信号量，串口传输结束位
	//RWFlag:互斥信号量，读写保护�????????????
	InitGY39();
	ReceiveGY39();
	ReceiveWT931();
	osDelay(200);
	GY39SensorData = ReceiveGY39();
	WT931SensorData = ReceiveWT931();
	xSemaphoreGive(GY39SensorDataRWFlagHandle);
	xSemaphoreGive(WT931SensorDataRWFlagHandle);
	/* Infinite loop */
	for (;;)
	{
		if (xSemaphoreTake(WT931TransFinishHandle,
				0) == pdTRUE)
		{
			if (xSemaphoreTake(WT931SensorDataRWFlagHandle,
					portTICK_PERIOD_MS) == pdTRUE)
			{
				WT931SensorData = ReceiveWT931();
				xSemaphoreGive(WT931SensorDataRWFlagHandle);
			}
		}
		if (xSemaphoreTake(GY39TransFinishHandle,
				0) == pdTRUE)
		{
			if (xSemaphoreTake(GY39SensorDataRWFlagHandle,
					portTICK_PERIOD_MS) == pdTRUE)
			{
				GY39SensorData = ReceiveGY39();
				xSemaphoreGive(GY39SensorDataRWFlagHandle);
			}
		}
		HAL_IWDG_Refresh(&hiwdg1);
	}
	/* USER CODE END SensorTaskF */
}

/* USER CODE BEGIN Header_BtUF */
/**
 * @brief Function implementing the BasetoUpTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_BtUF */
void BtUF(void const *argument)
{
	/* USER CODE BEGIN BtUF */
	UpDataDef BasetoUpData;
	/* Infinite loop */
	for (;;)
	{
		if (xSemaphoreTake(BasetoUpDataRWFlagHandle,
				portTICK_PERIOD_MS) == pdTRUE)
		{
			//汇�?�传感器数据并上�???????????
			BasetoUpData.CabinNum = 0b0001;
			BasetoUpData.WaterDetect = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10)
					<< 1;

			if (xSemaphoreTake(WT931SensorDataRWFlagHandle,
					portTICK_PERIOD_MS) == pdTRUE)
			{
				BasetoUpData.AccNum[0] = WT931SensorData.AccNum[0];
				BasetoUpData.AccNum[1] = WT931SensorData.AccNum[1];
				BasetoUpData.AccNum[2] = WT931SensorData.AccNum[2];
				BasetoUpData.RotNum[0] = WT931SensorData.RotNum[0];
				BasetoUpData.RotNum[1] = WT931SensorData.RotNum[1];
				BasetoUpData.RotNum[2] = WT931SensorData.RotNum[2];
				BasetoUpData.EulNum[0] = WT931SensorData.EulNum[0];
				BasetoUpData.EulNum[1] = WT931SensorData.EulNum[1];
				BasetoUpData.EulNum[2] = WT931SensorData.EulNum[2];
				BasetoUpData.MagNum[0] = WT931SensorData.MagNum[0];
				BasetoUpData.MagNum[1] = WT931SensorData.MagNum[1];
				BasetoUpData.MagNum[2] = WT931SensorData.MagNum[2];
				xSemaphoreGive(WT931SensorDataRWFlagHandle);
			}
			if (xSemaphoreTake(GY39SensorDataRWFlagHandle,
					portTICK_PERIOD_MS) == pdTRUE)
			{
				BasetoUpData.CabinBaro = GY39SensorData.Baro;
				BasetoUpData.CabinHum = GY39SensorData.Hum;
				BasetoUpData.CabinTemperature = GY39SensorData.Temperature;
				xSemaphoreGive(GY39SensorDataRWFlagHandle);
			}
			//发�?�上传数�????
			SendUpData(BasetoUpData);
			xSemaphoreGive(BasetoUpDataRWFlagHandle);
		}
		HAL_IWDG_Refresh(&hiwdg1);
	}
	/* USER CODE END BtUF */
}

/* USER CODE BEGIN Header_EmptyTaskF */
/**
 * @brief Function implementing the EmptyTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_EmptyTaskF */
void EmptyTaskF(void const *argument)
{
	/* USER CODE BEGIN EmptyTaskF */
	/* Infinite loop */
	for (;;)
	{
		HAL_IWDG_Refresh(&hiwdg1);
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
	if (htim->Instance == TIM6)
	{
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
