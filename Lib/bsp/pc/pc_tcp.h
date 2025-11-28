#ifndef PC_TCP_H
#define PC_TCP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "define.h"

  extern bool Tcp_CompareTxBuffer(u8 channel, const u8 *pData, u16 length);

#ifdef __cplusplus
}
#endif

#endif