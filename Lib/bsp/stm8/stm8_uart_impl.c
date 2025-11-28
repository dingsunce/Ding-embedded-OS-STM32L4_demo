#include "stm8_uart_impl.h"

#include "stm8l15x_conf.h"

#define ENABLE_UART1
// #define ENABLE_UART1_REMAP_PA2_PA3
// #define ENABLE_UART1_REMAP_PC5_PC6
// #define ENABLE_UART2
// #define ENABLE_UART3

#define USART_BAUDRATE 115200 // 256000

#ifdef ENABLE_UART1
#define USART_CHANNEL USART1
#define USART_CLK     CLK_Peripheral_USART1
#if defined ENABLE_UART1_REMAP_PA2_PA3
#define USART_TX_PORT GPIOA
#define USART_TX_PIN  GPIO_Pin_3
#define USART_RX_PORT GPIOA
#define USART_RX_PIN  GPIO_Pin_2
#elif defined ENABLE_UART1_REMAP_PC5_PC6
#define USART_TX_PORT GPIOC
#define USART_TX_PIN  GPIO_Pin_6
#define USART_RX_PORT GPIOC
#define USART_RX_PIN  GPIO_Pin_5
#else
#define USART_TX_PORT GPIOC
#define USART_TX_PIN  GPIO_Pin_3
#define USART_RX_PORT GPIOC
#define USART_RX_PIN  GPIO_Pin_2
#endif
#define USART_DMA_CHANNEL_TX    DMA1_Channel1
#define USART_DMA_CHANNEL_RX    DMA1_Channel2
#define USART_DMA_IT_TC         DMA1_IT_TC1
#define USART_TX_INTERRUPT_NAME USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQHandler
#define USART_TX_INTERRUPT_ID   27
#define USART_RX_INTERRUPT_NAME USART1_RX_TIM5_CC_IRQHandler
#define USART_RX_INTERRUPT_ID   28
#endif

#ifdef ENABLE_UART2
#define USART_CHANNEL           USART2
#define USART_CLK               CLK_Peripheral_USART2
#define USART_TX_PORT           GPIOE
#define USART_TX_PIN            GPIO_Pin_4
#define USART_RX_PORT           GPIOE
#define USART_RX_PIN            GPIO_Pin_3
#define USART_DMA_CHANNEL_TX    DMA1_Channel0
#define USART_DMA_CHANNEL_RX    DMA1_Channel3
#define USART_DMA_IT_TC         DMA1_IT_TC0
#define USART_TX_INTERRUPT_NAME TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler
#define USART_TX_INTERRUPT_ID   19
#define USART_RX_INTERRUPT_NAME TIM2_CC_USART2_RX_IRQHandler
#define USART_RX_INTERRUPT_ID   20
#endif

#ifdef ENABLE_UART3
#define USART_CHANNEL           USART3
#define USART_CLK               CLK_Peripheral_USART3
#define USART_TX_PORT           GPIOE
#define USART_TX_PIN            GPIO_Pin_7
#define USART_RX_PORT           GPIOE
#define USART_RX_PIN            GPIO_Pin_6
#define USART_DMA_CHANNEL_TX    DMA1_Channel1
#define USART_DMA_CHANNEL_RX    DMA1_Channel2
#define USART_DMA_IT_TC         DMA1_IT_TC1
#define USART_TX_INTERRUPT_NAME TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler
#define USART_TX_INTERRUPT_ID   21
#define USART_RX_INTERRUPT_NAME TIM3_CC_USART3_RX_IRQHandler
#define USART_RX_INTERRUPT_ID   22
#endif

#define TX_BUFFER_SIZE 50
#define RX_BUFFER_SIZE 100

static u8            TxBuffer[TX_BUFFER_SIZE];
static u8            RxBuffer[RX_BUFFER_SIZE];
static volatile bool IsSendCompleted = false;
static volatile bool IsRcvPackage = false;

// Static functions.
static void ClearIdleInterruptPending(void);
static void UartConfig(void);
static void DmaConfig(void);

void UartImpl_Init(void)
{
  UartConfig();

  DmaConfig();

  /* USART Enable */
  USART_Cmd(USART_CHANNEL, ENABLE);

  ClearIdleInterruptPending();

  UartImpl_RestartRx();
}

void UartImpl_Send(u8 *pData, u8 length)
{
  IsSendCompleted = false;

  memcpy(TxBuffer, pData, length);

  DMA_Cmd(USART_DMA_CHANNEL_TX, DISABLE);
  DMA_SetCurrDataCounter(USART_DMA_CHANNEL_TX, length);
  DMA_Cmd(USART_DMA_CHANNEL_TX, ENABLE);
}

bool UartImpl_IsSendCompleted(void)
{
  return IsSendCompleted;
}

bool UartImpl_IsRcvPackage(void)
{
  return IsRcvPackage;
}

