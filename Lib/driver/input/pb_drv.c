#include "pb_drv.h"
#include "d_mem.h"
#include "d_message.h"
#include "d_process.h"

#ifndef PB_DRV_ALTERNATING_CURRENT
#define PB_DRV_PRESS_DEBOUNCE_THR   (20 / PB_DRV_TIMEBASE) // 20ms
#define PB_DRV_RELEASE_DEBOUNCE_THR (20 / PB_DRV_TIMEBASE) // 20ms
#else
#define PB_DRV_PRESS_DEBOUNCE_THR   (5 / PB_DRV_TIMEBASE)  // 5ms
#define PB_DRV_RELEASE_DEBOUNCE_THR (20 / PB_DRV_TIMEBASE) // 20ms
#endif
#define PB_DRV_SHORT_THR (500 / PB_DRV_TIMEBASE) // 500ms
// #define PB_DRV_ENABLE_DOUBLE
#define PB_DRV_DOUBLE_THR                                                                          \
  (200 / PB_DRV_TIMEBASE) // 200ms, only work when PB_DRV_ENABLE_DOUBLE defined
#define PB_DRV_REPEAT_THR (100 / PB_DRV_TIMEBASE) // 100ms
// #define PB_DRV_ENABLE_STUCK
#define PB_DRV_STUCK_THR                                                                           \
  (20000 / PB_DRV_TIMEBASE) // 20s, only work when PB_DRV_ENABLE_STUCK defined

#define PB_DRV_MAX_TIME 0xFFFF // avoid overflow

// Static functions.
static void         PbDrv_UpdateMode(PbDrv_t *pPbDrv);
static void         PbDrv_Scan(PbDrv_t *pPbDrv);
static void         PbDrv_GenerateEvent(PbDrv_t *pPbDrv, PbDrvEvent_t event);
static PbDrvState_t PbDrv_GetCurrentState(PbDrv_t *pPbDrv);
static void         PbDrv_Init(PbDrv_t *pPbDrv);
static void         PbDrv_Process(PbDrv_t *pPbDrv);
static u8           Pb_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg);

typedef struct
{
  DProcess_t Process;
  PbDrv_t   *pTable;
  u8         Sum;
} PbTask_t;

PbTask_t PbTask;

