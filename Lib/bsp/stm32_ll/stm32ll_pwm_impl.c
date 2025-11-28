#include "stm32ll_pwm_impl.h"

#include "stm32ll_tim.h"

void PwmImpl_Init(PwmImpl_t *pPwmImpl)
{
  /**********************************/
  /* Start output signal generation */
  /**********************************/
  /* Enable output channel */
  LL_TIM_CC_EnableChannel(pPwmImpl->TIMx, pPwmImpl->Channels);

  /* Enable Timer outputs */
  LL_TIM_EnableAllOutputs(pPwmImpl->TIMx);

  /* Enable counter */
  LL_TIM_EnableCounter(pPwmImpl->TIMx);
}

void PwmImpl_SetDutyCycle(PwmImpl_t *pPwmImpl, float dutyCycle)
{
  /* PWM signal period is determined by the value of the auto-reload register */
  u32 period = LL_TIM_GetAutoReload(pPwmImpl->TIMx);

  /* Pulse duration is determined by the value of the compare register.       */
  /* Its value is calculated in order to match the requested duty cycle.      */
  u32 duration = (u32)(dutyCycle * period);

  // pPwmImpl->TIM_OC_SetCompare_Func(pPwmImpl->TIMx, duration);
  LL_TIM_OC_SetCompare(pPwmImpl->TIMx, pPwmImpl->Channels, duration);
}

float PwmImpl_GetDutyCycle(PwmImpl_t *pPwmImpl)
{
  /* PWM signal period is determined by the value of the auto-reload register */
  u32 period = LL_TIM_GetAutoReload(pPwmImpl->TIMx) + 1;

  u32 duration = LL_TIM_OC_GetCompare(pPwmImpl->TIMx, pPwmImpl->Channels);

  return (float)duration / (float)period;
}