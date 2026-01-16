#include "dip_drv.h"
#include "d_message.h"
#include "memory.h"
#include "process.h"

#define DIP_PROCESS_MSG      0
#define DIP_DRV_DEBOUNCE_THR (50 / DIP_DRV_TIMEBASE) // 50ms

static void        DipDrv_GenerateEvent(DipDrv_t *pDipDrv, DipDrvPos_t position);
static void        DipDrv_Scan(DipDrv_t *pDipDrv);
static DipDrvPos_t DipDrv_GetPhysicalPosition(DipDrv_t *pDipDrv);
static void        DipDrv_Init(DipDrv_t *pDipDrv);
static void        DipDrv_Process(DipDrv_t *pDipDrv);
static DipDrvPos_t DipDrv_GetCurrentPosition(DipDrv_t *pDipDrv);
static u8          Dip_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg);

typedef struct
{
  DProcess_t Process;
  DipDrv_t  *pTable;
  u8         Sum;
} DipTask_t;

static DipTask_t DipTask;
//-----------------------------------------------------------------------------------------------------------
void Dip_Init(DipDrv_t *pTable, u8 sum)
{
  DProcess_InitStructure(&DipTask.Process, Dip_Handler);
  DProcess_Start(&DipTask.Process);

  DipTask.pTable = pTable;
  DipTask.Sum = sum;

  for (u8 i = 0; i < DipTask.Sum; i++)
  {
    DipDrv_Init(DipTask.pTable + i);
  }

  DMsg_SendInstant((DProcess_t *)(&DipTask), DIP_PROCESS_MSG, MSG_NO_ARG);
}
//-----------------------------------------------------------------------------------------------------------
static u8 Dip_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg)
{
  PROCESS_SCHEDULE_BEGIN()

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == DIP_PROCESS_MSG);

    for (u8 i = 0; i < DipTask.Sum; i++)
    {
      DipDrv_Process(DipTask.pTable + i);
    }

    DMsg_SendLater((DProcess_t *)(&DipTask), DIP_PROCESS_MSG, MSG_NO_ARG, DIP_DRV_TIMEBASE);
  }

  PROCESS_SCHEDULE_END()
}
//-----------------------------------------------------------------------------------------------------------
DipDrvPos_t Dip_GetCurrentPosition(u8 dipId)
{
  return DipDrv_GetCurrentPosition(DipTask.pTable + dipId);
}
//-----------------------------------------------------------------------------------------------------------
static void DipDrv_Init(DipDrv_t *pDipDrv)
{
  Io_Init(pDipDrv->Io.Port, pDipDrv->Io.Pin, IO_INPUT_PULL_UP);
  pDipDrv->pRam->DebounceTime = 0;
  pDipDrv->pRam->Position = DipDrv_GetPhysicalPosition(pDipDrv);
  pDipDrv->pRam->LastPosition = pDipDrv->pRam->Position;
}
//-----------------------------------------------------------------------------------------------------------
static void DipDrv_Process(DipDrv_t *pDipDrv)
{
  DipDrv_Scan(pDipDrv);
  if (pDipDrv->pRam->Position != pDipDrv->pRam->LastPosition)
  {
    pDipDrv->pRam->LastPosition = pDipDrv->pRam->Position;

    DipDrv_GenerateEvent(pDipDrv, pDipDrv->pRam->Position);
  }
}
//-----------------------------------------------------------------------------------------------------------
static DipDrvPos_t DipDrv_GetCurrentPosition(DipDrv_t *pDipDrv)
{
  return pDipDrv->pRam->Position;
}
//-----------------------------------------------------------------------------------------------------------
static void DipDrv_Scan(DipDrv_t *pDipDrv)
{
  DipDrvPos_t physicalPosition = DipDrv_GetPhysicalPosition(pDipDrv);

  if (physicalPosition != pDipDrv->pRam->Position)
  {
    pDipDrv->pRam->DebounceTime++;
    if (pDipDrv->pRam->DebounceTime >= DIP_DRV_DEBOUNCE_THR)
    {
      pDipDrv->pRam->DebounceTime = 0;
      pDipDrv->pRam->Position = physicalPosition;
    }
  }
  else
  {
    pDipDrv->pRam->DebounceTime = 0;
  }
}
//-----------------------------------------------------------------------------------------------------------
static void DipDrv_GenerateEvent(DipDrv_t *pDipDrv, DipDrvPos_t position)
{
  if (pDipDrv->CallBackFunc != NULL)
  {
    pDipDrv->CallBackFunc(pDipDrv->Id, position);
  }
}
//-----------------------------------------------------------------------------------------------------------
static DipDrvPos_t DipDrv_GetPhysicalPosition(DipDrv_t *pDipDrv)
{
  if (Io_GetBits(pDipDrv->Io.Port, pDipDrv->Io.Pin) != 0)
  {
    return DIP_DRV_OFF;
  }

  return DIP_DRV_ON;
}