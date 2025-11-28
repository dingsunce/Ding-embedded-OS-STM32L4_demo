#include "stm32ll_i2c_impl.h"

#include "Io_func.h"
#include "error.h"
#include "i2c.h"
#include "i2c_func.h"
#include "stm32ll.h"

// #define I2C_CONFIG

#ifndef I2C_CONFIG

#if defined STM32_SERIAL_G0

#define I2Cx_INSTANCE I2C1
#define I2Cx_Init     MX_I2C1_Init
#define I2Cx_IRQn     I2C1_IRQn

#define I2Cx_TX_DMA_INSTANCE DMA1
#define I2Cx_TX_DMA_CHANNEL  LL_DMA_CHANNEL_1

#define I2Cx_RX_DMA_INSTANCE DMA1
#define I2Cx_RX_DMA_CHANNEL  LL_DMA_CHANNEL_2

#define I2Cx_PORT_SDA IO_PORT_B
#define I2Cx_PIN_SDA  IO_PIN7

#define I2Cx_PORT_SCL IO_PORT_B
#define I2Cx_PIN_SCL  IO_PIN6

#elif defined STM32_SERIAL_L4

#define I2Cx_INSTANCE I2C1
#define I2Cx_Init     MX_I2C1_Init
#define I2Cx_IRQn     I2C1_EV_IRQn

#define I2Cx_TX_DMA_INSTANCE DMA1
#define I2Cx_TX_DMA_CHANNEL  LL_DMA_CHANNEL_6

#define I2Cx_RX_DMA_INSTANCE DMA1
#define I2Cx_RX_DMA_CHANNEL  LL_DMA_CHANNEL_7

#define I2Cx_PORT_SDA IO_PORT_B
#define I2Cx_PIN_SDA  IO_PIN7

#define I2Cx_PORT_SCL IO_PORT_B
#define I2Cx_PIN_SCL  IO_PIN6

#endif

#endif

#ifndef TX_BUFFER_SIZE
#define TX_BUFFER_SIZE 10
#endif

#ifndef RX_BUFFER_SIZE
#define RX_BUFFER_SIZE 10
#endif

static volatile u8  TxBuffer[TX_BUFFER_SIZE];
static volatile u8 *pRxData;

static u8 TxLength;
static u8 RxLength;

static volatile I2cType_t I2cType;

static volatile u8 SlaveAddr;

static volatile bool IsTxRxComplete = true;

