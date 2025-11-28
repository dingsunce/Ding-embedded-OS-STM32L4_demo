#ifndef _STM32LL_PWM_IMPL_H
#define _STM32LL_PWM_IMPL_H

#include "define.h"
#include "stm32ll.h"

typedef struct
{
  bool tt;
} PwmImplRam_t;

typedef const struct
{
  PwmImplRam_t *pRam;

  TIM_TypeDef *TIMx;
  uint32_t     Channels;
} PwmImpl_t;

extern void  PwmImpl_Init(PwmImpl_t *pPwmImpl);
extern void  PwmImpl_SetDutyCycle(PwmImpl_t *pPwmImpl, float dutyCycle);
extern float PwmImpl_GetDutyCycle(PwmImpl_t *pPwmImpl);

#endif
