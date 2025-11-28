#ifndef _PWM_FUNC_H
#define _PWM_FUNC_H

#include "define.h"
#include "error.h"

// #define PWM_CONFIG

#ifndef PWM_CONFIG
#define PWM_FADE_ENABLE
#define PWM_FADE_SUM 10 /* fade 10 times to achieve goal PWM value */
#endif

#if defined PLATFORM_STM8
typedef enum
{
  TIMER1_CH1,
  TIMER1_CH2,
  TIMER1_CH3,
  TIMER2_CH1,
  TIMER2_CH2,
  TIMER3_CH1,
  TIMER3_CH2,
  TIMER5_CH1,
  TIMER5_CH2,
  MAX_TIMER_NUM,
} PwmTimerId_t;
#elif defined PLATFORM_STM32
typedef enum
{
  TIMER1_CH1,
  TIMER1_CH2,
  TIMER1_CH3,
  TIMER2_CH1,
  TIMER2_CH2,
  TIMER3_CH1,
  TIMER3_CH2,
  TIMER5_CH1,
  TIMER5_CH2,
  MAX_TIMER_NUM,
} PwmTimerId_t;
#else
typedef enum
{
  TIMER1_CH1,
  TIMER1_CH2,
  TIMER1_CH3,
  TIMER2_CH1,
  TIMER2_CH2,
  TIMER3_CH1,
  TIMER3_CH2,
  TIMER5_CH1,
  TIMER5_CH2,
  MAX_TIMER_NUM,
} PwmTimerId_t;
#endif

extern void Pwm_Init(void);
#ifdef PWM_FADE_ENABLE
extern void Pwm_Process(void);
#endif
extern OsErr_t Pwm_SetLevel(PwmTimerId_t timerId, float dutyCycle);
extern OsErr_t Pwm_GetLevel(PwmTimerId_t timerId, float *level);

extern void  Pwm_SetDutyCycle(u8 channel, float dutyCycle);
extern float Pwm_GetDutyCycle(u8 channel);

#endif
