#include "clock_func.h"
#include "stm8l15x_conf.h"

void Clock_Init(void)
{
  CLK_SYSCLKSourceSwitchCmd(ENABLE);
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI); // selection of target clock source
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);         // 16MHz

  while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI)
  {
  }
}