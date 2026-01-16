#ifndef SW_DRV_H
#define SW_DRV_H

#include "d_define.h"
#include "func.h"

#define SW_DRV_PULL_UP_IO

typedef enum
{
  SW_DRV_OPEN,
  SW_DRV_CLOSED,
} SwDrvState_t;

typedef enum
{
  SW_DRV_HIGH_ACTIVE,
  SW_DRV_LOW_ACTIVE,
} SwDrvActiveLevel_t;

typedef struct
{
  u16          DebounceTime;
  SwDrvState_t State;
  SwDrvState_t LastState;
} SwDrvRam_t;

typedef const struct
{
  u8                 Id;
  SwDrvRam_t        *pRam;
  Io_t               Io;
  SwDrvActiveLevel_t ActiveLevel;
  void (*CallBackFunc)(u8 id, SwDrvState_t state);
} SwDrv_t;

/*
#if SW_SUM > 0
// The ram data used in PbTable.
static SwDrvRam_t SwRam[SW_SUM];

// The switch table.
static SwDrv_t SwTable[SW_SUM] = {
    {
        0,      // Id
        SwRam,  // pRam
        {
            (IoPort_t)0,  // Io port
            (IoPin_t)0    // Io pin
        },
        (SwDrvActiveLevel_t)0,  // ActiveLevel
        CallBackFunc,      // CallBackFunc
    },
};

Sw_Init(SwTable, SW_SUM);

#endif
*/

void Sw_Init(SwDrv_t *pTable, u8 sum);

SwDrvState_t Sw_GetCurrentState(u8 swId);

#endif