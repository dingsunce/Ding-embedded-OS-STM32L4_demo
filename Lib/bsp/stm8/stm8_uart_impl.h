#ifndef _STM8_UART_IMPL_H
#define _STM8_UART_IMPL_H

#include "define.h"

extern void UartImpl_Init(void);

extern void UartImpl_Send(u8 *pData, u8 length);
extern bool UartImpl_IsSendCompleted(void);

extern void UartImpl_RestartRx(void);
extern bool UartImpl_IsRcvPackage(void);
extern u8  *UartImpl_GetRxData(u16 *pLength);

#endif