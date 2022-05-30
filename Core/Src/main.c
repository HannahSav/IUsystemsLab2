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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "stdbool.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"

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
int now_time_out = 1500;
bool now_mode = 1;
char buff[25];
char str_buff[25];
size_t iter = 0;
size_t i = 0;
char num_char[12];
size_t num_len = 4;


bool Compare(char a[], char b[], int i){
	for(int j = 0;  j < i; j++){
		if(a[j] != b[j])
			return false;
	}
	return true;
}

void ReadCommand(char consist[], int len){

		if(iter == 11){
			if(Compare(str_buff, "set mode 1.", 11)){
				HAL_UART_Transmit(&huart6, (uint8_t*)"\n\rmode with stops\n\r>>", 21, 5);

				oled_SetCursor(0, 28);
				oled_WriteString("with stops   ", Font_7x10, Black);
				oled_UpdateScreen();

				now_mode = 1;
			}else if(Compare(str_buff, "set mode 2.", 11)){
				HAL_UART_Transmit(&huart6, (uint8_t*)"\n\rmode without stops\n\r>>", 25, 5);

				oled_SetCursor(0, 28);
				oled_WriteString("without stops", Font_7x10, Black);
				oled_UpdateScreen();

				now_mode = 0;
			}else{
				HAL_UART_Transmit(&huart6, (uint8_t*)error, 40, 5);
			}
		}else if(iter >= 14 && iter < 25){
			if(Compare(str_buff, "set timeout", 11)){
				int j;
				int number = 0;
				for(j = 12; j < iter - 1; j++){
					number = number * 10;
					if((str_buff[j] - '0') > 9){
						HAL_UART_Transmit(&huart6, (uint8_t*)"\n\rneed a number in the end. try again\n\r>>", 41, 5);
						break;
					}
					number = number + (str_buff[j] - '0');
					now_time_out = number;
					sprintf(num_char, "%d", now_time_out);
				}
				if(j >= iter - 1){
					//num_char = try_num_char;
					num_len = iter - 13;
					now_time_out = number;
					sprintf(num_char, "%d", now_time_out);
					HAL_UART_Transmit(&huart6, (uint8_t*)"\n\rTimeout ", 10, 25);
					HAL_UART_Transmit(&huart6, (uint8_t*)num_char, num_len, 25);
					HAL_UART_Transmit(&huart6, (uint8_t*)"\n\r>>", 4, 25);

					oled_SetCursor(56, 38);
					oled_WriteString(num_char, Font_7x10, Black);
					oled_SetCursor(56 + j - 12, 38);

					for(int str_i = j-12; str_i < 8; str_i++){
						oled_SetCursor(56 + 7*str_i, 38);
						oled_WriteString(" ", Font_7x10, Black);
					}
					oled_UpdateScreen();
				}
			}else
				HAL_UART_Transmit(&huart6, (uint8_t*)error, 40, 5);
		}else if(iter == 2){
			if(Compare(str_buff, "?.", 2)){
				HAL_UART_Transmit(&huart6, (uint8_t*)"\n\rINFO:", 7, 5);
				HAL_UART_Transmit(&huart6, (uint8_t*)"\n\r  -", 5, 5);
				HAL_UART_Transmit(&huart6, (uint8_t*)consist, len, 5);
				if(now_mode == 1)
					HAL_UART_Transmit(&huart6, (uint8_t*)"  -mode 1\n\r", 11, 5);
				else
					HAL_UART_Transmit(&huart6, (uint8_t*)"  -mode 2\n\r", 11, 5);
				HAL_UART_Transmit(&huart6, (uint8_t*)"  -timeout ", 11, 5);
				HAL_UART_Transmit(&huart6, (uint8_t*)num_char, num_len, 5);
				HAL_UART_Transmit(&huart6, (uint8_t*)" \n\r>>", 5, 5);
			};
		}else{
			HAL_UART_Transmit(&huart6, (uint8_t*)error, 40, 5);


	}
	/*TODO:
	 */
}

void CheckRead(char consist[], int len, int now_i_light){
	 char symb = '7';
	 if(HAL_UART_Receive(&huart6, &symb, 1, 1) == HAL_OK){
		 HAL_UART_Transmit(&huart6, &symb, 1, 1);
		 buff[iter] = symb;
		 iter++;
	 }
	 if(symb == '.' || iter >= 25){
		 if(iter >= 25)
			 HAL_UART_Transmit(&huart6, (uint8_t*)error, 40, 5);
		 else
		 {
			 strcpy(str_buff,buff);
			 ReadCommand(consist, len);
		 }
		 iter = 0;
	 }
}

