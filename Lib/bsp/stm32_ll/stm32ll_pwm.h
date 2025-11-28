#ifndef STM32LL_PWM_H
#define STM32LL_PWM_H

#include "stm32ll_pwm_impl.h"

#ifndef PWM_CONFIG
#define PWM_CHANNEL_SUM 1
#endif

extern PwmImpl_t PwmImpl[PWM_CHANNEL_SUM];

#endif