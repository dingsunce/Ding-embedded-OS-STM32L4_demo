#include "deb_drv.h"
#include "d_message.h"
#include "memory.h"
#include "process.h"
typedef struct
{
  DProcess_t Process;
  DebDrv_t  *pTable;
  u8         Sum;
} DebTask_t;

static DebTask_t DebTask;

#define DEB_PROCESS_MSG 0

#define DEB_DRV_THR (50 / DEB_DRV_TIMEBASE) // 50ms

static u8            Deb_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg);
static void          DebDrv_Scan(DebDrv_t *pDebDrv);
static void          DebDrv_GenerateEvent(DebDrv_t *pDebDrv, DebDrvState_t state);
static DebDrvState_t DebDrv_GetPhysicalState(DebDrv_t *pDebDrv);
static void          DebDrv_Init(DebDrv_t *pDebDrv);
static void          DebDrv_Process(DebDrv_t *pDebDrv);
static DebDrvState_t DebDrv_GetCurrentState(DebDrv_t *pDebDrv);
//-----------------------------------------------------------------------------------------------------------
void Deb_Init(DebDrv_t *pTable, u8 sum)
{
  DProcess_InitStructure(&DebTask.Process, Deb_Handler);
  DProcess_Start(&DebTask.Process);

  DebTask.pTable = pTable;
  DebTask.Sum = sum;

  for (u8 i = 0; i < DebTask.Sum; i++)
  {
    DebDrv_Init(DebTask.pTable + i);
  }

  DMsg_SendInstant((DProcess_t *)(&DebTask), DEB_PROCESS_MSG, MSG_NO_ARG);
}
//-----------------------------------------------------------------------------------------------------------
static u8 Deb_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg)
{
  PROCESS_SCHEDULE_BEGIN()

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == DEB_PROCESS_MSG);

    for (u8 i = 0; i < DebTask.Sum; i++)
    {
      DebDrv_Process(DebTask.pTable + i);
    }

    DMsg_SendLater((DProcess_t *)(&DebTask), DEB_PROCESS_MSG, MSG_NO_ARG, DEB_DRV_TIMEBASE);
  }

  PROCESS_SCHEDULE_END()
}
//-----------------------------------------------------------------------------------------------------------
DebDrvState_t Deb_GetCurrentState(u8 id)
{
  return DebDrv_GetCurrentState(DebTask.pTable + id);
}
//-----------------------------------------------------------------------------------------------------------
static void DebDrv_Init(DebDrv_t *pDebDrv)
{
  pDebDrv->pRam->DebTime = 0;
  pDebDrv->pRam->State = DebDrv_GetPhysicalState(pDebDrv);
  pDebDrv->pRam->LastState = pDebDrv->pRam->State;

#ifndef DEB_DRV_USER_DEFINED_STATE
#ifdef DEB_DRV_PULL_UP_IO
  Io_Init(pDebDrv->Io.Port, pDebDrv->Io.Pin, IO_INPUT_PULL_UP);
#else
  Io_Init(pDebDrv->Io.Port, pDebDrv->Io.Pin, IO_INPUT_FLOATING);
#endif
#endif
}
//-----------------------------------------------------------------------------------------------------------
static void DebDrv_Process(DebDrv_t *pDebDrv)
{
  DebDrv_Scan(pDebDrv);

  if (pDebDrv->pRam->State != pDebDrv->pRam->LastState)
  {
    pDebDrv->pRam->LastState = pDebDrv->pRam->State;

    DebDrv_GenerateEvent(pDebDrv, pDebDrv->pRam->State);
  }
}
//-----------------------------------------------------------------------------------------------------------
static DebDrvState_t DebDrv_GetCurrentState(DebDrv_t *pDebDrv)
{
  return pDebDrv->pRam->State;
}
//-----------------------------------------------------------------------------------------------------------
static void DebDrv_Scan(DebDrv_t *pDebDrv)
{
  DebDrvState_t physicalState = DebDrv_GetPhysicalState(pDebDrv);

  if (physicalState != pDebDrv->pRam->State)
  {
    pDebDrv->pRam->DebTime++;
    if (pDebDrv->pRam->DebTime >= DEB_DRV_THR)
    {
      pDebDrv->pRam->DebTime = 0;
      pDebDrv->pRam->State = physicalState;
    }
  }
  else
  {
    pDebDrv->pRam->DebTime = 0;
  }
}
//-----------------------------------------------------------------------------------------------------------
static void DebDrv_GenerateEvent(DebDrv_t *pDebDrv, DebDrvState_t state)
{
  if (pDebDrv->CallBackFunc != NULL)
  {
    pDebDrv->CallBackFunc(pDebDrv->Id, state);
  }
}
//-----------------------------------------------------------------------------------------------------------
static DebDrvState_t DebDrv_GetPhysicalState(DebDrv_t *pDebDrv)
{
#ifndef DEB_DRV_USER_DEFINED_STATE
  if ((pDebDrv->ActiveLevel == DEB_DRV_HIGH_ACTIVE &&
       Io_GetBits(pDebDrv->Io.Port, pDebDrv->Io.Pin) != 0) // Active high
      || (pDebDrv->ActiveLevel == DEB_DRV_LOW_ACTIVE &&
          Io_GetBits(pDebDrv->Io.Port, pDebDrv->Io.Pin) == 0)) // Active low

  {
    return DEB_DRV_CONNECT;
  }

  return DEB_DRV_DISCONNECT;
#else
  if (pDebDrv->GetStateFunc != NULL)
  {
    return pDebDrv->GetStateFunc(pDebDrv->Id);
  }

  return DEB_DRV_DISCONNECT;
#endif
}