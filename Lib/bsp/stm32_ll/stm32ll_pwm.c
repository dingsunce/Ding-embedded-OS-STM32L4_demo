#include "stm32ll_pwm.h"

#include "assert_func.h"
#include "pwm_func.h"
#include "stm32ll.h"

#ifndef PWM_CONFIG

PwmImplRam_t PwmImplRam[PWM_CHANNEL_SUM];

PwmImpl_t PwmImpl[PWM_CHANNEL_SUM] = {
    {
        PwmImplRam, // pRam

        TIM17,              // TIMx
        LL_TIM_CHANNEL_CH1, // Channels
    },
};

#endif

void Pwm_Init(void)
{
  for (u8 i = 0; i < PWM_CHANNEL_SUM; i++)
  {
    PwmImpl_Init(PwmImpl + i);
  }
}

#ifdef PWM_FADE_ENABLE
void Pwm_Process(void)
{
}

#endif

void Pwm_SetDutyCycle(u8 channel, float dutyCycle)
{
  Assert(channel < PWM_CHANNEL_SUM);

  PwmImpl_SetDutyCycle(PwmImpl + channel, dutyCycle);
}

float Pwm_GetDutyCycle(u8 channel)
{
  Assert(channel < PWM_CHANNEL_SUM);

  return PwmImpl_GetDutyCycle(PwmImpl + channel);
}