void Phore(){
	int i, j;
	  while (1)
	  {
	   /*горит зеленый*/

	      oled_SetCursor(0, 18);
	      oled_WriteString("green    ", Font_7x10, Black);
	      oled_UpdateScreen();

	      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);

	      for(i = 0; i < now_time_out/2; i++){
	    	  HAL_Delay(1);
	    	  CheckRead("green\n\r", 7, i);
	      }
	   /*мигает зеленый*/

	      oled_SetCursor(0,18);
	      oled_WriteString("blinking", Font_7x10, Black);
	      oled_UpdateScreen();

	      for(i = 0; i < 7; ++i)
	      {
	    	  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
	    	  for(j = 0; j < now_time_out/10; j++){
	    		  HAL_Delay(1);//300
	    		  CheckRead("blinking green\n\r", 17, i);
	    	  }
	      }
	    /*горит оранжевый*/

	      oled_SetCursor(0, 18);
	      oled_WriteString("yellow  ", Font_7x10, Black);
	      oled_UpdateScreen();

	      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
	      for(i = 0; i < now_time_out/5; i++){
	    	  HAL_Delay(1); //600
	    	  CheckRead("yellow\n\r", 8, i);
	      }
	      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
	    /*горит красный*/

	      oled_SetCursor(0, 18);
	      oled_WriteString("red     ", Font_7x10, Black);
	      oled_UpdateScreen();

	      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
	      i = now_time_out*2;
	    /*ждем до нажатия кнопки или до окончания времени красного*/


	      while(i > 0 && (now_mode == 0 || HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) != GPIO_PIN_RESET)){
	       HAL_Delay(1);
	       CheckRead("red\n\r", 5, i);
	       i--;
	      }

	    /*если красный горел недостаточно долго, то пусть еще погорит*/
	      char char_j[8];
	      int part_j;
	      if(now_mode == 1 && i > now_time_out){
		      oled_SetCursor(0, 48);
		      oled_WriteString("Wait ", Font_7x10, Black);
		      oled_UpdateScreen();
	    	  for(j = i - now_time_out; j > 0 ; j--){
	    		  if(j%100 == 0){
	    		  part_j = j/100;
	    		  oled_SetCursor(35, 48);
	    		  sprintf(char_j, "%d", part_j);
			      oled_WriteString(char_j, Font_7x10, Black);
			      if(part_j < 1000){
			    	  oled_SetCursor(35 + 21, 48);
			    	  oled_WriteString(" ", Font_7x10, Black);
			      }
			      if(part_j < 100){
			      	  oled_SetCursor(35 + 14, 48);
			      	  oled_WriteString(" ", Font_7x10, Black);
			      }
			      if(part_j < 10){
			          oled_SetCursor(35 + 7, 48);
			      	  oled_WriteString(" ", Font_7x10, Black);
			      }
			      oled_UpdateScreen();
	    		  }
	    		  HAL_Delay(1);
	    		  CheckRead("red\n\r", 5, i);
	    	  }
	      }
	      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
	    /*горит оранжевый*/

	      oled_SetCursor(0, 48);
	      oled_WriteString("           ", Font_7x10, Black);

	      oled_SetCursor(0, 18);
	      oled_WriteString("yellow  ", Font_7x10, Black);
	      oled_UpdateScreen();

	      HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
	      for(i = 0; i < now_time_out/5; i++){
	    	  HAL_Delay(1);
	    	  CheckRead("yellow\n\r", 8, i);
	      }
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
  MX_I2C1_Init();
  MX_USART6_UART_Init();

  oled_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  oled_Fill(White);
  oled_SetCursor(0,0);
  oled_WriteString("INFO", Font_11x18, Black);
  oled_SetCursor(0,18);
  oled_WriteString("green", Font_7x10, Black);
  oled_SetCursor(0,28);
  oled_WriteString("with stops", Font_7x10, Black);
  oled_SetCursor(0,38);
  oled_WriteString("timeout:", Font_7x10, Black);
  oled_SetCursor(56,38);
  oled_WriteString("1500", Font_7x10, Black);
  oled_UpdateScreen();

  /* USER CODE BEGIN WHILE */
  sprintf(num_char, "%d", now_time_out);
  HAL_UART_Transmit(&huart6, (uint8_t*)ask, 42, 25);
  Phore();

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
