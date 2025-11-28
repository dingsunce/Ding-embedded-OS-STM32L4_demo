#ifndef _DEBUG_FUNC_H
#define _DEBUG_FUNC_H

#include "define.h"

// #define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
extern void Debug_Init(void);
extern void Debug_ToggleIo(u8 id);
extern void Debug_SetIo(u8 id);
extern void Debug_ResetIo(u8 id);
#else
#define Debug_Init()       ((void)0)
#define Debug_ToggleIo(id) ((void)0)
#define Debug_SetIo(id)    ((void)0)
#define Debug_ResetIo(id)  ((void)0)
#endif

#endif