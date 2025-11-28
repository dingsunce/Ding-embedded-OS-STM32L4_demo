#include "i2c_func.h"

void I2c_Init(void)
{
}

void I2c_Process(void)
{
}

OsErr_t I2c_StartTx(u8 add, u8 *pData, u8 length)
{
  return OS_ERR_OK;
}
OsErr_t I2c_StartRx(u8 add, u8 *pData, u8 length)
{
  return OS_ERR_OK;
}

OsErr_t I2c_Tx(u8 add, u8 *pData, u8 length)
{
  return OS_ERR_OK;
}
OsErr_t I2c_Rx(u8 add, u8 *pData, u8 length)
{
  return OS_ERR_OK;
}

OsErr_t I2c_StartMemoryTx(u8 add, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u8 length)
{
  return OS_ERR_OK;
}
OsErr_t I2c_StartMemoryRx(u8 add, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u8 length)
{
  return OS_ERR_OK;
}

OsErr_t I2c_MemoryTx(u8 add, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u8 length)
{
  return OS_ERR_OK;
}
OsErr_t I2c_MemoryRx(u8 add, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u8 length)
{
  return OS_ERR_OK;
}
