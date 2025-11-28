/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_i2c.h"
#include "stm32l4xx_ll_iwdg.h"
#include "stm32l4xx_ll_crs.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_pwr.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_tim.h"
#include "stm32l4xx_ll_usart.h"
#include "stm32l4xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define W5000_RST_OUT_Pin LL_GPIO_PIN_13
#define W5000_RST_OUT_GPIO_Port GPIOC
#define KNX1_UART4_TX_Pin LL_GPIO_PIN_0
#define KNX1_UART4_TX_GPIO_Port GPIOA
#define KNX1_UART4_RX_Pin LL_GPIO_PIN_1
#define KNX1_UART4_RX_GPIO_Port GPIOA
#define TEST_PB_Pin LL_GPIO_PIN_2
#define TEST_PB_GPIO_Port GPIOA
#define KNX1_RESET_OUT_Pin LL_GPIO_PIN_3
#define KNX1_RESET_OUT_GPIO_Port GPIOA
#define KNX1_POWER_DETECT_Pin LL_GPIO_PIN_4
#define KNX1_POWER_DETECT_GPIO_Port GPIOA
#define KNX2_RESET_OUT_Pin LL_GPIO_PIN_5
#define KNX2_RESET_OUT_GPIO_Port GPIOA
#define KNX2_POWER_DETECT_Pin LL_GPIO_PIN_6
#define KNX2_POWER_DETECT_GPIO_Port GPIOA
#define W5000_INT_IN_Pin LL_GPIO_PIN_7
#define W5000_INT_IN_GPIO_Port GPIOA
#define KNX_BUTTON_IN_Pin LL_GPIO_PIN_0
#define KNX_BUTTON_IN_GPIO_Port GPIOB
#define KNX_LED_OUT_Pin LL_GPIO_PIN_1
#define KNX_LED_OUT_GPIO_Port GPIOB
#define S2LP_INT_Pin LL_GPIO_PIN_2
#define S2LP_INT_GPIO_Port GPIOB
#define S2LP_INT_EXTI_IRQn EXTI2_IRQn
#define S2LP_SPI2_SCK_Pin LL_GPIO_PIN_10
#define S2LP_SPI2_SCK_GPIO_Port GPIOB
#define S2LP_CSN_Pin LL_GPIO_PIN_11
#define S2LP_CSN_GPIO_Port GPIOB
#define USB_POWER_LED_Pin LL_GPIO_PIN_12
#define USB_POWER_LED_GPIO_Port GPIOB
#define KNX_POWER_LED_Pin LL_GPIO_PIN_13
#define KNX_POWER_LED_GPIO_Port GPIOB
#define S2LP_SPI2_MISO_Pin LL_GPIO_PIN_14
#define S2LP_SPI2_MISO_GPIO_Port GPIOB
#define S2LP_SPI2_MOSI_Pin LL_GPIO_PIN_15
#define S2LP_SPI2_MOSI_GPIO_Port GPIOB
#define S2LP_SDN_Pin LL_GPIO_PIN_8
#define S2LP_SDN_GPIO_Port GPIOA
#define KNX2_UART1_TX_Pin LL_GPIO_PIN_9
#define KNX2_UART1_TX_GPIO_Port GPIOA
#define KNX2_UART1_RX_Pin LL_GPIO_PIN_10
#define KNX2_UART1_RX_GPIO_Port GPIOA
#define W5000_SPI3_SCK_Pin LL_GPIO_PIN_3
#define W5000_SPI3_SCK_GPIO_Port GPIOB
#define W5000_SPI3_MISO_Pin LL_GPIO_PIN_4
#define W5000_SPI3_MISO_GPIO_Port GPIOB
#define W5000_SPI3_MOSI_Pin LL_GPIO_PIN_5
#define W5000_SPI3_MOSI_GPIO_Port GPIOB
#define EEPROM_I2C1_SCL_Pin LL_GPIO_PIN_6
#define EEPROM_I2C1_SCL_GPIO_Port GPIOB
#define EEPROM_I2C1_SDA_Pin LL_GPIO_PIN_7
#define EEPROM_I2C1_SDA_GPIO_Port GPIOB
#define W5000_SELECT_OUT_Pin LL_GPIO_PIN_9
#define W5000_SELECT_OUT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
