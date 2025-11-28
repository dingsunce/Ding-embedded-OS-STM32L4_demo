#ifndef _ADC_FUNC_H
#define _ADC_FUNC_H

#include "define.h"

typedef enum
{
  AD_CHANNEL_0,
  AD_CHANNEL_1,
  AD_CHANNEL_2,
  AD_CHANNEL_3,
  AD_CHANNEL_4,
  AD_CHANNEL_5,
  AD_CHANNEL_6,
  AD_CHANNEL_7,
  AD_CHANNEL_8,
  AD_CHANNEL_9,
  AD_CHANNEL_10,
  AD_CHANNEL_11,
  AD_CHANNEL_12,
  AD_CHANNEL_13,
  AD_CHANNEL_14,
  AD_CHANNEL_15,
  AD_CHANNEL_16,
  AD_CHANNEL_17,
  AD_CHANNEL_18,
  AD_CHANNEL_19,
  AD_CHANNEL_20,
  AD_CHANNEL_21,
  AD_CHANNEL_22,
  AD_CHANNEL_23,
  AD_CHANNEL_24,
  AD_CHANNEL_25,
  AD_CHANNEL_26,
  AD_CHANNEL_27,
} AdcChannel_t;

extern void Adc_Init(void);
extern u16  Adc_Convert(AdcChannel_t channel);
extern void Adc_StartConvertion(void);
extern void Adc_StopConvertion(void);

#endif