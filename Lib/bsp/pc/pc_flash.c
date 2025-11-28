/*!*****************************************************************************
 * file		Flash.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "flash_func.h"

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
FLASH_Status Flash_Erase(u32 address) { return FLASH_COMPLETE; }
//-----------------------------------------------------------------------------------------------------------
FLASH_Status Flash_Program2Bytes(u32 address, u16 data) { return FLASH_COMPLETE; }
//-----------------------------------------------------------------------------------------------------------
FLASH_Status Flash_Program4Bytes(u32 address, u32 data) { return FLASH_COMPLETE; }
//-----------------------------------------------------------------------------------------------------------
u16 Flash_Get2Bytes(u32 address) { return (*(vu16 *)address); }
//-----------------------------------------------------------------------------------------------------------
u32 Flash_Get4Bytes(u32 address) { return (*(vu32 *)address); }
OsErr_t Flash_ErasePage(u8 startPage, u8 endPage) { return OS_ERR_OK; }

OsErr_t Flash_Program(u32 add, const u8 *pData, u16 length) { return OS_ERR_OK; }
