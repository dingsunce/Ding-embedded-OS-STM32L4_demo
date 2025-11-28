#ifndef STM32LL_UART_H
#define STM32LL_UART_H

#include "stm32ll_uart_impl.h"

#ifndef UART_CONFIG
#define UART_CHANNEL_SUM 1
#endif

extern UartImpl_t UartImpl[UART_CHANNEL_SUM];

#endif