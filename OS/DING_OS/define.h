/*!*****************************************************************************
 * file		define.h
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _DEFINE_H
#define _DEFINE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "osal.h"

#ifndef NULL
#define NULL 0
#endif

  typedef u16   MsgId_t;    // Message id type.
  typedef u32   MsgDelay_t; // Message delay type.
  typedef void *MsgArg_t;   // Message argument type.

#ifndef LOBYTE
#define LOBYTE(x) ((u8)((x) & 0x00ff))
#endif /* LOBYTE */

#ifndef HIBYTE
#define HIBYTE(x) ((u8)((x) >> 8))
#endif /* HIBYTE */

#define HILO(nWord)             HIBYTE(nWord), LOBYTE(nWord)
#define LOHI(nWord)             LOBYTE(nWord), HIBYTE(nWord)
#define BUILD_WORD(nHigh, nLow) ((u16)((u8)(nHigh) << 8) + (u16)((u8)(nLow)))

#define FOURTH_BYTE(nUint) ((u8)((nUint) >> 24))
#define THIRD_BYTE(nUint)  ((u8)((nUint) >> 16))
#define SECOND_BYTE(nUint) ((u8)((nUint) >> 8))
#define FIRST_BYTE(nUint)  ((u8)((nUint) & 0x000000ff))

#define UINT_BYTES(nUint)                                                                          \
  FOURTH_BYTE(nUint), THIRD_BYTE(nUint), SECOND_BYTE(nUint), FIRST_BYTE(nUint)
#define BUILD_UINT(n4, n3, n2, n1)                                                                 \
  ((u32)((u8)(n4) << 24) + (u32)((u8)(n3) << 16) + (u32)((u8)(n2) << 8) + (u32)((u8)(n1)))

// knx bus is big endian
#define KNX_HIBYTE(nWord) ((u8)((nWord) & 0x00ff))
#define KNX_LOBYTE(nWord) ((u8)((nWord) >> 8))
#define KNX_LOHI(nWord)   KNX_LOBYTE(nWord), KNX_HIBYTE(nWord)
#define KNX_HILO(nWord)   KNX_HIBYTE(nWord), KNX_LOBYTE(nWord)

#define KNX_FOURTH_BYTE(nUint) ((u8)((nUint) & 0x000000ff))
#define KNX_THIRD_BYTE(nUint)  ((u8)((nUint) >> 8))
#define KNX_SECOND_BYTE(nUint) ((u8)((nUint) >> 16))
#define KNX_FIRST_BYTE(nUint)  ((u8)((nUint) >> 24))
#define KNX_BUILD_UINT(n4, n3, n2, n1)                                                             \
  ((u32)((u8)(n1) << 24) + (u32)((u8)(n2) << 16) + (u32)((u8)(n3) << 8) + (u32)((u8)(n4)))

#define PLACE_BIT(VAR, Place) ((VAR) |= (u16)(1 << (Place)))
#define CLEAN_BIT(VAR, Place) ((VAR) &= (u16)((u16)(1 << (Place)) ^ 0xffff))
#define CHECK_BIT(VAR, Place) (((u16)(VAR) & (u16)(1 << (Place))) ? 1 : 0)

#ifndef BREAK_U32
#define BREAK_U32(value, num) ((u8)(((value) >> ((num) * 8)) & 0xFF))
#endif

#ifndef BUILD_U32
#define BUILD_U32(byte3, byte2, byte1, byte0)                                                      \
  ((u32)(((u32)((byte3) & 0xFF) << 24) + ((u32)((byte2) & 0xFF) << 16) +                           \
         ((u32)((byte1) & 0xFF) << 8) + ((u32)((byte0) & 0xFF))))
#endif

#ifndef BUILD_U16
#define BUILD_U16(highByte, lowByte)                                                               \
  ((u16)((u16)((u16)((highByte) & 0xFF) << 8) + (u16)((lowByte) & 0xFF)))
#endif

#ifndef HIGH_U16
#define HIGH_U16(value) ((u8)(((value) >> 8) & 0xFF))
#endif

#ifndef LOW_U16
#define LOW_U16(value) ((u8)((value) & 0xFF))
#endif

#ifndef LOW_U8
#define LOW_U8(value, num) ((u8)((value) & (u8)(((u16)1 << (num)) - 1)))
#endif

#ifndef HIGH_U8
#define HIGH_U8(value, num) ((u8)((value) >> (u8)(8 - (num))))
#endif

#ifndef BUILD_U8
#define BUILD_U8(highByte, highNum, lowByte, lowNum)                                               \
  (u8)((u8)(LOW_U8(highByte, highNum) << (lowNum)) + LOW_U8(lowByte, lowNum))
#endif

#ifndef GET_U8
#define GET_U8(value, lowIndex, highIndex)                                                         \
  ((u8)((u8)((value) & (u8)(((u16)1 << ((highIndex) + 1)) - 1)) >> (lowIndex)))
#endif

#ifndef GET_BIT
#define GET_BIT(value, index) ((bool)(((value) >> (index)) & 0x1))
#endif

#ifndef PLACE_BIT
#define PLACE_BIT(value, index) ((value) |= (u8)((u8)1 << (index)))
#endif

#ifndef RESET_BIT
#define RESET_BIT(value, index) ((value) &= (u8)(~(u8)((u8)1 << (index))))
#endif

#ifndef INVERT_BIT
#define INVERT_BIT(value, index) ((value) ^= (u32)((u32)1 << (index)))
#endif

#ifndef ROUND
#define ROUND(value, size) (((value) + (size) - 1) & (~((size) - 1)))
#endif

#ifdef __cplusplus
}
#endif

#endif