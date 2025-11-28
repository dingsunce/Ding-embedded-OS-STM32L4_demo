#include "stm8_pwm_impl.h"

#include "stm8_pwm_config.h"
#include "stm8l15x_conf.h"

#define PORT_TIME1_CH1 GPIOD
#define PIN_TIME1_CH1  GPIO_Pin_2

#define PORT_TIME1_CH1N GPIOD
#define PIN_TIME1_CH1N  GPIO_Pin_7

#define PORT_TIME1_CH2 GPIOD
#define PIN_TIME1_CH2  GPIO_Pin_4

#define PORT_TIME1_CH2N GPIOE
#define PIN_TIME1_CH2N  GPIO_Pin_1

#define PORT_TIME1_CH3 GPIOD
#define PIN_TIME1_CH3  GPIO_Pin_5

#define PORT_TIME1_CH3N GPIOE
#define PIN_TIME1_CH3N  GPIO_Pin_2

#define PORT_TIME2_CH1 GPIOB
#define PIN_TIME2_CH1  GPIO_Pin_0

#define PORT_TIME2_CH2 GPIOB
#define PIN_TIME2_CH2  GPIO_Pin_2

#define PORT_TIME3_CH1 GPIOB
#define PIN_TIME3_CH1  GPIO_Pin_1

#define PORT_TIME3_CH2 GPIOD
#define PIN_TIME3_CH2  GPIO_Pin_0

#define PORT_TIME5_CH1 GPIOA
#define PIN_TIME5_CH1  GPIO_Pin_7

#define PORT_TIME5_CH2 GPIOE
#define PIN_TIME5_CH2  GPIO_Pin_0

void Timer1_Init(void)
{
  TIM1_DeInit();
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);
  TIM1_TimeBaseInit(0, TIM1_CounterMode_Up, PWM_AUTO_RELOAD_VALUE, 0);
}

void Timer1_Enable(void)
{
  TIM1_ARRPreloadConfig(ENABLE);
  TIM1_CtrlPWMOutputs(ENABLE);
  TIM1_Cmd(ENABLE);
}

