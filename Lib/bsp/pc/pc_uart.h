#ifndef PC_UART_H
#define PC_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "define.h"

extern bool Uart_CompareTxBuffer(u8 channel, const u8 *pData, u16 length);

#ifdef __cplusplus
}
#endif

#endif