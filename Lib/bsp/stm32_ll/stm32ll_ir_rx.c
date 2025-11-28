#include "stm32ll_ir_rx.h"

#include "ir_rx_func.h"
#include "stm32ll.h"
#include "stm32ll_ir_tx.h"
#include "stm32ll_tim.h"

/*** RECEPTION PULSE TIMES ***/

/*! Lead pulse burst nominal time, including mark and space (13,5ms) */
#define IR_LEAD (13.5E-3 / IR_BASE_TIME)

/*! Leading pulse burst max time: 16,200ms (13,5ms + 20%) */
#define IR_LEAD_MAX (u16)((IR_LEAD * 1.2) + 0.5)

/*! Leading	pulse burst min time: 10,800ms (13,5ms - 20%) */
#define IR_LEAD_MIN (u16)((IR_LEAD * 0.8) + 0.5)

/*! '1' pulse nominal time, including mark and space (2,250ms) */
#define IR_ONE (2.25E-3 / IR_BASE_TIME)

/*! Logical '1' max time: 2,700ms (2,250ms + 20%) */
#define IR_ONE_MAX (u16)((IR_ONE * 1.2) + 0.5)

/*! Logical '1' min time: 1,800ms (2,250ms - 20%) */
#define IR_ONE_MIN (u16)((IR_ONE * 0.8) + 0.5)

/*! '0' pulse nominal time, including mark and space (1,125ms) */
#define IR_ZERO (1.125E-3 / IR_BASE_TIME)

/*! Logical '0' max time: 1,350ms (1,125ms + 20%) */
#define IR_ZERO_MAX (u16)((IR_ZERO * 1.2) + 0.5)

/*! Logical '0' min time: 0,900ms (1,125ms - 20%) */
#define IR_ZERO_MIN (u16)((IR_ZERO * 0.8) + 0.5)


/*** TIMER CONFIGURATION VALUES ***/

/*! Defines the time after the last falling edge of an IR frame before it can be
                considered no more bits will arrive, so the frame can be decoded. */
#define IR_RX_FRAME_TIMEOUT 20E-3

/*! Timer compare value needed to generate #IR_RX_FRAME_TIMEOUT. */
#define IR_RX_FRAME_TIMEOUT_COMPARE_VALUE ((u16)(IR_RX_FRAME_TIMEOUT / IR_BASE_TIME))

/*! Size of the IR reception buffer. It depends on xProtocol frame size. */
#define IR_RX_BUFFER_SIZE 64

/*! States used in IR reception ISR */
typedef enum
{
  IR_RX_IDLE,
  IR_RX_LEAD, /*!< First edge detected, check for lead pulse burst. */
  IR_RX_BITS, /*!< Lead pulse burst received, try to decode bits. */
} IR_RX_State_t;

/*! IR reception state */
static volatile IR_RX_State_t RxState = IR_RX_IDLE;

/*! Last counter of timer input capture. */
static volatile u16 LastCaptureCounter;

/*! Currently being received bits. */
static volatile u8 BitCursor;

/*! Currently being received bytes. */
static volatile u8 ByteCursor;

/*!< Indicate frame received or not. */
static volatile bool FrameRcv;

static volatile u8 StopCouter;

/*! Receive buffer. */
static volatile u8 RxBuffer[IR_RX_BUFFER_SIZE];

static Ir_RcvFunc_t RcvFunc;

#ifdef __ICCARM__
#pragma diag_suppress = Pa082
#endif
/*-------------------------------Ir functions--------------------------------*/
void IrRx_Init(Ir_RcvFunc_t rcvFunc)
{
  RcvFunc = rcvFunc;

  LL_TIM_EnableIT_CC(IR_TIMx_INSTANCE, IR_TIMx_INPUT_CAPTURE_CHANNEL);
  LL_TIM_CC_EnableChannel(IR_TIMx_INSTANCE, LL_TIM_CHANNEL_CH(IR_TIMx_INPUT_CAPTURE_CHANNEL));

  LL_TIM_EnableCounter(IR_TIMx_INSTANCE);
}

void IrRx_Process(void)
{
  if (FrameRcv)
  {
    FrameRcv = false;
    RcvFunc((u8 *)RxBuffer, ByteCursor);
  }
}


