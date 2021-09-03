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
#include "Sensor.h"
#include "BasicCtrl.h"
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

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart8;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart4_tx;
DMA_HandleTypeDef hdma_uart5_rx;
DMA_HandleTypeDef hdma_uart5_tx;
DMA_HandleTypeDef hdma_uart8_rx;
DMA_HandleTypeDef hdma_uart8_tx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart3_rx;

osThreadId CtrlTaskHandle;
osThreadId SensorTaskHandle;
osThreadId UptoBaseTaskHandle;
osThreadId BasetoUpTaskHandle;
osMutexId UptoBaseDataRWFlagHandle;
osMutexId BasetoUpDataRWFlagHandle;
osMutexId WT931SensorDataRWFlagHandle;
osMutexId GY39SensorDataRWFlagHandle;
osMutexId DeepSensorDataRWFlagHandle;
osMutexId BaseControlEnableFlagHandle;
osMutexId P30SensorDataRWFlagHandle;
osSemaphoreId UptoBaseTransFinishHandle;
osSemaphoreId WT931TransFinishHandle;
osSemaphoreId GY39TransFinishHandle;
osSemaphoreId BasetoUpTransFinishHandleHandle;
osSemaphoreId DeepTransFinishHandleHandle;
osSemaphoreId P30TransFinishHandle;
/* USER CODE BEGIN PV */
//DownDataDef UptoBaseData;
//UpDataDef BasetoUpData;
//UpDataDef Power_BasetoUpData;
DeepData DeepSensorData;
WT931Data WT931SensorData;
GY39Data GY39SensorData;
P30Data P30SensorData;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_IWDG1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_UART4_Init(void);
static void MX_UART8_Init(void);
static void MX_UART5_Init(void);
void CtrlTaskF(void const *argument);
void SensorTaskF(void const *argument);
void UtBF(void const *argument);
void BtUF(void const *argument);

/* USER CODE BEGIN PFP */
void OpenWrt_Delay(void);
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
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	MX_UART4_Init();
	MX_UART8_Init();
	MX_UART5_Init();
	/* USER CODE BEGIN 2 */
	HAL_IWDG_Refresh(&hiwdg1);

	//初始化下位机

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
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

	/* definition and creation of DeepSensorDataRWFlag */
	osMutexDef(DeepSensorDataRWFlag);
	DeepSensorDataRWFlagHandle = osMutexCreate(osMutex(DeepSensorDataRWFlag));

	/* definition and creation of BaseControlEnableFlag */
	osMutexDef(BaseControlEnableFlag);
	BaseControlEnableFlagHandle = osMutexCreate(osMutex(BaseControlEnableFlag));

	/* definition and creation of P30SensorDataRWFlag */
	osMutexDef(P30SensorDataRWFlag);
	P30SensorDataRWFlagHandle = osMutexCreate(osMutex(P30SensorDataRWFlag));

	/* USER CODE BEGIN RTOS_MUTEX */
	HAL_IWDG_Refresh(&hiwdg1);
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

	/* definition and creation of BasetoUpTransFinishHandle */
	osSemaphoreDef(BasetoUpTransFinishHandle);
	BasetoUpTransFinishHandleHandle = osSemaphoreCreate(
			osSemaphore(BasetoUpTransFinishHandle), 1);

	/* definition and creation of DeepTransFinishHandle */
	osSemaphoreDef(DeepTransFinishHandle);
	DeepTransFinishHandleHandle = osSemaphoreCreate(
			osSemaphore(DeepTransFinishHandle), 1);

	/* definition and creation of P30TransFinish */
	osSemaphoreDef(P30TransFinish);
	P30TransFinishHandle = osSemaphoreCreate(osSemaphore(P30TransFinish), 1);

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	HAL_IWDG_Refresh(&hiwdg1);
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

	/* definition and creation of UptoBaseTask */
	osThreadDef(UptoBaseTask, UtBF, osPriorityNormal, 0, 128);
	UptoBaseTaskHandle = osThreadCreate(osThread(UptoBaseTask), NULL);

	/* definition and creation of BasetoUpTask */
	osThreadDef(BasetoUpTask, BtUF, osPriorityNormal, 0, 128);
	BasetoUpTaskHandle = osThreadCreate(osThread(BasetoUpTask), NULL);

	/* USER CODE BEGIN RTOS_THREADS */
	HAL_IWDG_Refresh(&hiwdg1);
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
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART5
			| RCC_PERIPHCLK_UART8 | RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_UART4
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
 * @brief UART4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_UART4_Init(void)
{

	/* USER CODE BEGIN UART4_Init 0 */

	/* USER CODE END UART4_Init 0 */

	/* USER CODE BEGIN UART4_Init 1 */

	/* USER CODE END UART4_Init 1 */
	huart4.Instance = UART4;
	huart4.Init.BaudRate = 921600;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;
	huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart4) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8)
			!= HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8)
			!= HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN UART4_Init 2 */

	/* USER CODE END UART4_Init 2 */

}

