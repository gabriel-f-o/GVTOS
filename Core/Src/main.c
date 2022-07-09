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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OS/OS.h"
#include "math.h"
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

/* USER CODE BEGIN PV */
#define ASSERT(x)	if( !(x) ) Error_Handler();
#define COUNTOF(x)	(int) ((sizeof(x) / sizeof(*x)))

os_handle_t tasks[5];
os_handle_t mutex[5];
os_hMsgQ_t queue;

char * task_name[] = { "T0", "T1", "T2", "T3", "T4" };

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

}
/* USER CODE END PFP */
uint32_t jebatest;
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void* t0(void* arg){
	UNUSED_ARG(arg);

	ASSERT(os_obj_single_wait(mutex[0], OS_WAIT_FOREVER, NULL) != NULL);

	ASSERT(os_msgQ_wait(queue, OS_WAIT_FOREVER, NULL) == &jebatest);

	while(1){

	}

	os_task_end();
	return NULL;
}

void* t1(void* arg){
	UNUSED_ARG(arg);

	os_task_sleep(500);

	ASSERT(os_msgQ_wait(queue, OS_WAIT_FOREVER, NULL) == &jebatest);

	while(1){

	}

	os_task_end();
	return NULL;
}

void* t2(void* arg){
	UNUSED_ARG(arg);

	os_task_sleep(1000);

	os_msgQ_push(queue, &jebatest);
	ASSERT(os_obj_single_wait(mutex[0], 500, NULL) != NULL);

	os_task_end();
	return NULL;
}

void* t3(void* arg){
	UNUSED_ARG(arg);

	os_task_sleep(10000);

	os_msgQ_wait(queue, OS_WAIT_FOREVER, NULL);
	os_msgQ_push(queue, &jebatest);
	os_msgQ_wait(queue, OS_WAIT_FOREVER, NULL);

	while(1){

		os_task_sleep(1000);

		os_msgQ_push(queue, &jebatest);
		os_msgQ_wait(queue, OS_WAIT_FOREVER, NULL);

	}

	os_task_end();
	return NULL;
}

void* t4(void* arg){
	UNUSED_ARG(arg);

	os_task_end();
	return NULL;
}

void* (*tsks[])(void* arg) = {
		t0,
		t1,
		t2,
		t3,
		t4,
};

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

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM13_Init();
	/* USER CODE BEGIN 2 */

	ASSERT(os_init("main", 9, OS_DEFAULT_STACK_SIZE, "idle", OS_DEFAULT_STACK_SIZE) == OS_ERR_OK);

	ASSERT(os_task_create(&tasks[0], task_name[0], tsks[0], (int8_t)(10), OS_DEFAULT_STACK_SIZE, (void*) NULL) == OS_ERR_OK);
	ASSERT(os_task_create(&tasks[1], task_name[1], tsks[1], (int8_t)(11), OS_DEFAULT_STACK_SIZE, (void*) NULL) == OS_ERR_OK);
	ASSERT(os_task_create(&tasks[2], task_name[2], tsks[2], (int8_t)(12), OS_DEFAULT_STACK_SIZE, (void*) NULL) == OS_ERR_OK);
	ASSERT(os_task_create(&tasks[3], task_name[3], tsks[3], (int8_t)(13), OS_DEFAULT_STACK_SIZE, (void*) NULL) == OS_ERR_OK);
	ASSERT(os_msgQ_create(&queue, OS_MSGQ_MODE_FIFO, "Jeba") == OS_ERR_OK);
	ASSERT(os_mutex_create(&mutex[0], "Jeba") == OS_ERR_OK);

	os_scheduler_start();

	ASSERT(os_obj_multiple_WaitAll(NULL, OS_WAIT_FOREVER, 2, tasks[0], tasks[1]) != NULL);

	for(int i = 0; i < COUNTOF(tasks); i++){
		os_task_delete(tasks[i]);
	}

	for(int i = 0; i < COUNTOF(mutex); i++){
		os_mutex_delete(mutex[i]);
	}

	os_msgQ_delete(queue);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while(1){

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

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 84;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
int errCount = 0;
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	errCount++;
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
