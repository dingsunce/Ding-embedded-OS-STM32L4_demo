#include "tcp_func.h"

#define UART_CHANNEL   10
#define TX_BUFFER_SIZE 256

static u8  TxBuffer[UART_CHANNEL][TX_BUFFER_SIZE];
static u16 TxLength[UART_CHANNEL];

void Tcp_Init(u8 channel, Tcp_RcvFunc_t rcvFunc)
{
}

OsErr_t Tcp_Send(u8 channel, const u8 *pData, u16 length)
{
  TxLength[channel] = length;
  memcpy(TxBuffer[channel], pData, length);

  return OS_ERR_OK;
}

bool Tcp_CompareTxBuffer(u8 channel, const u8 *pData, u16 length)
{
  if (TxLength[channel] != length)
  {
    return false;
  }

  for (u8 i = 0; i < length; i++)
  {
    if (TxBuffer[channel][i] != pData[i])
    {
      return false;
    }
  }

  return true;
}