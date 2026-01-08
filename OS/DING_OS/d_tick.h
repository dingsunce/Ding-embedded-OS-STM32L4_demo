/*!*****************************************************************************
 * file		d_tick.h
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _D_TICK_H
#define _D_TICK_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "d_os.h"
#include "define.h"

  void DTick_Init(void);

  u32  DTick_GetCurTicks(void);
  bool DTick_IsTickOn(void);
  void DTick_ResetTickOn(void);

  void DTick_On(void);
  u32  DTick_Now(void);
  u32  DTick_GetDelayMs(u32 nOldTime);

  void DTick_Update(u32 pTime);
  void DTick_Reset(void);
  void DTick_DelayMs(u32 delay);

#ifdef __cplusplus
}
#endif

#endif
