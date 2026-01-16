#ifndef DEB_DRV_H
#define DEB_DRV_H

#include "d_define.h"
#include "func.h"

#define DEB_DRV_TIMEBASE 10 // 10ms

// #define DEB_DRV_PULL_UP_IO

typedef enum
{
  DEB_DRV_DISCONNECT,
  DEB_DRV_CONNECT,
} DebDrvState_t;

typedef enum
{
  DEB_DRV_HIGH_ACTIVE,
  DEB_DRV_LOW_ACTIVE,
} DebDrvActiveLevel_t;

typedef struct
{
  u16           DebTime;
  DebDrvState_t State;
  DebDrvState_t LastState;
} DebDrvRam_t;

#ifdef DEB_DRV_USER_DEFINED_STATE
typedef DebDrvState_t (*DebDrvGetStateFunc_t)(u8 id);
#endif

typedef const struct
{
  u8           Id;
  DebDrvRam_t *pRam;
#ifndef DEB_DRV_USER_DEFINED_STATE
  Io_t                Io;
  DebDrvActiveLevel_t ActiveLevel;
#else
  DebDrvGetStateFunc_t GetStateFunc;
#endif
  void (*CallBackFunc)(u8 id, DebDrvState_t state);
} DebDrv_t;

/*
#if DEB_SUM > 0
// The ram data used in PbTable.
static DebDrvRam_t DebRam[SW_SUM];

// The de-bounce table.
static DebDrv_t DebTable[SW_SUM] = {
    {
        0,       // Id
        DebRam,  // pRam
        {
            (IoPort_t)0,  // Io port
            (IoPin_t)0    // Io pin
        },
        (DebDrvActiveLevel_t)0,  // ActiveLevel
        CallbackFunc,            // Call back
    },
};

Deb_Init(DebTable, DEB_SUM);
#endif
 */
void Deb_Init(DebDrv_t *pTable, u8 sum);

DebDrvState_t Deb_GetCurrentState(u8 id);

#endif