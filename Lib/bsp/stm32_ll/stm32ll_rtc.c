#include "rtc_func.h"
#include "stm32ll.h"

void Rtc_Init(void) {}

void Rtc_SetDate(RtcDate_t *pDate) {
  LL_RTC_DateTypeDef RTC_DateStruct;

  RTC_DateStruct.Year = __LL_RTC_CONVERT_BIN2BCD(pDate->Year);
  RTC_DateStruct.Month = __LL_RTC_CONVERT_BIN2BCD(pDate->Month);
  RTC_DateStruct.Day = __LL_RTC_CONVERT_BIN2BCD(pDate->Day);
  RTC_DateStruct.WeekDay = __LL_RTC_CONVERT_BIN2BCD(pDate->Week);

  LL_RTC_DATE_Init(RTC, LL_RTC_FORMAT_BCD, &RTC_DateStruct);
}
void Rtc_GetDate(RtcDate_t *pDate) {
  pDate->Year = Rtc_GetYear();
  pDate->Month = Rtc_GetMonth();
  pDate->Day = Rtc_GetDay();
  pDate->Week = Rtc_GetWeek();
}

void Rtc_SetTime(RtcTime_t *pTime) {
  LL_RTC_TimeTypeDef RTC_TimeStruct;

  RTC_TimeStruct.TimeFormat = LL_RTC_TIME_FORMAT_AM_OR_24;
  RTC_TimeStruct.Hours = __LL_RTC_CONVERT_BIN2BCD(pTime->Hour);
  RTC_TimeStruct.Minutes = __LL_RTC_CONVERT_BIN2BCD(pTime->Minute);
  RTC_TimeStruct.Seconds = __LL_RTC_CONVERT_BIN2BCD(pTime->Second);

  LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BCD, &RTC_TimeStruct);
}

void Rtc_GetTime(RtcTime_t *pTime) {
  pTime->Hour = Rtc_GetHour();
  pTime->Minute = Rtc_GetMinute();
  pTime->Second = Rtc_GetSecond();
}

void Rtc_SetDateAndTime(RtcDate_t *pDate, RtcTime_t *pTime) {
  Rtc_SetDate(pDate);
  Rtc_SetTime(pTime);
}

// You must call Rtc_GetDate() after Rtc_GetTime() to unlock the values in the higher - order calendar shadow registers to ensure consistency
// between the time and date values. Reading RTC current time locks the values in calendar shadow registers until Current date is read to ensure
// consistency between the time and date values.
void Rtc_GetDataAndTime(RtcDate_t *pDate, RtcTime_t *pTime) {
  Rtc_GetTime(pTime);
  Rtc_GetDate(pDate);
}

static void StartConfig(void) {
  /* Disable the write protection for RTC registers */
  LL_RTC_DisableWriteProtection(RTC);

  /* Set Initialization mode */
  LL_RTC_EnterInitMode(RTC);
}

static void StopConfig(void) {
  /* Exit Initialization mode */
  LL_RTC_DisableInitMode(RTC);

  /* If  RTC_CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
  if (LL_RTC_IsShadowRegBypassEnabled(RTC) == 0U) {
    LL_RTC_WaitForSynchro(RTC);
  }
  /* Enable the write protection for RTC registers */
  LL_RTC_EnableWriteProtection(RTC);
}

void Rtc_SetYear(u8 year) {
  StartConfig();
  LL_RTC_DATE_SetYear(RTC, __LL_RTC_CONVERT_BIN2BCD(year));
  StopConfig();
}
u8 Rtc_GetYear(void) { return __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetYear(RTC)); }

void Rtc_SetMonth(u8 month) {
  StartConfig();
  LL_RTC_DATE_SetMonth(RTC, __LL_RTC_CONVERT_BIN2BCD(month));
  StopConfig();
}
u8 Rtc_GetMonth(void) { return __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetMonth(RTC)); }

void Rtc_SetDay(u8 day) {
  StartConfig();
  LL_RTC_DATE_SetDay(RTC, __LL_RTC_CONVERT_BIN2BCD(day));
  StopConfig();
}
u8 Rtc_GetDay(void) { return __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetDay(RTC)); }

void Rtc_SetWeek(u8 week) {
  StartConfig();
  LL_RTC_DATE_SetWeekDay(RTC, __LL_RTC_CONVERT_BIN2BCD(week));
  StopConfig();
}
u8 Rtc_GetWeek(void) { return __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetWeekDay(RTC)); }

void Rtc_SetHour(u8 hour) {
  StartConfig();
  LL_RTC_TIME_SetHour(RTC, __LL_RTC_CONVERT_BIN2BCD(hour));
  StopConfig();
}
u8 Rtc_GetHour(void) { return __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC)); }

void Rtc_SetMinute(u8 minute) {
  StartConfig();
  LL_RTC_TIME_SetMinute(RTC, __LL_RTC_CONVERT_BIN2BCD(minute));
  StopConfig();
}
u8 Rtc_GetMinute(void) { return __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC)); }

void Rtc_SetSecond(u8 second) {
  StartConfig();
  LL_RTC_TIME_SetSecond(RTC, __LL_RTC_CONVERT_BIN2BCD(second));
  StopConfig();
}
u8 Rtc_GetSecond(void) { return __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC)); }

#define PREDIV_S 255
// Second fraction = (PREDIV_S - SS) / (PREDIV_S + 1)
u16 Rtc_GetSubSecond(void) { return (u16)((float)(PREDIV_S - LL_RTC_TIME_GetSubSecond(RTC)) / (float)(PREDIV_S + 1) * 1000.0); }

void Rtc_IncHour(void) {
  LL_RTC_DisableWriteProtection(RTC);
  LL_RTC_TIME_IncHour(RTC);
  LL_RTC_EnableWriteProtection(RTC);
}

void Rtc_DecHour(void) {
  LL_RTC_DisableWriteProtection(RTC);
  LL_RTC_TIME_DecHour(RTC);
  LL_RTC_EnableWriteProtection(RTC);
}

// region BackupRegister
#define BACKUP_REGISTER_SUM 5
static const u32 BackupRegisterList[] = {LL_RTC_BKP_DR0, LL_RTC_BKP_DR1, LL_RTC_BKP_DR2, LL_RTC_BKP_DR3, LL_RTC_BKP_DR4};

void Rtc_SetBackUpRegister(u8 add, u32 value) {
  if (add < BACKUP_REGISTER_SUM) {
    LL_RTC_BKP_SetRegister(TAMP, BackupRegisterList[add], value);
  }
}

u32 Rtc_GetBackUpRegister(u8 add) {
  if (add < BACKUP_REGISTER_SUM) {
    return LL_RTC_BKP_GetRegister(TAMP, BackupRegisterList[add]);
  }

  return 0;
}
// endregion
