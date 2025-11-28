#include "adc_func.h"
#include "stm8l15x_conf.h"

#define ADC_CHANNEL_SUM 5

static u16 AdcValues[ADC_CHANNEL_SUM];

static const ADC_Channel_TypeDef Stm8AdcChannels[ADC_CHANNEL_SUM] = {
    ADC_Channel_0, ADC_Channel_1, ADC_Channel_4, ADC_Channel_5, ADC_Channel_6};
static const AdcChannel_t CommonAdcChannels[ADC_CHANNEL_SUM] = {AD_CHANNEL_0, AD_CHANNEL_1, AD_CHANNEL_4,
                                                                AD_CHANNEL_5, AD_CHANNEL_6};

// Static functions.
static void ADC_Config(void);
static void DMA_Config(void);

void Adc_Init(void)
{
  ADC_Config(); /* Start ADC1 Conversion using Software trigger*/
  DMA_Config();

  /* Enable ADC1 DMA requests*/
  ADC_DMACmd(ADC1, ENABLE);

  /* Start ADC1 Conversion using Software trigger*/
  ADC_SoftwareStartConv(ADC1);
}

static void ADC_Config(void)
{
  /* Enable ADC1 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);

  /* Initialise and configure ADC1 */
  ADC_Init(ADC1, ADC_ConversionMode_Continuous, ADC_Resolution_12Bit, ADC_Prescaler_2);
  ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels,
                         ADC_SamplingTime_384Cycles); // slow channel 0~23; fast channel 24~27

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 Channels */
  for (u8 i = 0; i < ADC_CHANNEL_SUM; i++)
  {
    ADC_ChannelCmd(ADC1, Stm8AdcChannels[i], ENABLE);
  }
}

static void DMA_Config(void)
{
  /* Enable DMA1 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);

  /* Connect ADC to DMA channel 0 */
  SYSCFG_REMAPDMAChannelConfig(REMAP_DMA1Channel_ADC1ToChannel0);

  DMA_Init(DMA1_Channel0, (uint16_t)AdcValues, (uint16_t)&ADC1->DRH, ADC_CHANNEL_SUM,
           DMA_DIR_PeripheralToMemory, DMA_Mode_Circular, DMA_MemoryIncMode_Inc, DMA_Priority_High,
           DMA_MemoryDataSize_HalfWord);

  /* DMA Channel0 enable */
  DMA_Cmd(DMA1_Channel0, ENABLE);

  /* DMA enable */
  DMA_GlobalCmd(ENABLE);
}

u16 Adc_Convert(AdcChannel_t channel)
{
  for (u8 i = 0; i < ADC_CHANNEL_SUM; i++)
  {
    if (CommonAdcChannels[i] == channel)
    {
      return AdcValues[i];
    }
  }

  return 0xFFFF;
}