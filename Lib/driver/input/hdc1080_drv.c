#include "hdc1080_drv.h"

#define HDC1080_DRV_ADD (0x40 << 1)

#define HDC1080_DRV_TEMP_REGISTER     0x00
#define HDC1080_DRV_HUMIDITY_REGISTER 0x01
#define HDC1080_DRV_CONFIG_REGISTER   0x02

#define HDC1080_DRV_CONFIG_TEMP_BIT     10
#define HDC1080_DRV_CONFIG_HUMIDITY_BIT 8
// Temperature or Humidity is acquired in one read, 14bits resolution, Heater Disabled
#define HDC1080_DRV_CONFIG_DEFAULT_VALUE 0x1000

#define HDC1080_DRV_INVALID_VALUE 0xFF
//-----------------------------------------------------------------------------------------------------------
void Hdc1080Drv_Init(Hdc1080Drv_t *pHdc1080Drv)
{
  pHdc1080Drv->Temp = 0xffff;
  pHdc1080Drv->Humidity = 0xffff;
  pHdc1080Drv->I2cReadBuffer[0] = HDC1080_DRV_INVALID_VALUE;
  pHdc1080Drv->I2cReadBuffer[1] = HDC1080_DRV_INVALID_VALUE;
  pHdc1080Drv->I2cReadBuffer[2] = HDC1080_DRV_INVALID_VALUE;
  pHdc1080Drv->I2cReadBuffer[3] = HDC1080_DRV_INVALID_VALUE;
  // wait 100ms for start up
}
//-----------------------------------------------------------------------------------------------------------
void Hdc1080Drv_Config(Hdc1080Drv_t *pHdc1080Drv)
{
  u16 data = HDC1080_DRV_CONFIG_DEFAULT_VALUE;
  u8  buffer[2];
  if (pHdc1080Drv->Resolution == 11)
  {
    data |= (1 << HDC1080_DRV_CONFIG_TEMP_BIT);
    data |= (1 << HDC1080_DRV_CONFIG_HUMIDITY_BIT);
  }

  buffer[0] = HIBYTE(data);
  buffer[1] = LOBYTE(data);
  I2c_MemoryTx(HDC1080_DRV_ADD, HDC1080_DRV_CONFIG_REGISTER, I2C_MEMORY_8BIT, buffer, 2);
}
//-----------------------------------------------------------------------------------------------------------
void Hdc1080Drv_TriggerMeasurement(Hdc1080Drv_t *pHdc1080Drv)
{
  u8 data = 0;
  I2c_Tx(HDC1080_DRV_ADD, &data, 1);
  // wait 100ms for result
}
//-----------------------------------------------------------------------------------------------------------
OsErr_t Hdc1080Drv_GetMeasurementResult(Hdc1080Drv_t *pHdc1080Drv)
{
  OsErr_t result = I2c_Rx(HDC1080_DRV_ADD, pHdc1080Drv->I2cReadBuffer, 4);
  if (result == OS_ERR_OK)
  {
    if (pHdc1080Drv->I2cReadBuffer[0] != HDC1080_DRV_INVALID_VALUE &&
        pHdc1080Drv->I2cReadBuffer[1] != HDC1080_DRV_INVALID_VALUE &&
        pHdc1080Drv->I2cReadBuffer[2] != HDC1080_DRV_INVALID_VALUE &&
        pHdc1080Drv->I2cReadBuffer[3] != HDC1080_DRV_INVALID_VALUE)
    {
      pHdc1080Drv->Temp = BUILD_U16(pHdc1080Drv->I2cReadBuffer[0], pHdc1080Drv->I2cReadBuffer[1]);
      pHdc1080Drv->Humidity =
          BUILD_U16(pHdc1080Drv->I2cReadBuffer[2], pHdc1080Drv->I2cReadBuffer[3]);
    }
    else
    {
      result = OS_ERR_NULL;
    }
  }

  return result;
}
//-----------------------------------------------------------------------------------------------------------
double Hdc1080Drv_GetTemp(u16 value)
{
  return (double)(((double)(value) / (double)(0x10000)) * (double)165) - (double)40;
}
//-----------------------------------------------------------------------------------------------------------
double Hdc1080Drv_GetHumidity(u16 value)
{
  return (double)(((double)(value) / (double)(0x10000)) * (double)100);
}