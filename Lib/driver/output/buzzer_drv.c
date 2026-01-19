#include "buzzer_drv.h"
#include "d_mem.h"
#include "d_message.h"
#include "d_process.h"
#include "pwm_func.h"

static void    BuzzerDrv_Init(BuzzerDrv_t *pBuzzerDrv);
static bool    BuzzerDrv_Process(BuzzerDrv_t *pBuzzerDrv);
static void    BuzzerDrv_SetOff(BuzzerDrv_t *pBuzzerDrv);
static void    BuzzerDrv_SetOn(BuzzerDrv_t *pBuzzerDrv, u16 activeTime);
static OsErr_t BuzzerDrv_SetFlash(BuzzerDrv_t *pBuzzerDrv, u16 activeTime, u16 flashOnTime,
                                  u16 flashOffTime);
static u8      Buzzer_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg);

typedef struct
{
  DProcess_t   Process;
  BuzzerDrv_t *pTable;
  u8           Sum;
  bool         IsProcessRunning;
} BuzzerTask_t;

BuzzerTask_t BuzzerTask;

#define BUZZER_PROCESS_MSG 0

//-----------------------------------------------------------------------------------------------------------
void Buzzer_Init(BuzzerDrv_t *pTable, u8 sum)
{
  DProcess_InitStructure(&BuzzerTask.Process, Buzzer_Handler);
  DProcess_Start(&BuzzerTask.Process);

  BuzzerTask.pTable = pTable;
  BuzzerTask.Sum = sum;
  BuzzerTask.IsProcessRunning = false;

  for (u8 i = 0; i < BuzzerTask.Sum; i++)
  {
    BuzzerDrv_Init(BuzzerTask.pTable + i);
  }
}
//-----------------------------------------------------------------------------------------------------------
void Buzzer_Off(u8 id)
{
  if (id < BuzzerTask.Sum)
  {
    BuzzerDrv_SetOff(BuzzerTask.pTable + id);
  }
}
//-----------------------------------------------------------------------------------------------------------
void Buzzer_On(u8 id, u16 activeTime)
{
  if (id < BuzzerTask.Sum)
  {
    BuzzerDrv_SetOn(BuzzerTask.pTable + id, activeTime);
    if (activeTime > 0 && !BuzzerTask.IsProcessRunning)
    {
      BuzzerTask.IsProcessRunning = true;
      DMsg_SendInstant((DProcess_t *)&BuzzerTask, BUZZER_PROCESS_MSG, MSG_NO_ARG);
    }
  }
}
//-----------------------------------------------------------------------------------------------------------
void Buzzer_Flash(u8 id, u16 activeTime, u16 flashOnTime, u16 flashOffTime)
{
  if (id < BuzzerTask.Sum)
  {
    BuzzerDrv_SetFlash(BuzzerTask.pTable + id, activeTime, flashOnTime, flashOffTime);
    if (!BuzzerTask.IsProcessRunning)
    {
      BuzzerTask.IsProcessRunning = true;
      DMsg_SendInstant((DProcess_t *)&BuzzerTask, BUZZER_PROCESS_MSG, MSG_NO_ARG);
    }
  }
}
//-----------------------------------------------------------------------------------------------------------
static u8 Buzzer_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg)
{
  PROCESS_SCHEDULE_BEGIN()

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == BUZZER_PROCESS_MSG);

    BuzzerTask.IsProcessRunning = false;

    for (u8 i = 0; i < BuzzerTask.Sum; i++)
    {
      if (BuzzerDrv_Process(BuzzerTask.pTable + i))
      {
        BuzzerTask.IsProcessRunning = true;
      }
    }

    if (BuzzerTask.IsProcessRunning)
    {
      DMsg_SendLater((DProcess_t *)(&BuzzerTask), BUZZER_PROCESS_MSG, MSG_NO_ARG,
                     BUZZER_DRV_TIMEBASE);
    }
  }

  PROCESS_SCHEDULE_END()
}
//-----------------------------------------------------------------------------------------------------------
static void BuzzerDrv_Init(BuzzerDrv_t *pBuzzerDrv)
{
  pBuzzerDrv->pRam->Mode = BUZZER_DRV_OFF_MODE;
  pBuzzerDrv->pRam->ActiveTime = 0;
  pBuzzerDrv->pRam->IsWithinFlashOn = false;
  pBuzzerDrv->pRam->FlashOnTime = 0;
  pBuzzerDrv->pRam->FlashOffTime = 0;
  pBuzzerDrv->pRam->FlashTick = 0;

#ifdef PLATFORM_STM8
  Io_Init(pBuzzerDrv->Io.Port, pBuzzerDrv->Io.Pin, IO_OUTPUT_PUSH_PULL_LOW);
#endif
}
//-----------------------------------------------------------------------------------------------------------
static bool BuzzerDrv_Process(BuzzerDrv_t *pBuzzerDrv)
{
  // Off mode
  if (pBuzzerDrv->pRam->Mode == BUZZER_DRV_OFF_MODE)
  {
    return false;
  }

  // On mode and permanent
  if (pBuzzerDrv->pRam->Mode == BUZZER_DRV_ON_MODE && pBuzzerDrv->pRam->ActiveTime == 0)
  {
    return false;
  }

  if (pBuzzerDrv->pRam->Mode == BUZZER_DRV_ON_MODE ||
      pBuzzerDrv->pRam->Mode == BUZZER_DRV_FLASH_MODE)
  {
    if (pBuzzerDrv->pRam->ActiveTime > 0)
    {
      pBuzzerDrv->pRam->ActiveTime--;

      // ActiveTime expires
      if (pBuzzerDrv->pRam->ActiveTime == 0)
      {
        BuzzerDrv_SetOff(pBuzzerDrv);
        return false;
      }
    }
  }

  if (pBuzzerDrv->pRam->Mode == BUZZER_DRV_FLASH_MODE)
  {
    pBuzzerDrv->pRam->FlashTick--;
    if (pBuzzerDrv->pRam->FlashTick == 0)
    {
      if (pBuzzerDrv->pRam->IsWithinFlashOn)
      {
        pBuzzerDrv->pRam->IsWithinFlashOn = false;

        Pwm_SetDutyCycle(pBuzzerDrv->TimerId, 0);
        pBuzzerDrv->pRam->FlashTick = pBuzzerDrv->pRam->FlashOffTime;
      }
      else
      {
        pBuzzerDrv->pRam->IsWithinFlashOn = true;

        Pwm_SetDutyCycle(pBuzzerDrv->TimerId, 0.5f);
        pBuzzerDrv->pRam->FlashTick = pBuzzerDrv->pRam->FlashOnTime;
      }
    }
  }

  return true;
}
//-----------------------------------------------------------------------------------------------------------
static void BuzzerDrv_SetOff(BuzzerDrv_t *pBuzzerDrv)
{
  pBuzzerDrv->pRam->Mode = BUZZER_DRV_OFF_MODE;

  Pwm_SetDutyCycle(pBuzzerDrv->TimerId, 0);
}
//-----------------------------------------------------------------------------------------------------------
static void BuzzerDrv_SetOn(BuzzerDrv_t *pBuzzerDrv, u16 activeTime)
{
  pBuzzerDrv->pRam->Mode = BUZZER_DRV_ON_MODE;
  pBuzzerDrv->pRam->ActiveTime = activeTime / BUZZER_DRV_TIMEBASE;

  Pwm_SetDutyCycle(pBuzzerDrv->TimerId, 1);
}
//-----------------------------------------------------------------------------------------------------------
/*
activeTime unit is ms, 0 means permanent.
flashOnTime unit is ms.
flashOffTime unit is ms. */
static OsErr_t BuzzerDrv_SetFlash(BuzzerDrv_t *pBuzzerDrv, u16 activeTime, u16 flashOnTime,
                                  u16 flashOffTime)
{
  if (flashOnTime == 0 || flashOffTime == 0)
  {
    return OS_ERR_RANGE;
  }
  pBuzzerDrv->pRam->Mode = BUZZER_DRV_FLASH_MODE;
  pBuzzerDrv->pRam->ActiveTime = activeTime / BUZZER_DRV_TIMEBASE;
  pBuzzerDrv->pRam->FlashOnTime = flashOnTime / BUZZER_DRV_TIMEBASE;
  pBuzzerDrv->pRam->FlashOffTime = flashOffTime / BUZZER_DRV_TIMEBASE;
  pBuzzerDrv->pRam->IsWithinFlashOn = true;
  pBuzzerDrv->pRam->FlashTick = pBuzzerDrv->pRam->FlashOnTime;

  Pwm_SetDutyCycle(pBuzzerDrv->TimerId, 0.5f);

  return OS_ERR_OK;
}