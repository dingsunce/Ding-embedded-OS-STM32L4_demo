#include "stm8_i2c_impl.h"

#include "Io_func.h"
#include "error.h"
#include "i2c_func.h"
#include "stm8l15x_conf.h"

#define I2C_PORT_SDA IO_PORT_C
#define I2C_PIN_SDA  IO_PIN0

#define I2C_PORT_SCL IO_PORT_C
#define I2C_PIN_SCL  IO_PIN1

#define TX_BUFFER_SIZE 10
#define RX_BUFFER_SIZE 10

static volatile u8  TxBuffer[TX_BUFFER_SIZE];
static volatile u8 *pRxData;

static u8 TxLength;
static u8 RxLength;

static volatile u8 TxIndex;
static volatile u8 RxIndex;

static volatile I2cType_t I2cType;

static volatile u8 SlaveAddr;

static volatile bool IsTxRxComplete = true;

void I2cImpl_Init(void)
{
  /* I2C  clock Enable*/
  CLK_PeripheralClockConfig(CLK_Peripheral_I2C1, ENABLE);

  /* Initialize I2C peripheral */
  I2C_Init(I2C1, 100000, 0, I2C_Mode_I2C, I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);

  disableInterrupts();
  ITC_SetSoftwarePriority(I2C1_SPI2_IRQn, ITC_PriorityLevel_1);
  enableInterrupts();
}

void I2cImpl_Reset(void)
{
  I2C_SoftwareResetCmd(I2C1, ENABLE);
  I2C_DeInit(I2C1);

  // In some case, I2C SDA line will be pull down by the I2C slave IC
  // Simulate I2C SCL 9 cycles + 1 Stop bit can release it.
  Io_Init(I2C_PORT_SDA, I2C_PIN_SDA, IO_INPUT_PULL_UP);
  if (Io_GetBits(I2C_PORT_SDA, I2C_PIN_SDA) == 0)
  {
    // SCL 9 cycles
    Io_Init(I2C_PORT_SCL, I2C_PIN_SCL, IO_OUTPUT_PUSH_PULL_HIGH);
    for (u8 i = 0; i < 9; i++)
    {
      Io_ResetBits(I2C_PORT_SCL, I2C_PIN_SCL);
      Io_SetBits(I2C_PORT_SCL, I2C_PIN_SCL);
    }
    // Stop bit
    Io_Init(I2C_PORT_SDA, I2C_PIN_SDA, IO_OUTPUT_PUSH_PULL_HIGH);
    Io_ResetBits(I2C_PORT_SDA, I2C_PIN_SDA);
    Io_SetBits(I2C_PORT_SDA, I2C_PIN_SDA);
  }

  I2cImpl_Init();
}

static void I2cStart(void)
{
  /* Enable Acknowledgement */
  I2C_AcknowledgeConfig(I2C1, ENABLE);

  /* Enable Buffer and Event Interrupt*/
  I2C_ITConfig(I2C1, (I2C_IT_TypeDef)(I2C_IT_EVT | I2C_IT_BUF), ENABLE);

  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);

  IsTxRxComplete = false;
}

OsErr_t I2cImpl_StartTx(u8 add, u8 *pData, u8 length)
{
  if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
    return OS_ERR_BUSY;
  }

  SlaveAddr = add;
  memcpy((void *)TxBuffer, pData, length);
  TxLength = length;
  TxIndex = 0;

  I2cType = I2C_TX;

  I2cStart();

  return OS_ERR_OK;
}

OsErr_t I2cImpl_StartRx(u8 add, u8 *pData, u8 length)
{
  if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
    return OS_ERR_BUSY;
  }

  SlaveAddr = add;
  pRxData = pData;
  RxLength = length;
  RxIndex = 0;

  I2cType = I2C_RX;

  I2cStart();

  return OS_ERR_OK;
}

OsErr_t I2cImpl_StartMemoryTx(u8 add, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u8 length)
{
  if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
    return OS_ERR_BUSY;
  }

  SlaveAddr = add;
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
  TxIndex = 0;

  I2cType = I2C_TX;

  I2cStart();

  return OS_ERR_OK;
}

OsErr_t I2cImpl_StartMemoryRx(u8 add, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u8 length)
{
  if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
    return OS_ERR_BUSY;
  }
  SlaveAddr = add;
  pRxData = pData;
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
  TxIndex = 0;

  RxLength = length;
  RxIndex = 0;

  I2cType = I2C_MEMORY_RX;

  I2cStart();

  return OS_ERR_OK;
}

void OnError(void)
{
  I2C1->SR2 = 0; // Clear errors
  I2C_ITConfig(I2C1, (I2C_IT_TypeDef)(I2C_IT_EVT | I2C_IT_BUF), DISABLE);
  I2C_GenerateSTOP(I2C1, ENABLE);
}

static void TxInterrupt(void)
{
  switch (I2C_GetLastEvent(I2C1))
  {
    /* EV5 */
  case I2C_EVENT_MASTER_MODE_SELECT:
    /* Send slave Address for write */
    I2C_Send7bitAddress(I2C1, SlaveAddr, I2C_Direction_Transmitter);
    break;

    /* EV6 */
  case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
    /* EV8 */
  case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
    if (TxIndex < TxLength)
    {
      /* Transmit Data */
      I2C_SendData(I2C1, TxBuffer[TxIndex++]);
    }
    else
    {
      I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
    }
    break;

    /* EV8_2 */
  case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
    /* Send STOP condition */
    I2C_GenerateSTOP(I2C1, ENABLE);

    I2C_ITConfig(I2C1, I2C_IT_EVT, DISABLE);

    IsTxRxComplete = true;
    break;

  default:
    OnError();
    break;
  }
}