static void IrRx_CaptureInterrupt(void)
{
  /* Enable IR timeout interrupt. Previously, clear any pending flag. */
  LL_TIM_ClearFlag_CC(IR_TIMx_INSTANCE, IR_TIMx_TIMEOUT_CHANNEL);
  LL_TIM_OC_SetCompareCH(IR_TIMx_INSTANCE, IR_TIMx_TIMEOUT_CHANNEL,
                        LL_TIM_GetCounter(IR_TIMx_INSTANCE) + IR_RX_FRAME_TIMEOUT_COMPARE_VALUE);
  LL_TIM_EnableIT_CC(IR_TIMx_INSTANCE, IR_TIMx_TIMEOUT_CHANNEL);

  /* Find pulse width by getting current counter value */
  StopCouter=0;
  u16 IrPulseWidth=0;
  u16 ThisCaptureCounter=LL_TIM_GetCounter(IR_TIMx_INSTANCE);
  if(ThisCaptureCounter>LastCaptureCounter)
  {
     IrPulseWidth = ThisCaptureCounter - LastCaptureCounter;
  }
  else
  {
     IrPulseWidth = 0xffff-LastCaptureCounter + ThisCaptureCounter;
  }
  
  LastCaptureCounter = ThisCaptureCounter;

  switch (RxState)
  {
    case IR_RX_IDLE:
      /* First falling edge detected. boot code */
      ByteCursor = BitCursor = 0;
      RxState = IR_RX_LEAD;
    break;

    case IR_RX_LEAD:
    /* After receiving first falling edge, check for leading pulse burst. */
    /* Reset bit counters */
    ByteCursor = BitCursor = 0;
    if ((IrPulseWidth > IR_LEAD_MIN) && (IrPulseWidth < IR_LEAD_MAX))
    {
      /* Go to next state */
      RxState = IR_RX_BITS;
    }
    else
    {
      RxState = IR_RX_LEAD;
    }
    break;

    case IR_RX_BITS:
    /* Once received first leading pulse, try to decode following bits. In
       the NEC protocol, bits are sent LSB first and MSB last!!! */

    /* Check for "1" pulse */
    if ((IrPulseWidth > IR_ONE_MIN) && (IrPulseWidth < IR_ONE_MAX))
    {
      RxBuffer[ByteCursor] >>= 1;
      RxBuffer[ByteCursor] |= 0x80;
      BitCursor++;
    }

    /* Check for "0" pulse */
    else if ((IrPulseWidth > IR_ZERO_MIN) && (IrPulseWidth < IR_ZERO_MAX))
    {
      RxBuffer[ByteCursor] >>= 1;
      BitCursor++;
    }
    else if((IrPulseWidth>IR_LEAD_MIN)&&(IrPulseWidth<IR_LEAD_MAX))
    {       
        ByteCursor = BitCursor = 0;
    }

    /* Check if we have received a full byte */
    if (BitCursor >= 8)
    {
      BitCursor = 0;

      ByteCursor++;
    }
    break;

    default:
    /* Code should never reach this point. If so, go to idle state */
    RxState = IR_RX_IDLE;
    ByteCursor = 0;
    break;
  
  
 
  }
}

static void IrRx_TimeoutExpire(void)
{
 
  StopCouter++;
  if(StopCouter>=2)
  {
      /* Disable IR timeout interrupt */
    LL_TIM_DisableIT_CC(IR_TIMx_INSTANCE, IR_TIMx_TIMEOUT_CHANNEL);
      /* Check we have received at least a full byte, to avoid that a spurious

                        signal triggers the decoding routine. */
    if(ByteCursor>0)
    {
      FrameRcv = true;
    }   

    /* Reset reception state */
    RxState = IR_RX_IDLE;

  }
  

 
}


void IR_TIMx_IRQHandler(void)
{
  if (LL_TIM_IsEnabledIT_CC(IR_TIMx_INSTANCE, IR_TIMx_INPUT_CAPTURE_CHANNEL) &&
      LL_TIM_IsActiveFlag_CC(IR_TIMx_INSTANCE, IR_TIMx_INPUT_CAPTURE_CHANNEL)) // IR rx capture
  {
    LL_TIM_ClearFlag_CC(IR_TIMx_INSTANCE, IR_TIMx_INPUT_CAPTURE_CHANNEL);

    IrRx_CaptureInterrupt();
  }
#ifdef IR_ENABLE_TX
  else if (LL_TIM_IsEnabledIT_CC(IR_TIMx_INSTANCE, IR_TIMx_OUTPUT_COMPARE_CHANNEL) &&
           LL_TIM_IsActiveFlag_CC(IR_TIMx_INSTANCE, IR_TIMx_OUTPUT_COMPARE_CHANNEL)) // IR tx compare
  {
    LL_TIM_ClearFlag_CC(IR_TIMx_INSTANCE, IR_TIMx_OUTPUT_COMPARE_CHANNEL);

    IrTx_CompareInterrupt();
  }
#endif

  else if (LL_TIM_IsEnabledIT_CC(IR_TIMx_INSTANCE, IR_TIMx_TIMEOUT_CHANNEL) &&
           LL_TIM_IsActiveFlag_CC(IR_TIMx_INSTANCE, IR_TIMx_TIMEOUT_CHANNEL)) // IR rx timeout
  {
    LL_TIM_ClearFlag_CC(IR_TIMx_INSTANCE, IR_TIMx_TIMEOUT_CHANNEL);

    IrRx_TimeoutExpire();
  }
}

bool IrRx_IsBusy(void)
{
  return RxState != IR_RX_IDLE;
}

#ifdef __ICCARM__
#pragma diag_default = Pa082
#endif
