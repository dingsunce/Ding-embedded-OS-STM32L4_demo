#include "stm32ll.h"

void LL_TIM_OC_SetCompare(TIM_TypeDef *TIMx, uint32_t channel, uint32_t CompareValue)
{
  if (channel == LL_TIM_CHANNEL_CH1 || channel == LL_TIM_CHANNEL_CH1N)
  {
    LL_TIM_OC_SetCompareCH1(TIMx, CompareValue);
  }
  else if (channel == LL_TIM_CHANNEL_CH2 || channel == LL_TIM_CHANNEL_CH2N)
  {
    LL_TIM_OC_SetCompareCH2(TIMx, CompareValue);
  }
  else if (channel == LL_TIM_CHANNEL_CH3 || channel == LL_TIM_CHANNEL_CH3N)
  {
    LL_TIM_OC_SetCompareCH3(TIMx, CompareValue);
  }
  else if (channel == LL_TIM_CHANNEL_CH4)
  {
    LL_TIM_OC_SetCompareCH4(TIMx, CompareValue);
  }
  else if (channel == LL_TIM_CHANNEL_CH5)
  {
    LL_TIM_OC_SetCompareCH5(TIMx, CompareValue);
  }
  else if (channel == LL_TIM_CHANNEL_CH6)
  {
    LL_TIM_OC_SetCompareCH6(TIMx, CompareValue);
  }
}

uint32_t LL_TIM_OC_GetCompare(const TIM_TypeDef *TIMx, uint32_t channel)
{
  if (channel == LL_TIM_CHANNEL_CH1 || channel == LL_TIM_CHANNEL_CH1N)
  {
    return LL_TIM_OC_GetCompareCH1(TIMx);
  }
  else if (channel == LL_TIM_CHANNEL_CH2 || channel == LL_TIM_CHANNEL_CH2N)
  {
    return LL_TIM_OC_GetCompareCH2(TIMx);
  }
  else if (channel == LL_TIM_CHANNEL_CH3 || channel == LL_TIM_CHANNEL_CH3N)
  {
    return LL_TIM_OC_GetCompareCH3(TIMx);
  }
  else if (channel == LL_TIM_CHANNEL_CH4)
  {
    return LL_TIM_OC_GetCompareCH4(TIMx);
  }
  else if (channel == LL_TIM_CHANNEL_CH5)
  {
    return LL_TIM_OC_GetCompareCH5(TIMx);
  }
  else if (channel == LL_TIM_CHANNEL_CH6)
  {
    return LL_TIM_OC_GetCompareCH6(TIMx);
  }

  return 0;
}