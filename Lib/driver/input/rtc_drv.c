#include "rtc_drv.h"
#include "d_mem.h"
#include "d_message.h"
#include "d_process.h"

#define RTC_ADD 0xD0

#define SECOND_INDEX      0
#define MINUTE_INDEX      1
#define HOUR_INDEX        2
#define DAY_OF_WEEK_INDEX 3
#define DAY_INDEX         4
#define MONTH_INDEX       5
#define YEAR_INDEX        6
#define CALIBRATION_INDEX 7

#define RTC_INVALID_VALUE 0xFF

// Static functions.
static void RtcDrv_ReadSensor(RtcDrv_t *pRtcDrv);
static void RtcDrv_GenerateEvent(RtcDrv_t *pRtcDrv, RtcDrvTime_t *pTime);
static u8   RtcDrv_BcdToHex(u8 bcd);
static u8   RtcDrv_HexToBcd(u8 hex);
static void RtcDrv_Calibration(void);

static void          RtcDrv_Init(RtcDrv_t *pRtcDrv);
static void          RtcDrv_Process(RtcDrv_t *pRtcDrv);
static RtcDrvTime_t *RtcDrv_Get(RtcDrv_t *pRtcDrv);
static void          RtcDrv_SetAll(RtcDrv_t *pRtcDrv, RtcDrvTime_t *pTime);
static void          RtcDrv_SetTime(RtcDrv_t *pRtcDrv, u8 hour, u8 minute, u8 second);
static void          RtcDrv_SetDate(RtcDrv_t *pRtcDrv, u8 year, u8 month, u8 day, u8 week);
static void          RtcDrv_SetHour(RtcDrv_t *pRtcDrv, u8 hour);
static u8            Rtc_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg);

typedef struct
{
  DProcess_t Process;
  RtcDrv_t  *pTable;
  u8         Sum;
} RtcTask_t;

static RtcTask_t RtcTask;

