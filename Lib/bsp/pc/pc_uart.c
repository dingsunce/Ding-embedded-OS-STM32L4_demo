#include "d_mem.h"
#include "s_list.h"
#include "uart_func.h"

#define UART_CHANNEL_SUM 10

typedef struct
{
  LIST_HEADER;
  u8 *pData;
  u16 Length;
} UartTxElement_t;

typedef struct
{
  LIST_STRUCT(TxList);
} Uart_t;

static Uart_t Uart[UART_CHANNEL_SUM];

void Uart_Init(Uart_RcvFunc_t rcvFunc)
{
  for (u8 i = 0; i < UART_CHANNEL_SUM; i++)
  {
    LIST_STRUCT_INIT(&Uart[i], TxList);
  }
}

void Uart_Process(void)
{
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

static void FreeElement(UartTxElement_t *pElement)
{
  if (pElement != NULL)
  {
    if (pElement->pData != NULL)
    {
      DMem_Free(pElement->pData);
    }

    DMem_Free(pElement);
  }
}

bool Uart_CompareTxBuffer(u8 channel, const u8 *pData, u16 length)
{
  UartTxElement_t *pElement = List_Pop(Uart[channel].TxList);
  if (pElement == NULL)
  {
    return false;
  }

  if (pElement->Length != length)
  {
    return false;
  }

  bool arrayEqual = Util_CompareByteArray(pElement->pData, pData, length);

  FreeElement(pElement);

  return arrayEqual;
}