#include "d_list.h"
#include "d_mem.h"
#include "d_tick.h"
#include "eeprom_func.h"
#include "stm32ll_i2c_impl.h"
#include "wdg_func.h"

// #define EXTERNAL_EEPROM_CONFIG

#ifndef EXTERNAL_EEPROM_CONFIG
#define AT24C16
#endif

// #define AT24C01
// #define AT24C02
// #define AT24C04
// #define AT24C08
// #define AT24C16
// #define AT24C32
// #define AT24C64
// #define AT24C128
// #define AT24C256
// #define AT24C512

#if defined AT24C01
#define EE_PAGE_SIZE       8
#define EE_TOTAL_SIZE      128
#define EE_I2C_MEMORY_SIZE I2C_MEMORY_8BIT

#elif defined AT24C02
#define EE_PAGE_SIZE       8
#define EE_TOTAL_SIZE      256
#define EE_I2C_MEMORY_SIZE I2C_MEMORY_8BIT

#elif defined AT24C04
#define EE_PAGE_SIZE       16
#define EE_TOTAL_SIZE      512
#define EE_I2C_MEMORY_SIZE I2C_MEMORY_8BIT

#elif defined AT24C08
#define EE_PAGE_SIZE       16
#define EE_TOTAL_SIZE      1024
#define EE_I2C_MEMORY_SIZE I2C_MEMORY_8BIT

#elif defined AT24C16
#define EE_PAGE_SIZE       16
#define EE_TOTAL_SIZE      2048
#define EE_I2C_MEMORY_SIZE I2C_MEMORY_8BIT

#elif defined AT24C32
#define EE_PAGE_SIZE       32
#define EE_TOTAL_SIZE      4096
#define EE_I2C_MEMORY_SIZE I2C_MEMORY_16BIT

#elif defined AT24C64
#define EE_PAGE_SIZE       32
#define EE_TOTAL_SIZE      8192
#define EE_I2C_MEMORY_SIZE I2C_MEMORY_16BIT

#elif defined AT24C128
#define EE_PAGE_SIZE       64
#define EE_TOTAL_SIZE      16384
#define EE_I2C_MEMORY_SIZE I2C_MEMORY_16BIT

#elif defined AT24C256
#define EE_PAGE_SIZE       64
#define EE_TOTAL_SIZE      32768
#define EE_I2C_MEMORY_SIZE I2C_MEMORY_16BIT

#elif defined AT24C512
#define EE_PAGE_SIZE       128
#define EE_TOTAL_SIZE      65536
#define EE_I2C_MEMORY_SIZE I2C_MEMORY_16BIT

#endif

#define AT_DEVICE_ADD (0x50 << 1)

#define EE_WAIT_TIMEOUT 5  // 5ms
#define EE_WRITE_TIME   15 // 15ms

// #define EE_DIRECT_WRITE
// #define EE_BUFFER_WRITE

#ifndef EE_DIRECT_WRITE
#ifndef EE_BUFFER_WRITE
typedef struct
{
  DLIST_HEADER;
  EeAdd_t Addr;
  u8      Value;
} EeElement_t;
#else
typedef struct
{
  DLIST_HEADER;
  EeAdd_t Addr;
  u8     *pData;
  u8      Length;
} EeElement_t;
#endif

DLIST(EeList);

static bool DirectWrite = false;
#endif

/*--------------------------------Chip choose--------------------------------*/
#if defined AT24C04 || defined AT24C08 || defined AT24C16
static u8                      GetDeviceAddr(EeAdd_t addr)
{
  return AT_DEVICE_ADD | (HI_U16(addr) << 1);
}

#else
#define GetDeviceAddr(addr) AT_DEVICE_ADD

#endif

/*-----------------------------Init and process------------------------------*/
void Ee_Init(void)
{
#ifndef EE_DIRECT_WRITE
  DList_Init(EeList);
#endif
}

