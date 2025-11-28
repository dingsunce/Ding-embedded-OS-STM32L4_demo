#include "pwm_func.h"

void Pwm_Init(void)
{
}

void Pwm_Process(void)
{
}

OsErr_t Pwm_SetLevel(PwmTimerId_t timerId, float dutyCycle)
{
  return OS_ERR_OK;
}

OsErr_t Pwm_GetLevel(PwmTimerId_t timerId, float *level)
{
  return OS_ERR_OK;
}

void Pwm_SetDutyCycle(u8 channel, float dutyCycle)
{
}

float Pwm_GetDutyCycle(u8 channel)
{
  return 0;
}
