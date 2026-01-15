/*!*****************************************************************************
 * file		d_define.h
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _D_DEFINE_H
#define _D_DEFINE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "osal.h"

#ifndef NULL
#define NULL 0
#endif

  typedef u16   DMsgId_t;    // Message id type.
  typedef u32   DMsgDelay_t; // Message delay type.
  typedef void *DMsgArg_t;   // Message argument type.

#ifndef LO_U16
#define LO_U16(x) ((u8)((x) & 0x00ff))
#endif

#ifndef HI_U16
#define HI_U16(x) ((u8)((x) >> 8))
#endif

#define HI_LO_U16(nWord) HI_U16(nWord), LO_U16(nWord)
#define LO_HI_U16(nWord) LO_U16(nWord), HI_U16(nWord)

#ifndef BUILD_U16
#define BUILD_U16(nHigh, nLow) ((u16)((u8)(nHigh) << 8) + (u16)((u8)(nLow)))
#endif

#ifndef BUILD_U32
#define BUILD_U32(n4, n3, n2, n1)                                                                  \
  ((u32)((u8)(n4) << 24) + (u32)((u8)(n3) << 16) + (u32)((u8)(n2) << 8) + (u32)((u8)(n1)))
#endif

#ifndef BREAK_U32
#define BREAK_U32(value, num) ((u8)(((value) >> ((num) * 8)) & 0xFF))
#endif

#define FOURTH_BYTE(nUint) ((u8)((nUint) >> 24))
#define THIRD_BYTE(nUint)  ((u8)((nUint) >> 16))
#define SECOND_BYTE(nUint) ((u8)((nUint) >> 8))
#define FIRST_BYTE(nUint)  ((u8)((nUint) & 0x000000ff))

#define U32_BYTES(nUint)                                                                           \
  FOURTH_BYTE(nUint), THIRD_BYTE(nUint), SECOND_BYTE(nUint), FIRST_BYTE(nUint)

#ifndef PLACE_BIT
#define PLACE_BIT(VAR, Place) ((VAR) |= (u16)(1 << (Place)))
#endif

#ifndef CLEAN_BIT
#define CLEAN_BIT(VAR, Place) ((VAR) &= (u16)((u16)(1 << (Place)) ^ 0xffff))
#endif

#ifndef CHECK_BIT
#define CHECK_BIT(VAR, Place) (((u16)(VAR) & (u16)(1 << (Place))) ? 1 : 0)
#endif

#ifndef INVERT_BIT
#define INVERT_BIT(VAR, Place) ((VAR) ^= (u16)((u16)1 << (Place)))
#endif

#ifndef ROUND
#define ROUND(value, size) (((value) + (size) - 1) & (~((size) - 1)))
#endif

#ifdef __cplusplus
}
#endif

#endif