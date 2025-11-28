#include "random_func.h"
#include "stm8l15x_conf.h"

void Random_Init(void)
{
}

u8 Random_Get8(void)
{
  return (u8)(TIM3_GetCounter());
}

u16 Random_Get16(void)
{
  return TIM3_GetCounter();
}