#define PB_PROCESS_MSG 0
//-----------------------------------------------------------------------------------------------------------
void Pb_Init(PbDrv_t *pTable, u8 sum)
{
  DProcess_InitStructure(&PbTask.Process, Pb_Handler);
  DProcess_Start(&PbTask.Process);

  PbTask.pTable = pTable;

  PbTask.Sum = sum;

  for (u8 i = 0; i < PbTask.Sum; i++)
  {
    PbDrv_Init(PbTask.pTable + i);
  }

  DMsg_SendInstant((DProcess_t *)(&PbTask), PB_PROCESS_MSG, MSG_NO_ARG);
}
//-----------------------------------------------------------------------------------------------------------
static u8 Pb_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg)
{
  PROCESS_SCHEDULE_BEGIN()

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == PB_PROCESS_MSG);

    for (u8 i = 0; i < PbTask.Sum; i++)
    {
      PbDrv_Process(PbTask.pTable + i);
    }

    DMsg_SendLater((DProcess_t *)(&PbTask), PB_PROCESS_MSG, MSG_NO_ARG, PB_DRV_TIMEBASE);
  }

  PROCESS_SCHEDULE_END()
}
//-----------------------------------------------------------------------------------------------------------
static void PbDrv_Init(PbDrv_t *pPbDrv)
{
  pPbDrv->pRam->DebounceTime = 0;
  pPbDrv->pRam->State = PB_DRV_RELEASED;

  pPbDrv->pRam->PressedTime = 0;
  pPbDrv->pRam->ReleasedTime = 0;
  pPbDrv->pRam->RepeatTime = 0;

  pPbDrv->pRam->Mode = PB_DRV_RELEASE_MODE;

#ifndef PB_DRV_USER_DEFINED_STATE
#ifdef PB_DRV_PULL_UP_IO
  Io_Init(pPbDrv->Io.Port, pPbDrv->Io.Pin, IO_INPUT_PULL_UP);
#else
  Io_Init(pPbDrv->Io.Port, pPbDrv->Io.Pin, IO_INPUT_FLOATING);
#endif
#endif
}
//-----------------------------------------------------------------------------------------------------------
static void PbDrv_Process(PbDrv_t *pPbDrv)
{
  PbDrv_Scan(pPbDrv);
  PbDrv_UpdateMode(pPbDrv);
}
//-----------------------------------------------------------------------------------------------------------
static void PbDrv_UpdateMode(PbDrv_t *pPbDrv)
{
  switch (pPbDrv->pRam->Mode)
  {
  case PB_DRV_RELEASE_MODE:
    if (pPbDrv->pRam->State == PB_DRV_PRESSED)
    {
      pPbDrv->pRam->Mode = PB_DRV_SHORT_MODE;

      PbDrv_GenerateEvent(pPbDrv, PB_DRV_PRESS_EVENT);
    }
    break;

  case PB_DRV_SHORT_MODE:
    if (pPbDrv->pRam->State == PB_DRV_RELEASED)
    {
#ifdef PB_DRV_ENABLE_DOUBLE
      pPbDrv->pRam->Mode = PB_DRV_PRE_DOUBLE_MODE;
#else
      pPbDrv->pRam->Mode = PB_DRV_RELEASE_MODE;

      PbDrv_GenerateEvent(pPbDrv, PB_DRV_SHORT_PRESS_EVENT);
#endif

      PbDrv_GenerateEvent(pPbDrv, PB_DRV_RELEASE_EVENT);
    }
    else if (pPbDrv->pRam->State == PB_DRV_PRESSED)
    {
      if (pPbDrv->pRam->PressedTime >= PB_DRV_SHORT_THR)
      {
        pPbDrv->pRam->Mode = PB_DRV_LONG_MODE;
        pPbDrv->pRam->RepeatTime = 0;

        PbDrv_GenerateEvent(pPbDrv, PB_DRV_LONG_PRESS_START_EVENT);
      }
    }
    break;

  case PB_DRV_LONG_MODE:
    if (pPbDrv->pRam->State == PB_DRV_RELEASED)
    {
      pPbDrv->pRam->Mode = PB_DRV_RELEASE_MODE;

      PbDrv_GenerateEvent(pPbDrv, PB_DRV_LONG_PRESS_STOP_EVENT);
      PbDrv_GenerateEvent(pPbDrv, PB_DRV_RELEASE_EVENT);
    }
    else if (pPbDrv->pRam->State == PB_DRV_PRESSED)
    {
#ifdef PB_DRV_ENABLE_STUCK
      if (pPbDrv->pRam->PressedTime >= PB_DRV_STUCK_THR)
      {
        pPbDrv->pRam->Mode = PB_DRV_STUCK_MODE;

        PbDrv_GenerateEvent(pPbDrv, PB_DRV_STUCK_EVENT);
      }
      else
      {
        pPbDrv->pRam->RepeatTime++;
        if (pPbDrv->pRam->RepeatTime >= PB_DRV_REPEAT_THR)
        {
          pPbDrv->pRam->RepeatTime = 0;
          PbDrv_GenerateEvent(pPbDrv, PB_DRV_LONG_PRESS_REPEAT_EVENT);
        }
      }
#else
      pPbDrv->pRam->RepeatTime++;
      if (pPbDrv->pRam->RepeatTime >= PB_DRV_REPEAT_THR)
      {
        pPbDrv->pRam->RepeatTime = 0;
        PbDrv_GenerateEvent(pPbDrv, PB_DRV_LONG_PRESS_REPEAT_EVENT);
      }
#endif
    }
    break;

  case PB_DRV_PRE_DOUBLE_MODE:
    if (pPbDrv->pRam->State == PB_DRV_RELEASED)
    {
      if (pPbDrv->pRam->ReleasedTime >= PB_DRV_DOUBLE_THR)
      {
        pPbDrv->pRam->Mode = PB_DRV_RELEASE_MODE;

        PbDrv_GenerateEvent(pPbDrv, PB_DRV_SHORT_PRESS_EVENT);
      }
    }
    else if (pPbDrv->pRam->State == PB_DRV_PRESSED)
    {
      pPbDrv->pRam->Mode = PB_DRV_DOUBLE_MODE;

      PbDrv_GenerateEvent(pPbDrv, PB_DRV_DOUBLE_PRESS_EVENT);
      PbDrv_GenerateEvent(pPbDrv, PB_DRV_PRESS_EVENT);
    }
    break;

  case PB_DRV_DOUBLE_MODE:
    if (pPbDrv->pRam->State == PB_DRV_RELEASED)
    {
      pPbDrv->pRam->Mode = PB_DRV_RELEASE_MODE;

      PbDrv_GenerateEvent(pPbDrv, PB_DRV_RELEASE_EVENT);
    }
#ifdef PB_DRV_ENABLE_STUCK
    else if (pPbDrv->pRam->State == PB_DRV_PRESSED)
    {
      if (pPbDrv->pRam->PressedTime >= PB_DRV_STUCK_THR)
      {
        pPbDrv->pRam->Mode = PB_DRV_STUCK_MODE;

        PbDrv_GenerateEvent(pPbDrv, PB_DRV_STUCK_EVENT);
      }
    }
#endif
    break;

  case PB_DRV_STUCK_MODE:
    if (pPbDrv->pRam->State == PB_DRV_RELEASED)
    {
      pPbDrv->pRam->Mode = PB_DRV_RELEASE_MODE;

      PbDrv_GenerateEvent(pPbDrv, PB_DRV_FREE_EVENT);
      PbDrv_GenerateEvent(pPbDrv, PB_DRV_RELEASE_EVENT);
    }
    break;

  default:
    break;
  }
}
//-----------------------------------------------------------------------------------------------------------
static void PbDrv_GenerateEvent(PbDrv_t *pPbDrv, PbDrvEvent_t event)
{
  if (pPbDrv->CallBackFunc != NULL)
  {
    pPbDrv->CallBackFunc(pPbDrv->Id, event);
  }
}
//-----------------------------------------------------------------------------------------------------------
static void PbDrv_Scan(PbDrv_t *pPbDrv)
{
  switch (pPbDrv->pRam->State)
  {
  case PB_DRV_RELEASED:
    if (PbDrv_GetCurrentState(pPbDrv) == PB_DRV_PRESSED)
    {
      pPbDrv->pRam->DebounceTime++;
      if (pPbDrv->pRam->DebounceTime >= PB_DRV_PRESS_DEBOUNCE_THR)
      {
        pPbDrv->pRam->DebounceTime = 0;
        pPbDrv->pRam->State = PB_DRV_PRESSED;
        pPbDrv->pRam->PressedTime = 0; // reset pressed time
      }
    }
    else
    {
      if (pPbDrv->pRam->ReleasedTime < PB_DRV_MAX_TIME) // avoid overflow
      {
        pPbDrv->pRam->ReleasedTime++;
      }

      pPbDrv->pRam->DebounceTime = 0;
    }
    break;

  case PB_DRV_PRESSED:
    if (PbDrv_GetCurrentState(pPbDrv) == PB_DRV_RELEASED)
    {
      pPbDrv->pRam->DebounceTime++;
      if (pPbDrv->pRam->DebounceTime >= PB_DRV_RELEASE_DEBOUNCE_THR)
      {
        pPbDrv->pRam->DebounceTime = 0;
        pPbDrv->pRam->State = PB_DRV_RELEASED;
        pPbDrv->pRam->ReleasedTime = 0; // reset released time
      }
    }
    else
    {
      if (pPbDrv->pRam->PressedTime < PB_DRV_MAX_TIME) // avoid overflow
      {
        pPbDrv->pRam->PressedTime++;
      }

      pPbDrv->pRam->DebounceTime = 0;
    }
    break;

  default:
    break;
  }
}
//-----------------------------------------------------------------------------------------------------------
static PbDrvState_t PbDrv_GetCurrentState(PbDrv_t *pPbDrv)
{
#ifndef PB_DRV_USER_DEFINED_STATE
  if ((pPbDrv->ActiveLevel == PB_DRV_HIGH_ACTIVE &&
       Io_GetBits(pPbDrv->Io.Port, pPbDrv->Io.Pin) != 0) // Active high
      || (pPbDrv->ActiveLevel == PB_DRV_LOW_ACTIVE &&
          Io_GetBits(pPbDrv->Io.Port, pPbDrv->Io.Pin) == 0)) // Active low
  {
    return PB_DRV_PRESSED;
  }

  return PB_DRV_RELEASED;
#else
  if (pPbDrv->GetStateFunc != NULL)
  {
    return pPbDrv->GetStateFunc(pPbDrv->Id);
  }

  return PB_DRV_RELEASED;
#endif
}