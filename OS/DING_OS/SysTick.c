/*!*****************************************************************************
 * file		SysTick.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "SysTick.h"
#include "memb.h"
#include "message.h"
#include "s_list.h"

static volatile u8   Tick200us = 0;
static volatile u32  Tick1ms = 0;
static volatile bool Tick1msOn = false;
static volatile u32  Tick200usBackup = 0;
//-----------------------------------------------------------------------------------------------------------
u32 SysTick_Get1msTicks(void)
{
  return Tick1ms;
}
//-----------------------------------------------------------------------------------------------------------
u32 sys_now(void)
{
  return Tick1ms;
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_Init(void)
{
  Tick1ms = 0;
  Tick1msOn = false;
}
//-----------------------------------------------------------------------------------------------------------
bool SysTick_IsTick1msOn(void)
{
  return Tick1msOn;
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_ResetTick1msOn(void)
{
  Tick1msOn = false;
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_On(void)
{
  Tick1ms++;

  Tick1msOn = true;

  Msg_PostSem();
}
//-----------------------------------------------------------------------------------------------------------
u32 SysTick_GetDelayMs(u32 nOldTime)
{
  u32 delay;

  if (Tick1ms >= nOldTime)
    delay = Tick1ms - nOldTime;
  else
    delay = 0xffffffff - nOldTime + Tick1ms;

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
  Tick1ms = Tick1ms + pTime;
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_Reset(void)
{
  Tick1ms = 0;
  Tick1msOn = false;
}
