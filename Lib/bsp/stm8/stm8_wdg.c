#include "stm8l15x_conf.h"

void Wdg_Init(void)
{
  // Enable the IWDG.
  IWDG_Enable();

  // Enable the access to the IWDG registers.
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  // Set IWDG Prescaler to /64.
  IWDG_SetPrescaler(IWDG_Prescaler_64);

  // Set Start/Reload-Value to 168ms=1/38K*64*100.
  IWDG_SetReload(100);

  // Refresh IWDG.
  IWDG_ReloadCounter();
}

void Wdg_Clear(void)
{
  // Refresh IWDG.
  IWDG_ReloadCounter();
}