#include "delay_func.h"
#include "stm8l15x_conf.h"

void Delay_DelayUs(u16 us)
{
  u16 startTick;
  u16 currentTick;

  // 1us time base
  startTick = TIM3_GetCounter();
  do
  {
    currentTick = TIM3_GetCounter();
  } while ((currentTick - startTick) < us);
}

void Delay_DelayMs(u16 ms)
{
  while (ms > 0)
  {
    Delay_DelayUs(1000);
    ms--;
  }
}