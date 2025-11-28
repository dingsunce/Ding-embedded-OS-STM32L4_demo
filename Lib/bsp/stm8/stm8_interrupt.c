#include "stm8l15x_conf.h"

void Interrupt_Enable(void)
{
  enableInterrupts();
}

void Interrupt_Disable(void)
{
  disableInterrupts();
}