#ifndef RTC_DRV_H
#define RTC_DRV_H

#include "d_define.h"
#include "func.h"

#define RTC_DRV_TIMEBASE 100 // 100ms

#define RTC_DRV_SUM 7

typedef struct
{
  u8 Second;
  u8 Minute;
  u8 Hour;
  u8 Week;
  u8 Day;
  u8 Month;
  u8 Year;
} RtcDrvTime_t;

typedef struct
{
  RtcDrvTime_t Time;
  RtcDrvTime_t LastTime;
  RtcDrvTime_t ReadTime;
  RtcDrvTime_t WriteTime;
} RtcDrvRam_t;

typedef const struct
{
  u8           Id;
  RtcDrvRam_t *pRam;
  void (*CallBackFunc)(u8 id, RtcDrvTime_t *pTime);
} RtcDrv_t;

/*
#if RTC_SUM > 0
// The ram data used in RtcTable.
static RtcDrvRam_t RtcRam[RTC_SUM];

// The rtc table.
static RtcDrv_t RtcTable[RTC_SUM] = {
    {
        0,                   // Id
        RtcRam,              // pRam
        CallBackFunc,        // CallBackFunc
    },
};


Rtc_I2cInit(RtcTable, RTC_SUM);


#endif
*/
void Rtc_I2cInit(RtcDrv_t *pTable, u8 sum);

void Rtc_I2cSetAll(u8 id, RtcDrvTime_t *pTime);

void Rtc_I2cSetTime(u8 id, u8 hour, u8 minute, u8 second);

void Rtc_I2cSetDate(u8 id, u8 year, u8 month, u8 day, u8 week);

void Rtc_I2cSetHour(u8 id, u8 hour);

RtcDrvTime_t *Rtc_Get(u8 id);

#endif