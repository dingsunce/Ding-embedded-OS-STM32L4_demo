#ifndef _TCP_FUNC_H
#define _TCP_FUNC_H

#include "define.h"
#include "error.h"

typedef void (*Tcp_RcvFunc_t)(u8 channel, const u8 *pData, u16 length);

extern void    Tcp_Init(u8 channel, Tcp_RcvFunc_t rcvFunc);
extern OsErr_t Tcp_Send(u8 channel, const u8 *pData, u16 length);

#endif