#include "stm8l15x_conf.h"

// HSI 38K, RTC clock divide 2, wake up divide 16
#define TIME_OF_ONE_COUNT (1.0 / 38.0 * 32.0)

void Sleep_Init(void)
{
}

void Sleep_Enter(void)
{
  wfi();
}

void Sleep_SleepMs(u16 ms)
{
  /* Configures RTC clock */
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_2);
  CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);

  /* Configures the RTC */
  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
  RTC_ITConfig(RTC_IT_WUT, ENABLE);

  RTC_SetWakeUpCounter((u16)(ms / TIME_OF_ONE_COUNT));
  RTC_WakeUpCmd(ENABLE);

  /* Enter Halt mode*/
  halt();

  RTC_WakeUpCmd(DISABLE);
}

INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler, 4)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  RTC_ClearITPendingBit(RTC_IT_WUT);
}