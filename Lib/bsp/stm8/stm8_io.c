#include "io_func.h"
#include "stm8l15x_conf.h"

static const GPIO_TypeDef *Port[] = {
    GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF,
};

static const GPIO_Mode_TypeDef Mode[] = {
    GPIO_Mode_In_FL_No_IT,      // IO_INPUT_FLOATING
    GPIO_Mode_In_PU_No_IT,      // IO_INPUT_PULL_UP
    GPIO_Mode_Out_OD_Low_Fast,  // IO_OUTPUT_OPEN_DRAIN
    GPIO_Mode_Out_PP_Low_Fast,  // IO_OUTPUT_PUSH_PULL_LOW
    GPIO_Mode_Out_PP_High_Fast, // IO_OUTPUT_PUSH_PULL_HIGH
};

void Io_Init(IoPort_t port, IoBits_t pins, IoMode_t mode)
{
  GPIO_Init((GPIO_TypeDef *)Port[port], pins, Mode[mode]);
}

IoBits_t Io_GetBits(IoPort_t port, IoBits_t pins)
{
  return (IoBits_t)GPIO_ReadInputDataBit((GPIO_TypeDef *)Port[port], (GPIO_Pin_TypeDef)pins);
}

void Io_WriteBits(IoPort_t port, IoBits_t pins, IoLevel_t level)
{
  if (level == IO_LOW)
  {
    GPIO_ResetBits((GPIO_TypeDef *)Port[port], pins);
  }
  else
  {
    GPIO_SetBits((GPIO_TypeDef *)Port[port], pins);
  }
}

void Io_SetBits(IoPort_t port, IoBits_t pins)
{
  GPIO_SetBits((GPIO_TypeDef *)Port[port], pins);
}

void Io_ResetBits(IoPort_t port, IoBits_t pins)
{
  GPIO_ResetBits((GPIO_TypeDef *)Port[port], pins);
}

void Io_ToggleBits(IoPort_t port, IoBits_t pins)
{
  GPIO_ToggleBits((GPIO_TypeDef *)Port[port], pins);
}