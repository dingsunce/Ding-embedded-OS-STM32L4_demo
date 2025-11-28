#include "eeprom_emul.h"
#include "eeprom_func.h"

// Erase all eeprom pages
#pragma location = START_PAGE_ADDRESS
__root static const u8 Content[PAGE_SIZE * PAGES_NUMBER] = {[0 ...(PAGE_SIZE * PAGES_NUMBER - 1)] = 0xFF};

void Ee_Init(void) {
  HAL_FLASH_Unlock();

  EE_Init(EE_FORCED_ERASE);
}

void Ee_Process(void) {}

static EeAdd_t ConvertAdd(EeAdd_t add) {
  // address can't be 0x0000 or 0xFFFF
  if (add == 0) {
    add = 0xFFFE;
  }

  return add;
}

static EE_Status CleanUpIfRequired(EE_Status status) {
  /* Start cleanup polling mode, if cleanup is needed */
  if ((status & EE_STATUSMASK_CLEANUP) == EE_STATUSMASK_CLEANUP) {
    status |= EE_CleanUp();
  }

  return status;
}

u8 Ee_Read8(EeAdd_t add) {
  u8 data;
  EE_ReadVariable8bits(ConvertAdd(add), &data);
  return data;
}

u16 Ee_Read16(EeAdd_t add) {
  u16 data;
  EE_ReadVariable16bits(ConvertAdd(add), &data);
  return data;
}

u32 Ee_Read32(EeAdd_t add) {
#if defined(EE_ACCESS_32BITS)
  u32 data;
  EE_ReadVariable32bits(ConvertAdd(add), &data);
  return data;
#else
  return 0;
#endif
}

OsErr_t Ee_Read(EeAdd_t add, u8 *pData, u16 length);

void Ee_StartDirectWrite(void);
void Ee_StopDirectWrite(void);

OsErr_t Ee_Write8(EeAdd_t add, u8 value) {
  EE_Status status = EE_OK;
  status = EE_WriteVariable8bits(ConvertAdd(add), value);
  status |= CleanUpIfRequired(status);

  if (status == EE_OK) {
    return OS_ERR_OK;
  } else {
    return OS_ERR_FAIL;
  }
}

OsErr_t Ee_Write16(EeAdd_t add, u16 value) {
  EE_Status status = EE_OK;
  status = EE_WriteVariable16bits(ConvertAdd(add), value);
  status |= CleanUpIfRequired(status);

  if (status == EE_OK) {
    return OS_ERR_OK;
  } else {
    return OS_ERR_FAIL;
  }
}

OsErr_t Ee_Write32(EeAdd_t add, u32 value) {
#if defined(EE_ACCESS_32BITS)
  EE_Status status = EE_OK;
  status = EE_WriteVariable32bits(ConvertAdd(add), value);
  status |= CleanUpIfRequired(status);

  if (status == EE_OK) {
    return OS_ERR_OK;
  } else {
    return OS_ERR_FAIL;
  }
#else
  return OS_ERR_OK;
#endif
}

OsErr_t Ee_Write(EeAdd_t add, u8 *pData, u16 length);
