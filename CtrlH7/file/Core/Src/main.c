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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
u8 SensorCabin = 0; 		//上传传感器仓位标志位
u8 UpSideFinish = 0; 		//上位机接收完毕标志位
u8 DownSideFinish = 0; 		//下位机接收完毕标志位

//传感器读取完成标志位
u8 DeepFinish = 0;			//水深传感器读取完成标志位
u8 GY39Finish = 0;			//温湿度大气压传感器读取完成标志位
u8 WT931Finish = 0;			//九轴读取完成标志位

//传感器缓存读写标志位
volatile u8 DeepIO = 0;		//水深传感器缓存读写标志位
volatile u8 GY39IO = 0;		//温湿度大气压传感器缓存读写标志位
volatile u8 WT931IO = 0;	//九轴缓存读写标志位

volatile u8 UpIO = 0; 		//上位发送指令读写标志位
volatile u8 DownIO = 0;		//下位回传数据读写标志位

u8 DownEn = 0; 				//允许下位机数据回传标志位

u8 UpCache[UART1RXLen]; 	//上位发送数据缓存
u8 DownCache[UART8RXLen]; 	//下位回传数据缓存

u8 DIPFlag = 0; 			//定向定深初始标志位
u16 DIPStartNum = 0; 		//定向定深初始值

u8 WaterDetect = 0;			//漏水检测标志位

//传感器缓存区
u16 AccelerationCache[3] =
{ 0 }; //加速度值缓存区
u16 RotSpeedCache[3] =
{ 0 }; //角速度值缓存区
u16 EulerAngleCache[3] =
{ 0 }; //角度值缓存区
u16 MagnetisCache[3] =
{ 0 }; //磁偏角值缓存区
u16 TemCache = 0; //温度值缓存区
u16 BaroCache[2] =
{ 0 }; //大气压值缓存区
u16 HumCache = 0; //湿度值缓存区
u16 DepthCache = 0; //水深值缓存区
u16 WaterTempertureCache = 0; //水温值缓存区

u8 WT931Len = 0; //传感器数据长度

//串口1DMA缓存
u8 UART1RXCache[UART1RXLen]; //上位机（MT7621）指令接收缓存区
u8 UART1TXCache_P[UART1TXLen]; //上位机（MT7621）传感器数据发缓存区，数据来自下位仓STM32
u8 UART1TXCache_C[UART1TXLen]; //上位机（MT7621）传感器数据发缓存区，数据来自本仓传感器
//串口2DMA缓存
u8 UART2RXCache[UART2RXLen]; //温湿度大气压传感器串口数据接收缓存区
u8 UART2TXCache[UART2TXLen]; //温湿度大气压传感器串口数据发送缓存区
//串口3DMA缓存
u8 UART3RXCache[UART3RXLen]; //九轴传感器串口数据接收缓存区
//串口4DMA缓存
u8 UART4RXCache[UART4RXLen]; //深度传感器（解算板）数据接收缓存区
u8 UART4TXCache[UART4TXLen]; //深度传感器（解算板）数据发送缓存区
//串口8DMA缓存
u8 UART8RXCache[UART8RXLen]; //下位仓通信串口数据接收缓存
u8 UART8TXCache[UART8TXLen]; //下位仓通信串口数据发送缓存

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

IWDG_HandleTypeDef hiwdg1;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart8;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart4_tx;
DMA_HandleTypeDef hdma_uart8_rx;
DMA_HandleTypeDef hdma_uart8_tx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart3_rx;

osThreadId UpTaskHandle;
osThreadId DownTaskHandle;
osThreadId InitialTaskHandle;
osThreadId SensorTaskHandle;
osThreadId EmptyTaskHandle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_IWDG1_Init(void);
static void MX_UART4_Init(void);
static void MX_UART8_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
void UpTaskF(void const * argument);
void DownTaskF(void const * argument);
void InitialTaskF(void const * argument);
void SensorTaskF(void const * argument);
void EmptyTaskF(void const * argument);