// Call this function every 20ms, because write cycle time of AT24Cxx is 10ms.
void Ee_Process(void)
{
#ifndef EE_DIRECT_WRITE
  EeElement_t *pElement = DList_Pop(EeList);
  if (pElement != NULL)
  {
#ifndef EE_BUFFER_WRITE
    I2c_StartMemoryTx(GetDeviceAddr(pElement->Addr), pElement->Addr, EE_I2C_MEMORY_SIZE,
                      &pElement->Value, 1);
#else
    I2c_StartMemoryTx(GetDeviceAddr(pElement->Addr), pElement->Addr, EE_I2C_MEMORY_SIZE,
                      pElement->pData, pElement->Length);
#endif
    DMem_Free(pElement);
  }
#endif
}

static OsErr_t WaitI2cTxRxComplete(void)
{
  u32 startTick = DTick_Now();
  while (!I2cImpl_IsTxRxComplete())
  {
    if ((DTick_Now() - startTick) >= EE_WAIT_TIMEOUT)
    {
      return OS_ERR_BUSY;
    }
  }

  return OS_ERR_OK;
}

/*-----------------------------------Read------------------------------------*/
OsErr_t Ee_Read(EeAdd_t addr, u8 *pData, u16 length)
{
  WaitI2cTxRxComplete();

  I2cImpl_StartMemoryRx(GetDeviceAddr(addr), addr, EE_I2C_MEMORY_SIZE, pData, length);

  WaitI2cTxRxComplete();

  return OS_ERR_OK;
}

u8 Ee_Read8(EeAdd_t addr)
{
  u8 data = 0;

  Ee_Read(addr, &data, 1);

  return data;
}

u16 Ee_Read16(EeAdd_t addr)
{
  u8 data[2] = {0};

  Ee_Read(addr, data, 2);

  return BUILD_U16(data[0], data[1]);
}

u32 Ee_Read32(EeAdd_t addr)
{
  u8 data[4] = {0};

  Ee_Read(addr, data, 4);

  return BUILD_U32(data[0], data[1], data[2], data[3]);
}

/*--------------------------------Byte write---------------------------------*/
#ifndef EE_BUFFER_WRITE
#ifndef EE_DIRECT_WRITE
static bool Ee_CheckExistAndUpdate(EeAdd_t addr, u8 value)
{
  EeElement_t *pElement = (EeElement_t *)DList_Head(EeList);

  while (pElement != NULL)
  {
    if (pElement->Addr == addr)
    {
      pElement->Value = value;

      return true;
    }

    pElement = (EeElement_t *)DList_ItemNext(pElement);
  }

  return false;
}
#endif

void Ee_StartDirectWrite(void)
{
#ifndef EE_DIRECT_WRITE
  WaitI2cTxRxComplete();

  EeElement_t *pElement = DList_Pop(EeList);
  while (pElement != NULL)
  {
    I2cImpl_StartMemoryTx(GetDeviceAddr(pElement->Addr), pElement->Addr, EE_I2C_MEMORY_SIZE,
                          &pElement->Value, 1);
    WaitI2cTxRxComplete();

    DMem_Free(pElement);

    pElement = DList_Pop(EeList);
  }

  DirectWrite = true;
#endif
}

void Ee_StopDirectWrite(void)
{
#ifndef EE_DIRECT_WRITE
  DirectWrite = false;
#endif
}

static void WaitEeWriteComplete(void)
{
  u32 startTick = DTick_Now();
  while ((DTick_Now() - startTick) < EE_WRITE_TIME)
  {
  }
}

