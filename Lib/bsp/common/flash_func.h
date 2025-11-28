/*!*****************************************************************************
 * file		flash_func.h
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef flash_func_h
#define flash_func_h

#ifdef __cplusplus
extern "C" {
#endif

#include "define.h"
#include "error.h"

typedef enum {
  FLASH_BUSY = 1,
  FLASH_ERROR,
  FLASH_COMPLETE,
} FLASH_Status;

typedef volatile uint32_t vu32;
typedef volatile uint16_t vu16;
typedef volatile uint8_t vu8;

void Flash_Init(u32 flashStart, u32 flashEnd, u32 flashPageSize);

u16 Flash_Get2Bytes(u32 address);

u32 Flash_Get4Bytes(u32 address);

FLASH_Status Flash_Program2Bytes(u32 address, u16 data);

FLASH_Status Flash_Program4Bytes(u32 address, u32 data);

FLASH_Status Flash_Erase(u32 address);

extern OsErr_t Flash_ErasePage(u8 startPage, u8 endPage);

extern OsErr_t Flash_Program(u32 add, const u8 *pData, u16 length);

#ifdef __cplusplus
}
#endif

#endif
