#include "sw_drv.h"
#include "d_mem.h"
#include "d_message.h"
#include "d_process.h"

// Static functions.
static void         SwDrv_Scan(SwDrv_t *pSwDrv);
static void         SwDrv_GenerateEvent(SwDrv_t *pSwDrv, SwDrvState_t state);
static SwDrvState_t SwDrv_GetPhysicalState(SwDrv_t *pSwDrv);
static void         SwDrv_Init(SwDrv_t *pSwDrv);
static void         SwDrv_Process(SwDrv_t *pSwDrv);
static SwDrvState_t SwDrv_GetCurrentState(SwDrv_t *pSwDrv);
static u8           Sw_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg);

typedef struct
{
  DProcess_t Process;
  SwDrv_t   *pTable;
  u8         Sum;
} SwTask_t;

static SwTask_t SwTask;

#define SW_PROCESS_MSG 0

#define SW_DRV_TIMEBASE     15                     // 15ms
#define SW_DRV_DEBOUNCE_THR (60 / SW_DRV_TIMEBASE) // 60ms
//-----------------------------------------------------------------------------------------------------------
void Sw_Init(SwDrv_t *pTable, u8 sum)
{
  DProcess_InitStructure(&SwTask.Process, Sw_Handler);
  DProcess_Start(&SwTask.Process);

  SwTask.pTable = pTable;

  SwTask.Sum = sum;

  for (u8 i = 0; i < SwTask.Sum; i++)
  {
    SwDrv_Init(SwTask.pTable + i);
  }

  DMsg_SendInstant((DProcess_t *)(&SwTask), SW_PROCESS_MSG, MSG_NO_ARG);
}
//-----------------------------------------------------------------------------------------------------------
static u8 Sw_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg)
{
  PROCESS_SCHEDULE_BEGIN()

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == SW_PROCESS_MSG);

    for (u8 i = 0; i < SwTask.Sum; i++)
    {
      SwDrv_Process(SwTask.pTable + i);
    }

    DMsg_SendLater((DProcess_t *)(&SwTask), SW_PROCESS_MSG, MSG_NO_ARG, SW_DRV_TIMEBASE);
  }

  PROCESS_SCHEDULE_END()
}
//-----------------------------------------------------------------------------------------------------------
SwDrvState_t Sw_GetCurrentState(u8 swId)
{
  return SwDrv_GetCurrentState(SwTask.pTable + swId);
}
//-----------------------------------------------------------------------------------------------------------
static void SwDrv_Init(SwDrv_t *pSwDrv)
{
#ifdef SW_DRV_PULL_UP_IO
  Io_Init(pSwDrv->Io.Port, pSwDrv->Io.Pin, IO_INPUT_PULL_UP);
#else
  Io_Init(pSwDrv->Io.Port, pSwDrv->Io.Pin, IO_INPUT_FLOATING);
#endif

  pSwDrv->pRam->DebounceTime = 0;
  pSwDrv->pRam->State = SwDrv_GetPhysicalState(pSwDrv);
  pSwDrv->pRam->LastState = pSwDrv->pRam->State;
}
//-----------------------------------------------------------------------------------------------------------
static void SwDrv_Process(SwDrv_t *pSwDrv)
{
  SwDrv_Scan(pSwDrv);

  if (pSwDrv->pRam->State != pSwDrv->pRam->LastState)
  {
    pSwDrv->pRam->LastState = pSwDrv->pRam->State;

    SwDrv_GenerateEvent(pSwDrv, pSwDrv->pRam->State);
  }
}
//-----------------------------------------------------------------------------------------------------------
static SwDrvState_t SwDrv_GetCurrentState(SwDrv_t *pSwDrv)
{
  return pSwDrv->pRam->State;
}
//-----------------------------------------------------------------------------------------------------------
static void SwDrv_Scan(SwDrv_t *pSwDrv)
{
  SwDrvState_t physicalState = SwDrv_GetPhysicalState(pSwDrv);

  if (physicalState != pSwDrv->pRam->State)
  {
    pSwDrv->pRam->DebounceTime++;
    if (pSwDrv->pRam->DebounceTime >= SW_DRV_DEBOUNCE_THR)
    {
      pSwDrv->pRam->DebounceTime = 0;
      pSwDrv->pRam->State = physicalState;
    }
  }
  else
  {
    pSwDrv->pRam->DebounceTime = 0;
  }
}
//-----------------------------------------------------------------------------------------------------------
static void SwDrv_GenerateEvent(SwDrv_t *pSwDrv, SwDrvState_t state)
{
  if (pSwDrv->CallBackFunc != NULL)
  {
    pSwDrv->CallBackFunc(pSwDrv->Id, state);
  }
}
//-----------------------------------------------------------------------------------------------------------
static SwDrvState_t SwDrv_GetPhysicalState(SwDrv_t *pSwDrv)
{
  if ((pSwDrv->ActiveLevel == SW_DRV_HIGH_ACTIVE &&
       Io_GetBits(pSwDrv->Io.Port, pSwDrv->Io.Pin) != 0) // Active high
      || (pSwDrv->ActiveLevel == SW_DRV_LOW_ACTIVE &&
          Io_GetBits(pSwDrv->Io.Port, pSwDrv->Io.Pin) == 0)) // Active low
  {
    return SW_DRV_CLOSED;
  }

  return SW_DRV_OPEN;
}