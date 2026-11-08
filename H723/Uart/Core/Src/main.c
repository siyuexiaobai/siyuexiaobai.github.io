/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "iwdg.h"
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    RTC_TimeTypeDef Set_Time;
    RTC_TimeTypeDef Get_Time;
    RTC_DateTypeDef Set_Date;
    RTC_DateTypeDef Get_Date;
} RTC_DATE_T;

typedef struct {
    uint8_t buffer[1024];
    uint16_t len;
    uint16_t size;
    uint8_t flag;
} UART_REC_BUFF_T;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UART_REC_BUFF_SIZE 1024
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
RTC_DATE_T rtc_date = {0};
UART_REC_BUFF_T __attribute__((section(".RAM_D1"))) uart_buff = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void MPU_Config(void);

/* USER CODE BEGIN PFP */
int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart1, (uint8_t *) ptr, len, 1000);
    return len;
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* USER CODE BEGIN 1 */
    SCB_EnableDCache();
    SCB_EnableICache();
    /* USER CODE END 1 */

    /* MPU Configuration--------------------------------------------------------*/
    MPU_Config();

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
    MX_DMA_Init();
    MX_IWDG1_Init();
    MX_RTC_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */
    printf("uart demo start\r\n");
    uart_buff.len = UART_REC_BUFF_SIZE;
    uart_buff.flag = 0;
    if (HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR0) != 0x0202) {
        printf("first set RTC time\r\n");
        HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0, 0x0202);
        rtc_date.Set_Time.Seconds = 0;
        rtc_date.Set_Time.Minutes = 0;
        rtc_date.Set_Time.Hours = 0;

        rtc_date.Set_Date.Date = 1;
        rtc_date.Set_Date.Month = 1;
        rtc_date.Set_Date.Year = 98;
        HAL_RTC_SetTime(&hrtc, &rtc_date.Set_Time,RTC_FORMAT_BIN);
        HAL_RTC_SetDate(&hrtc, &rtc_date.Set_Date,RTC_FORMAT_BIN);
    }
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    uint32_t count = 0;
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart_buff.buffer, uart_buff.len);
    while (1) {
        count++;
        if (count % 80 == 0) {
            HAL_IWDG_Refresh(&hiwdg1);
        }
        if (count % 100 == 0) {
            HAL_RTC_GetTime(&hrtc, &rtc_date.Get_Time,RTC_FORMAT_BIN);
            HAL_RTC_GetDate(&hrtc, &rtc_date.Get_Date,RTC_FORMAT_BIN);
            printf("20%02d-%02d-%02d %02d:%02d:%02d\r\n", rtc_date.Get_Date.Year, rtc_date.Get_Date.Month,
                   rtc_date.Get_Date.Date, rtc_date.Get_Time.Hours, rtc_date.Get_Time.Minutes,
                   rtc_date.Get_Time.Seconds);
        }
        if (uart_buff.flag && uart_buff.size > 0) {
            uart_buff.flag = 0;
            int data_time[6] = {0};
            sscanf((char *) uart_buff.buffer, "%02d-%02d-%02d %02d:%02d:%02d", data_time, data_time + 1, data_time + 2,
                   data_time + 3, data_time + 4, data_time + 5);
            rtc_date.Set_Date.Year = (uint8_t) data_time[0];
            rtc_date.Set_Date.Month = (uint8_t) data_time[1];
            rtc_date.Set_Date.Date = (uint8_t) data_time[2];
            rtc_date.Set_Time.Hours = (uint8_t) data_time[3];
            rtc_date.Set_Time.Minutes = (uint8_t) data_time[4];
            rtc_date.Set_Time.Seconds = (uint8_t) data_time[5];
            HAL_RTC_SetTime(&hrtc, &rtc_date.Set_Time,RTC_FORMAT_BIN);
            HAL_RTC_SetDate(&hrtc, &rtc_date.Set_Date,RTC_FORMAT_BIN);
            memset(uart_buff.buffer, 0, uart_buff.len);
            uart_buff.size = 0;
        }
        HAL_Delay(10);
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Supply configuration update enable
    */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

    /** Configure the main internal regulator output voltage
    */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
    }

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = 64;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 34;
    RCC_OscInitStruct.PLL.PLLP = 1;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 3072;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
                                  | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance == USART1) {
        uart_buff.flag = 1;
        uart_buff.size = Size;
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart_buff.buffer, uart_buff.len);
    }
}

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void) {
    MPU_Region_InitTypeDef MPU_InitStruct = {0};

    /* Disables the MPU */
    HAL_MPU_Disable();

    /** Initializes and configures the Region and the memory to be protected
    */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.BaseAddress = 0x0;
    MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
    MPU_InitStruct.SubRegionDisable = 0x87;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    /* Enables the MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