#define RTC_PROCESS_MSG 0
//-----------------------------------------------------------------------------------------------------------
void Rtc_I2cInit(RtcDrv_t *pTable, u8 sum)
{
  DProcess_InitStructure(&RtcTask.Process, Rtc_Handler);
  DProcess_Start(&RtcTask.Process);

  RtcTask.pTable = pTable;

  RtcTask.Sum = sum;

  for (u8 i = 0; i < RtcTask.Sum; i++)
  {
    RtcDrv_Init(RtcTask.pTable + i);
  }

  DMsg_SendInstant((DProcess_t *)(&RtcTask), RTC_PROCESS_MSG, MSG_NO_ARG);
}
//-----------------------------------------------------------------------------------------------------------
void Rtc_I2cSetAll(u8 id, RtcDrvTime_t *pTime)
{
  if (id < RtcTask.Sum)
  {
    RtcDrv_SetAll(RtcTask.pTable + id, pTime);
  }
}
//-----------------------------------------------------------------------------------------------------------
void Rtc_I2cSetTime(u8 id, u8 hour, u8 minute, u8 second)
{
  if (id < RtcTask.Sum)
  {
    RtcDrv_SetTime(RtcTask.pTable + id, hour, minute, second);
  }
}
//-----------------------------------------------------------------------------------------------------------
void Rtc_I2cSetDate(u8 id, u8 year, u8 month, u8 day, u8 week)
{
  if (id < RtcTask.Sum)
  {
    RtcDrv_SetDate(RtcTask.pTable + id, year, month, day, week);
  }
}
//-----------------------------------------------------------------------------------------------------------
void Rtc_I2cSetHour(u8 id, u8 hour)
{
  if (id < RtcTask.Sum)
  {
    RtcDrv_SetHour(RtcTask.pTable + id, hour);
  }
}
//-----------------------------------------------------------------------------------------------------------
RtcDrvTime_t *Rtc_Get(u8 id)
{
  if (id < RtcTask.Sum)
  {
    return RtcDrv_Get(RtcTask.pTable + id);
  }

  return NULL;
}
//-----------------------------------------------------------------------------------------------------------
static u8 Rtc_Handler(DProcess_t *process, DMsgId_t msgId, DMsgArg_t pArg)
{
  PROCESS_SCHEDULE_BEGIN()

  while (1)
  {
    PROCESS_WAIT_FOR_MSG(msgId == RTC_PROCESS_MSG);

    for (u8 i = 0; i < RtcTask.Sum; i++)
    {
      RtcDrv_Process(RtcTask.pTable + i);
    }

    DMsg_SendLater((DProcess_t *)(&RtcTask), RTC_PROCESS_MSG, MSG_NO_ARG, RTC_DRV_TIMEBASE);
  }

  PROCESS_SCHEDULE_END()
}
//-----------------------------------------------------------------------------------------------------------
static void RtcDrv_Init(RtcDrv_t *pRtcDrv)
{
  RtcDrv_Calibration();
  RtcDrv_ReadSensor(pRtcDrv);
}
//-----------------------------------------------------------------------------------------------------------
static void RtcDrv_Process(RtcDrv_t *pRtcDrv)
{
  if (pRtcDrv->pRam->ReadTime.Second != pRtcDrv->pRam->LastTime.Second    // Second
      || pRtcDrv->pRam->ReadTime.Minute != pRtcDrv->pRam->LastTime.Minute // Minute
      || pRtcDrv->pRam->ReadTime.Hour != pRtcDrv->pRam->LastTime.Hour     // Hour
      || pRtcDrv->pRam->ReadTime.Week != pRtcDrv->pRam->LastTime.Week     // Week
      || pRtcDrv->pRam->ReadTime.Day != pRtcDrv->pRam->LastTime.Day       // Day
      || pRtcDrv->pRam->ReadTime.Month != pRtcDrv->pRam->LastTime.Month   // Month
      || pRtcDrv->pRam->ReadTime.Year != pRtcDrv->pRam->LastTime.Year)    // Year)
  {
    memcpy(&pRtcDrv->pRam->LastTime, &pRtcDrv->pRam->ReadTime, RTC_DRV_SUM);

    pRtcDrv->pRam->Time.Second = RtcDrv_BcdToHex(pRtcDrv->pRam->ReadTime.Second);
    pRtcDrv->pRam->Time.Minute = RtcDrv_BcdToHex(pRtcDrv->pRam->ReadTime.Minute);
    pRtcDrv->pRam->Time.Hour = RtcDrv_BcdToHex(pRtcDrv->pRam->ReadTime.Hour);
    pRtcDrv->pRam->Time.Week = RtcDrv_BcdToHex(pRtcDrv->pRam->ReadTime.Week);
    pRtcDrv->pRam->Time.Day = RtcDrv_BcdToHex(pRtcDrv->pRam->ReadTime.Day);
    pRtcDrv->pRam->Time.Month = RtcDrv_BcdToHex(pRtcDrv->pRam->ReadTime.Month);
    pRtcDrv->pRam->Time.Year = RtcDrv_BcdToHex(pRtcDrv->pRam->ReadTime.Year);

    RtcDrv_GenerateEvent(pRtcDrv, &pRtcDrv->pRam->Time);
  }

  RtcDrv_ReadSensor(pRtcDrv);
}
//-----------------------------------------------------------------------------------------------------------
static RtcDrvTime_t *RtcDrv_Get(RtcDrv_t *pRtcDrv)
{
  return &pRtcDrv->pRam->Time;
}
//-----------------------------------------------------------------------------------------------------------
static void RtcDrv_SetAll(RtcDrv_t *pRtcDrv, RtcDrvTime_t *pTime)
{
  pRtcDrv->pRam->WriteTime.Second = RtcDrv_HexToBcd(pTime->Second);
  pRtcDrv->pRam->WriteTime.Minute = RtcDrv_HexToBcd(pTime->Minute);
  pRtcDrv->pRam->WriteTime.Hour = RtcDrv_HexToBcd(pTime->Hour);
  pRtcDrv->pRam->WriteTime.Week = RtcDrv_HexToBcd(pTime->Week);
  pRtcDrv->pRam->WriteTime.Day = RtcDrv_HexToBcd(pTime->Day);
  pRtcDrv->pRam->WriteTime.Month = RtcDrv_HexToBcd(pTime->Month);
  pRtcDrv->pRam->WriteTime.Year = RtcDrv_HexToBcd(pTime->Year);

  I2c_StartMemoryTx(RTC_ADD, SECOND_INDEX, I2C_MEMORY_8BIT, (u8 *)&pRtcDrv->pRam->WriteTime,
                    RTC_DRV_SUM);
}
//-----------------------------------------------------------------------------------------------------------
static void RtcDrv_SetTime(RtcDrv_t *pRtcDrv, u8 hour, u8 minute, u8 second)
{
  pRtcDrv->pRam->WriteTime.Second = RtcDrv_HexToBcd(second);
  pRtcDrv->pRam->WriteTime.Minute = RtcDrv_HexToBcd(minute);
  pRtcDrv->pRam->WriteTime.Hour = RtcDrv_HexToBcd(hour);

  I2c_StartMemoryTx(RTC_ADD, SECOND_INDEX, I2C_MEMORY_8BIT, (u8 *)&pRtcDrv->pRam->WriteTime.Second,
                    3);
}
//-----------------------------------------------------------------------------------------------------------
static void RtcDrv_SetDate(RtcDrv_t *pRtcDrv, u8 year, u8 month, u8 day, u8 week)
{
  pRtcDrv->pRam->WriteTime.Week = RtcDrv_HexToBcd(week);
  pRtcDrv->pRam->WriteTime.Day = RtcDrv_HexToBcd(day);
  pRtcDrv->pRam->WriteTime.Month = RtcDrv_HexToBcd(month);
  pRtcDrv->pRam->WriteTime.Year = RtcDrv_HexToBcd(year);

  I2c_StartMemoryTx(RTC_ADD, DAY_OF_WEEK_INDEX, I2C_MEMORY_8BIT,
                    (u8 *)&pRtcDrv->pRam->WriteTime.Week, 4);
}
//-----------------------------------------------------------------------------------------------------------
static void RtcDrv_SetHour(RtcDrv_t *pRtcDrv, u8 hour)
{
  pRtcDrv->pRam->WriteTime.Hour = RtcDrv_HexToBcd(hour);

  I2c_StartMemoryTx(RTC_ADD, HOUR_INDEX, I2C_MEMORY_8BIT, (u8 *)&pRtcDrv->pRam->WriteTime.Hour, 1);
}
//-----------------------------------------------------------------------------------------------------------
static void RtcDrv_ReadSensor(RtcDrv_t *pRtcDrv)
{
  I2c_StartMemoryRx(RTC_ADD, SECOND_INDEX, I2C_MEMORY_8BIT, (u8 *)&pRtcDrv->pRam->ReadTime,
                    RTC_DRV_SUM);
}
//-----------------------------------------------------------------------------------------------------------
static void RtcDrv_GenerateEvent(RtcDrv_t *pRtcDrv, RtcDrvTime_t *pTime)
{
  if (pRtcDrv->CallBackFunc != NULL)
  {
    pRtcDrv->CallBackFunc(pRtcDrv->Id, pTime);
  }
}
//-----------------------------------------------------------------------------------------------------------
static u8 RtcDrv_BcdToHex(u8 bcd)
{
  return HI_U8(bcd) * 10 + LO_U8(bcd);
}
//-----------------------------------------------------------------------------------------------------------
static u8 RtcDrv_HexToBcd(u8 hex)
{
  return BUILD_U8((hex / 10) & 0xF, (hex % 10) & 0xF);
}
//-----------------------------------------------------------------------------------------------------------
// Measured frequency is 512.0107Hz
// (512.0107 - 512 ) / 512 = +20.89 ppm
// One bit +4.068 or -2.034 ppm
// So calibration value = 20.89 / 2.034 = 10
static void RtcDrv_Calibration(void)
{
  const static u8 calibrationValue = 10;

  I2c_StartMemoryTx(RTC_ADD, CALIBRATION_INDEX, I2C_MEMORY_8BIT, (u8 *)&calibrationValue, 1);
}