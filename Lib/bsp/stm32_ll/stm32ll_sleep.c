#include "sleep_func.h"
#include "stm32ll.h"

// define RTC_SLEEP

void Sleep_Init(void)
{
}

void Sleep_Enter(void)
{
  __WFI();
}

#ifdef RTC_SLEEP
#include "rtc.h"

// HSI 32K, wake up divide 16
#define TIME_OF_ONE_COUNT (1.0 / 32.0 * 16.0)

static void ConfigLSI(void)
{
  /* LSI configuration and activation */
  LL_RCC_LSI_Enable();
  while (LL_RCC_LSI_IsReady() != 1)
  {
  }

  LL_PWR_EnableBkUpAccess();
}

static void EnterStop1Mode(void)
{
  /* Enable Power Clock */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* Set STOP 0 mode when CPU enters deep sleep */
  LL_PWR_SetPowerMode(LL_PWR_MODE_STOP1);

  /* Set SLEEPDEEP bit of Cortex System Control Register */
  LL_LPM_EnableDeepSleep();

  /* Request Wait For Interrupt */
  __WFI();
}

// Fixed 200ms
void Sleep_SleepMs(u16 ms)
{
  ConfigLSI();

  MX_RTC_Init();

  EnterStop1Mode();

  LL_LPM_EnableSleep();

  LL_RCC_DisableRTC();
}

void RTC_TAMP_IRQHandler(void)
{
  if (LL_RTC_IsActiveFlag_WUT(RTC) != RESET)
  {
    LL_RTC_ClearFlag_WUT(RTC);
  }
}
#else
void Sleep_SleepMs(u16 ms)
{
}
#endif