/**
 * @brief UART5 Initialization Function
 * @param None
 * @retval None
 */
static void MX_UART5_Init(void)
{

	/* USER CODE BEGIN UART5_Init 0 */

	/* USER CODE END UART5_Init 0 */

	/* USER CODE BEGIN UART5_Init 1 */

	/* USER CODE END UART5_Init 1 */
	huart5.Instance = UART5;
	huart5.Init.BaudRate = 115200;
	huart5.Init.WordLength = UART_WORDLENGTH_8B;
	huart5.Init.StopBits = UART_STOPBITS_1;
	huart5.Init.Parity = UART_PARITY_NONE;
	huart5.Init.Mode = UART_MODE_TX_RX;
	huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart5.Init.OverSampling = UART_OVERSAMPLING_16;
	huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart5.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart5) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart5, UART_TXFIFO_THRESHOLD_1_8)
			!= HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart5, UART_RXFIFO_THRESHOLD_1_8)
			!= HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart5) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN UART5_Init 2 */

	/* USER CODE END UART5_Init 2 */

}

/**
 * @brief UART8 Initialization Function
 * @param None
 * @retval None
 */
static void MX_UART8_Init(void)
{

	/* USER CODE BEGIN UART8_Init 0 */

	/* USER CODE END UART8_Init 0 */

	/* USER CODE BEGIN UART8_Init 1 */

	/* USER CODE END UART8_Init 1 */
	huart8.Instance = UART8;
	huart8.Init.BaudRate = 115200;
	huart8.Init.WordLength = UART_WORDLENGTH_8B;
	huart8.Init.StopBits = UART_STOPBITS_1;
	huart8.Init.Parity = UART_PARITY_NONE;
	huart8.Init.Mode = UART_MODE_TX_RX;
	huart8.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart8.Init.OverSampling = UART_OVERSAMPLING_16;
	huart8.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart8.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart8.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart8, UART_TXFIFO_THRESHOLD_1_8)
			!= HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart8, UART_RXFIFO_THRESHOLD_1_8)
			!= HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart8) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN UART8_Init 2 */

	/* USER CODE END UART8_Init 2 */

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
	huart2.Init.BaudRate = 9600;
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
	huart3.Init.BaudRate = 921600;
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
	__HAL_RCC_DMA2_CLK_ENABLE();

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
	/* DMA1_Stream5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	/* DMA1_Stream6_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
	/* DMA1_Stream7_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);
	/* DMA2_Stream0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	/* DMA2_Stream1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
	/* DMA2_Stream2_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

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
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

	/*Configure GPIO pin : PE4 */
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pin : PE6 */
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pin : PC13 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PA8 */
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/**
 * @brief  跳过OpenWrt开机启动信息(dmesg)
 */
