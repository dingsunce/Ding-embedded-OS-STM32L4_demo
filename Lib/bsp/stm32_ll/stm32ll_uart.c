#include "stm32ll_uart.h"

#include "d_mem.h"
#include "s_list.h"
#include "uart_func.h"

typedef struct
{
  LIST_HEADER;
  u8 *pData;
  u16 Length;
} UartTxElement_t;

typedef struct
{
  bool             WithinTx;
  UartTxElement_t *pTxCurrent;
  LIST_STRUCT(TxList);
} Uart_t;

static Uart_t         Uart[UART_CHANNEL_SUM];
static Uart_RcvFunc_t Uart_RcvFunc;

static void Uart_TxProcess(void);
static void Uart_RxProcess(void);
static void FreeTxCurrent(u8 channel);

/*------------------------------------Init------------------------------------*/
void Uart_Init(Uart_RcvFunc_t rcvFunc)
{
  Uart_RcvFunc = rcvFunc;

  for (u8 i = 0; i < UART_CHANNEL_SUM; i++)
  {
    Uart[i].WithinTx = false;

    LIST_STRUCT_INIT(&Uart[i], TxList);

    UartImpl_Init(UartImpl + i);
  }
}

/*----------------------------------Process-----------------------------------*/
void Uart_Process(void)
{
  Uart_TxProcess();
  Uart_RxProcess();
}

/*-------------------------------------Tx-------------------------------------*/
static void Uart_TxProcess(void)
{
  for (u8 i = 0; i < UART_CHANNEL_SUM; i++)
  {
    if (!Uart[i].WithinTx)
    {
      Uart[i].pTxCurrent = List_Pop(Uart[i].TxList);
      if (Uart[i].pTxCurrent != NULL)
      {
        Uart[i].WithinTx = true;

        UartImpl_Send(UartImpl + i, Uart[i].pTxCurrent->pData, Uart[i].pTxCurrent->Length);
      }
    }
    else
    {
      if (UartImpl_IsSendCompleted(UartImpl + i))
      {
        FreeTxCurrent(i);

        Uart[i].WithinTx = false;
      }
    }
  }
}

OsErr_t Uart_Send(u8 channel, const u8 *pData, u16 length)
{
  UartTxElement_t *pElement = DMem_Malloc(sizeof(UartTxElement_t));
  u8              *pElementData = DMem_Malloc(length);
  if (pElement != NULL && pElementData != NULL)
  {
    memcpy(pElementData, pData, length);
    pElement->pData = pElementData;
    pElement->Length = length;

    List_Add(Uart[channel].TxList, pElement);

    return OS_ERR_OK;
  }
  else
  {
    if (pElement != NULL)
    {
      DMem_Free(pElement);
    }

    if (pElementData != NULL)
    {
      DMem_Free(pElementData);
    }

    return OS_ERR_ALLOC;
  }
}

static void FreeTxCurrent(u8 channel)
{
  if (Uart[channel].pTxCurrent != NULL)
  {
    if (Uart[channel].pTxCurrent->pData != NULL)
    {
      DMem_Free(Uart[channel].pTxCurrent->pData);
    }

    DMem_Free(Uart[channel].pTxCurrent);
  }
}

/*-------------------------------------Rx-------------------------------------*/
static void Uart_RxProcess(void)
{
  for (u8 i = 0; i < UART_CHANNEL_SUM; i++)
  {
    if (UartImpl_IsRcvPackage(UartImpl + i))
    {
      u16 length;
      u8 *pData = UartImpl_GetRxData(UartImpl + i, &length);

      if (Uart_RcvFunc != NULL)
      {
        Uart_RcvFunc(i, pData, length);
      }

      UartImpl_RestartRx(UartImpl + i);
    }
  }
}