#include "ir_rx_func.h"
#include "stm8l15x_conf.h"

/*----------------------------Ir time definition-----------------------------*/
/*! Sets the pulse width tolerance when receiving incoming IR NEC frames. */
#define NEC_TOLERANCE 20E-2

/*! Sets the timer's base time used in the current application. */
#define TIMER_BASE_TIME 1E-6

/*! Leading pulse burst max time length: 16.20ms (13.5ms + 20%)*/
#define IR_START_MAX ((u16)((135E-4 * (1 + NEC_TOLERANCE)) / TIMER_BASE_TIME))

/*! Leading pulse burst min time length: 10.08ms (13.5ms - 20%)*/
#define IR_START_MIN ((u16)((135E-4 * (1 - NEC_TOLERANCE)) / TIMER_BASE_TIME))

/*! Logical '1' max time length: 2.700ms (2.250ms + 20%) / (Timer base time)*/
#define IR_ONE_MAX ((u16)((225E-5 * (1 + NEC_TOLERANCE)) / TIMER_BASE_TIME))

/*! Logical '1' min time length: 1.800ms (2.250ms - 20%) / (Timer base time)*/
#define IR_ONE_MIN ((u16)((225E-5 * (1 - NEC_TOLERANCE)) / TIMER_BASE_TIME))

/*! Logical '0' max time length: 1.350ms (1.125ms + 20%) / (Timer base time)*/
#define IR_ZERO_MAX ((u16)((1125E-6 * (1 + NEC_TOLERANCE)) / TIMER_BASE_TIME))

/*! Logical '0' min time length: 900ms (1.125ms + 20%) / (Timer base time)*/
#define IR_ZERO_MIN ((u16)((1125E-6 * (1 - NEC_TOLERANCE)) / TIMER_BASE_TIME))

/*! Defines the time after the last falling edge of an IR frame before it can be considered no more bits will
 * arrive, so the frame can be decoded. */
#define IR_RX_FRAME_TIMEOUT ((u16)(20E-3 / TIMER_BASE_TIME))

/*-------------------------------Ir variables--------------------------------*/
/*!< Indicates if this is the first edge of the frame. */
static volatile bool FirstEdge;

/*!< Indicates if leading start pulse has been received. */
static volatile bool LeadPulse;

/*!< Enables/disables frame reception timeout. */
static volatile bool FrameTimeout;

/*!< Indicate frame received or not. */
static volatile bool FrameRcv;

/*! Currently being received byte. */
static volatile u8 IrRxByte;

/*! Currently being received bits. */
static volatile u8 IrRxBits;

/*! Holds the timer value captured previously. Used to calculate received
  pulse width.*/
static volatile u16 IrRxPrevCounterValue;

/*! Holds the current captured timer value. Used to calculate received pulse
  width.*/
static volatile u16 IrRxCurrCounterValue;

/*! Number of bytes received. */
static volatile u8 IrRxLength;

/*! Size of the IR reception buffer */
#define IR_RX_BUFFER_SIZE 64

/*! Receive buffer. */
static volatile u8 IrRxBuffer[IR_RX_BUFFER_SIZE];

static Ir_RcvFunc_t RcvFunc;

#pragma diag_suppress = Pa082
/*-------------------------------Ir functions--------------------------------*/
void IrRx_Init(Ir_RcvFunc_t rcvFunc)
{
  RcvFunc = rcvFunc;

  CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);
  TIM1_TimeBaseInit(16, TIM1_CounterMode_Up, 0xffff, 0);

  /* Timer1-Channel 1 - Generates an interrupt every 562,5us, for the IR TX */
  TIM1_OC1Init(TIM1_OCMode_Timing, TIM1_OutputState_Disable, TIM1_OutputNState_Disable, IR_RX_FRAME_TIMEOUT,
               TIM1_OCPolarity_High, TIM1_OCNPolarity_Low, TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
  // TIM1_ITConfig(TIM1_IT_CC1, ENABLE);

  /* Timer1-Channel 3 - Captures the time between input IR falling edges */
  TIM1_ICInit(TIM1_Channel_3, TIM1_ICPolarity_Falling, TIM1_ICSelection_DirectTI, TIM1_ICPSC_DIV1, 0x00);

  TIM1_ITConfig(TIM1_IT_CC3, ENABLE);
  TIM1_Cmd(ENABLE);
}