void OpenWrt_Delay(void)
{
	for (int i = 0; i < 60; i++)
	{
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		for (int j = 0; j < 1000; j++)
		{
			osDelay(1);
		}
	}
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}
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

	/* Infinite loop */
	for (;;)
	{
		/* 该任务作为空任务 */
		osDelay(1);
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
	InitGY39();
	InitP30();
	osDelay(100);
	ReceiveDeep();
	ReceiveGY39();
	ReceiveWT931();
	ReceiveP30();
	osDelay(100);
	DeepSensorData = ReceiveDeep();
	GY39SensorData = ReceiveGY39();
	WT931SensorData = ReceiveWT931();
	P30SensorData = ReceiveP30();
	osDelay(100);
	xSemaphoreGive(DeepSensorDataRWFlagHandle);
	xSemaphoreGive(GY39SensorDataRWFlagHandle);
	xSemaphoreGive(WT931SensorDataRWFlagHandle);
	xSemaphoreGive(P30SensorDataRWFlagHandle);
	vTaskSuspend(SensorTaskHandle);
	osDelay(1);
	/* Infinite loop */
	for (;;)
	{
		//收取深度数据
		if (xSemaphoreTake(DeepTransFinishHandleHandle,
				0) == pdTRUE)
		{
			if (xSemaphoreTake(DeepSensorDataRWFlagHandle,
					portTICK_PERIOD_MS) == pdTRUE)
			{
				DeepSensorData = ReceiveDeep();
				xSemaphoreGive(DeepSensorDataRWFlagHandle);
			}
		}
		//收取九轴数据
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
		//收取温湿度数�??????
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
		//收取声呐数据
		if (xSemaphoreTake(P30TransFinishHandle,0) == pdTRUE)
		{
			if (xSemaphoreTake(P30SensorDataRWFlagHandle,
					portTICK_PERIOD_MS) == pdTRUE)
			{
				P30SensorData = ReceiveP30();
				xSemaphoreGive(P30SensorDataRWFlagHandle);
			}
		}
		HAL_IWDG_Refresh(&hiwdg1);
	}
	/* USER CODE END SensorTaskF */
}

/* USER CODE BEGIN Header_UtBF */
/**
 * @brief Function implementing the UptoBaseTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_UtBF */
