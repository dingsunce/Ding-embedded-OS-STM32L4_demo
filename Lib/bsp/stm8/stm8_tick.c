#include "Systick.h"
#include "stm8l15x_conf.h"

static volatile u16 Tick = 0;

void Tick_Init(void)
{
  // sys timer init 200us cycle
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
  TIM4_TimeBaseInit(TIM4_Prescaler_16, 200);
  TIM4_ITConfig(TIM4_IT_Update, ENABLE);
  TIM4_Cmd(ENABLE);

  disableInterrupts();
  ITC_SetSoftwarePriority(TIM4_UPD_OVF_TRG_IRQn, ITC_PriorityLevel_2);
  enableInterrupts();
}

u16 Tick_GetTick(void)
{
  return Tick;
}

void Tick_IncTick(void)
{
  Tick++;
}

INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler, 25)
{
  TIM4_ClearITPendingBit(TIM4_IT_Update);
  Tick++;
  SysTick_Handler();
}