/* USER CODE BEGIN PFP */

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
  MX_IWDG1_Init();
  MX_UART4_Init();
  MX_UART8_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

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
  /* definition and creation of UpTask */
  osThreadDef(UpTask, UpTaskF, osPriorityNormal, 0, 128);
  UpTaskHandle = osThreadCreate(osThread(UpTask), NULL);

  /* definition and creation of DownTask */
  osThreadDef(DownTask, DownTaskF, osPriorityNormal, 0, 128);
  DownTaskHandle = osThreadCreate(osThread(DownTask), NULL);

  /* definition and creation of InitialTask */
  osThreadDef(InitialTask, InitialTaskF, osPriorityRealtime, 0, 128);
  InitialTaskHandle = osThreadCreate(osThread(InitialTask), NULL);

  /* definition and creation of SensorTask */
  osThreadDef(SensorTask, SensorTaskF, osPriorityNormal, 0, 128);
  SensorTaskHandle = osThreadCreate(osThread(SensorTask), NULL);

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
  RCC_OscInitStruct.PLL.PLLQ = 6;
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
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART8|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_UART4|RCC_PERIPHCLK_USART2
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
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
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
  if (HAL_UARTEx_SetTxFifoThreshold(&huart8, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart8, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
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
  huart1.Init.BaudRate = 921600;
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
  huart2.Init.BaudRate = 115200;
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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);

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

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_UpTaskF */
/**
 * @brief  Function implementing the UpTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_UpTaskF */
void UpTaskF(void const * argument)
{
  /* USER CODE BEGIN 5 */
	/* Infinite loop */
	for (;;)
	{
		while (!UpSideFinish) //等待上位机下传指令解析完毕
		{
			osDelay(1);
		}
		UpSideFinish = 0; //下传指令解析完毕
		if (IdTest(UART1RXCache, 0)) //校验从上位机发来的数据
		{
			while (UpIO) //等待上位机数据读写完毕
			{
				osDelay(1);
			}
			UpIO = 1; //正在读写上位机数据
			for (u8 i = 0; i < UART1RXLen; ++i)
			{
				UpCache[i] = UART1RXCache[i]; //存储上位机发来的数据
			}
			UpIO = 0; //上位机数据读写完成
		}
		__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
		HAL_UART_Receive_DMA(&huart1, UART1RXCache, UART1RXLen); //接收上位机发来的数据

		DownEn = 1; //允许下位仓通讯
		if (SensorCabin)
		{
			while (DownIO) //等待下位仓串口读写完成
			{
				osDelay(1);
			}
			DownIO = 1; //下位仓串口正在读�?
			for (u8 i = 0; i < UART1TXLen; ++i)
			{
				UART1TXCache_P[i] = DownCache[i]; //接收下位机回传的数据
			}
			DownIO = 0; //下位仓串口读写完�?
			HAL_UART_Transmit_DMA(&huart1, UART1TXCache_P, UART1TXLen); //转发下位机数据给上位�?
		}
		else
		{
			/* 上传数据处理 */
			SendBack(WaterDetect, TemCache, BaroCache, HumCache,
					AccelerationCache, RotSpeedCache, EulerAngleCache,
					MagnetisCache, WaterTempertureCache, DepthCache);
			HAL_UART_Transmit_DMA(&huart1, UART1TXCache_C, UART1TXLen);
		}
		SensorCabin = ~SensorCabin; //每间隔一次任务执行一次上传数据处�?
		HAL_IWDG_Refresh(&hiwdg1);
	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_DownTaskF */
/**
 * @brief Function implementing the DownTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_DownTaskF */
void DownTaskF(void const * argument)
{
  /* USER CODE BEGIN DownTaskF */
	while (!DownEn) //等待下位仓激活指�?
	{
		osDelay(1); //否则不会启动该任�?
	}
	/* Infinite loop */
	for (;;)
	{
		/* �?启PID定向定深模式 */
		if (DownMoveDetect() == 2)
		{
			if (DIPFlag == 0)
			{
				DIPFlag = 1;
				DIPStartNum = EulerAngleCache[2];
			}
			SpecialMovePID(2, DIPStartNum, EulerAngleCache[2]);
		}
		else if (DownMoveDetect() == 4)
		{
			if (DIPFlag == 0)
			{
				DIPFlag = 1;
				DIPStartNum = DepthCache;
			}
			SpecialMovePID(4, DIPStartNum, DepthCache);
		}
		else
		{
			DIPFlag = 0; //不开启定向定�?
		}

		/* 下位仓数据收发就是个傻任务�?�传就完事了 */
		/* 有一说一这还不如直接把上位仓的串口直接连到下位仓来得方便 */
		while (UpIO) //等待上位仓数据读写完�?
		{
			osDelay(1);
		}
		UpIO = 1; //�?始上位机数据读写
		for (u8 i = 0; i < UART8TXLen; ++i) //向下位仓发�?�从上位机发来的指令数据
		{
			UART8TXCache[i] = UpCache[i];
		}
		UpIO = 0; //上位仓数据读写完�?
		HAL_UART_Transmit_DMA(&huart8, UART8TXCache, UART8TXLen); //�?始发送数�?

		while (!DownSideFinish) //等待下位仓�?�信处理完毕
		{
			osDelay(1);
		}
		DownSideFinish = 0; //下位仓�?�信处理完毕
		if (IdTest(UART8RXCache, 1)) //校验下位仓发来的数据
		{
			while (DownIO) //等待下位仓数据读写完�?
			{
				osDelay(1);
			}
			DownIO = 1; //�?始下位仓数据读写
			for (u8 i = 0; i < UART8RXLen; ++i) //收取下位仓发来的数据到缓存区
			{
				DownCache[i] = UART8RXCache[i];
			}
			DownIO = 0; //下位仓数据读写完�?
		}
		__HAL_UART_ENABLE_IT(&huart8, UART_IT_IDLE);
		HAL_UART_Receive_DMA(&huart8, UART8RXCache, UART8RXLen); //收取下位仓发来的串口数据
		HAL_IWDG_Refresh(&hiwdg1);
	}
  /* USER CODE END DownTaskF */
}

/* USER CODE BEGIN Header_InitialTaskF */
/**
 * @brief Function implementing the InitialTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_InitialTaskF */
void InitialTaskF(void const * argument)
{
  /* USER CODE BEGIN InitialTaskF */
	//挂起任务防止初始化进程被打断
	vTaskSuspend(SensorTaskHandle);
	vTaskSuspend(UpTaskHandle);
	vTaskSuspend(DownTaskHandle);
	//初始化下位机
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
	//初始化回传指�?
	SendBackPoint(UART1TXCache_C, &DeepIO, &WT931IO, &GY39IO);
	//初始化下传数据读取指�?
	DownDetectPoint(UpCache);
	//先开启上传任务和传感器处理任�?
	vTaskResume(UpTaskHandle);
	vTaskResume(SensorTaskHandle);
	//然后喂狗
	HAL_IWDG_Refresh(&hiwdg1);
	//挂起初始化任务不再执�?
	vTaskSuspend(InitialTaskHandle);
	/* Infinite loop */
	for (;;)
	{
		HAL_IWDG_Refresh(&hiwdg1);
		osDelay(1);
	}
  /* USER CODE END InitialTaskF */
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
	//初始化九轴对应DMA
	WT931Point(UART3RXCache);
	//初始化温湿度气压传感器对应DMA，并发�?�初始化数据
	GY39InitStr(UART2TXCache);
	HAL_UART_Transmit_DMA(&huart2, UART2TXCache, UART2TXLen);
	GY39Point(UART2RXCache);
	//初始化深度传感器对应DMA
	DeepPoint(UART4TXCache);
	/* Infinite loop */
	for (;;)
	{
		WaterDetect = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6); //�?测漏�?

		if (WT931Finish) //等待九轴传感器数据解析完�?
		{
			while (WT931IO) //等待九轴传感器数据读写完�?
			{
				osDelay(1);
			}
			WT931IO = 1; //正在读写传感器数�?
			if (WT931Len == 44) //�?查读取到的传感器数据长度
			{
				WT931Take(AccelerationCache, RotSpeedCache, EulerAngleCache,
						MagnetisCache); //解析九轴数据
			}
			WT931Finish = 0; //传感器数据解析完�?
			HAL_UART_Receive_DMA(&huart3, UART3RXCache, UART2RXLen);
			__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
			WT931IO = 0; //传感器数据读写完�?
		}
		if (GY39Finish) //等待温湿度大气压传感器数据解析完�?
		{
			while (GY39IO) //等待传感器数据读写完�?
			{
				osDelay(1);
			}
			GY39IO = 1; //正在读写传感器数�?
			GY39Take(&TemCache, BaroCache, &HumCache); //解析温湿度大气压传感器数�?
			GY39Finish = 0; //传感器数据解析完�?
			HAL_UART_Receive_DMA(&huart2, UART2RXCache, UART2RXLen);
			__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
			GY39IO = 0; //传感器数据读写完�?
		}
		if (DeepFinish) //等待深度传感器数据解析完�?
		{
			while (DeepIO) //等待传感器数据读写完�?
			{
				osDelay(1);
			}
			DeepIO = 1; //正在读写传感器数�?
			DeepTake(&DepthCache, &WaterTempertureCache); //解析深度传感器数�?
			DeepFinish = 0; //传感器数据解析完�?
			HAL_UART_Receive_DMA(&huart4, UART4RXCache, UART4RXLen);
			__HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);
			DeepIO = 0; //传感器数据读写完�?
		}
		HAL_IWDG_Refresh(&hiwdg1);
	}
  /* USER CODE END SensorTaskF */
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
		HAL_IWDG_Refresh(&hiwdg1); //空闲时喂�?
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
