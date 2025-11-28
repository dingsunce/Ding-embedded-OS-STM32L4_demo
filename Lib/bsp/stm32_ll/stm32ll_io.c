#include "io_func.h"
#include "stm32ll.h"

static const GPIO_TypeDef *Port[] = {
    GPIOA, GPIOB, GPIOC, GPIOD,
#ifdef GPIOE
    GPIOE,
#endif
#ifdef GPIOF
    GPIOF,
#endif
};

void Io_Init(IoPort_t port, IoBits_t pins, IoMode_t mode)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /*Configure GPIO pin Output Level */
  if (mode == IO_OUTPUT_PUSH_PULL_HIGH)
  {
    LL_GPIO_SetOutputPin((GPIO_TypeDef *)Port[port], pins);
  }
  else if (mode == IO_OUTPUT_PUSH_PULL_LOW)
  {
    LL_GPIO_ResetOutputPin((GPIO_TypeDef *)Port[port], pins);
  }

#ifdef STM32_SERIAL_F1
  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = pins;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  if (mode == IO_INPUT_FLOATING)
  {
    GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  }
  else if (mode == IO_INPUT_PULL_UP)
  {
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  }
  else if (mode == IO_OUTPUT_OPEN_DRAIN)
  {
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  }
  else if (mode == IO_OUTPUT_PUSH_PULL_LOW || mode == IO_OUTPUT_PUSH_PULL_HIGH)
  {
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  }

#elif defined STM32_SERIAL_G0
  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = pins;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  if (mode == IO_INPUT_FLOATING)
  {
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  }
  else if (mode == IO_INPUT_PULL_UP)
  {
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  }
  else if (mode == IO_OUTPUT_OPEN_DRAIN)
  {
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  }
  else if (mode == IO_OUTPUT_PUSH_PULL_LOW || mode == IO_OUTPUT_PUSH_PULL_HIGH)
  {
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  }

#endif

  LL_GPIO_Init((GPIO_TypeDef *)Port[port], &GPIO_InitStruct);
}

IoBits_t Io_GetBits(IoPort_t port, IoBits_t pins)
{
  return LL_GPIO_ReadInputPort((GPIO_TypeDef *)Port[port]) & pins;
}

void Io_WriteBits(IoPort_t port, IoBits_t pins, IoLevel_t level)
{
  if (level == IO_HIGH)
  {
    LL_GPIO_SetOutputPin((GPIO_TypeDef *)Port[port], pins);
  }
  else
  {
    LL_GPIO_ResetOutputPin((GPIO_TypeDef *)Port[port], pins);
  }
}

void Io_SetBits(IoPort_t port, IoBits_t pins)
{
  LL_GPIO_SetOutputPin((GPIO_TypeDef *)Port[port], pins);
}

void Io_ResetBits(IoPort_t port, IoBits_t pins)
{
  LL_GPIO_ResetOutputPin((GPIO_TypeDef *)Port[port], pins);
}

void Io_ToggleBits(IoPort_t port, IoBits_t pins)
{
  LL_GPIO_TogglePin((GPIO_TypeDef *)Port[port], pins);
}