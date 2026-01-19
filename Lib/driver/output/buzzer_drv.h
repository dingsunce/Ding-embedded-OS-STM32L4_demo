#ifndef _BUZZER_DRV_H
#define _BUZZER_DRV_H

#include "d_define.h"
#include "func.h"

#define BUZZER_DRV_TIMEBASE 10 // 10ms

typedef enum
{
  BUZZER_DRV_OFF_MODE,
  BUZZER_DRV_ON_MODE,
  BUZZER_DRV_FLASH_MODE,
} BuzzerDrvMode_t;

typedef struct
{
  BuzzerDrvMode_t Mode;
  u16             ActiveTime;
  bool            IsWithinFlashOn;
  u16             FlashOnTime;  // unit is BUZZER_DRV_TIMEBASE
  u16             FlashOffTime; // unit is BUZZER_DRV_TIMEBASE
  u16             FlashTick;    // unit is BUZZER_DRV_TIMEBASE
} BuzzerDrvRam_t;

typedef const struct
{
  u8              Id;
  PwmTimerId_t    TimerId;
  BuzzerDrvRam_t *pRam;
  Io_t            Io;
} BuzzerDrv_t;

/*
#if BUZZER_SUM > 0
// The ram data used in BuzzerTable.
static BuzzerDrvRam_t BuzzerRam[BUZZER_SUM];

// The Buzzer table.
static BuzzerDrv_t BuzzerTable[BUZZER_SUM] = {
    {
        0,          // Id
        0,          // TimerId
        BuzzerRam,  // pRam
        {
            (IoPort_t)0,  // Io port
            (IoPin_t)0,   // Io pin
        },
    },
};

Buzzer_Init(BuzzerTable, BUZZER_SUM);

#endif
*/
void Buzzer_Init(BuzzerDrv_t *pTable, u8 sum);

void Buzzer_Off(u8 id);

void Buzzer_On(u8 id, u16 activeTime);

void Buzzer_Flash(u8 id, u16 activeTime, u16 flashOnTime, u16 flashOffTime);

#endif