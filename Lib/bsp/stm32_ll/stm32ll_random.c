#include "random_func.h"
#include "stm32ll.h"

void Random_Init(void)
{
}

u8 Random_Get8(void)
{
  return (u8)(LL_TIM_GetCounter(TIM1));
}

u16 Random_Get16(void)
{
  return (u16)LL_TIM_GetCounter(TIM1);
}
