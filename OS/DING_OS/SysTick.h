/*!*****************************************************************************
 * file		SysTick.h
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _SYS_TICKS_H
#define _SYS_TICKS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "d_os.h"
#include "define.h"

  void SysTick_Init(void);

  u32  SysTick_Get1msTicks(void);
  bool SysTick_IsTick1msOn(void);
  void SysTick_ResetTick1msOn(void);

  void SysTick_On(void);
  u32  sys_now(void);
  u32  SysTick_GetDelayMs(u32 nOldTime);

  void SysTick_Update(u32 pTime);
  void SysTick_Reset(void);
  void SysTick_DelayMs(u32 delay);

#ifdef __cplusplus
}
#endif

#endif