static void RxInterrupt(void)
{
  u16 Event = I2C_GetLastEvent(I2C1);
  switch (Event)
  {
    /* EV5 */
  case I2C_EVENT_MASTER_MODE_SELECT:
    /* Send slave Address for Read */
    I2C_Send7bitAddress(I2C1, SlaveAddr, I2C_Direction_Receiver);
    break;

    /* EV6 */
  case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
    if (RxLength - RxIndex == 1)
    {
      I2C_AcknowledgeConfig(I2C1, DISABLE);
      I2C_GenerateSTOP(I2C1, ENABLE);
    }
    break;

  default:
    // EV8 I2C_EVENT_MASTER_BYTE_RECEIVED: BUSY, MSL and RXNE flags
    // In some case, only RXNE is set, so just check RXNE flag.
    if ((Event & I2C_SR1_RXNE) != 0)
    {
      // Read data byte
      pRxData[RxIndex++] = I2C_ReceiveData(I2C1);

      if (RxLength - RxIndex == 1)
      {
        I2C_AcknowledgeConfig(I2C1, DISABLE);
        I2C_GenerateSTOP(I2C1, ENABLE);
      }
      else if (RxLength == RxIndex)
      {
        I2C_ITConfig(I2C1, (I2C_IT_TypeDef)(I2C_IT_EVT | I2C_IT_BUF), DISABLE);

        IsTxRxComplete = true;
      }
    }
    else
    {
      OnError();
    }
    break;
  }
}

static void MemoryRxInterrupt(void)
{
  u16 Event = I2C_GetLastEvent(I2C1);
  switch (Event)
  {
    /* EV5 */
  case I2C_EVENT_MASTER_MODE_SELECT:
    if (TxIndex == 0)
    {
      I2C_Send7bitAddress(I2C1, SlaveAddr, I2C_Direction_Transmitter);
    }
    else
    {
      I2C_Send7bitAddress(I2C1, SlaveAddr, I2C_Direction_Receiver);
    }
    break;

    /* EV6 */
  case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
    /* EV8 */
  case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
    if (TxIndex < TxLength)
    {
      /* Transmit Data */
      I2C_SendData(I2C1, TxBuffer[TxIndex++]);
    }
    break;

    /* EV8_2 */
  case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
    /* Send RESTART condition */
    I2C_GenerateSTART(I2C1, ENABLE);
    break;

    /* EV6 */
  case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
    if (RxLength - RxIndex == 1)
    {
      I2C_AcknowledgeConfig(I2C1, DISABLE);
      I2C_GenerateSTOP(I2C1, ENABLE);
    }
    break;

  default:
    // EV8 I2C_EVENT_MASTER_BYTE_RECEIVED: BUSY, MSL and RXNE flags
    // In some case, only RXNE is set, so just check RXNE flag.
    if ((Event & I2C_SR1_RXNE) != 0)
    {
      // Read data byte
      pRxData[RxIndex++] = I2C_ReceiveData(I2C1);

      if (RxLength - RxIndex == 1)
      {
        I2C_AcknowledgeConfig(I2C1, DISABLE);
        I2C_GenerateSTOP(I2C1, ENABLE);
      }
      else if (RxLength == RxIndex)
      {
        I2C_ITConfig(I2C1, (I2C_IT_TypeDef)(I2C_IT_EVT | I2C_IT_BUF), DISABLE);

        IsTxRxComplete = true;
      }
    }
    else
    {
      OnError();
    }
    break;
  }
}

INTERRUPT_HANDLER(I2C1_SPI2_IRQHandler, 29)
{
  switch (I2cType)
  {
  case I2C_TX:
  case I2C_MEMORY_TX:
    TxInterrupt();
    break;

  case I2C_RX:
    RxInterrupt();
    break;

  case I2C_MEMORY_RX:
    MemoryRxInterrupt();
    break;

  default:
    break;
  }
}

bool I2cImpl_IsBusy(void)
{
  return I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY);
}

bool I2cImpl_IsTxRxComplete(void)
{
  return IsTxRxComplete;
}

static OsErr_t WaiteForBusyOk()
{
  u16 count = 0;
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
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

OsErr_t I2cImpl_MemoryTx(u8 add, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u16 length)
{
  if (WaiteForBusyOk() != OS_ERR_OK)
    return OS_ERR_BUSY;

  I2cImpl_StartMemoryTx(add, memory, memorySize, pData, length);

  if (WaiteForBusyOk() != OS_ERR_OK)
    return OS_ERR_BUSY;

  return OS_ERR_OK;
}

OsErr_t I2cImpl_MemoryRx(u8 add, u16 memory, I2cMemorySize_t memorySize, u8 *pData, u16 length)
{
  if (WaiteForBusyOk() != OS_ERR_OK)
    return OS_ERR_BUSY;

  I2cImpl_StartMemoryRx(add, memory, memorySize, pData, length);

  if (WaiteForBusyOk() != OS_ERR_OK)
    return OS_ERR_BUSY;

  return OS_ERR_OK;
}