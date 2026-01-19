#include "relay_drv.h"
#include "d_mem.h"
#include "d_message.h"
#include "d_process.h"

static void           RelayDrv_Init(RelayDrv_t *pRelayDrv);
static bool           RelayDrv_Process(RelayDrv_t *pRelayDrv);
static OsErr_t        RelayDrv_Set(RelayDrv_t *pRelayDrv, RelayDrvState_t state);
static RelayDrvType_t RelayDrv_GetType(RelayDrv_t *pRelayDrv);
static u8             Relay_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg);

typedef struct
{
  DProcess_t  Process;
  RelayDrv_t *pTable;
  u8          Sum;
  bool        IsProcessRunning;
} RelayTask_t;

static RelayTask_t RelayTask;

#define RELAY_PROCESS_MSG 0
//-----------------------------------------------------------------------------------------------------------
void Relay_Init(RelayDrv_t *pTable, u8 sum)
{
  DProcess_InitStructure(&RelayTask.Process, Relay_Handler);
  DProcess_Start(&RelayTask.Process);

  RelayTask.pTable = pTable;
  RelayTask.Sum = sum;
  RelayTask.IsProcessRunning = false;

  for (u8 i = 0; i < RelayTask.Sum; i++)
  {
    RelayDrv_Init(RelayTask.pTable + i);
  }
}
//-----------------------------------------------------------------------------------------------------------
void Relay_Set(u8 id, RelayDrvState_t state)
{
  if (id < RelayTask.Sum)
  {
    RelayDrv_Set(RelayTask.pTable + id, state);
    // Only latching relay need to process
    if (RelayDrv_GetType(RelayTask.pTable + id) == RELAY_DRV_LATCH && !RelayTask.IsProcessRunning)
    {
      RelayTask.IsProcessRunning = true;
      DMsg_SendInstant((DProcess_t *)(&RelayTask), RELAY_PROCESS_MSG, MSG_NO_ARG);
    }
  }
}
//-----------------------------------------------------------------------------------------------------------
static u8 Relay_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg)
{
  PROCESS_SCHEDULE_BEGIN()

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == RELAY_PROCESS_MSG);

    RelayTask.IsProcessRunning = false;

    for (u8 i = 0; i < RelayTask.Sum; i++)
    {
      if (RelayDrv_Process(RelayTask.pTable + i))
      {
        RelayTask.IsProcessRunning = true;
      }
    }

    if (RelayTask.IsProcessRunning)
    {
      DMsg_SendLater((DProcess_t *)(&RelayTask), RELAY_PROCESS_MSG, MSG_NO_ARG, RELAY_DRV_TIMEBASE);
    }
  }

  PROCESS_SCHEDULE_END()
}
//-----------------------------------------------------------------------------------------------------------
static void RelayDrv_Init(RelayDrv_t *pRelayDrv)
{
  pRelayDrv->pRam->State = RELAY_DRV_UNKNOWN;
  pRelayDrv->pRam->Mode = RELAY_DRV_IDLE_MODE;

  if (pRelayDrv->Type == RELAY_DRV_NORMAL)
  {
    if (pRelayDrv->NormalIo.ActiveLevel == RELAY_DRV_HIGH_ACTIVE)
    {
      Io_Init(pRelayDrv->NormalIo.Io.Port, pRelayDrv->NormalIo.Io.Pin, IO_OUTPUT_PUSH_PULL_LOW);
    }
    else
    {
      Io_Init(pRelayDrv->NormalIo.Io.Port, pRelayDrv->NormalIo.Io.Pin, IO_OUTPUT_PUSH_PULL_HIGH);
    }
  }
  else
  {
    Io_Init(pRelayDrv->LatchIo.CloseIo.Port, pRelayDrv->LatchIo.CloseIo.Pin,
            IO_OUTPUT_PUSH_PULL_LOW);
    Io_Init(pRelayDrv->LatchIo.OpenIo.Port, pRelayDrv->LatchIo.OpenIo.Pin, IO_OUTPUT_PUSH_PULL_LOW);
  }
}
//-----------------------------------------------------------------------------------------------------------
static bool RelayDrv_Process(RelayDrv_t *pRelayDrv)
{
  if (pRelayDrv->Type == RELAY_DRV_LATCH)
  {
    if (pRelayDrv->pRam->Mode == RELAY_DRV_START_IMPULSE_MODE)
    {
      if (pRelayDrv->pRam->State == RELAY_DRV_CLOSE)
      {
        Io_SetBits(pRelayDrv->LatchIo.CloseIo.Port, pRelayDrv->LatchIo.CloseIo.Pin);
        Io_ResetBits(pRelayDrv->LatchIo.OpenIo.Port, pRelayDrv->LatchIo.OpenIo.Pin);
      }
      else
      {
        Io_ResetBits(pRelayDrv->LatchIo.CloseIo.Port, pRelayDrv->LatchIo.CloseIo.Pin);
        Io_SetBits(pRelayDrv->LatchIo.OpenIo.Port, pRelayDrv->LatchIo.OpenIo.Pin);
      }

      pRelayDrv->pRam->Mode = RELAY_DRV_STOP_IMPULSE_MODE;

      return true;
    }
    else if (pRelayDrv->pRam->Mode == RELAY_DRV_STOP_IMPULSE_MODE)
    {
      Io_ResetBits(pRelayDrv->LatchIo.CloseIo.Port, pRelayDrv->LatchIo.CloseIo.Pin);
      Io_ResetBits(pRelayDrv->LatchIo.OpenIo.Port, pRelayDrv->LatchIo.OpenIo.Pin);

      pRelayDrv->pRam->Mode = RELAY_DRV_IDLE_MODE;
    }
  }

  return false;
}
//-----------------------------------------------------------------------------------------------------------
static OsErr_t RelayDrv_Set(RelayDrv_t *pRelayDrv, RelayDrvState_t state)
{
  if (pRelayDrv->pRam->Mode != RELAY_DRV_IDLE_MODE)
  {
    return OS_ERR_BUSY;
  }

  if (pRelayDrv->pRam->State != state)
  {
    pRelayDrv->pRam->State = state;

    if (pRelayDrv->Type == RELAY_DRV_NORMAL)
    {
      if ((pRelayDrv->NormalIo.ActiveLevel == RELAY_DRV_HIGH_ACTIVE && state == RELAY_DRV_CLOSE) ||
          (pRelayDrv->NormalIo.ActiveLevel == RELAY_DRV_LOW_ACTIVE && state == RELAY_DRV_OPEN))
      {
        Io_SetBits(pRelayDrv->NormalIo.Io.Port, pRelayDrv->NormalIo.Io.Pin);
      }
      else
      {
        Io_ResetBits(pRelayDrv->NormalIo.Io.Port, pRelayDrv->NormalIo.Io.Pin);
      }
    }
    else
    {
      pRelayDrv->pRam->Mode = RELAY_DRV_START_IMPULSE_MODE;
    }
  }

  return OS_ERR_OK;
}
//-----------------------------------------------------------------------------------------------------------
static RelayDrvType_t RelayDrv_GetType(RelayDrv_t *pRelayDrv)
{
  return pRelayDrv->Type;
}