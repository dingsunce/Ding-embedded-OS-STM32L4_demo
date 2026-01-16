#ifndef DIP_DRV_H
#define DIP_DRV_H

#include "d_define.h"
#include "func.h"

#define DIP_DRV_TIMEBASE 10 // 10ms

typedef enum
{
  DIP_DRV_OFF,
  DIP_DRV_ON,
} DipDrvPos_t;

typedef struct
{
  DipDrvPos_t Position;
  u16         DebounceTime;
  DipDrvPos_t LastPosition;
} DipDrvRam_t;

typedef const struct
{
  u8           Id;
  DipDrvRam_t *pRam;
  Io_t         Io;
  void (*CallBackFunc)(u8 id, DipDrvPos_t position);
} DipDrv_t;

/*
#if DIP_SUM > 0
// The ram data used in DipTable.
static DipDrvRam_t DipRam[DIP_SUM];

// The dip table.
static DipDrv_t DipTable[DIP_SUM] = {
    {
        0,       // Id
        DipRam,  // pRam
        {
            (IoPort_t)0,  // Io port
            (IoPin_t)0,   // Io pin
        },
        CallbackFunc,  // CallBackFunc
    },
};

Dip_Init(DipTable, DIP_SUM);

#endif
 */
void Dip_Init(DipDrv_t *pTable, u8 sum);

DipDrvPos_t Dip_GetCurrentPosition(u8 dipId);

#endif