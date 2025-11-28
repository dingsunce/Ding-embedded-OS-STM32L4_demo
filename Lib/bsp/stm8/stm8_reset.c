#include "stm8l15x_conf.h"

void Reset_Mcu(void)
{
  WWDG_Enable(0);
  WWDG_SWReset();
}