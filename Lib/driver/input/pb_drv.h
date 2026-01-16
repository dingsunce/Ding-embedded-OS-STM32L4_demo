#ifndef PB_DRV_H
#define PB_DRV_H

#include "d_define.h"
#include "func.h"

// #define PB_CONFIG

#ifndef PB_CONFIG
// #define PB_DRV_PULL_UP_IO
// #define PB_DRV_USER_DEFINED_STATE
// #define PB_DRV_ALTERNATING_CURRENT // 10ms high and 10ms low
#endif

#ifndef PB_DRV_ALTERNATING_CURRENT
#define PB_DRV_TIMEBASE 10 // 10ms
#else
#define PB_DRV_TIMEBASE 1 // 1ms
#endif

typedef enum
{
  PB_DRV_PRESS_EVENT,
  PB_DRV_RELEASE_EVENT,
  PB_DRV_SHORT_PRESS_EVENT,
  PB_DRV_DOUBLE_PRESS_EVENT,
  PB_DRV_LONG_PRESS_START_EVENT,
  PB_DRV_LONG_PRESS_REPEAT_EVENT,
  PB_DRV_LONG_PRESS_STOP_EVENT,
  PB_DRV_STUCK_EVENT,
  PB_DRV_FREE_EVENT
} PbDrvEvent_t;

typedef enum
{
  PB_DRV_RELEASE_MODE,
  PB_DRV_SHORT_MODE,
  PB_DRV_LONG_MODE,
  PB_DRV_PRE_DOUBLE_MODE,
  PB_DRV_DOUBLE_MODE,
  PB_DRV_STUCK_MODE,
} PbDrvMode_t;

typedef enum
{
  PB_DRV_HIGH_ACTIVE,
  PB_DRV_LOW_ACTIVE,
} PbDrvActiveLevel_t;

typedef enum
{
  PB_DRV_RELEASED,
  PB_DRV_PRESSED,
} PbDrvState_t;

#ifdef PB_DRV_USER_DEFINED_STATE
typedef PbDrvState_t (*PbDrvGetStateFunc_t)(u8 id);
#endif

typedef struct
{
  u16          DebounceTime;
  PbDrvState_t State;

  u16 PressedTime;
  u16 ReleasedTime;
  u16 RepeatTime;

  PbDrvMode_t Mode;
} PbDrvRam_t;

typedef const struct
{
  u8          Id;
  PbDrvRam_t *pRam;
#ifndef PB_DRV_USER_DEFINED_STATE
  Io_t               Io;
  PbDrvActiveLevel_t ActiveLevel;
#else
  PbDrvGetStateFunc_t GetStateFunc;
#endif
  void (*CallBackFunc)(u8 id, PbDrvEvent_t event);
} PbDrv_t;

/*
#if PB_SUM > 0
// The ram data used in PbTable.
static PbDrvRam_t PbRam[PB_SUM];

// The pushbutton table.
static PbDrv_t PbTable[PB_SUM] = {
    {
        0,       // Id
        PbRam,  // pRam
        {
            (IoPort_t)0,  // Io port
            (IoPin_t)0,   // Io pin
        },
        (PbDrvActiveLevel_t)0,  // ActiveLevel
        CallbackFunc,           // CallBackFunc
    },
};

Pb_Init(PbTable, PB_SUM);

#endif
*/
void Pb_Init(PbDrv_t *pTable, u8 sum);

#endif