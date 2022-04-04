/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"
#include "stdbool.h"

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

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
char error[] = "\n\rI don't understand you, try again\n\r>>";
char ask[] = "\n\rWhat do you want? Print . in the end\n\r>>";
bool def_mode = 1;
int def_time_out = 3000;
int now_time_out = 3000;
bool now_mode = 1;

bool Compare(char a[], char b[], int i){
	for(int j = 0;  j < i; j++){
		if(a[j] != b[j])
			return false;
	}
	return true;
}

void ReadCommand(){
	char buff[20];
	char symb = '7';
	char str_buff[25];
	size_t i = 0;
	HAL_UART_Transmit(&huart6, (uint8_t*)ask, 42, 25);
	while(symb != '.' && i < 20){
	    	  //HAL_UART_Transmit(&huart6, (uint8_t*)txt1, 3, 20);
	  if(HAL_UART_Receive(&huart6, &symb, 1, 1000) == HAL_OK){
		  HAL_UART_Transmit(&huart6, &symb, 1, 20);
		  buff[i] = symb;
		  i++;
	  }
	}
	if(i >= 25){
		HAL_UART_Transmit(&huart6, (uint8_t*)error, 50, 25);
	}else{
		strcpy(str_buff,buff);

		if(i == 11){
			if(Compare(str_buff, "set mode 1.", 11)){
				HAL_UART_Transmit(&huart6, (uint8_t*)"mur", 10, 25);
				HAL_Delay(20);
				now_mode = 1;
			}else if(Compare(str_buff, "set mode 2.", 11)){
				HAL_UART_Transmit(&huart6, (uint8_t*)"mur22", 10, 25);
				HAL_Delay(20);
				now_mode = 0;
			}
		}else if(i >= 14 && i < 20){
			if(Compare(str_buff, "set timeout", 11)){
				for( int j = i; j > 11; j--){
					if(str_buff[j] < '0' || str_buff[j]>'9'){
						HAL_UART_Transmit(&huart6, (uint8_t*)"HUY", 3, 25);
						HAL_Delay(20);
						break;
					}
				}
				HAL_UART_Transmit(&huart6, (uint8_t*)"Timeout eee", 111, 25);
				HAL_Delay(20);
				now_time_out = 1000;//Function for it
			}
		}else{
			HAL_UART_Transmit(&huart6, (uint8_t*)"HUY", 3, 25);
			HAL_Delay(2000);
		}
		HAL_UART_Transmit(&huart6, (uint8_t*)str_buff, 15, 25);
		HAL_Delay(20);

	}
	/*TODO: parser
	 * ?
	 * parralel
	 * parse timeout
	 */
}

void Phore(bool mode, int time_out){
	int i;
	  while (1)
	  {
	   /*горит зеленый*/
	      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
	      HAL_Delay(time_out/2); //1500
	   /*мигает зеленый*/
	      for(i = 0; i < 7; ++i)
	      {
	      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
	           HAL_Delay(time_out/10);//300
	      }
	    /*горит оранжевый*/
	      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
	      HAL_Delay(time_out/5); //600
	      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
	    /*горит красный*/
	      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
	      i = time_out*2;
	    /*ждем до нажатия кнопки или до окончания времени красного*/
	      while(i > 0 && (mode == 0 || HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) != GPIO_PIN_RESET)){
	       HAL_Delay(1);
	       i--;
	      }
	    /*если красный горел недостаточно долго, то пусть еще погорит*/
	      if(mode == 1 && i > time_out)//3000
	    	  HAL_Delay(i-time_out);
	      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
	    /*горит оранжевый*/
	      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
	      HAL_Delay(time_out/5);
	      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
	  }

}

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
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /* USER CODE BEGIN WHILE */
  Phore(1, 1500);
  /*while(1){
	  ReadCommand();
	  HAL_Delay(1000);
  }*/
  uint8_t str[100];

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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
