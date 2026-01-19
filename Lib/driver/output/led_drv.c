#include "led_drv.h"
#include "d_mem.h"
#include "d_message.h"
#include "d_process.h"

typedef struct
{
  DProcess_t Process;
  LedDrv_t  *pTable;
  u8         Sum;
  bool       IsProcessRunning;
} LedTask_t;

static LedTask_t LedTask;

static u8      Led_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg);
static void    LedDrv_PhysicalOn(LedDrv_t *pLedDrv);
static void    LedDrv_PhysicalOff(LedDrv_t *pLedDrv);
static void    LedDrv_Init(LedDrv_t *pLedDrv);
static bool    LedDrv_Process(LedDrv_t *pLedDrv);
static void    LedDrv_SetOff(LedDrv_t *pLedDrv);
static void    LedDrv_SetOn(LedDrv_t *pLedDrv, u16 activeTime);
static OsErr_t LedDrv_SetFlash(LedDrv_t *pLedDrv, u16 activeTime, u16 flashOnTime,
                               u16 flashOffTime);

#define LED_PROCESS_MSG 0
//-----------------------------------------------------------------------------------------------------------
void Led_Init(LedDrv_t *pTable, u8 sum)
{
  DProcess_InitStructure(&LedTask.Process, Led_Handler);
  DProcess_Start(&LedTask.Process);

  LedTask.pTable = pTable;
  LedTask.Sum = sum;
  LedTask.IsProcessRunning = false;

  for (u8 i = 0; i < LedTask.Sum; i++)
  {
    LedDrv_Init(LedTask.pTable + i);
  }
}
//-----------------------------------------------------------------------------------------------------------
void Led_Off(u8 id)
{
  if (id < LedTask.Sum)
  {
    LedDrv_SetOff(LedTask.pTable + id);
  }
}
//-----------------------------------------------------------------------------------------------------------
void Led_On(u8 id, u16 activeTime)
{
  if (id < LedTask.Sum)
  {
    LedDrv_SetOn(LedTask.pTable + id, activeTime);

    if (activeTime > 0 && !LedTask.IsProcessRunning)
    {
      LedTask.IsProcessRunning = true;
      DMsg_SendInstant((DProcess_t *)&LedTask, LED_PROCESS_MSG, MSG_NO_ARG);
    }
  }
}
//-----------------------------------------------------------------------------------------------------------
void Led_Flash(u8 id, u16 activeTime, u16 flashOnTime, u16 flashOffTime)
{
  if (id < LedTask.Sum)
  {
    LedDrv_SetFlash(LedTask.pTable + id, activeTime, flashOnTime, flashOffTime);

    if (!LedTask.IsProcessRunning)
    {
      LedTask.IsProcessRunning = true;
      DMsg_SendInstant((DProcess_t *)&LedTask, LED_PROCESS_MSG, MSG_NO_ARG);
    }
  }
}
//-----------------------------------------------------------------------------------------------------------
static u8 Led_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg)
{
  PROCESS_SCHEDULE_BEGIN()

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == LED_PROCESS_MSG);

    LedTask.IsProcessRunning = false;

    for (u8 i = 0; i < LedTask.Sum; i++)
    {
      if (LedDrv_Process(LedTask.pTable + i))
      {
        LedTask.IsProcessRunning = true;
      }
    }

    if (LedTask.IsProcessRunning)
    {
      DMsg_SendLater((DProcess_t *)&LedTask, LED_PROCESS_MSG, MSG_NO_ARG, LED_DRV_TIMEBASE);
    }
  }

  PROCESS_SCHEDULE_END()
}
//-----------------------------------------------------------------------------------------------------------
static void LedDrv_Init(LedDrv_t *pLedDrv)
{
  pLedDrv->pRam->Mode = LED_DRV_OFF_MODE;
  pLedDrv->pRam->ActiveTime = 0;
  pLedDrv->pRam->IsWithinFlashOn = false;
  pLedDrv->pRam->FlashOnTime = 0;
  pLedDrv->pRam->FlashOffTime = 0;
  pLedDrv->pRam->FlashTick = 0;

  if (pLedDrv->ActiveLevel == LED_DRV_HIGH_ACTIVE)
  {
    Io_Init(pLedDrv->Io.Port, pLedDrv->Io.Pin, IO_OUTPUT_PUSH_PULL_LOW);
  }
  else
  {
    Io_Init(pLedDrv->Io.Port, pLedDrv->Io.Pin, IO_OUTPUT_PUSH_PULL_HIGH);
  }
}
//-----------------------------------------------------------------------------------------------------------
static bool LedDrv_Process(LedDrv_t *pLedDrv)
{
  // Off mode
  if (pLedDrv->pRam->Mode == LED_DRV_OFF_MODE)
  {
    return false;
  }

  // On mode and permanent
  if (pLedDrv->pRam->Mode == LED_DRV_ON_MODE && pLedDrv->pRam->ActiveTime == 0)
  {
    return false;
  }

  if (pLedDrv->pRam->Mode == LED_DRV_ON_MODE || pLedDrv->pRam->Mode == LED_DRV_FLASH_MODE)
  {
    if (pLedDrv->pRam->ActiveTime > 0)
    {
      pLedDrv->pRam->ActiveTime--;

      // ActiveTime expires
      if (pLedDrv->pRam->ActiveTime == 0)
      {
        LedDrv_SetOff(pLedDrv);
        return false;
      }
    }
  }

  if (pLedDrv->pRam->Mode == LED_DRV_FLASH_MODE)
  {
    pLedDrv->pRam->FlashTick--;
    if (pLedDrv->pRam->FlashTick == 0)
    {
      if (pLedDrv->pRam->IsWithinFlashOn)
      {
        pLedDrv->pRam->IsWithinFlashOn = false;

        LedDrv_PhysicalOff(pLedDrv);
        pLedDrv->pRam->FlashTick = pLedDrv->pRam->FlashOffTime;
      }
      else
      {
        pLedDrv->pRam->IsWithinFlashOn = true;

        LedDrv_PhysicalOn(pLedDrv);
        pLedDrv->pRam->FlashTick = pLedDrv->pRam->FlashOnTime;
      }
    }
  }

  return true;
}
//-----------------------------------------------------------------------------------------------------------
static void LedDrv_SetOff(LedDrv_t *pLedDrv)
{
  pLedDrv->pRam->Mode = LED_DRV_OFF_MODE;

  LedDrv_PhysicalOff(pLedDrv);
}
//-----------------------------------------------------------------------------------------------------------
static void LedDrv_SetOn(LedDrv_t *pLedDrv, u16 activeTime)
{
  pLedDrv->pRam->Mode = LED_DRV_ON_MODE;
  pLedDrv->pRam->ActiveTime = activeTime / LED_DRV_TIMEBASE;

  LedDrv_PhysicalOn(pLedDrv);
}
//-----------------------------------------------------------------------------------------------------------
/*
activeTime unit is ms, 0 means permanent.
flashOnTime unit is ms.
flashOffTime unit is ms. */
static OsErr_t LedDrv_SetFlash(LedDrv_t *pLedDrv, u16 activeTime, u16 flashOnTime, u16 flashOffTime)
{
  if (flashOnTime == 0 || flashOffTime == 0)
  {
    return OS_ERR_RANGE;
  }
  pLedDrv->pRam->Mode = LED_DRV_FLASH_MODE;
  pLedDrv->pRam->ActiveTime = activeTime / LED_DRV_TIMEBASE;
  pLedDrv->pRam->FlashOnTime = flashOnTime / LED_DRV_TIMEBASE;
  pLedDrv->pRam->FlashOffTime = flashOffTime / LED_DRV_TIMEBASE;
  pLedDrv->pRam->IsWithinFlashOn = true;
  pLedDrv->pRam->FlashTick = pLedDrv->pRam->FlashOnTime;

  LedDrv_PhysicalOn(pLedDrv);

  return OS_ERR_OK;
}
//-----------------------------------------------------------------------------------------------------------
static void LedDrv_PhysicalOn(LedDrv_t *pLedDrv)
{
  if (pLedDrv->ActiveLevel == LED_DRV_HIGH_ACTIVE)
  {
    Io_SetBits(pLedDrv->Io.Port, pLedDrv->Io.Pin);
  }
  else
  {
    Io_ResetBits(pLedDrv->Io.Port, pLedDrv->Io.Pin);
  }
}
//-----------------------------------------------------------------------------------------------------------
static void LedDrv_PhysicalOff(LedDrv_t *pLedDrv)
{
  if (pLedDrv->ActiveLevel == LED_DRV_HIGH_ACTIVE)
  {
    Io_ResetBits(pLedDrv->Io.Port, pLedDrv->Io.Pin);
  }
  else
  {
    Io_SetBits(pLedDrv->Io.Port, pLedDrv->Io.Pin);
  }
}