OsErr_t Ee_Write8(EeAdd_t addr, u8 value)
{
#ifndef EE_DIRECT_WRITE
  if (!DirectWrite)
  {
    if (Ee_CheckExistAndUpdate(addr, value))
    {
      return OS_ERR_OK;
    }

    EeElement_t *pElement = DMem_Malloc(sizeof(EeElement_t));
    if (pElement != NULL)
    {
      pElement->Addr = addr;
      pElement->Value = value;

      DList_Add(EeList, pElement);
    }
    else
    {
      return OS_ERR_ALLOC;
    }
  }
  else
  {
    Wdg_Clear();

    WaitI2cTxRxComplete();

    I2cImpl_StartMemoryTx(GetDeviceAddr(addr), addr, EE_I2C_MEMORY_SIZE, &value, 1);

    WaitI2cTxRxComplete();
    WaitEeWriteComplete();
  }

  return OS_ERR_OK;
#else
  Wdg_Clear();

  WaitI2cTxRxComplete();

  I2cImpl_StartMemoryTx(GetDeviceAddr(addr), addr, EE_I2C_MEMORY_SIZE, &value, 1);

  WaitI2cTxRxComplete();
  WaitEeWriteComplete();

  return OS_ERR_OK;
#endif
}

OsErr_t Ee_Write16(EeAdd_t addr, u16 value)
{
  OsErr_t err;

  err = Ee_Write8(addr, HI_U16(value));
  if (err != OS_ERR_OK)
  {
    return err;
  }

  err = Ee_Write8(addr + 1, LO_U16(value));
  return err;
}

OsErr_t Ee_Write32(EeAdd_t addr, u32 value)
{
  OsErr_t err;

  for (u8 i = 0; i < 4; i++)
  {
    err = Ee_Write8(addr + i, BREAK_U32(value, 3 - i));
    if (err != OS_ERR_OK)
    {
      return err;
    }
  }

  return err;
}

bool Ee_IsEmpty(void)
{
#ifndef EE_DIRECT_WRITE
  return DList_Length(EeList) == 0;
#else
  return true;
#endif
}

/*-------------------------------Buffer write--------------------------------*/
#else
static EeAdd_t GetLastAddOfPage(EeAdd_t startAdd)
{
  return (startAdd / EE_PAGE_SIZE + 1) * EE_PAGE_SIZE - 1;
}

static OsErr_t Ee_StoreBufferList(EeAdd_t addr, u8 *pData, u16 length)
{
  EeElement_t *pElement = DMem_Malloc(sizeof(EeElement_t));
  u8          *pElementData = DMem_Malloc(length);
  if (pElement != NULL && pElementData != NULL)
  {
    pElement->Addr = addr;
    memcpy(pElementData, pData, length);
    pElement->pData = pData;
    pElement->Length = length;
    DList_Add(EeList, pElement);

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

OsErr_t Ee_Write(EeAdd_t addr, u8 *pData, u16 length)
{
  OsErr_t err;
  u8      pageLength;
  EeAdd_t lastAddOfPage;

  while (length > 0)
  {
    lastAddOfPage = GetLastAddOfPage(addr);
    if (addr + length <= lastAddOfPage) // Within one page
    {
      pageLength = length;
    }
    else
    {
      pageLength = lastAddOfPage - addr + 1;
    }

    if (!DirectWrite)
    {
      err = Ee_StoreBufferList(addr, pData, pageLength);
      if (err != OS_ERR_OK)
      {
        return err;
      }
    }
    else
    {
      Wdg_Clear();

      WaitI2cTxRxComplete();

      I2cImpl_StartMemoryTx(GetDeviceAddr(addr), addr, EE_I2C_MEMORY_SIZE, pData, pageLength);

      WaitI2cTxRxComplete();
      WaitEeWriteComplete();
    }

    addr += pageLength;
    pData += pageLength;
    length -= pageLength;
  }

  return OS_ERR_OK;
}

OsErr_t Ee_Write8(EeAdd_t addr, u8 value)
{
  return Ee_Write(addr, &value, 1);
}

OsErr_t Ee_Write16(EeAdd_t addr, u16 value)
{
  u8 data[2];

  data[0] = HIGH_U16(value);
  data[0] = LOW_U16(value);

  return Ee_Write(addr, data, 2);
}

OsErr_t Ee_Write32(EeAdd_t addr, u32 value)
{
  u8 data[4];
  for (u8 i = 0; i < 4; i++)
  {
    data[i] = BREAK_U32(value, 3 - i);
  }

  return Ee_Write(addr, data, 4);
}

#endif
