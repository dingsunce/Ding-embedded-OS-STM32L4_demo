/*!*****************************************************************************
 * file		Flash.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "flash_func.h"
#include "stm32ll.h"

static u32 FlashStart = 0;
static u32 FlashEnd = 0;
static u32 FlashPageSize = 0;
//-----------------------------------------------------------------------------------------------------------
void Flash_Init(u32 flashStart, u32 flashEnd, u32 flashPageSize) {
  FlashStart = flashStart;
  FlashEnd = flashEnd;
  FlashPageSize = flashPageSize;
}
//-----------------------------------------------------------------------------------------------------------
static u32 GetPage(u32 Addr) {
  u32 page = 0;

  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE)) {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  } else {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }

  return page;
}
//-----------------------------------------------------------------------------------------------------------
FLASH_Status Flash_Erase(u32 address) {
  if (address == FlashStart) {
    FLASH_EraseInitTypeDef f;
    f.TypeErase = FLASH_TYPEERASE_PAGES;
    f.Banks = FLASH_BANK_1;
    f.Page = GetPage(address);
    f.NbPages = (FlashEnd - FlashStart) / FlashPageSize;

    u32 PageError = 0;
    if (HAL_FLASHEx_Erase(&f, &PageError) != HAL_OK) return FLASH_ERROR;
  }

  return FLASH_COMPLETE;
}
//-----------------------------------------------------------------------------------------------------------
FLASH_Status Flash_Program2Bytes(u32 address, u16 data) { return FLASH_COMPLETE; }
static u32 PageAddr = 0;
static u8 data_u64[8];
//-----------------------------------------------------------------------------------------------------------
FLASH_Status Flash_Program4Bytes(u32 address, u32 data) {
  u8 offset = address % 8;
  if (offset == 1 || offset == 3 || offset == 5 || offset == 7) return FLASH_ERROR;

  if (offset == 0) {
    PageAddr = address;
    data_u64[0] = FIRST_BYTE(data);
    data_u64[1] = SECOND_BYTE(data);
    data_u64[2] = THIRD_BYTE(data);
    data_u64[3] = FOURTH_BYTE(data);
  } else if (offset == 4) {
    data_u64[4] = FIRST_BYTE(data);
    data_u64[5] = SECOND_BYTE(data);
    data_u64[6] = THIRD_BYTE(data);
    data_u64[7] = FOURTH_BYTE(data);
    uint64_t value = *(uint64_t *)(data_u64);

    if (Flash_Get4Bytes(PageAddr) != 0xffffffff || Flash_Get4Bytes(PageAddr + 4) != 0xffffffff) return FLASH_ERROR;

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, PageAddr, value) == HAL_OK)
      return FLASH_COMPLETE;
    else
      return FLASH_ERROR;
  }

  return FLASH_COMPLETE;
}
//-----------------------------------------------------------------------------------------------------------
u16 Flash_Get2Bytes(u32 address) { return (*(vu16 *)address); }
//-----------------------------------------------------------------------------------------------------------
u32 Flash_Get4Bytes(u32 address) { return (*(vu32 *)address); }

static u64 BuildData(const u8 *pData, u8 length) {
  u64 data = 0;
  for (u8 i = 0; i < 8; i++) {
    data <<= 8;
    if ((7 - i) < length) {
      data += pData[7 - i];
    } else {
      data += 0xFF;
    }
  }
  return data;
}

OsErr_t Flash_ErasePage(u8 startPage, u8 endPage) {
  FLASH_EraseInitTypeDef EraseInitStruct;
  u32 PageError;

  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks = FLASH_BANK_1;
  EraseInitStruct.Page = startPage;
  EraseInitStruct.NbPages = endPage - startPage + 1;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
    return OS_ERR_BUSY;
  }

  return OS_ERR_OK;
}

OsErr_t Flash_Program(u32 add, const u8 *pData, u16 length) {
  u32 endAdd = add + length;

  if ((add % 8) != 0) return OS_ERR_RANGE;

  if (!IS_FLASH_MAIN_MEM_ADDRESS(add) || !IS_FLASH_MAIN_MEM_ADDRESS(endAdd)) {
    return OS_ERR_RANGE;
  }

  while (add < endAdd) {
    u64 data = BuildData(pData, (endAdd - add) < 8 ? (endAdd - add) : 8);
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, add, data) != HAL_OK) {
      return OS_ERR_BUSY;
    }

    add += 8;
    pData += 8;
  }

  return OS_ERR_OK;
}
