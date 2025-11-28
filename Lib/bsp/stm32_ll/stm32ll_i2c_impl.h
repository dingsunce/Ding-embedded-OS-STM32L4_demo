#ifndef _STM32LL_I2C_IMPL_H
#define _STM32LL_I2C_IMPL_H

#include "i2c_func.h"

typedef enum I2cType
{
  I2C_TX,
  I2C_RX,
  I2C_MEMORY_TX,
  I2C_MEMORY_RX,
} I2cType_t;

extern void I2cImpl_Init(void);
extern void I2cImpl_Reset(void);

extern OsErr_t I2cImpl_StartTx(u8 addr, u8 *pData, u8 length);
extern OsErr_t I2cImpl_StartRx(u8 addr, u8 *pData, u8 length);

extern OsErr_t I2cImpl_Tx(u8 addr, u8 *pData, u8 length);
extern OsErr_t I2cImpl_Rx(u8 addr, u8 *pData, u8 length);

extern OsErr_t I2cImpl_StartMemoryTx(u8 addr, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u16 length);
extern OsErr_t I2cImpl_StartMemoryRx(u8 addr, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u16 length);

extern OsErr_t I2cImpl_MemoryTx(u8 addr, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u16 length);
extern OsErr_t I2cImpl_MemoryRx(u8 addr, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u16 length);

extern u8  *I2cImpl_GetRxBuffer(void);
extern bool I2cImpl_IsBusy(void);
extern bool I2cImpl_IsTxRxComplete(void);

#endif