/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <stdlib.h>

#include "kex.h"
#include "ntt.h"
#include "randombytes.h" 


#ifdef __GNUC__
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
  uint8_t temp[1]={ch};
  HAL_UART_Transmit(&huart1,temp,1,0xffff);
  return ch;
}
int _write(int file, char *ptr, int len)
{
  int DataIdx;
  for (DataIdx=0;DataIdx<len;DataIdx++)
  {
    __io_putchar(*ptr++);
  }
  return len;
}
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* Private user code ---------------------------------------------------------*/
#define NTESTS 100




/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  int i;

  unsigned int
      t[NTESTS];  // = (unsigned int*) malloc(sizeof(unsigned int)*NTESTS);
  unsigned char sk_a[KYBER_SECRETKEYBYTES];
  unsigned char key_a[32], key_b[32];
  unsigned char *senda = (unsigned char *)malloc(KYBER_PUBLICKEYBYTES);
  unsigned char *sendb = (unsigned char *)malloc(KYBER_CIPHERTEXTBYTES);

	uint8_t len;	  //串口数据长度
  uint8_t times;  //测试时间相关
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  //MX_TIM1_Init();
  MX_USART1_UART_Init();
  /* Infinite loop */
  while (1)
  {

     if(USART_RX_STA&0x8000)
		{		   
			len=USART_RX_STA&0x3fff;//数据长度
			printf("\r\nYou are sending a message as:%d\r\n",len);               
      HAL_UART_Transmit(&huart1,(uint8_t*)USART_RX_BUF,len,1000);    
      while(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TC)!=SET);
      printf("\r\n");  
      USART_RX_STA=0; 
	  }else{
			times++;
			if(times%1000==0)
			{
        crypto_kem_keypair(senda, sk_a);
        printf("\n");
        printf("senda: ");
        for (i = 0; i < 32; i++) printf("%02x", senda[i]);
        printf("\n");
        printf("sk_a: ");        
        for (i = 0; i < 32; i++) printf("%02x", sk_a[i]);

        crypto_kem_enc(sendb, key_b, senda);
        printf("\n");
        printf("key_b: ");
        for (i = 0; i < 32; i++) printf("%02x", key_b[i]);

        crypto_kem_dec(key_a, sendb, sk_a);
        printf("\n");
        printf("key_a: ");
        for (i = 0; i < 32; i++) printf("%02x", key_a[i]);
        printf("\n");


        free(senda);
        free(sendb);
        /*
        ntt(a);
        crypto_kem_keypair(senda, sk_a);
        crypto_kem_enc(sendb, key_b, senda);
        crypto_kem_dec(key_a, sendb, sk_a);
        printf("key_b: ");
        for (i = 0; i < 32; i++) printf("%02x", key_b[i]);
        printf("\n");
        printf("key_a: ");
        for (i = 0; i < 32; i++) printf("%02x", key_a[i]);
        printf("\n");

        free(a);
        free(senda);
        free(sendb);*/


        HAL_GPIO_TogglePin(GPIOE, LED0_Pin);
			}
    } 
	}


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
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 288;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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
