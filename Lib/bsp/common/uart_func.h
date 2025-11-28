#ifndef _UART_FUNC_H
#define _UART_FUNC_H

#include "define.h"
#include "error.h"

typedef void (*Uart_RcvFunc_t)(u8 channel, const u8 *pData, u16 length);

extern void    Uart_Init(Uart_RcvFunc_t rcvFunc);
extern void    Uart_Process(void);
extern OsErr_t Uart_Send(u8 channel, const u8 *pData, u16 length);

#endif