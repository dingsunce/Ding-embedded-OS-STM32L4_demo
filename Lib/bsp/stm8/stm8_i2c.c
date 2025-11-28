#include "d_mem.h"
#include "error.h"
#include "i2c_func.h"
#include "s_list.h"
#include "stm8_i2c_impl.h"

#define I2C_REPAIR_INTERVAL 10 // 10ms
#define I2C_TIMEOUT         10 // 10ms

typedef enum
{
  I2C_IDLE,
  I2C_BUSY,
} I2cMode_t;

typedef struct
{
  LIST_HEADER;
  I2cType_t       Type;
  u8              Add;
  u16             Memory;
  I2cMemorySize_t MemorySize;
  u8             *pData;
  u8              Length;
} I2cElement_t;

LIST(I2cList);
static I2cElement_t *pCurrent = NULL;

static I2cMode_t I2cMode = I2C_IDLE;
static u8        Timeout = 0;

static bool IsAvailable = true;

void I2c_Init(void)
{
  I2cImpl_Init();

  List_Init(I2cList);
}

static bool I2c_IsAvailableAndRepair(void)
{
  static u8 repairInterval = 0;

  if (I2cImpl_IsBusy())
  {
    IsAvailable = false;

    repairInterval++;
    if (repairInterval >= I2C_REPAIR_INTERVAL)
    {
      repairInterval = 0;
      I2cImpl_Reset();
    }
  }
  else
  {
    IsAvailable = true;

    repairInterval = 0;
  }

  return IsAvailable;
}

static void FreeCurrent(void)
{
  if (pCurrent != NULL)
  {
    if (pCurrent->Type == I2C_TX || pCurrent->Type == I2C_MEMORY_TX)
    {
      if (pCurrent->pData != NULL)
      {
        DMem_Free(pCurrent->pData);
      }
    }

    DMem_Free(pCurrent);
  }
}

void I2c_Process(void)
{
  OsErr_t result = OS_ERR_OK;

  switch (I2cMode)
  {
  case I2C_IDLE:
    if (!I2c_IsAvailableAndRepair())
    {
      return;
    }

    pCurrent = List_Pop(I2cList);
    if (pCurrent != NULL)
    {
      if (pCurrent->Type == I2C_TX)
      {
        result = I2cImpl_StartTx(pCurrent->Add, pCurrent->pData, pCurrent->Length);
      }
      else if (pCurrent->Type == I2C_RX)
      {
        result = I2cImpl_StartRx(pCurrent->Add, pCurrent->pData, pCurrent->Length);
      }
      else if (pCurrent->Type == I2C_MEMORY_TX)
      {
        result = I2cImpl_StartMemoryTx(pCurrent->Add, pCurrent->Memory, pCurrent->MemorySize,
                                       pCurrent->pData, pCurrent->Length);
      }
      else if (pCurrent->Type == I2C_MEMORY_RX)
      {
        result = I2cImpl_StartMemoryRx(pCurrent->Add, pCurrent->Memory, pCurrent->MemorySize,
                                       pCurrent->pData, pCurrent->Length);
      }

      if (result == OS_ERR_OK)
      {
        Timeout = 0;
        I2cMode = I2C_BUSY;
      }
      else
      {
        FreeCurrent();
      }
    }
    break;

  case I2C_BUSY:
    if (!I2cImpl_IsTxRxComplete())
    {
      Timeout++;
      if (Timeout >= I2C_TIMEOUT)
      {
        FreeCurrent();

        I2cMode = I2C_IDLE;

        I2cImpl_Reset();
      }
    }
    else
    {
      FreeCurrent();

      I2cMode = I2C_IDLE;
    }
    break;

  default:
    break;
  }
}

static OsErr_t I2c_StoreList(I2cType_t type, u8 add, u8 memory, I2cMemorySize_t memorySize,
                             u8 *pData, u8 length)
{
  if (!IsAvailable)
  {
    return OS_ERR_NOT_AVAILABLE;
  }

  I2cElement_t *pElement = DMem_Malloc(sizeof(I2cElement_t));
  if (pElement != NULL)
  {
    pElement->Type = type;
    pElement->Add = add;
    pElement->Memory = memory;
    pElement->MemorySize = memorySize;
    if (type == I2C_TX || type == I2C_MEMORY_TX)
    {
      pElement->pData = DMem_Malloc(length);
      if (pElement->pData != NULL)
      {
        memcpy(pElement->pData, pData, length);
      }
      else
      {
        DMem_Free(pElement);
        return OS_ERR_ALLOC;
      }
    }
    else
    {
      pElement->pData = pData;
    }
    pElement->Length = length;

    List_Add(I2cList, pElement);

    return OS_ERR_OK;
  }

  return OS_ERR_ALLOC;
}

OsErr_t I2c_StartTx(u8 add, u8 *pData, u8 length)
{
  return I2c_StoreList(I2C_TX, add, 0, I2C_MEMORY_8BIT, pData, length);
}

OsErr_t I2c_StartRx(u8 add, u8 *pData, u8 length)
{
  return I2c_StoreList(I2C_RX, add, 0, I2C_MEMORY_8BIT, pData, length);
}

OsErr_t I2c_StartMemoryTx(u8 add, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u8 length)
{
  return I2c_StoreList(I2C_MEMORY_TX, add, memory, memorySize, pData, length);
}

OsErr_t I2c_StartMemoryRx(u8 add, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u8 length)
{
  return I2c_StoreList(I2C_MEMORY_RX, add, memory, memorySize, pData, length);
}

OsErr_t I2c_MemoryTx(u8 add, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u8 length)
{
  return I2cImpl_MemoryTx(add, memory, memorySize, pData, length);
}

OsErr_t I2c_MemoryRx(u8 add, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u8 length)
{
  return I2cImpl_MemoryRx(add, memory, memorySize, pData, length);
}