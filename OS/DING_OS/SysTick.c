/*!*****************************************************************************
 * file		SysTick.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "SysTick.h"
#include "memb.h"
#include "message.h"
#include "s_list.h"

static volatile u32  CurTicks = 0;
static volatile bool TickOn = false;
//-----------------------------------------------------------------------------------------------------------
u32 SysTick_GetCurTicks(void)
{
  return CurTicks;
}
//-----------------------------------------------------------------------------------------------------------
u32 sys_now(void)
{
  return CurTicks * OS_TICK_PERIOD_MS;
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_Init(void)
{
  CurTicks = 0;
  TickOn = false;
}
//-----------------------------------------------------------------------------------------------------------
bool SysTick_IsTickOn(void)
{
  return TickOn;
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_ResetTickOn(void)
{
  TickOn = false;
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_On(void)
{
  CurTicks++;

  TickOn = true;

  Msg_PostSem();
}
//-----------------------------------------------------------------------------------------------------------
u32 SysTick_GetDelayMs(u32 nOldTime)
{
  u32 delay;

  if (sys_now() >= nOldTime)
    delay = sys_now() - nOldTime;
  else
    delay = 0xffffffff - nOldTime + sys_now();

  return delay;
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_DelayMs(u32 delay)
{
  u32 tickstart = sys_now();
  u32 wait = delay;

  while ((sys_now() - tickstart) < wait)
  {
  }
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_Update(u32 pTime)
{
  CurTicks = CurTicks + pTime / OS_TICK_PERIOD_MS;
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_Reset(void)
{
  CurTicks = 0;
  TickOn = false;
}