u8 *UartImpl_GetRxData(u16 *pLength)
{
  *pLength = RX_BUFFER_SIZE - DMA_GetCurrDataCounter(USART_DMA_CHANNEL_RX);
  return RxBuffer;
}

static void ClearIdleInterruptPending(void)
{
  // To clear IDLE state, clear SR first, then clear DR.
  USART_CHANNEL->SR;
  USART_CHANNEL->DR;
}

void UartImpl_RestartRx(void)
{
  IsRcvPackage = false;

  ClearIdleInterruptPending();
  USART_ITConfig(USART_CHANNEL, USART_IT_IDLE, ENABLE);
  DMA_SetCurrDataCounter(USART_DMA_CHANNEL_RX, RX_BUFFER_SIZE);
  DMA_Cmd(USART_DMA_CHANNEL_RX, ENABLE);
}

static void UartImpl_StopRx(void)
{
  DMA_Cmd(USART_DMA_CHANNEL_RX, DISABLE);
  USART_ITConfig(USART_CHANNEL, USART_IT_IDLE, DISABLE);
}

static void UartConfig(void)
{
  /* Enable USART clock */
  CLK_PeripheralClockConfig(USART_CLK, ENABLE);

  /* Configure USART Tx as alternate function push-pull  (software pull up)*/
  GPIO_ExternalPullUpConfig(USART_TX_PORT, USART_TX_PIN, ENABLE);

  /* Configure USART Rx as alternate function push-pull  (software pull up)*/
  GPIO_ExternalPullUpConfig(USART_TX_PORT, USART_RX_PIN, ENABLE);

#ifdef ENABLE_UART1_REMAP_PA2_PA3
  SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA, ENABLE);
#endif

#ifdef ENABLE_UART1_REMAP_PC5_PC6
  SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortC, ENABLE);
#endif

  /* USART configured as follow:
      - BaudRate = USART_BAUDRATE baud
      - Word Length = 8 Bits
      - One Stop Bit
      - No parity
      - Receive and transmit enabled
      - USART Clock disabled
  */

  /* USART configuration */
  USART_Init(USART_CHANNEL, USART_BAUDRATE, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No,
             (USART_Mode_TypeDef)(USART_Mode_Tx | USART_Mode_Rx));

  // Rx idle interrupt
  // USART_ITConfig(USART_CHANNEL, USART_IT_IDLE, ENABLE); // Enable the Rx Idle interrupt manually using
  // UartImpl_RestartRx function.

  USART_ITConfig(USART_CHANNEL, USART_IT_TC, ENABLE);

  /* USART Disable */
  USART_Cmd(USART_CHANNEL, DISABLE);
}

static void DmaConfig(void)
{
  /*Enable DMA clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);

  /* DMA channel Tx of USART Configuration */
  DMA_Init(USART_DMA_CHANNEL_TX, (uint16_t)TxBuffer, (uint16_t)&USART_CHANNEL->DR, TX_BUFFER_SIZE,
           DMA_DIR_MemoryToPeripheral, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_High,
           DMA_MemoryDataSize_Byte);

  /* DMA channel Rx of USART Configuration */
  DMA_Init(USART_DMA_CHANNEL_RX, (uint16_t)RxBuffer, (uint16_t)&USART_CHANNEL->DR, RX_BUFFER_SIZE,
           DMA_DIR_PeripheralToMemory, DMA_Mode_Normal, DMA_MemoryIncMode_Inc, DMA_Priority_Low,
           DMA_MemoryDataSize_Byte);

  /* Enable the USART Tx/Rx DMA requests */
  USART_DMACmd(USART_CHANNEL, USART_DMAReq_TX, ENABLE);
  USART_DMACmd(USART_CHANNEL, USART_DMAReq_RX, ENABLE);

  /* Global DMA Enable */
  DMA_GlobalCmd(ENABLE);

  /* Enable the USART Tx DMA channel */
  // DMA_Cmd(USART_DMA_CHANNEL_TX, ENABLE); // Turn on the Tx channel when data need to be sent.

  /* Enable the USART Rx DMA channel */
  // DMA_Cmd(USART_DMA_CHANNEL_RX, ENABLE); // Turn on the Rx channel manually using UartImpl_RestartRx
  // function.
}

INTERRUPT_HANDLER(USART_TX_INTERRUPT_NAME, USART_TX_INTERRUPT_ID)
{
  if (USART_GetITStatus(USART_CHANNEL, USART_IT_TC) != RESET)
  {
    USART_ClearITPendingBit(USART_CHANNEL, USART_IT_TC);

    IsSendCompleted = true;
  }
}

INTERRUPT_HANDLER(USART_RX_INTERRUPT_NAME, USART_RX_INTERRUPT_ID)
{
  if (USART_GetITStatus(USART_CHANNEL, USART_IT_IDLE) != RESET)
  {
    ClearIdleInterruptPending();

    IsRcvPackage = true;

    UartImpl_StopRx();
  }
}
