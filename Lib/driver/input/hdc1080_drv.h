#ifndef HDC_1080_DRV_H
#define HDC_1080_DRV_H

#include "d_define.h"
#include "func.h"

#define HDC1080_WAIT_FOR_MEASUREMENT_TIME 100 // 100ms

typedef struct
{
  u8 I2cReadBuffer[4];
} Hdc1080DrvRam_t;

typedef struct
{
  u8  Id;
  u8  Resolution;
  u16 Temp;
  u16 Humidity;
  u8  I2cReadBuffer[4];
} Hdc1080Drv_t;

void    Hdc1080Drv_Init(Hdc1080Drv_t *pHdc1080Drv);
void    Hdc1080Drv_Config(Hdc1080Drv_t *pHdc1080Drv);
void    Hdc1080Drv_TriggerMeasurement(Hdc1080Drv_t *pHdc1080Drv);
OsErr_t Hdc1080Drv_GetMeasurementResult(Hdc1080Drv_t *pHdc1080Drv);
double  Hdc1080Drv_GetTemp(u16 value);
double  Hdc1080Drv_GetHumidity(u16 value);

#endif