void UtBF(void const *argument)
{
	/* USER CODE BEGIN UtBF */
	DownDataDef UptoBaseData =
	{ 0 };
//	DownDataDef temp_UptoBaseData =
//	{ 0 };
//	u8 DIPFlag = 0;
//	u16 DIPStartNum = 0; //PID定向定深预期�???????????

	OpenWrt_Delay(); //跳过openwrt�?机时�?

	CaptureDownData();
	osDelay(100);
	//打开串口接收
	UptoBaseData = CaptureDownData();
	xSemaphoreGive(BasetoUpDataRWFlagHandle);
	xSemaphoreGive(UptoBaseDataRWFlagHandle);
	vTaskResume(BasetoUpTaskHandle);
	vTaskResume(SensorTaskHandle);
	osDelay(1);
	/* Infinite loop */
	for (;;)
	{
		//收取上位机指�?????
		if (xSemaphoreTake(UptoBaseTransFinishHandle,
				0) == pdTRUE)
		{
			if (xSemaphoreTake(UptoBaseDataRWFlagHandle,
					portTICK_PERIOD_MS) == pdTRUE)
			{
				UptoBaseData = CaptureDownData();
				xSemaphoreGive(UptoBaseDataRWFlagHandle);
			}

			//根据控制位判断是否执行自主定向定�??????
//			if (UptoBaseData.Mode & 0x02 == 0x02) //0b0010,使用定向模式
//			{
//				if (DIPFlag == 0)
//				{
//					DIPFlag = 1;
//					DIPStartNum = WT931SensorData.EulNum[2];
//				}
//				UptoBaseData.StraightNum = SpecialMovePID(2, DIPStartNum,
//						WT931SensorData.EulNum[2]); //PID定向
//			}
//			else if (UptoBaseData.Mode & 0x04 == 0x04) //0b0100,使用定深模式
//			{
//				if (DIPFlag == 0)
//				{
//					DIPFlag = 1;
//					DIPStartNum = DeepSensorData.WaterDepth;
//				}
//				UptoBaseData.VerticalNum = SpecialMovePID(4, DIPStartNum,
//						DeepSensorData.WaterDepth); //PID定深
//			}
//			else
//			{
//				DIPFlag = 0; //不开启定向定�??
//			}

			//下传指令
			if (xSemaphoreTake(BasetoUpDataRWFlagHandle,
					portTICK_PERIOD_MS) == pdTRUE)
			{
//				temp_UptoBaseData = UptoBaseData;
//				SendDownData(temp_UptoBaseData);
				SendDownData(UptoBaseData);
				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
				xSemaphoreGive(BasetoUpDataRWFlagHandle);
			}
		}

		HAL_IWDG_Refresh(&hiwdg1);
	}
	/* USER CODE END UtBF */
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
	//置为1的时候表示推进器控制�???????????
	//置为0的时候表示主控仓
	UpDataDef Power_BasetoUpData =
	{ 0 };
	UpDataDef temp_Power_BasetoUpData =
	{ 0 };
	UpDataDef BasetoUpData =
	{ 0 };
	u8 SensorCarbin = 0;

	vTaskSuspend(BasetoUpTaskHandle);
	CaptureUpData();
	osDelay(100);
	BasetoUpData = CaptureUpData();
	osDelay(50);
	Power_BasetoUpData = CaptureUpData();
	osDelay(50);
	/* Infinite loop */
	for (;;)
	{
		if (SensorCarbin == 1) //发送电源仓数据
		{
			//单独接收下位仓回传的数据
			if (xSemaphoreTake(BasetoUpTransFinishHandleHandle,
					portTICK_PERIOD_MS) == pdTRUE) //原来是0
			{
//				if (xSemaphoreTake(BasetoUpDataRWFlagHandle,
//						portTICK_PERIOD_MS) == pdTRUE)
//				{
				Power_BasetoUpData = CaptureUpData();
//					temp_Power_BasetoUpData = Power_BasetoUpData;
				//上传数据
//					if (temp_Power_BasetoUpData.CabinNum == 1)
//					{
				if (Power_BasetoUpData.CabinNum == 0b0001)
				{
					if (xSemaphoreTake(UptoBaseDataRWFlagHandle,
							portTICK_PERIOD_MS) == pdTRUE)
					{
//							SendUpData(temp_Power_BasetoUpData);
						SendUpData(Power_BasetoUpData);
						xSemaphoreGive(UptoBaseDataRWFlagHandle);
					}
				}
//					xSemaphoreGive(BasetoUpDataRWFlagHandle);
//				}
			}
			SensorCarbin = 0;
		}
		else if (SensorCarbin == 0)	//发�?�主控仓数据
		{
			//汇�?�本仓传感器数据
			//仓位数据
			BasetoUpData.CabinNum = 0;
			//漏水�??????�??????
			if (HAL_GPIO_ReadPin(GPIOE,
			GPIO_PIN_6) == GPIO_PIN_SET)
			{
				BasetoUpData.WaterDetect = 1 << 1;
			}
			else
			{
				BasetoUpData.WaterDetect = 0 << 1;
			}
			//九轴数据
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
			//温湿度大气压数据
			if (xSemaphoreTake(GY39SensorDataRWFlagHandle,
					portTICK_PERIOD_MS) == pdTRUE)
			{
				BasetoUpData.CabinBaro = GY39SensorData.Baro;
				BasetoUpData.CabinHum = GY39SensorData.Hum;
				BasetoUpData.CabinTemperature = GY39SensorData.Temperature;
				xSemaphoreGive(GY39SensorDataRWFlagHandle);
			}
			//水深水温数据
			if (xSemaphoreTake(DeepSensorDataRWFlagHandle,
					portTICK_PERIOD_MS) == pdTRUE)
			{
				BasetoUpData.WaterDepth = DeepSensorData.WaterDepth;
				BasetoUpData.WaterTemperature = DeepSensorData.WaterTemperature;
				xSemaphoreGive(DeepSensorDataRWFlagHandle);
			}
			//声呐数据
			if (xSemaphoreTake(P30SensorDataRWFlagHandle,
					portTICK_PERIOD_MS) == pdTRUE)
			{
				BasetoUpData.DepthToBottom = P30SensorData.DepthToBottom;
				BasetoUpData.Confidence = P30SensorData.Confidence;
				xSemaphoreGive(P30SensorDataRWFlagHandle);
			}
			//上传数据
			if (xSemaphoreTake(UptoBaseDataRWFlagHandle,
					portTICK_PERIOD_MS) == pdTRUE)
			{
				SendUpData(BasetoUpData);
				xSemaphoreGive(UptoBaseDataRWFlagHandle);
			}
			SensorCarbin = 1;
		}
		HAL_IWDG_Refresh(&hiwdg1);
	}
	/* USER CODE END BtUF */
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
