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
#include <stdio.h>
#include <stdlib.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"

#include "OS/inc/OS.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum{
	PHILO_THINKING,
	PHILO_HUNGRY,
	PHILO_EATING,
}philo_state_e;
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

static philo_state_e philosopher_state[4];
static os_handle_t philosopher[4];
static os_handle_t forks[4];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
void* philosopher_fn(int argc, char* argv[]){

	int i = (int) argc;
	while(1)
	{
		/* Thinking
		-----------------------------------------------*/
		philosopher_state[i] = PHILO_THINKING;
		uint32_t thinkTime_s = (uint32_t)((rand() % 4000) + 1000); // 1 to 5s;
		os_task_sleep(thinkTime_s);

		/* Hungry
		-----------------------------------------------*/
		philosopher_state[i] = PHILO_HUNGRY;
		os_obj_multiple_WaitAll(NULL, OS_WAIT_FOREVER, 2, forks[i], forks[i == 0 ? COUNTOF(forks) - 1 : i - 1]);

		/* Eating
		-----------------------------------------------*/
		philosopher_state[i] = PHILO_EATING;
		uint32_t eatTime_s = (uint32_t)((rand() % 2000) + 1000); // 1 to 3s;
		os_task_sleep(eatTime_s);

		os_mutex_release(forks[i]);
		os_mutex_release(forks[i == 0 ? COUNTOF(forks) - 1 : i - 1]);
	}

	return NULL;
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
	srand(0);
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

	for(int i = 0; i < COUNTOF(philosopher); i++){
		char name[50];
		snprintf(name, sizeof(name), "Philo %d", i);
		ASSERT(os_task_create(&philosopher[i], name, philosopher_fn, OS_TASK_MODE_DELETE, (int8_t)(10) , OS_DEFAULT_STACK_SIZE, (void*) i, NULL) == OS_ERR_OK);

		snprintf(name, sizeof(name), "Mux %d", i);
		ASSERT(os_mutex_create(&forks[i], name) == OS_ERR_OK);
	}

	os_scheduler_start();

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	uint32_t timeout_ms[4];

	struct{
		GPIO_TypeDef* GPIOx;
		uint16_t GPIO_Pin;
	} leds[4] = {
			[0] = { .GPIOx = LED_GREEN_GPIO_Port, 	.GPIO_Pin = LED_GREEN_Pin },
			[1] = { .GPIOx = LED_ORANGE_GPIO_Port, 	.GPIO_Pin = LED_ORANGE_Pin },
			[2] = { .GPIOx = LED_RED_GPIO_Port, 	.GPIO_Pin = LED_RED_Pin },
			[3] = { .GPIOx = LED_BLUE_GPIO_Port, 	.GPIO_Pin = LED_BLUE_Pin }
	};

	while(1)
	{
		/* Control led for each philosopher
		-----------------------------------------------*/
		for(int i = 0; i < COUNTOF(philosopher); i++){
			switch(philosopher_state[i]){
				case PHILO_THINKING : {
					timeout_ms[i] = os_getMsTick();
					HAL_GPIO_WritePin(leds[i].GPIOx, leds[i].GPIO_Pin, 0);
					break;
				}

				case PHILO_HUNGRY : {
					uint32_t now = os_getMsTick();
					if(now - timeout_ms[i] > 100){
						timeout_ms[i] = now;
						HAL_GPIO_TogglePin(leds[i].GPIOx, leds[i].GPIO_Pin);
					}
					break;
				}

				case PHILO_EATING : {
					HAL_GPIO_WritePin(leds[i].GPIOx, leds[i].GPIO_Pin, 1);
					break;
				}

				default : break;
			}
		}

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
