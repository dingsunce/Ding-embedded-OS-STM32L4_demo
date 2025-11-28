#ifndef _RESET_FUNC_H
#define _RESET_FUNC_H

#include "define.h"

void Reset_Mcu(void);
void Reset_Init(void);
void Reset_Start(u32 delay);
u32  Reset_GetResetRemainTimer(void);
void Reset_Cancel(void);

#endif