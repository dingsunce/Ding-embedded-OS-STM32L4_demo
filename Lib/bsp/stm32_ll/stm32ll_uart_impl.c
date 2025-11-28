#include "stm32ll_uart_impl.h"

/*------------------------------------Init------------------------------------*/
void UartImpl_Init(UartImpl_t *pUartImpl)
{
  /* Enable DMA TX Requests */
  LL_USART_EnableDMAReq_TX(pUartImpl->USARTx);

  /* Enable DMA RX Requests */
  LL_USART_EnableDMAReq_RX(pUartImpl->USARTx);

  LL_USART_ClearFlag_TC(pUartImpl->USARTx);
  LL_USART_EnableIT_TC(pUartImpl->USARTx);

  LL_USART_ClearFlag_IDLE(pUartImpl->USARTx);
  LL_USART_EnableIT_IDLE(pUartImpl->USARTx);

  LL_USART_EnableIT_ERROR(pUartImpl->USARTx);

  /* Configure the DMA functional parameters for reception */
#ifdef STM32_SERIAL_F1
  LL_DMA_SetPeriphAddress(pUartImpl->DMAx, pUartImpl->TxDmaChannel,
                          LL_USART_DMA_GetRegAddr(pUartImpl->USARTx));

  LL_DMA_ConfigAddresses(pUartImpl->DMAx, pUartImpl->RxDmaChannel, LL_USART_DMA_GetRegAddr(pUartImpl->USARTx),
                         (u32)pUartImpl->pRam->RxBuffer, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
#elif defined STM32_SERIAL_G0
  LL_DMA_SetPeriphAddress(pUartImpl->DMAx, pUartImpl->TxDmaChannel,
                          LL_USART_DMA_GetRegAddr(pUartImpl->USARTx, LL_USART_DMA_REG_DATA_TRANSMIT));

  LL_DMA_ConfigAddresses(pUartImpl->DMAx, pUartImpl->RxDmaChannel,
                         LL_USART_DMA_GetRegAddr(pUartImpl->USARTx, LL_USART_DMA_REG_DATA_RECEIVE),
                         (u32)pUartImpl->pRam->RxBuffer, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
#endif
  LL_DMA_SetDataLength(pUartImpl->DMAx, pUartImpl->RxDmaChannel, UART_RX_BUFFER_SIZE);

  LL_DMA_EnableIT_TC(pUartImpl->DMAx, pUartImpl->RxDmaChannel);

  /* Enable DMA Channel Rx */
  LL_DMA_EnableChannel(pUartImpl->DMAx, pUartImpl->RxDmaChannel);
}

/*-------------------------------------Tx-------------------------------------*/
void UartImpl_Send(UartImpl_t *pUartImpl, const u8 *pData, u16 length)
{
  pUartImpl->pRam->IsSendCompleted = false;

#ifdef UART_HALF_DUPLEX
  LL_GPIO_SetOutputPin(pUartImpl->Port, pUartImpl->Pin);
#endif

  /* Disable DMA Channel Tx */
  LL_DMA_DisableChannel(pUartImpl->DMAx, pUartImpl->TxDmaChannel);

  LL_DMA_SetMemoryAddress(pUartImpl->DMAx, pUartImpl->TxDmaChannel, (u32)pData);

  LL_DMA_SetDataLength(pUartImpl->DMAx, pUartImpl->TxDmaChannel, length);

  /* Enable DMA Channel Tx */
  LL_DMA_EnableChannel(pUartImpl->DMAx, pUartImpl->TxDmaChannel);
}

bool UartImpl_IsSendCompleted(UartImpl_t *pUartImpl)
{
  return pUartImpl->pRam->IsSendCompleted;
}

/*-------------------------------------Rx-------------------------------------*/
void UartImpl_RestartRx(UartImpl_t *pUartImpl)
{
  pUartImpl->pRam->IsRcvPackage = false;
  pUartImpl->pRam->IsRxBufferFull = false;

  LL_DMA_SetDataLength(pUartImpl->DMAx, pUartImpl->RxDmaChannel, UART_RX_BUFFER_SIZE);

  /* Enable DMA Channel Rx */
  LL_DMA_EnableChannel(pUartImpl->DMAx, pUartImpl->RxDmaChannel);
}

bool UartImpl_IsRcvPackage(UartImpl_t *pUartImpl)
{
  return pUartImpl->pRam->IsRcvPackage;
}

u8 *UartImpl_GetRxData(UartImpl_t *pUartImpl, u16 *pLength)
{
  *pLength = UART_RX_BUFFER_SIZE - LL_DMA_GetDataLength(pUartImpl->DMAx, pUartImpl->RxDmaChannel);

  return pUartImpl->pRam->RxBuffer;
}

/*---------------------------------Interrupt----------------------------------*/
void UartImpl_UartIRQHandler(UartImpl_t *pUartImpl)
{
  /* Check TC flag value in SR register */
  if (LL_USART_IsActiveFlag_TC(pUartImpl->USARTx))
  {
    /* Clear TC flag */
    LL_USART_ClearFlag_TC(pUartImpl->USARTx);

    pUartImpl->pRam->IsSendCompleted = true;

#ifdef UART_HALF_DUPLEX
    LL_GPIO_ResetOutputPin(pUartImpl->Port, pUartImpl->Pin);
#endif
  }
  /* Check IDLE flag value in ISR register */
  else if (LL_USART_IsActiveFlag_IDLE(pUartImpl->USARTx))
  {
    /* Clear IDLE flag */
    LL_USART_ClearFlag_IDLE(pUartImpl->USARTx);

    /* Disable DMA Channel Rx */
    LL_DMA_DisableChannel(pUartImpl->DMAx, pUartImpl->RxDmaChannel);

    if (!pUartImpl->pRam->IsRxBufferFull)
    {
      pUartImpl->pRam->IsRcvPackage = true;
    }
    else
    {
      UartImpl_RestartRx(pUartImpl);
    }
  }
  // Error
  else
  {
    // If DMA rx buffer is full and new data follows, overrun error will be set
    if (LL_USART_IsActiveFlag_ORE(pUartImpl->USARTx))
    { // OverRun Error
      LL_USART_ClearFlag_ORE(pUartImpl->USARTx);
    }

    if (LL_USART_IsActiveFlag_FE(pUartImpl->USARTx))
    { // Framing Error
      LL_USART_ClearFlag_FE(pUartImpl->USARTx);
    }

    if (LL_USART_IsActiveFlag_NE(pUartImpl->USARTx))
    { // Noise error
      LL_USART_ClearFlag_NE(pUartImpl->USARTx);
    }
  }
}

void UartImpl_DmaIRQHandler(UartImpl_t *pUartImpl)
{
  if (pUartImpl->DMA_IsActiveFlag_TC_Func(pUartImpl->DMAx))
  {
    pUartImpl->DMA_ClearFlag_TC_Func(pUartImpl->DMAx);

    pUartImpl->pRam->IsRxBufferFull = true;

    // Abandon the received data and start new reception.
    LL_DMA_DisableChannel(pUartImpl->DMAx, pUartImpl->RxDmaChannel);
    LL_DMA_SetDataLength(pUartImpl->DMAx, pUartImpl->RxDmaChannel, UART_RX_BUFFER_SIZE);
    LL_DMA_EnableChannel(pUartImpl->DMAx, pUartImpl->RxDmaChannel);
  }
}