void I2cImpl_Init(void)
{
  LL_I2C_EnableIT_TC(I2Cx_INSTANCE);
  LL_I2C_EnableIT_STOP(I2Cx_INSTANCE);

  LL_I2C_EnableIT_ERR(I2Cx_INSTANCE);

  /* Enable DMA TX Requests */
  LL_I2C_EnableDMAReq_TX(I2Cx_INSTANCE);

  /* Enable DMA RX Requests */
  LL_I2C_EnableDMAReq_RX(I2Cx_INSTANCE);

  LL_DMA_ConfigAddresses(I2Cx_TX_DMA_INSTANCE, I2Cx_TX_DMA_CHANNEL, (u32)TxBuffer,
                         LL_I2C_DMA_GetRegAddr(I2Cx_INSTANCE, LL_I2C_DMA_REG_DATA_TRANSMIT),
                         LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetPeriphAddress(I2Cx_RX_DMA_INSTANCE, I2Cx_RX_DMA_CHANNEL,
                          LL_I2C_DMA_GetRegAddr(I2Cx_INSTANCE, LL_I2C_DMA_REG_DATA_RECEIVE));
}

void I2cImpl_Reset(void)
{
  LL_I2C_DeInit(I2Cx_INSTANCE);

  // In some case, I2C SDA line will be pull down by the I2C slave IC
  // Simulate I2C SCL 9 cycles + 1 Stop bit can release it.
  Io_Init(I2Cx_PORT_SDA, I2Cx_PIN_SDA, IO_INPUT_PULL_UP);
  if (Io_GetBits(I2Cx_PORT_SDA, I2Cx_PIN_SDA) == 0)
  {
    // SCL 9 cycles
    Io_Init(I2Cx_PORT_SCL, I2Cx_PIN_SCL, IO_OUTPUT_PUSH_PULL_HIGH);
    for (u8 i = 0; i < 9; i++)
    {
      Io_ResetBits(I2Cx_PORT_SCL, I2Cx_PIN_SCL);
      Io_SetBits(I2Cx_PORT_SCL, I2Cx_PIN_SCL);
    }
    // Stop bit
    Io_Init(I2Cx_PORT_SDA, I2Cx_PIN_SDA, IO_OUTPUT_PUSH_PULL_HIGH);
    Io_ResetBits(I2Cx_PORT_SDA, I2Cx_PIN_SDA);
    Io_SetBits(I2Cx_PORT_SDA, I2Cx_PIN_SDA);
  }

  I2Cx_Init();
  I2cImpl_Init();
}

OsErr_t I2cImpl_StartTx(u8 addr, u8 *pData, u8 length)
{
  if (LL_I2C_IsActiveFlag_BUSY(I2Cx_INSTANCE))
    return OS_ERR_BUSY;

  SlaveAddr = addr;
  memcpy((void *)TxBuffer, pData, length);
  TxLength = length;
  IsTxRxComplete = false;

  I2cType = I2C_TX;

  /* Disable DMA Channel Tx */
  LL_DMA_DisableChannel(I2Cx_TX_DMA_INSTANCE, I2Cx_TX_DMA_CHANNEL);

  /* Master Generate Start condition for a write request :
   *    - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS
   *    - with an auto stop condition generation when transmit all bytes
   */
  LL_I2C_HandleTransfer(I2Cx_INSTANCE, SlaveAddr, LL_I2C_ADDRSLAVE_7BIT, TxLength, LL_I2C_MODE_AUTOEND,
                        LL_I2C_GENERATE_START_WRITE);

  LL_DMA_SetDataLength(I2Cx_TX_DMA_INSTANCE, I2Cx_TX_DMA_CHANNEL, TxLength);

  /* Enable DMA Channel Tx */
  LL_DMA_EnableChannel(I2Cx_TX_DMA_INSTANCE, I2Cx_TX_DMA_CHANNEL);

  return OS_ERR_OK;
}

static OsErr_t WaiteForBusyOk()
{
  u16 count = 0;
  while (LL_I2C_IsActiveFlag_BUSY(I2Cx_INSTANCE))
  {
    count++;
    if (count > 10000)
    {
      I2cImpl_Reset();
      return OS_ERR_BUSY;
    }
  }

  return OS_ERR_OK;
}

OsErr_t I2cImpl_Tx(u8 addr, u8 *pData, u8 length)
{
  if (WaiteForBusyOk() != OS_ERR_OK)
    return OS_ERR_BUSY;

  I2cImpl_StartTx(addr, pData, length);

  while (LL_I2C_IsActiveFlag_BUSY(I2Cx_INSTANCE))
  {
  }

  if (WaiteForBusyOk() != OS_ERR_OK)
    return OS_ERR_BUSY;

  return OS_ERR_OK;
}

OsErr_t I2cImpl_StartRx(u8 addr, u8 *pData, u8 length)
{
  if (LL_I2C_IsActiveFlag_BUSY(I2Cx_INSTANCE))
    return OS_ERR_BUSY;

  SlaveAddr = addr;
  pRxData = pData;
  RxLength = length;
  IsTxRxComplete = false;

  I2cType = I2C_RX;

  /* Disable DMA Channel Rx */
  LL_DMA_DisableChannel(I2Cx_RX_DMA_INSTANCE, I2Cx_RX_DMA_CHANNEL);

  /* Master Generate Start condition for a read request:
   *  - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS
   *  - with an auto stop condition generation when receive all bytes
   */
  LL_I2C_HandleTransfer(I2Cx_INSTANCE, SlaveAddr, LL_I2C_ADDRSLAVE_7BIT, RxLength, LL_I2C_MODE_AUTOEND,
                        LL_I2C_GENERATE_START_READ);

  LL_DMA_SetMemoryAddress(I2Cx_RX_DMA_INSTANCE, I2Cx_RX_DMA_CHANNEL, (u32)pRxData);

  LL_DMA_SetDataLength(I2Cx_RX_DMA_INSTANCE, I2Cx_RX_DMA_CHANNEL, RxLength);

  /* Enable DMA Channel Rx */
  LL_DMA_EnableChannel(I2Cx_RX_DMA_INSTANCE, I2Cx_RX_DMA_CHANNEL);

  return OS_ERR_OK;
}

OsErr_t I2cImpl_Rx(u8 addr, u8 *pData, u8 length)
{
  if (WaiteForBusyOk() != OS_ERR_OK)
    return OS_ERR_BUSY;

  I2cImpl_StartRx(addr, pData, length);

  if (WaiteForBusyOk() != OS_ERR_OK)
    return OS_ERR_BUSY;

  return OS_ERR_OK;
}

OsErr_t I2cImpl_StartMemoryTx(u8 addr, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u16 length)
{
  if (LL_I2C_IsActiveFlag_BUSY(I2Cx_INSTANCE))
    return OS_ERR_BUSY;

  SlaveAddr = addr;
  if (memorySize == I2C_MEMORY_8BIT)
  {
    TxBuffer[0] = memory;
    memcpy((void *)(TxBuffer + 1), pData, length);
    TxLength = length + 1;
  }
  else
  {
    TxBuffer[0] = HIGH_U16(memory);
    TxBuffer[1] = LOW_U16(memory);
    memcpy((void *)(TxBuffer + 2), pData, length);
    TxLength = length + 2;
  }

  IsTxRxComplete = false;

  I2cType = I2C_TX;

  /* Disable DMA Channel Tx */
  LL_DMA_DisableChannel(I2Cx_TX_DMA_INSTANCE, I2Cx_TX_DMA_CHANNEL);

  /* Master Generate Start condition for a write request :
   *    - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS
   *    - with an auto stop condition generation when transmit all bytes
   */
  LL_I2C_HandleTransfer(I2Cx_INSTANCE, SlaveAddr, LL_I2C_ADDRSLAVE_7BIT, TxLength, LL_I2C_MODE_AUTOEND,
                        LL_I2C_GENERATE_START_WRITE);

  LL_DMA_SetDataLength(I2Cx_TX_DMA_INSTANCE, I2Cx_TX_DMA_CHANNEL, TxLength);

  /* Enable DMA Channel Tx */
  LL_DMA_EnableChannel(I2Cx_TX_DMA_INSTANCE, I2Cx_TX_DMA_CHANNEL);

  return OS_ERR_OK;
}

OsErr_t I2cImpl_MemoryTx(u8 addr, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u16 length)
{
  if (WaiteForBusyOk() != OS_ERR_OK)
    return OS_ERR_BUSY;

  I2cImpl_StartMemoryTx(addr, memory, memorySize, pData, length);

  if (WaiteForBusyOk() != OS_ERR_OK)
    return OS_ERR_BUSY;

  return OS_ERR_OK;
}

OsErr_t I2cImpl_StartMemoryRx(u8 addr, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u16 length)
{
  if (LL_I2C_IsActiveFlag_BUSY(I2Cx_INSTANCE))
    return OS_ERR_BUSY;

  SlaveAddr = addr;
  if (memorySize == I2C_MEMORY_8BIT)
  {
    TxBuffer[0] = memory;
    TxLength = 1;
  }
  else
  {
    TxBuffer[0] = HIGH_U16(memory);
    TxBuffer[1] = LOW_U16(memory);
    TxLength = 2;
  }

  pRxData = pData;
  RxLength = length;
  IsTxRxComplete = false;

  I2cType = I2C_MEMORY_RX;

  /* Disable DMA Channel Tx */
  LL_DMA_DisableChannel(I2Cx_TX_DMA_INSTANCE, I2Cx_TX_DMA_CHANNEL);

  /* Disable DMA Channel Rx */
  LL_DMA_DisableChannel(I2Cx_RX_DMA_INSTANCE, I2Cx_RX_DMA_CHANNEL);

  /* Master Generate Start condition for a write request :
   *    - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS
   *    - software end mode: TC flag is set when all data are transferred
   *
   */
  LL_I2C_HandleTransfer(I2Cx_INSTANCE, SlaveAddr, LL_I2C_ADDRSLAVE_7BIT, TxLength, LL_I2C_MODE_SOFTEND,
                        LL_I2C_GENERATE_START_WRITE);

  LL_DMA_SetDataLength(I2Cx_TX_DMA_INSTANCE, I2Cx_TX_DMA_CHANNEL, TxLength);

  /* Enable DMA Channel Tx */
  LL_DMA_EnableChannel(I2Cx_TX_DMA_INSTANCE, I2Cx_TX_DMA_CHANNEL);

  return OS_ERR_OK;
}

OsErr_t I2cImpl_MemoryRx(u8 addr, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u16 length)
{
  if (WaiteForBusyOk() != OS_ERR_OK)
    return OS_ERR_BUSY;

  I2cImpl_StartMemoryRx(addr, memory, memorySize, pData, length);

  if (WaiteForBusyOk() != OS_ERR_OK)
    return OS_ERR_BUSY;

  return OS_ERR_OK;
}

void I2cImpl_OnError(void)
{
  NVIC_DisableIRQ(I2Cx_IRQn);
  LL_I2C_GenerateStopCondition(I2Cx_INSTANCE);
}

#if defined STM32_SERIAL_G0

void I2C1_IRQHandler(void)

#elif defined STM32_SERIAL_L4

void I2C1_EV_IRQHandler(void)
#endif
{
  /* Check TC flag value in ISR register */
  if (LL_I2C_IsActiveFlag_TC(I2Cx_INSTANCE))
  { // only "software end mode" will set TC flag
    /* TC flag is cleared setting START bit or STOP bit. */

    /* Master Generate Start condition for a read request:
     *  - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS
     *  - with an auto stop condition generation when receive all bytes
     */
    LL_I2C_HandleTransfer(I2Cx_INSTANCE, SlaveAddr, LL_I2C_ADDRSLAVE_7BIT, RxLength, LL_I2C_MODE_AUTOEND,
                          LL_I2C_GENERATE_START_READ);

    LL_DMA_SetMemoryAddress(I2Cx_RX_DMA_INSTANCE, I2Cx_RX_DMA_CHANNEL, (u32)pRxData);

    LL_DMA_SetDataLength(I2Cx_RX_DMA_INSTANCE, I2Cx_RX_DMA_CHANNEL, RxLength);

    /* Enable DMA Channel Rx */
    LL_DMA_EnableChannel(I2Cx_RX_DMA_INSTANCE, I2Cx_RX_DMA_CHANNEL);
  }
  /* Check STOP flag value in ISR register */
  else if (LL_I2C_IsActiveFlag_STOP(I2Cx_INSTANCE))
  {
    /* (3) Clear pending flags, Data consistency are checking into Slave process */

    /* Clear STOP flag value in ISR register */
    LL_I2C_ClearFlag_STOP(I2Cx_INSTANCE);

    IsTxRxComplete = true;
  }
  else
  {
    I2cImpl_OnError();
  }
}

bool I2cImpl_IsBusy(void)
{
  return LL_I2C_IsActiveFlag_BUSY(I2Cx_INSTANCE);
}

bool I2cImpl_IsTxRxComplete(void)
{
  return IsTxRxComplete;
}