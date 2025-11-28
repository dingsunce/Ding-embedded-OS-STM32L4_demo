#ifndef _I2C_FUNC_H
#define _I2C_FUNC_H

#include "define.h"
#include "error.h"

#ifndef I2C_MEMORY_BITS
#define I2C_MEMORY_BITS 8
#endif

typedef enum
{
  I2C_MEMORY_8BIT,
  I2C_MEMORY_16BIT,
} I2cMemorySize_t;

extern void I2c_Init(void);
extern void I2c_Process(void);

extern OsErr_t I2c_StartTx(u8 addr, u8 *pData, u8 length);
extern OsErr_t I2c_StartRx(u8 addr, u8 *pData, u8 length);

extern OsErr_t I2c_Tx(u8 addr, u8 *pData, u8 length);
extern OsErr_t I2c_Rx(u8 addr, u8 *pData, u8 length);

extern OsErr_t I2c_StartMemoryTx(u8 addr, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u8 length);
extern OsErr_t I2c_StartMemoryRx(u8 addr, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u8 length);

extern OsErr_t I2c_MemoryTx(u8 addr, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u8 length);
extern OsErr_t I2c_MemoryRx(u8 addr, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u8 length);

#endif
