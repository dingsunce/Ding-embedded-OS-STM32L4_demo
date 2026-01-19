#ifndef _LED_DRV_H
#define _LED_DRV_H

#include "d_define.h"
#include "func.h"

#ifndef LED_DRV_TIMEBASE
#define LED_DRV_TIMEBASE 10 // 10ms
#endif

typedef enum
{
  LED_DRV_HIGH_ACTIVE,
  LED_DRV_LOW_ACTIVE,
} LedDrvActiveLevel_t;

typedef enum
{
  LED_DRV_OFF_MODE,
  LED_DRV_ON_MODE,
  LED_DRV_FLASH_MODE,
} LedDrvMode_t;

typedef struct
{
  LedDrvMode_t Mode;
  u16          ActiveTime;
  bool         IsWithinFlashOn;
  u16          FlashOnTime;  // unit is LED_DRV_TIMEBASE
  u16          FlashOffTime; // unit is LED_DRV_TIMEBASE
  u16          FlashTick;    // unit is LED_DRV_TIMEBASE
} LedDrvRam_t;

typedef const struct
{
  u8                  Id;
  LedDrvRam_t        *pRam;
  Io_t                Io;
  LedDrvActiveLevel_t ActiveLevel;
} LedDrv_t;

/*
#if LED_SUM > 0
// The ram data used in LedTable.
static LedDrvRam_t LedRam[LED_SUM];

// The LED table.
static LedDrv_t LedTable[LED_SUM] = {
    {
        0,                // Id
        LedRam,           // pRam
        {
            (IoPort_t)0,  // Io port
            (IoPin_t)0,   // Io pin
        },
        (LedDrvActiveLevel_t)0,  // Active level
    },
};

Led_Init(LedTable, LED_SUM);

#endif
*/
void Led_Init(LedDrv_t *pTable, u8 sum);

void Led_Off(u8 id);

void Led_On(u8 id, u16 activeTime);

void Led_Flash(u8 id, u16 activeTime, u16 flashOnTime, u16 flashOffTime);

#endif