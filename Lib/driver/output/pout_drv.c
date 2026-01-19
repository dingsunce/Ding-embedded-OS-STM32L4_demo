#include "pout_drv.h"
#include "d_mem.h"
#include "d_message.h"
#include "d_process.h"

static void PoutDrv_Init(PoutDrv_t *pPoutDrv);
static void PoutDrv_WriteData(PoutDrv_t *pPoutDrv, const u8 *pData, u8 length);
static void PoutDrv_WriteByte(PoutDrv_t *pPoutDrv, u8 data);
static void PoutDrv_WriteTakeEffect(PoutDrv_t *pPoutDrv);

typedef struct
{
  DProcess_t Process;
  PoutDrv_t *pTable;
  u8         Sum;
} PoutTask_t;

static PoutTask_t PoutTask;
//-----------------------------------------------------------------------------------------------------------
void Pout_Init(PoutDrv_t *pTable, u8 sum)
{
  PoutTask.pTable = pTable;
  PoutTask.Sum = sum;

  for (u8 i = 0; i < PoutTask.Sum; i++)
  {
    PoutDrv_Init(PoutTask.pTable + i);
  }
}
//-----------------------------------------------------------------------------------------------------------
void Pout_WriteData(u8 id, u8 *pData, u8 length)
{
  if (id < PoutTask.Sum)
  {
    PoutDrv_WriteData(PoutTask.pTable + id, pData, length);
  }
}
//-----------------------------------------------------------------------------------------------------------
static void PoutDrv_Init(PoutDrv_t *pPoutDrv)
{
  Io_Init(pPoutDrv->DataIo.Port, pPoutDrv->DataIo.Pin, IO_OUTPUT_PUSH_PULL_LOW);
  Io_Init(pPoutDrv->ShiftClockIo.Port, pPoutDrv->ShiftClockIo.Pin, IO_OUTPUT_PUSH_PULL_LOW);
  Io_Init(pPoutDrv->StoreClockIo.Port, pPoutDrv->StoreClockIo.Pin, IO_OUTPUT_PUSH_PULL_LOW);
}
//-----------------------------------------------------------------------------------------------------------
static void PoutDrv_WriteData(PoutDrv_t *pPoutDrv, const u8 *pData, u8 length)
{
  for (u8 i = 0; i < length; i++)
  {
    PoutDrv_WriteByte(pPoutDrv, *(pData + i));
  }

  PoutDrv_WriteTakeEffect(pPoutDrv);
}
//-----------------------------------------------------------------------------------------------------------
static void PoutDrv_WriteByte(PoutDrv_t *pPoutDrv, u8 data)
{
  for (u8 i = 0; i < 8; i++)
  {
    if (GET_BIT(data, 7 - i))
    {
      Io_SetBits(pPoutDrv->DataIo.Port, pPoutDrv->DataIo.Pin);
    }
    else
    {
      Io_ResetBits(pPoutDrv->DataIo.Port, pPoutDrv->DataIo.Pin);
    }

    Io_ResetBits(pPoutDrv->ShiftClockIo.Port, pPoutDrv->ShiftClockIo.Pin);
    Io_SetBits(pPoutDrv->ShiftClockIo.Port, pPoutDrv->ShiftClockIo.Pin);
  }
}
//-----------------------------------------------------------------------------------------------------------
static void PoutDrv_WriteTakeEffect(PoutDrv_t *pPoutDrv)
{
  Io_ResetBits(pPoutDrv->StoreClockIo.Port, pPoutDrv->StoreClockIo.Pin);
  Io_SetBits(pPoutDrv->StoreClockIo.Port, pPoutDrv->StoreClockIo.Pin);
}