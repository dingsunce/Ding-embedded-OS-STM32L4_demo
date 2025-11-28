#include "interrupt_func.h"
#include "stm32ll.h"

void Interrupt_Enable(void)
{
  __enable_irq();
}

void Interrupt_Disable(void)
{
  __disable_irq();
}