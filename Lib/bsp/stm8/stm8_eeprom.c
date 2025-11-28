#include "d_mem.h"
#include "eeprom_func.h"
#include "s_list.h"
#include "stm8l15x_conf.h"
#include "wdg_func.h"

typedef struct
{
  LIST_HEADER;
  EeAdd_t Add;   // Address of the stored element.
  u8      Value; // Value of the stored element.
} EeElement_t;

LIST(EeList);
static bool DirectWrite = false;

/*-----------------------------Init and process------------------------------*/
void Ee_Init(void)
{
  List_Init(EeList);

  FLASH_Unlock(FLASH_MemType_Data);
}

// Call this function every 10ms, because write cycle time of STM8 is 5ms.
void Ee_Process(void)
{
  if (!FLASH_GetFlagStatus(FLASH_FLAG_HVOFF))
  { // Last operation has not finished.
    return;
  }

  EeElement_t *pElement = List_Pop(EeList);
  if (pElement != NULL)
  {
    FLASH_ProgramByte((u32)pElement->Add, pElement->Value);

    DMem_Free(pElement);
  }
}

/*-----------------------------------Read------------------------------------*/
u8 Ee_Read8(EeAdd_t add)
{
  return FLASH_ReadByte((u32)add);
}

u16 Ee_Read16(EeAdd_t add)
{
  u8 valueH = FLASH_ReadByte((u32)add);
  u8 valueL = FLASH_ReadByte((u32)(add + 1));

  return BUILD_U16(valueH, valueL);
}

u32 Ee_Read32(EeAdd_t add)
{
  u8 value3 = FLASH_ReadByte((u32)add);
  u8 value2 = FLASH_ReadByte((u32)(add + 1));
  u8 value1 = FLASH_ReadByte((u32)(add + 2));
  u8 value0 = FLASH_ReadByte((u32)(add + 3));

  return BUILD_U32(value3, value2, value1, value0);
}

/*-----------------------------------Write-----------------------------------*/
static bool Ee_CheckExistAndUpdate(EeAdd_t add, u8 value)
{
  EeElement_t *pElement = (EeElement_t *)List_Head(EeList);

  while (pElement != NULL)
  {
    if (pElement->Add == add)
    {
      pElement->Value = value;

      return true;
    }

    pElement = (EeElement_t *)List_ItemNext(pElement);
  }

  return false;
}

void Ee_StartDirectWrite(void)
{
  FLASH_WaitForLastOperation(FLASH_MemType_Data);

  EeElement_t *pElement = List_Pop(EeList);
  while (pElement != NULL)
  {
    FLASH_ProgramByte((u32)pElement->Add, pElement->Value);
    FLASH_WaitForLastOperation(FLASH_MemType_Data);

    DMem_Free(pElement);

    pElement = List_Pop(EeList);
  }

  DirectWrite = true;
}

void Ee_StopDirectWrite(void)
{
  DirectWrite = false;
}

OsErr_t Ee_Write8(EeAdd_t add, u8 value)
{
  if (!DirectWrite)
  {
    if (Ee_CheckExistAndUpdate(add, value))
    {
      return OS_ERR_OK;
    }

    if (Ee_Read8(add) == value)
    {
      return OS_ERR_OK;
    }

    EeElement_t *pElement = DMem_Malloc(sizeof(EeElement_t));
    if (pElement != NULL)
    {
      pElement->Add = add;
      pElement->Value = value;

      List_Add(EeList, pElement);
    }
    else
    {
      return OS_ERR_ALLOC;
    }
  }
  else
  {
    if (Ee_Read8(add) == value)
    {
      return OS_ERR_OK;
    }

    Wdg_Clear();

    FLASH_ProgramByte((u32)add, value);
    FLASH_WaitForLastOperation(FLASH_MemType_Data);
    Wdg_Clear();
  }

  return OS_ERR_OK;
}

OsErr_t Ee_Write16(EeAdd_t add, u16 value)
{
  OsErr_t err;

  err = Ee_Write8(add, HIGH_U16(value));
  if (err != OS_ERR_OK)
  {
    return err;
  }

  err = Ee_Write8(add + 1, LOW_U16(value));
  return err;
}

OsErr_t Ee_Write32(EeAdd_t add, u32 value)
{
  OsErr_t err;

  for (u8 i = 0; i < 4; i++)
  {
    err = Ee_Write8(add + i, BREAK_U32(value, 3 - i));
    if (err != OS_ERR_OK)
    {
      return err;
    }
  }

  return err;
}

bool Ee_IsEmpty(void)
{
  return List_Length(EeList) == 0;
}