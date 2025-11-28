#ifndef _RTC_FUNC_H
#define _RTC_FUNC_H

#include "define.h"

typedef struct {
  u8 Year;   // 0 ~ 99
  u8 Month;  // 1 ~ 12
  u8 Day;    // 1 ~ 31
  u8 Week;   // 1 ~ 7
} RtcDate_t;

typedef struct {
  u8 Hour;    // 0 ~ 23
  u8 Minute;  // 0 ~ 59
  u8 Second;  // 0 ~ 59
} RtcTime_t;

extern void Rtc_Init(void);

extern void Rtc_SetDate(RtcDate_t *pDate);
extern void Rtc_GetDate(RtcDate_t *pDate);

extern void Rtc_SetTime(RtcTime_t *pTime);
extern void Rtc_GetTime(RtcTime_t *pTime);

extern void Rtc_SetDateAndTime(RtcDate_t *pDate, RtcTime_t *pTime);
extern void Rtc_GetDataAndTime(RtcDate_t *pDate, RtcTime_t *pTime);

extern void Rtc_SetYear(u8 year);
extern u8 Rtc_GetYear(void);

extern void Rtc_SetMonth(u8 month);
extern u8 Rtc_GetMonth(void);

extern void Rtc_SetDay(u8 day);
extern u8 Rtc_GetDay(void);

extern void Rtc_SetWeek(u8 week);
extern u8 Rtc_GetWeek(void);

extern void Rtc_SetHour(u8 hour);
extern u8 Rtc_GetHour(void);

extern void Rtc_SetMinute(u8 minute);
extern u8 Rtc_GetMinute(void);

extern void Rtc_SetSecond(u8 second);
extern u8 Rtc_GetSecond(void);

extern u16 Rtc_GetSubSecond(void);

extern void Rtc_IncHour(void);
extern void Rtc_DecHour(void);

extern void Rtc_SetBackUpRegister(u8 add, u32 value);
extern u32 Rtc_GetBackUpRegister(u8 add);

#endif
