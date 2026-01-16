#include "mop_drv.h"
#include "d_message.h"
#include "memory.h"
#include "process.h"

#define MOP_DRV_TIMEBASE 10 // 10ms

#define MOP_PROCESS_MSG 0

#define MOP_DRV_DEBOUNCE 5
#define MOP_DRV_MEA_SUM  10

// Static functions.
static u8   MopDrv_AdcToIndex(MopDrv_t *pMopDrv, u16 adc);
static void MopDrv_GenerateEvent(MopDrv_t *pMopDrv, u8 index);
static void MopDrv_Init(MopDrv_t *pMopDrv);
static void MopDrv_Process(MopDrv_t *pMopDrv);
static u8   MopDrv_GetCurrentIndex(MopDrv_t *pMopDrv);
static u8   Mop_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg);

typedef struct
{
  DProcess_t Process;
  MopDrv_t  *pTable;
  u8         Sum;
} MopTask_t;

static MopTask_t MopTask;
//-----------------------------------------------------------------------------------------------------------
void Mop_Init(MopDrv_t *pTable, u8 sum)
{
  DProcess_InitStructure(&MopTask.Process, Mop_Handler);
  DProcess_Start(&MopTask.Process);

  MopTask.pTable = pTable;

  MopTask.Sum = sum;

  for (u8 i = 0; i < MopTask.Sum; i++)
  {
    MopDrv_Init(MopTask.pTable + i);
  }

  DMsg_SendInstant((DProcess_t *)(&MopTask), MOP_PROCESS_MSG, MSG_NO_ARG);
}
//-----------------------------------------------------------------------------------------------------------
static u8 Mop_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg)
{
  PROCESS_SCHEDULE_BEGIN()

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == MOP_PROCESS_MSG);

    for (u8 i = 0; i < MopTask.Sum; i++)
    {
      MopDrv_Process(MopTask.pTable + i);
    }

    DMsg_SendLater((DProcess_t *)(&MopTask), MOP_PROCESS_MSG, MSG_NO_ARG, MOP_DRV_TIMEBASE);
  }

  PROCESS_SCHEDULE_END()
}
//-----------------------------------------------------------------------------------------------------------
u8 Mop_GetCurrentIndex(u8 mopId)
{
  return MopDrv_GetCurrentIndex(MopTask.pTable + mopId);
}
//-----------------------------------------------------------------------------------------------------------
static void MopDrv_Init(MopDrv_t *pMopDrv)
{
#ifndef PLATFORM_STM32
  Io_Init(pMopDrv->Io.Port, pMopDrv->Io.Pin, IO_INPUT_FLOATING);
#endif
  pMopDrv->pRam->Index = MopDrv_AdcToIndex(pMopDrv, Adc_Convert(pMopDrv->Channel));
  pMopDrv->pRam->LastIndex = pMopDrv->pRam->Index;
  pMopDrv->pRam->MeaCount = 0;
  pMopDrv->pRam->MeaSum = 0;
  pMopDrv->pRam->MeaAvg = 0;
}
//-----------------------------------------------------------------------------------------------------------
static void MopDrv_Process(MopDrv_t *pMopDrv)
{
  pMopDrv->pRam->MeaSum += Adc_Convert(pMopDrv->Channel);
  pMopDrv->pRam->MeaCount++;
  if (pMopDrv->pRam->MeaCount >= MOP_DRV_MEA_SUM)
  {
    pMopDrv->pRam->MeaCount = 0;
    pMopDrv->pRam->MeaAvg = pMopDrv->pRam->MeaSum / MOP_DRV_MEA_SUM;
    pMopDrv->pRam->Index = MopDrv_AdcToIndex(pMopDrv, pMopDrv->pRam->MeaAvg);
    pMopDrv->pRam->MeaSum = 0;

    if (pMopDrv->pRam->Index != pMopDrv->pRam->LastIndex)
    {
      pMopDrv->pRam->LastIndex = pMopDrv->pRam->Index;

      MopDrv_GenerateEvent(pMopDrv, pMopDrv->pRam->Index);
    }
  }
}
//-----------------------------------------------------------------------------------------------------------
static u8 MopDrv_GetCurrentIndex(MopDrv_t *pMopDrv)
{
  return pMopDrv->pRam->Index;
}
//-----------------------------------------------------------------------------------------------------------
static void MopDrv_GenerateEvent(MopDrv_t *pMopDrv, u8 index)
{
  if (pMopDrv->CallBackFunc != NULL)
  {
    pMopDrv->CallBackFunc(pMopDrv->Id, index);
  }
}
//-----------------------------------------------------------------------------------------------------------
static u8 MopDrv_AdcToIndex(MopDrv_t *pMopDrv, u16 adc)
{
  u16 *pWeight = pMopDrv->List;
  u16  thr;

  for (u8 i = 0; i < pMopDrv->ListSize; i++)
  {
    if (pMopDrv->pRam->Index > 0 && i == pMopDrv->pRam->Index - 1)
    {
      thr = *pWeight - MOP_DRV_DEBOUNCE;
    }
    else if (i == pMopDrv->pRam->Index)
    {
      thr = *pWeight + MOP_DRV_DEBOUNCE;
    }
    else
    {
      thr = *pWeight;
    }

    if (adc <= thr)
    {
      return i;
    }

    pWeight++;
  }

  return pMopDrv->ListSize;
}