void IrRx_Process(void)
{
  if (FrameRcv)
  {
    FrameRcv = false;

    if (IrRxLength == 4 && IrRxBuffer[0] + IrRxBuffer[1] == 0xFF && IrRxBuffer[2] + IrRxBuffer[3] == 0xFF)
    {
      RcvFunc((u8 *)IrRxBuffer, IrRxLength);
    }
  }
}

static void IrRx_CaptureInterrupt(void)
{
  /* Reload time out counter and enable timer channel */
  TIM1_ClearFlag(TIM1_FLAG_CC1);
  TIM1_SetCompare1(TIM1_GetCounter() + IR_RX_FRAME_TIMEOUT);
  TIM1_ITConfig(TIM1_IT_CC1, ENABLE);

  FrameTimeout = true;

  /* Get current counter value */
  IrRxCurrCounterValue = TIM1_GetCapture3();

  /* First falling edge must be treated in a special way in order to initialize pulse-width counters.*/
  if (FirstEdge)
  {
    FirstEdge = false;

    IrRxPrevCounterValue = IrRxCurrCounterValue;
  }
  else
  {
    /* Find pulse width. No need to take into account overflow because 16 bit roll over ensures a correct
     * result. */
    u16 IrPulseWidth = IrRxCurrCounterValue - IrRxPrevCounterValue;
    /* Update previous counter value */
    IrRxPrevCounterValue = IrRxCurrCounterValue;

    /* Check for the leading pulse burst */
    if ((IrPulseWidth > IR_START_MIN) && (IrPulseWidth < IR_START_MAX))
    {
      LeadPulse = true;
      IrRxLength = IrRxByte = IrRxBits = 0;
    }

    if (LeadPulse)
    {
      /* Check for "1" pulse */
      if ((IrPulseWidth > IR_ONE_MIN) && (IrPulseWidth < IR_ONE_MAX))
      {
        IrRxByte >>= 1;
        IrRxByte |= 0x80;
        IrRxBits++;
      }

      /* Check for "0" pulse */
      if ((IrPulseWidth > IR_ZERO_MIN) && (IrPulseWidth < IR_ZERO_MAX))
      {
        IrRxByte >>= 1;
        IrRxBits++;
      }
    }

    /* Check if we have received a full byte */
    if (IrRxBits == 8)
    {
      /* Push received byte into the queue and restart the bit counter */
      IrRxBuffer[IrRxLength++] = IrRxByte;
      IrRxByte = IrRxBits = 0;
    }
  }
}

static void IrRx_TimeoutExpire(void)
{
  TIM1_ITConfig(TIM1_IT_CC1, DISABLE);

  if (FrameTimeout)
  {
    FrameTimeout = false;

    /* Reset reception flags and variables */
    LeadPulse = false;
    FirstEdge = true;
    IrRxByte = IrRxBits = 0;

    /* This is to check we have received at least a full byte, to avoid that a
                           spurious signal triggers the decoding routine. */
    if (IrRxLength > 0)
    {
      FrameRcv = true;
    }
  }
}

INTERRUPT_HANDLER(TIM1_CAP_IRQHandler, 24)
{
  if (TIM1_GetFlagStatus(TIM1_FLAG_CC3)) // IR rx capture
  {
    TIM1_ClearFlag(TIM1_FLAG_CC3);

    IrRx_CaptureInterrupt();
  }

  if (TIM1_GetFlagStatus(TIM1_FLAG_CC1)) // IR rx timeout
  {
    TIM1_ClearFlag(TIM1_FLAG_CC1);

    IrRx_TimeoutExpire();
  }
}

#pragma diag_default = Pa082