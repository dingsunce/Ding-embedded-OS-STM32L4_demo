#ifndef MOP_DRV_H
#define MOP_DRV_H

#include "d_define.h"
#include "func.h"

typedef struct
{
  u8  Index;
  u8  LastIndex;
  u8  MeaCount;
  u16 MeaSum;
  u16 MeaAvg;
} MopDrvRam_t;

typedef const struct
{
  u8           Id;
  MopDrvRam_t *pRam;
  Io_t         Io;
  AdcChannel_t Channel;
  u16         *List;
  u8           ListSize;
  void (*CallBackFunc)(u8 id, u8 Index);
} MopDrv_t;

/*
#if MOP_SUM > 0
// The ram data used in MopTable.
static MopDrvRam_t MopRam[MOP_SUM];

// The mop table.
static MopDrv_t MopTable[MOP_SUM] = {
    {
        0,       // Id
        MopRam,  // pRam
        {
            (IoPort_t)0,  // Io port
            (IoPin_t)0,   // Io pin
        },
        (AdcChannel_t)0,     // Channel
        NULL,                // List
        0,                   // ListSize
        CallbackFunc,  // CallBackFunc
    },
};

Mop_Init(MopTable, MOP_SUM);

#endif
*/
void Mop_Init(MopDrv_t *pTable, u8 sum);

u8 Mop_GetCurrentIndex(u8 mopId);

#endif