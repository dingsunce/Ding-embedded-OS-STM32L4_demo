#ifndef _STM32G0XX_HAL_CONF_H
#define _STM32G0XX_HAL_CONF_H

#include "stm32g0xx_hal_def.h"
#include "stm32g0xx_hal_flash.h"

#define HAL_FLASH_MODULE_ENABLED

/* Exported macro ------------------------------------------------------------*/
#ifdef USE_FULL_ASSERT
/**
 * @brief  The assert_param macro is used for functions parameters check.
 * @param  expr If expr is false, it calls assert_failed function
 *         which reports the name of the source file and the source
 *         line number of the call that failed.
 *         If expr is true, it returns no value.
 * @retval None
 */
#define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
void assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

#define HAL_GetTick SysTick_Get1msTicks

#endif