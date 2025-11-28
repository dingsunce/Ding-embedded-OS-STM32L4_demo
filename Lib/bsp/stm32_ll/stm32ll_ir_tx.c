#include "stm32ll_ir_tx.h"

#include "ir_tx_func.h"
#include "pwm_func.h"
#include "stm32ll.h"
#include "stm32ll_ir_rx.h"
#include "stm32ll_tim.h"

/*** TRANSMISSION PULSE TIMES***/

/*! Lead pulse burst mark time: 9ms */
#define IR_LEAD_MARK (u16)(9E-3 / IR_BASE_TIME)

/*! Lead pulse burst space time: 4,5ms */
#define IR_LEAD_SPACE (u16)(4.5E-3 / IR_BASE_TIME)

/*! '0' , '1' or end Bit mask time: 562us */
#define IR_BIT_MASK (u16)(562E-6 / IR_BASE_TIME)

/*! '0' Bit space time: 562us */
#define IR_BIT_0_SPACE (u16)(562E-6 / IR_BASE_TIME)

/*! '1' Bit space time: 1,688ms */
#define IR_BIT_1_SPACE (u16)(1.688E-3 / IR_BASE_TIME)

/*! Ending guard time: 40,5ms */
// #define IR_END_DELAY (u16)(562E-6 / IR_BASE_TIME)
#define IR_END_DELAY (u16)(40.5E-3 / IR_BASE_TIME)

/*! Size of the IR transmission buffer. It depends on xProtocol frame size. */
#define IR_TX_BUFFER_SIZE 64

/*! States used in IR transmission ISR */
typedef enum
{
  IR_TX_IDLE,
  IR_TX_LEAD_MARK,
  IR_TX_LEAD_SPACE,
  IR_TX_BIT_MARK,
  IR_TX_BIT_SPACE,
  IR_TX_END_MARK,
  IR_TX_END_DELAY,
} IR_TX_State_t;

/*! IR transmit state */
volatile IR_TX_State_t TxState = IR_TX_IDLE;

/*! Cursor used to navigate the transmission buffer */
static volatile u8 ByteCursor;

static volatile u8 BitCursor;

/*! Number of bytes to be transmitted */
volatile u8 TxLength;

/*! Transmitter buffer */
static volatile u8 IrTxBuffer[IR_TX_BUFFER_SIZE];

#ifdef __ICCARM__
#pragma diag_suppress = Pa082
#endif

void IrTx_Init(void)
{
  /* Start output signal generation */
  /**********************************/
  /* Enable output channel */
  LL_TIM_CC_EnableChannel(TIM17, LL_TIM_CHANNEL_CH1);

  /* Enable Timer outputs */
  LL_TIM_EnableAllOutputs(TIM17);

  /* Enable counter */
  LL_TIM_EnableCounter(TIM17);
}

#define IrTx_EnablePwm()                                                                                     \
  LL_TIM_OC_SetCompareCH1(TIM17, 210) // base on 16MHz clock, period 420us to generate 38KHz

#define IrTx_DisablePwm() LL_TIM_OC_SetCompareCH1(TIM17, 0)

#define IrTx_SetCompare(counter)                                                                             \
  LL_TIM_OC_SetCompareCH(IR_TIMx_INSTANCE, IR_TIMx_OUTPUT_COMPARE_CHANNEL,                                   \
                         (u16)(LL_TIM_GetCounter(IR_TIMx_INSTANCE) + (counter)))

#define IrTx_Enable()                                                                                        \
  LL_TIM_ClearFlag_CC(IR_TIMx_INSTANCE, IR_TIMx_OUTPUT_COMPARE_CHANNEL);                                     \
  LL_TIM_EnableIT_CC(IR_TIMx_INSTANCE, IR_TIMx_OUTPUT_COMPARE_CHANNEL)

#define IrTx_Disable() LL_TIM_DisableIT_CC(IR_TIMx_INSTANCE, IR_TIMx_OUTPUT_COMPARE_CHANNEL)

void IrTx_Send(u8 *pData, u8 length)
{
  if (TxState != IR_TX_IDLE || IrRx_IsBusy())
  {
    return;
  }

  if (length >= IR_TX_BUFFER_SIZE)
  {
    return;
  }

  memcpy((void *)IrTxBuffer, pData, length);
  TxLength = length;

  IrRx_Disable();

  /* Signal lead pulse burst */
  IrTx_EnablePwm();

  ByteCursor = BitCursor = 0;
  TxState = IR_TX_LEAD_MARK;

  IrTx_SetCompare(IR_LEAD_MARK);
  IrTx_Enable();
}

void IrTx_CompareInterrupt(void)
{
  /* Check IR TX state */
  switch (TxState)
  {
  case IR_TX_IDLE:
    break;

  case IR_TX_LEAD_MARK:
    IrTx_DisablePwm();

    TxState = IR_TX_LEAD_SPACE;
    IrTx_SetCompare(IR_LEAD_SPACE);
    break;

  case IR_TX_LEAD_SPACE:
    IrTx_EnablePwm();

    TxState = IR_TX_BIT_MARK;
    IrTx_SetCompare(IR_BIT_MASK);
    break;

  case IR_TX_BIT_MARK:
    IrTx_DisablePwm();

    TxState = IR_TX_BIT_SPACE;
    if (GET_BIT(IrTxBuffer[ByteCursor], BitCursor))
    {
      IrTx_SetCompare(IR_BIT_1_SPACE);
    }
    else
    {
      IrTx_SetCompare(IR_BIT_0_SPACE);
    }
    break;

  case IR_TX_BIT_SPACE:
    IrTx_EnablePwm();

    BitCursor++;
    if (BitCursor >= 8)
    {
      BitCursor = 0;

      ByteCursor++;
      if (ByteCursor >= TxLength)
      {
        TxState = IR_TX_END_MARK;
        IrTx_SetCompare(IR_BIT_MASK);
      }
      else
      {
        TxState = IR_TX_BIT_MARK;
        IrTx_SetCompare(IR_BIT_MASK);
      }
    }
    else
    {
      TxState = IR_TX_BIT_MARK;
      IrTx_SetCompare(IR_BIT_MASK);
    }
    break;

  case IR_TX_END_MARK:
    IrTx_DisablePwm();

    IrRx_Enable();

    TxState = IR_TX_END_DELAY;
    IrTx_SetCompare(IR_END_DELAY);
    break;

  case IR_TX_END_DELAY:
    TxState = IR_TX_IDLE;

    IrTx_Disable();
    break;

  default:
    /* Safety guard */
    TxState = IR_TX_IDLE;
    IrTx_Disable();
    IrRx_Enable();
    break;
  }
}

#ifdef __ICCARM__
#pragma diag_default = Pa082
#endif
