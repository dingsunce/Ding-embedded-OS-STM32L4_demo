#ifndef _STM32LL_UART_IMPL_H
#define _STM32LL_UART_IMPL_H

#include "define.h"
#include "stm32ll.h"

// #define UART_HALF_DUPLEX
// #define UART_CONFIG

#define UART_RX_BUFFER_SIZE 50

typedef struct
{
  u8   RxBuffer[UART_RX_BUFFER_SIZE];
  bool IsSendCompleted;
  bool IsRcvPackage;
  bool IsRxBufferFull;
} UartImplRam_t;

typedef const struct
{
  UartImplRam_t *pRam;

  USART_TypeDef *USARTx;
  DMA_TypeDef   *DMAx;
  u32            TxDmaChannel;
  u32            RxDmaChannel;

  u32 (*DMA_IsActiveFlag_TC_Func)( DMA_TypeDef *DMAx);
  void (*DMA_ClearFlag_TC_Func)(DMA_TypeDef *DMAx);

#ifdef UART_HALF_DUPLEX
  GPIO_TypeDef *Port;
  u32           Pin;
#endif
} UartImpl_t;

extern void UartImpl_Init(UartImpl_t *pUartImpl);

extern void UartImpl_Send(UartImpl_t *pUartImpl, const u8 *pData, u16 length);
extern bool UartImpl_IsSendCompleted(UartImpl_t *pUartImpl);

extern void UartImpl_RestartRx(UartImpl_t *pUartImpl);
extern bool UartImpl_IsRcvPackage(UartImpl_t *pUartImpl);
extern u8  *UartImpl_GetRxData(UartImpl_t *pUartImpl, u16 *pLength);

extern void UartImpl_UartIRQHandler(UartImpl_t *pUartImpl);
extern void UartImpl_DmaIRQHandler(UartImpl_t *pUartImpl);

#endif