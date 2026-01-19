#ifndef _RELAY_DRV_H
#define _RELAY_DRV_H

#include "d_define.h"
#include "func.h"

#ifndef RELAY_DRV_TIMEBASE
#define RELAY_DRV_TIMEBASE 15 // 15ms to act
#endif

typedef enum
{
  RELAY_DRV_NORMAL,
  RELAY_DRV_LATCH,
} RelayDrvType_t;

typedef enum
{
  RELAY_DRV_OPEN,
  RELAY_DRV_CLOSE,
  RELAY_DRV_UNKNOWN,
} RelayDrvState_t;

typedef enum
{
  RELAY_DRV_HIGH_ACTIVE,
  RELAY_DRV_LOW_ACTIVE,
} RelayDrvActiveLevel_t;

typedef enum
{
  RELAY_DRV_IDLE_MODE,
  RELAY_DRV_START_IMPULSE_MODE,
  RELAY_DRV_STOP_IMPULSE_MODE,
} RelayDrvMode_t;

typedef struct
{
  RelayDrvState_t State;
  RelayDrvMode_t  Mode; // Only use when relay is latching
} RelayDrvRam_t;

typedef const struct
{
  u8             Id;
  RelayDrvRam_t *pRam;
  RelayDrvType_t Type;
  union {
    struct
    {
      Io_t CloseIo;
      Io_t OpenIo;
    } LatchIo;

    struct
    {
      RelayDrvActiveLevel_t ActiveLevel;
      Io_t                  Io;
    } NormalIo;
  };

} RelayDrv_t;

/*
#if RELAY_SUM > 0
// The ram data used in RelayTable.
static RelayDrvRam_t RelayRam[RELAY_SUM];

// The relay table.
static RelayDrv_t RelayTable[RELAY_SUM] = {
    {
        0,                // Id
        RelayRam,         // pRam
        RELAY_DRV_LATCH,  // Type
        .NormalIo =
            {
                RELAY_DRV_LOW_ACTIVE,  // ActiveLevel
                {
                    (IoPort_t)0,  // Io port
                    (IoPin_t)0,   // Io pin
                },
            },
    },
    {
        1,                // Id
        RelayRam + 1,     // pRam
        RELAY_DRV_LATCH,  // Type
        .LatchIo =
            {
                {
                    (IoPort_t)0,  // CloseIo port
                    (IoPin_t)0,   // CloseIo Io pin
                },
                {
                    (IoPort_t)0,  // OpenIo port
                    (IoPin_t)0,   // OpenIo pin
                },
            },
    },
};

Relay_Init(RelayTable, RELAY_SUM);

#endif
*/
void Relay_Init(RelayDrv_t *pTable, u8 sum);

void Relay_Set(u8 id, RelayDrvState_t state);

#endif