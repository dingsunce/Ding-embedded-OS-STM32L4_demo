#include "iwdg.h"
#include "stm32ll.h"
#include "wdg_func.h"

void Wdg_Init(void)
{
}

void Wdg_Clear(void)
{
  LL_IWDG_ReloadCounter(IWDG);
}
