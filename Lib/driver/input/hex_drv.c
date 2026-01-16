#include "hex_drv.h"
#include "d_message.h"
#include "memory.h"
#include "process.h"

#define HEX_DRV_DEBOUNCE_THR (50 / HEX_DRV_TIMEBASE) // 50ms

// Static functions.
static void HexDrv_GenerateEvent(HexDrv_t *pHexDrv, u8 position);
static void HexDrv_Scan(HexDrv_t *pHexDrv);
static u8   HexDrv_GetPhysicalPosition(HexDrv_t *pHexDrv);
static void HexDrv_Init(HexDrv_t *pHexDrv);
static void HexDrv_Process(HexDrv_t *pHexDrv);
static u8   HexDrv_GetCurrentPosition(HexDrv_t *pHexDrv);
static u8   Hex_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg);

typedef struct
{
  DProcess_t Process;
  HexDrv_t  *pTable;
  u8         Sum;
} HexTask_t;

static HexTask_t HexTask;

#define HEX_PROCESS_MSG 0
//-----------------------------------------------------------------------------------------------------------
void Hex_Init(HexDrv_t *pTable, u8 sum)
{
  DProcess_InitStructure(&HexTask.Process, Hex_Handler);
  DProcess_Start(&HexTask.Process);

  HexTask.pTable = pTable;

  HexTask.Sum = sum;

  for (u8 i = 0; i < HexTask.Sum; i++)
  {
    HexDrv_Init(HexTask.pTable + i);
  }

  DMsg_SendInstant((DProcess_t *)(&HexTask), HEX_PROCESS_MSG, MSG_NO_ARG);
}
//-----------------------------------------------------------------------------------------------------------
static u8 Hex_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg)
{
  PROCESS_SCHEDULE_BEGIN()

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == HEX_PROCESS_MSG);

    for (u8 i = 0; i < HexTask.Sum; i++)
    {
      HexDrv_Process(HexTask.pTable + i);
    }

    DMsg_SendLater((DProcess_t *)(&HexTask), HEX_PROCESS_MSG, MSG_NO_ARG, HEX_DRV_TIMEBASE);
  }

  PROCESS_SCHEDULE_END()
}
//-----------------------------------------------------------------------------------------------------------
u8 Hex_GetCurrentPosition(u8 hexId)
{
  return HexDrv_GetCurrentPosition(HexTask.pTable + hexId);
}
//-----------------------------------------------------------------------------------------------------------
static void HexDrv_Init(HexDrv_t *pHexDrv)
{
  for (u8 i = 0; i < HEX_IO_SUM; i++)
  {
    Io_Init(pHexDrv->Io[i].Port, pHexDrv->Io[i].Pin, IO_INPUT_PULL_UP);
  }

  pHexDrv->pRam->DebounceTime = 0;

  pHexDrv->pRam->Position = HexDrv_GetPhysicalPosition(pHexDrv);
  pHexDrv->pRam->LastPosition = pHexDrv->pRam->Position;
}
//-----------------------------------------------------------------------------------------------------------
static void HexDrv_Process(HexDrv_t *pHexDrv)
{
  HexDrv_Scan(pHexDrv);

  if (pHexDrv->pRam->Position != pHexDrv->pRam->LastPosition)
  {
    pHexDrv->pRam->LastPosition = pHexDrv->pRam->Position;

    HexDrv_GenerateEvent(pHexDrv, pHexDrv->pRam->Position);
  }
}
//-----------------------------------------------------------------------------------------------------------
static u8 HexDrv_GetCurrentPosition(HexDrv_t *pHexDrv)
{
  return pHexDrv->pRam->Position;
}
//-----------------------------------------------------------------------------------------------------------
static void HexDrv_Scan(HexDrv_t *pHexDrv)
{
  u8 physicalPosition = HexDrv_GetPhysicalPosition(pHexDrv);

  if (physicalPosition != pHexDrv->pRam->Position)
  {
    pHexDrv->pRam->DebounceTime++;
    if (pHexDrv->pRam->DebounceTime >= HEX_DRV_DEBOUNCE_THR)
    {
      pHexDrv->pRam->DebounceTime = 0;
      pHexDrv->pRam->Position = physicalPosition;
    }
  }
  else
  {
    pHexDrv->pRam->DebounceTime = 0;
  }
}
//-----------------------------------------------------------------------------------------------------------
static void HexDrv_GenerateEvent(HexDrv_t *pHexDrv, u8 position)
{
  if (pHexDrv->CallBackFunc != NULL)
  {
    pHexDrv->CallBackFunc(pHexDrv->Id, position);
  }
}
//-----------------------------------------------------------------------------------------------------------
static u8 HexDrv_GetPhysicalPosition(HexDrv_t *pHexDrv)
{
  u8 position = 0;

  for (u8 i = 0; i < HEX_IO_SUM; i++)
  {
    if (Io_GetBits(pHexDrv->Io[i].Port, pHexDrv->Io[i].Pin) == 0)
    {
      PLACE_BIT(position, i);
    }
  }

  return position;
}