void Timer1Ch1_Init(void)
{
#ifdef TIME1_CH1_COMPLEMENTARY
  /* TIM1 Channel 1N */
  GPIO_Init(PORT_TIME1_CH1N, PIN_TIME1_CH1N, GPIO_Mode_Out_PP_High_Fast);

#if (!defined PWM_SIGNAL_INVERSE && !defined TIMER1_CH1_SHIFT)
  TIM1_OC1Init(TIM1_OCMode_PWM1, TIM1_OutputState_Disable, TIM1_OutputNState_Enable, 0, TIM1_OCPolarity_High,
               TIM1_OCNPolarity_High, TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
#elif (defined PWM_SIGNAL_INVERSE && !defined TIMER1_CH1_SHIFT)
  TIM1_OC1Init(TIM1_OCMode_PWM1, TIM1_OutputState_Disable, TIM1_OutputNState_Enable, 0, TIM1_OCPolarity_Low,
               TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
#elif (!defined PWM_SIGNAL_INVERSE && defined TIMER1_CH1_SHIFT)
  TIM1_OC1Init(TIM1_OCMode_PWM1, TIM1_OutputState_Disable, TIM1_OutputNState_Enable, PWM_MAX_VALUE,
               TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
#else
  TIM1_OC1Init(TIM1_OCMode_PWM1, TIM1_OutputState_Disable, TIM1_OutputNState_Enable, PWM_MAX_VALUE,
               TIM1_OCPolarity_High, TIM1_OCNPolarity_High, TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
#endif

#else
  /* TIM1 Channel 1 */
  GPIO_Init(PORT_TIME1_CH1, PIN_TIME1_CH1, GPIO_Mode_Out_PP_High_Fast);

#if (!defined PWM_SIGNAL_INVERSE && !defined TIMER1_CH1_SHIFT)
  TIM1_OC1Init(TIM1_OCMode_PWM1, TIM1_OutputState_Enable, TIM1_OutputNState_Disable, 0, TIM1_OCPolarity_High,
               TIM1_OCNPolarity_High, TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
#elif (defined PWM_SIGNAL_INVERSE && !defined TIMER1_CH1_SHIFT)
  TIM1_OC1Init(TIM1_OCMode_PWM1, TIM1_OutputState_Enable, TIM1_OutputNState_Disable, 0, TIM1_OCPolarity_Low,
               TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
#elif (!defined PWM_SIGNAL_INVERSE && defined TIMER1_CH1_SHIFT)
  TIM1_OC1Init(TIM1_OCMode_PWM1, TIM1_OutputState_Enable, TIM1_OutputNState_Disable, PWM_MAX_VALUE,
               TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
#else
  TIM1_OC1Init(TIM1_OCMode_PWM1, TIM1_OutputState_Enable, TIM1_OutputNState_Disable, PWM_MAX_VALUE,
               TIM1_OCPolarity_High, TIM1_OCNPolarity_High, TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
#endif

#endif
  TIM1_OC1PreloadConfig(ENABLE);
}

void Timer1Ch2_Init(void)
{
#ifdef TIME1_CH2_COMPLEMENTARY
  /* TIM1 Channe 2N */
  GPIO_Init(PORT_TIME1_CH2N, PIN_TIME1_CH2N, GPIO_Mode_Out_PP_High_Fast);

#if (!defined PWM_SIGNAL_INVERSE && !defined TIMER1_CH2_SHIFT)
  TIM1_OC2Init(TIM1_OCMode_PWM1, TIM1_OutputState_Disable, TIM1_OutputNState_Enable, 0, TIM1_OCPolarity_High,
               TIM1_OCNPolarity_High, TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
#elif (defined PWM_SIGNAL_INVERSE && !defined TIMER1_CH2_SHIFT)
  TIM1_OC2Init(TIM1_OCMode_PWM1, TIM1_OutputState_Disable, TIM1_OutputNState_Enable, 0, TIM1_OCPolarity_Low,
               TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
#elif (!defined PWM_SIGNAL_INVERSE && defined TIMER1_CH2_SHIFT)
  TIM1_OC2Init(TIM1_OCMode_PWM1, TIM1_OutputState_Disable, TIM1_OutputNState_Enable, PWM_MAX_VALUE,
               TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
#else
  TIM1_OC2Init(TIM1_OCMode_PWM1, TIM1_OutputState_Disable, TIM1_OutputNState_Enable, PWM_MAX_VALUE,
               TIM1_OCPolarity_High, TIM1_OCNPolarity_High, TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
#endif

#else
  /* TIM1 Channel 2 */
  GPIO_Init(PORT_TIME1_CH2, PIN_TIME1_CH2, GPIO_Mode_Out_PP_High_Fast);

#if (!defined PWM_SIGNAL_INVERSE && !defined TIMER1_CH2_SHIFT)
  TIM1_OC2Init(TIM1_OCMode_PWM1, TIM1_OutputState_Enable, TIM1_OutputNState_Disable, 0, TIM1_OCPolarity_High,
               TIM1_OCNPolarity_High, TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
#elif (defined PWM_SIGNAL_INVERSE && !defined TIMER1_CH2_SHIFT)
  TIM1_OC2Init(TIM1_OCMode_PWM1, TIM1_OutputState_Enable, TIM1_OutputNState_Disable, 0, TIM1_OCPolarity_Low,
               TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
#elif (!defined PWM_SIGNAL_INVERSE && defined TIMER1_CH2_SHIFT)
  TIM1_OC2Init(TIM1_OCMode_PWM1, TIM1_OutputState_Enable, TIM1_OutputNState_Disable, PWM_MAX_VALUE,
               TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
#else
  TIM1_OC2Init(TIM1_OCMode_PWM1, TIM1_OutputState_Enable, TIM1_OutputNState_Disable, PWM_MAX_VALUE,
               TIM1_OCPolarity_High, TIM1_OCNPolarity_High, TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
#endif

#endif

  TIM1_OC2PreloadConfig(ENABLE);
}

void Timer1Ch3_Init(void)
{
#if TIME1_CH3_COMPLEMENTARY
  /* TIM1 Channe 3N */
  GPIO_Init(PORT_TIME1_CH3N, PIN_TIME1_CH3N, GPIO_Mode_Out_PP_High_Fast);

#if (!defined PWM_SIGNAL_INVERSE && !defined TIMER1_CH3_SHIFT)
  TIM1_OC3Init(TIM1_OCMode_PWM1, TIM1_OutputState_Disable, TIM1_OutputNState_Enable, 0, TIM1_OCPolarity_High,
               TIM1_OCNPolarity_High, TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
#elif (defined PWM_SIGNAL_INVERSE && !defined TIMER1_CH3_SHIFT)
  TIM1_OC3Init(TIM1_OCMode_PWM1, TIM1_OutputState_Disable, TIM1_OutputNState_Enable, 0, TIM1_OCPolarity_Low,
               TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
#elif (!defined PWM_SIGNAL_INVERSE && defined TIMER1_CH3_SHIFT)
  TIM1_OC3Init(TIM1_OCMode_PWM1, TIM1_OutputState_Disable, TIM1_OutputNState_Enable, PWM_MAX_VALUE,
               TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
#else
  TIM1_OC3Init(TIM1_OCMode_PWM1, TIM1_OutputState_Disable, TIM1_OutputNState_Enable, PWM_MAX_VALUE,
               TIM1_OCPolarity_High, TIM1_OCNPolarity_High, TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
#endif

#else
  /* TIM1 Channel 3 */
  GPIO_Init(PORT_TIME1_CH3, PIN_TIME1_CH3, GPIO_Mode_Out_PP_High_Fast);

#if (!defined PWM_SIGNAL_INVERSE && !defined TIMER1_CH3_SHIFT)
  TIM1_OC3Init(TIM1_OCMode_PWM1, TIM1_OutputState_Enable, TIM1_OutputNState_Disable, 0, TIM1_OCPolarity_High,
               TIM1_OCNPolarity_High, TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
#elif (defined PWM_SIGNAL_INVERSE && !defined TIMER1_CH3_SHIFT)
  TIM1_OC3Init(TIM1_OCMode_PWM1, TIM1_OutputState_Enable, TIM1_OutputNState_Disable, 0, TIM1_OCPolarity_Low,
               TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
#elif (!defined PWM_SIGNAL_INVERSE && defined TIMER1_CH3_SHIFT)
  TIM1_OC3Init(TIM1_OCMode_PWM1, TIM1_OutputState_Enable, TIM1_OutputNState_Disable, PWM_MAX_VALUE,
               TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
#else
  TIM1_OC3Init(TIM1_OCMode_PWM1, TIM1_OutputState_Enable, TIM1_OutputNState_Disable, PWM_MAX_VALUE,
               TIM1_OCPolarity_High, TIM1_OCNPolarity_High, TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
#endif

#endif

  TIM1_OC3PreloadConfig(ENABLE);
}

void Timer1Ch1_SetLevel(u16 level)
{
  TIM1_SetCompare1(level);
}

void Timer1Ch2_SetLevel(u16 level)
{
  TIM1_SetCompare2(level);
}

void Timer1Ch3_SetLevel(u16 level)
{
  TIM1_SetCompare3(level);
}

void Timer2_Init(void)
{
  TIM2_DeInit();
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
  TIM2_TimeBaseInit(TIM2_Prescaler_1, TIM2_CounterMode_Up, PWM_AUTO_RELOAD_VALUE);
}

void Timer2_Enable(void)
{
  TIM2_ARRPreloadConfig(ENABLE);
  TIM2_CtrlPWMOutputs(ENABLE);
  TIM2_Cmd(ENABLE);
}

void Timer2Ch1_Init(void)
{
  GPIO_Init(PORT_TIME2_CH1, PIN_TIME2_CH1, GPIO_Mode_Out_PP_High_Fast);

#if (!defined PWM_SIGNAL_INVERSE && !defined TIMER2_CH1_SHIFT)
  TIM2_OC1Init(TIM2_OCMode_PWM1, TIM2_OutputState_Enable, 0, TIM2_OCPolarity_High, TIM2_OCIdleState_Reset);
#elif (defined PWM_SIGNAL_INVERSE && !defined TIMER2_CH1_SHIFT)
  TIM2_OC1Init(TIM2_OCMode_PWM1, TIM2_OutputState_Enable, 0, TIM2_OCPolarity_Low, TIM2_OCIdleState_Set);
#elif (!defined PWM_SIGNAL_INVERSE && defined TIMER2_CH1_SHIFT)
  TIM2_OC1Init(TIM2_OCMode_PWM1, TIM2_OutputState_Enable, PWM_MAX_VALUE, TIM2_OCPolarity_Low,
               TIM2_OCIdleState_Set);
#else
  TIM2_OC1Init(TIM2_OCMode_PWM1, TIM2_OutputState_Enable, PWM_MAX_VALUE, TIM2_OCPolarity_High,
               TIM2_OCIdleState_Reset);
#endif

  TIM2_OC1PreloadConfig(ENABLE);
}

void Timer2Ch2_Init(void)
{
  GPIO_Init(PORT_TIME2_CH2, PIN_TIME2_CH2, GPIO_Mode_Out_PP_High_Fast);

#if (!defined PWM_SIGNAL_INVERSE && !defined TIMER2_CH2_SHIFT)
  TIM2_OC2Init(TIM2_OCMode_PWM1, TIM2_OutputState_Enable, 0, TIM2_OCPolarity_High, TIM2_OCIdleState_Reset);
#elif (defined PWM_SIGNAL_INVERSE && !defined TIMER2_CH2_SHIFT)
  TIM2_OC2Init(TIM2_OCMode_PWM1, TIM2_OutputState_Enable, 0, TIM2_OCPolarity_Low, TIM2_OCIdleState_Set);
#elif (!defined PWM_SIGNAL_INVERSE && defined TIMER2_CH2_SHIFT)
  TIM2_OC2Init(TIM2_OCMode_PWM1, TIM2_OutputState_Enable, PWM_MAX_VALUE, TIM2_OCPolarity_Low,
               TIM2_OCIdleState_Set);
#else
  TIM2_OC2Init(TIM2_OCMode_PWM1, TIM2_OutputState_Enable, PWM_MAX_VALUE, TIM2_OCPolarity_High,
               TIM2_OCIdleState_Reset);
#endif

  TIM2_OC2PreloadConfig(ENABLE);
}

void Timer2Ch1_SetLevel(u16 level)
{
  TIM2_SetCompare1(level);
}

void Timer2Ch2_SetLevel(u16 level)
{
  TIM2_SetCompare2(level);
}

void Timer3_Init(void)
{
  TIM3_DeInit();
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);
  TIM3_TimeBaseInit(TIM3_Prescaler_1, TIM3_CounterMode_Up, PWM_AUTO_RELOAD_VALUE);
}

void Timer3_Enable(void)
{
  TIM3_ARRPreloadConfig(ENABLE);
  TIM3_CtrlPWMOutputs(ENABLE);
  TIM3_Cmd(ENABLE);
}

void Timer3Ch1_Init(void)
{
  GPIO_Init(PORT_TIME3_CH1, PIN_TIME3_CH1, GPIO_Mode_Out_PP_High_Fast);

#if (!defined PWM_SIGNAL_INVERSE && !defined TIMER3_CH1_SHIFT)
  TIM3_OC1Init(TIM3_OCMode_PWM1, TIM3_OutputState_Enable, 0, TIM3_OCPolarity_High, TIM3_OCIdleState_Reset);
#elif (defined PWM_SIGNAL_INVERSE && !defined TIMER3_CH1_SHIFT)
  TIM3_OC1Init(TIM3_OCMode_PWM1, TIM3_OutputState_Enable, 0, TIM3_OCPolarity_Low, TIM3_OCIdleState_Set);
#elif (!defined PWM_SIGNAL_INVERSE && defined TIMER3_CH1_SHIFT)
  TIM3_OC1Init(TIM3_OCMode_PWM1, TIM3_OutputState_Enable, PWM_MAX_VALUE, TIM3_OCPolarity_Low,
               TIM3_OCIdleState_Set);
#else
  TIM3_OC1Init(TIM3_OCMode_PWM1, TIM3_OutputState_Enable, PWM_MAX_VALUE, TIM3_OCPolarity_High,
               TIM3_OCIdleState_Reset);
#endif

  TIM3_OC1PreloadConfig(ENABLE);
}

void Timer3Ch2_Init(void)
{
  GPIO_Init(PORT_TIME3_CH2, PIN_TIME3_CH2, GPIO_Mode_Out_PP_High_Fast);

#if (!defined PWM_SIGNAL_INVERSE && !defined TIMER3_CH2_SHIFT)
  TIM3_OC2Init(TIM3_OCMode_PWM1, TIM3_OutputState_Enable, 0, TIM3_OCPolarity_High, TIM3_OCIdleState_Reset);
#elif (defined PWM_SIGNAL_INVERSE && !defined TIMER3_CH2_SHIFT)
  TIM3_OC2Init(TIM3_OCMode_PWM1, TIM3_OutputState_Enable, 0, TIM3_OCPolarity_Low, TIM3_OCIdleState_Set);
#elif (!defined PWM_SIGNAL_INVERSE && defined TIMER3_CH2_SHIFT)
  TIM3_OC2Init(TIM3_OCMode_PWM1, TIM3_OutputState_Enable, PWM_MAX_VALUE, TIM3_OCPolarity_Low,
               TIM3_OCIdleState_Set);
#else
  TIM3_OC2Init(TIM3_OCMode_PWM1, TIM3_OutputState_Enable, PWM_MAX_VALUE, TIM3_OCPolarity_High,
               TIM3_OCIdleState_Reset);
#endif

  TIM3_OC2PreloadConfig(ENABLE);
}

void Timer3Ch1_SetLevel(u16 level)
{
  TIM3_SetCompare1(level);
}

void Timer3Ch2_SetLevel(u16 level)
{
  TIM3_SetCompare2(level);
}

void Timer5_Init(void)
{
  TIM5_DeInit();
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM5, ENABLE);
  TIM5_TimeBaseInit(TIM5_Prescaler_1, TIM5_CounterMode_Up, PWM_AUTO_RELOAD_VALUE);
}

void Timer5_Enable(void)
{
  TIM5_ARRPreloadConfig(ENABLE);
  TIM5_CtrlPWMOutputs(ENABLE);
  TIM5_Cmd(ENABLE);
}

void Timer5Ch1_Init(void)
{
  GPIO_Init(PORT_TIME5_CH1, PIN_TIME5_CH1, GPIO_Mode_Out_PP_High_Fast);

#if (!defined PWM_SIGNAL_INVERSE && !defined TIMER5_CH1_SHIFT)
  TIM5_OC1Init(TIM5_OCMode_PWM1, TIM5_OutputState_Enable, 0, TIM5_OCPolarity_High, TIM5_OCIdleState_Reset);
#elif (defined PWM_SIGNAL_INVERSE && !defined TIMER5_CH1_SHIFT)
  TIM5_OC1Init(TIM5_OCMode_PWM1, TIM5_OutputState_Enable, 0, TIM5_OCPolarity_Low, TIM5_OCIdleState_Set);
#elif (!defined PWM_SIGNAL_INVERSE && defined TIMER5_CH1_SHIFT)
  TIM5_OC1Init(TIM5_OCMode_PWM1, TIM5_OutputState_Enable, PWM_MAX_VALUE, TIM5_OCPolarity_Low,
               TIM5_OCIdleState_Set);
#else
  TIM5_OC1Init(TIM5_OCMode_PWM1, TIM5_OutputState_Enable, PWM_MAX_VALUE, TIM5_OCPolarity_High,
               TIM5_OCIdleState_Reset);
#endif

  TIM5_OC1PreloadConfig(ENABLE);
}

void Timer5Ch2_Init(void)
{
  GPIO_Init(PORT_TIME5_CH2, PIN_TIME5_CH2, GPIO_Mode_Out_PP_High_Fast);

#if (!defined PWM_SIGNAL_INVERSE && !defined TIMER5_CH2_SHIFT)
  TIM5_OC2Init(TIM5_OCMode_PWM1, TIM5_OutputState_Enable, 0, TIM5_OCPolarity_High, TIM5_OCIdleState_Reset);
#elif (defined PWM_SIGNAL_INVERSE && !defined TIMER5_CH2_SHIFT)
  TIM5_OC2Init(TIM5_OCMode_PWM1, TIM5_OutputState_Enable, 0, TIM5_OCPolarity_Low, TIM5_OCIdleState_Set);
#elif (!defined PWM_SIGNAL_INVERSE && defined TIMER5_CH2_SHIFT)
  TIM5_OC2Init(TIM5_OCMode_PWM1, TIM5_OutputState_Enable, PWM_MAX_VALUE, TIM5_OCPolarity_Low,
               TIM5_OCIdleState_Set);
#else
  TIM5_OC2Init(TIM5_OCMode_PWM1, TIM5_OutputState_Enable, PWM_MAX_VALUE, TIM5_OCPolarity_High,
               TIM5_OCIdleState_Reset);
#endif

  TIM5_OC2PreloadConfig(ENABLE);
}

void Timer5Ch1_SetLevel(u16 level)
{
  TIM5_SetCompare1(level);
}

void Timer5Ch2_SetLevel(u16 level)
{
  TIM5_SetCompare2(level);
}