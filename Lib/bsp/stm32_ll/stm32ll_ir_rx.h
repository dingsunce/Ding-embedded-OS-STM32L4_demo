#ifndef STM32LL_IR_RX_H
#define STM32LL_IR_RX_H

#include "define.h"

// #define IR_CONFIG

#ifndef IR_CONFIG

// #define IR_ENABLE_TX

#define IR_TIMx_INSTANCE   TIM3
#define IR_TIMx_IRQHandler TIM3_IRQHandler

#define IR_TIMx_INPUT_CAPTURE_CHANNEL  1
#define IR_TIMx_OUTPUT_COMPARE_CHANNEL 2
#define IR_TIMx_TIMEOUT_CHANNEL        4

#endif

/*! Timer counter resolution used to determine received pulses time, set to 1us */
#define IR_BASE_TIME 1E-6

extern bool IrRx_IsBusy(void);

/*! Enables IR Reception hardware timer. Used by IR Transmit driver */
#define IrRx_Enable()                                                                                        \
  LL_TIM_ClearFlag_CC(IR_TIMx_INSTANCE, IR_TIMx_INPUT_CAPTURE_CHANNEL);                                      \
  LL_TIM_EnableIT_CC(IR_TIMx_INSTANCE, IR_TIMx_INPUT_CAPTURE_CHANNEL)

/*! Disables IR Reception hardware timer. Used by IR Transmit driver */
#define IrRx_Disable() LL_TIM_DisableIT_CC(IR_TIMx_INSTANCE, IR_TIMx_INPUT_CAPTURE_CHANNEL)

#endif
