/*!*****************************************************************************
 * file		d_tick.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "d_tick.h"
#include "d_list.h"
#include "d_memb.h"
#include "d_message.h"

static volatile u32  CurTicks = 0;
static volatile bool TickOn = false;
//-----------------------------------------------------------------------------------------------------------
u32 DTick_GetCurTicks(void)
{
  return CurTicks;
}
//-----------------------------------------------------------------------------------------------------------
u32 DTick_Now(void)
{
  return CurTicks * OS_TICK_PERIOD_MS;
}
//-----------------------------------------------------------------------------------------------------------
void DTick_Init(void)
{
  CurTicks = 0;
  TickOn = false;
}
//-----------------------------------------------------------------------------------------------------------
bool DTick_IsTickOn(void)
{
  return TickOn;
}
//-----------------------------------------------------------------------------------------------------------
void DTick_ResetTickOn(void)
{
  TickOn = false;
}
//-----------------------------------------------------------------------------------------------------------
void DTick_On(void)
{
  CurTicks++;

  TickOn = true;

  DMsg_PostSem();
}
//-----------------------------------------------------------------------------------------------------------
u32 DTick_GetDelayMs(u32 nOldTime)
{
  u32 delay;

  if (DTick_Now() >= nOldTime)
    delay = DTick_Now() - nOldTime;
  else
    delay = 0xffffffff - nOldTime + DTick_Now();

  return delay;
}
//-----------------------------------------------------------------------------------------------------------
void DTick_DelayMs(u32 delay)
{
  u32 tickstart = DTick_Now();
  u32 wait = delay;

  while ((DTick_Now() - tickstart) < wait)
  {
  }
}
//-----------------------------------------------------------------------------------------------------------
void DTick_Update(u32 pTime)
{
  CurTicks = CurTicks + pTime / OS_TICK_PERIOD_MS;
}
//-----------------------------------------------------------------------------------------------------------
void DTick_Reset(void)
{
  CurTicks = 0;
  TickOn = false;
}
