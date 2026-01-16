#include "pin_drv.h"
#include "d_mem.h"
#include "d_message.h"
#include "d_process.h"

static void PinDrv_Init(PinDrv_t *pPinDrv);
static void PinDrv_Process(PinDrv_t *pPinDrv);
static u8   PinDrv_GetValue(PinDrv_t *pPinDrv);
static u8   Pin_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg);

typedef struct
{
  DProcess_t Process;
  PinDrv_t  *pTable;
} PinTask_t;

#define PIN_PROCESS_MSG 0

#define PIN_DRV_TIMEBASE 11 // 11ms

static PinTask_t PinTask;
//-----------------------------------------------------------------------------------------------------------
void Pin_Init(PinDrv_t *pTable)
{
  DProcess_InitStructure(&PinTask.Process, Pin_Handler);
  DProcess_Start(&PinTask.Process);

  PinTask.pTable = pTable;

  PinDrv_Init(PinTask.pTable);

  DMsg_SendInstant((DProcess_t *)(&PinTask), PIN_PROCESS_MSG, MSG_NO_ARG);
}
//-----------------------------------------------------------------------------------------------------------
static u8 Pin_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg)
{
  PROCESS_SCHEDULE_BEGIN()

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == PIN_PROCESS_MSG);

    if (msgId == PIN_PROCESS_MSG)
    {
      PinDrv_Process(PinTask.pTable);

      DMsg_SendLater((DProcess_t *)(&PinTask), PIN_PROCESS_MSG, MSG_NO_ARG, PIN_DRV_TIMEBASE);
    }
  }

  PROCESS_SCHEDULE_END()
}
//-----------------------------------------------------------------------------------------------------------
u8 Pin_GetValue(void)
{
  return PinDrv_GetValue(PinTask.pTable);
}
//-----------------------------------------------------------------------------------------------------------
static void PinDrv_Init(PinDrv_t *pPinDrv)
{
  pPinDrv->pRam->Value = 0;

  Io_Init(pPinDrv->ShiftLoadIo.Port, pPinDrv->ShiftLoadIo.Pin, IO_OUTPUT_PUSH_PULL_LOW);
  Io_Init(pPinDrv->ClockIo.Port, pPinDrv->ClockIo.Pin, IO_OUTPUT_PUSH_PULL_LOW);
  Io_Init(pPinDrv->ClockInhibitIo.Port, pPinDrv->ClockInhibitIo.Pin, IO_OUTPUT_PUSH_PULL_LOW);
  Io_Init(pPinDrv->DataIo.Port, pPinDrv->DataIo.Pin, IO_INPUT_FLOATING);
}
//-----------------------------------------------------------------------------------------------------------
static void PinDrv_Process(PinDrv_t *pPinDrv)
{
  u8 value = 0;

  Io_SetBits(pPinDrv->ClockIo.Port, pPinDrv->ClockIo.Pin);

  Io_ResetBits(pPinDrv->ShiftLoadIo.Port, pPinDrv->ShiftLoadIo.Pin); // read the HC165
  Io_SetBits(pPinDrv->ShiftLoadIo.Port, pPinDrv->ShiftLoadIo.Pin);   // stop reading

  for (u8 i = 0; i < 8; i++)
  {
    Io_ResetBits(pPinDrv->ClockIo.Port, pPinDrv->ClockIo.Pin);
    value <<= 1;

    if (Io_GetBits(pPinDrv->DataIo.Port, pPinDrv->DataIo.Pin))
    {
      value |= 0x01;
    }

    Io_SetBits(pPinDrv->ClockIo.Port, pPinDrv->ClockIo.Pin);
  }

  Io_ResetBits(pPinDrv->ClockIo.Port, pPinDrv->ClockIo.Pin);

  pPinDrv->pRam->Value = value;
}
//-----------------------------------------------------------------------------------------------------------
static u8 PinDrv_GetValue(PinDrv_t *pPinDrv)
{
  return pPinDrv->pRam->Value;
}