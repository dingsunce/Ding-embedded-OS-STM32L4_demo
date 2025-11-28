#include "d_mem.h"
#include "s_list.h"
#include "stm8_uart_impl.h"
#include "uart_func.h"

typedef struct
{
  LIST_HEADER;
  u8 *pData;
  u16 Length;
} UartElement_t;

LIST(TxList);
static bool           WithinTx = false;
static UartElement_t *pTxCurrent;
static Uart_RcvFunc_t Uart_RcvFunc;

static void Uart_TxProcess(void);
static void Uart_RxProcess(void);

void Uart_Init(Uart_RcvFunc_t rcvFunc)
{
  Uart_RcvFunc = rcvFunc;

  UartImpl_Init();

  List_Init(TxList);
}

void Uart_Process(void)
{
  Uart_TxProcess();
  Uart_RxProcess();
}

static void FreeTxCurrent(void)
{
  if (pTxCurrent != NULL)
  {
    if (pTxCurrent->pData != NULL)
    {
      DMem_Free(pTxCurrent->pData);
    }

    DMem_Free(pTxCurrent);
  }
}

static void Uart_TxProcess(void)
{
  if (!WithinTx)
  {
    pTxCurrent = List_Pop(TxList);
    if (pTxCurrent != NULL)
    {
      WithinTx = true;

      UartImpl_Send(pTxCurrent->pData, pTxCurrent->Length);
    }
  }
  else
  {
    if (UartImpl_IsSendCompleted())
    {
      FreeTxCurrent();

      WithinTx = false;
    }
  }
}

OsErr_t Uart_Send(u8 channel, const u8 *pData, u16 length)
{
  UartElement_t *pElement = DMem_Malloc(sizeof(UartElement_t));
  u8            *pElementData = DMem_Malloc(length);
  if (pElement != NULL && pElementData != NULL)
  {
    memcpy(pElementData, pData, length);
    pElement->pData = pElementData;
    pElement->Length = length;

    List_Add(TxList, pElement);

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

static void Uart_RxProcess(void)
{
  if (UartImpl_IsRcvPackage())
  {
    u16 length;
    u8 *pData = UartImpl_GetRxData(&length);

    if (Uart_RcvFunc != NULL)
    {
      Uart_RcvFunc(0, pData, length);
    }

    UartImpl_RestartRx();
  }
}