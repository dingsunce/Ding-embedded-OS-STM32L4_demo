#ifndef _STM32LL_TIM_H
#define _STM32LL_TIM_H

#define _LL_TIM_CHANNEL_CH(channel) LL_TIM_CHANNEL_CH##channel
#define LL_TIM_CHANNEL_CH(channel)  _LL_TIM_CHANNEL_CH(channel)

#define _LL_TIM_EnableIT_CC(TIMx, channel) LL_TIM_EnableIT_CC##channel(TIMx)
#define LL_TIM_EnableIT_CC(TIMx, channel)  _LL_TIM_EnableIT_CC(TIMx, channel)

#define _LL_TIM_DisableIT_CC(TIMx, channel) LL_TIM_DisableIT_CC##channel(TIMx)
#define LL_TIM_DisableIT_CC(TIMx, channel)  _LL_TIM_DisableIT_CC(TIMx, channel)

#define _LL_TIM_IsEnabledIT_CC(TIMx, channel) LL_TIM_IsEnabledIT_CC##channel(TIMx)
#define LL_TIM_IsEnabledIT_CC(TIMx, channel)  _LL_TIM_IsEnabledIT_CC(TIMx, channel)

#define _LL_TIM_IsActiveFlag_CC(TIMx, channel) LL_TIM_IsActiveFlag_CC##channel(TIMx)
#define LL_TIM_IsActiveFlag_CC(TIMx, channel)  _LL_TIM_IsActiveFlag_CC(TIMx, channel)

#define _LL_TIM_ClearFlag_CC(TIMx, channel) LL_TIM_ClearFlag_CC##channel(TIMx)
#define LL_TIM_ClearFlag_CC(TIMx, channel)  _LL_TIM_ClearFlag_CC(TIMx, channel)

#define _LL_TIM_OC_SetCompareCH(TIMx, channel, CompareValue)                                                 \
  LL_TIM_OC_SetCompareCH##channel(TIMx, CompareValue)
#define LL_TIM_OC_SetCompareCH(TIMx, channel, CompareValue)                                                  \
  _LL_TIM_OC_SetCompareCH(TIMx, channel, CompareValue)

extern void     LL_TIM_OC_SetCompare(TIM_TypeDef *TIMx, uint32_t channel, uint32_t CompareValue);
extern uint32_t LL_TIM_OC_GetCompare(const TIM_TypeDef *TIMx, uint32_t channel);

#endif