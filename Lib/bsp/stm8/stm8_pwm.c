#include <stdlib.h>

#include "pwm_func.h"
#include "stm8_pwm_config.h"
#include "stm8_pwm_impl.h"

typedef void (*PwmTimerInitFunc_t)(void);
typedef void (*PwmTimerSetLevel_t)(u16 level);

typedef struct
{
  PwmTimerId_t TimerId;

  bool IsShift;

  u16 CurrentLevel;
#ifdef PWM_FADE_ENABLE
  u16 GoalLevel;
  u16 StepLevel;
  u16 FadeCount;
#endif

  PwmTimerInitFunc_t TimerInit;
  PwmTimerSetLevel_t TimerSetLevel;
} Pwm_t;

#ifdef PWM_FADE_ENABLE
static void Timer_Calculate(Pwm_t *pTimer, float duty_cycle);
static void Timer_Process(Pwm_t *pTimer);
#endif
static void   Pwm_InitTimer(void);
static void   Pwm_EnableTimer(void);
static u16    ConvertToLevel(bool isShift, float duty_cycle);
static float  ConvertToDutyCycle(bool isShift, u16 level);
static void   Timer_SetLevel(Pwm_t *pTimer, u16 level);
static Pwm_t *FindTimerById(PwmTimerId_t timerId);

Pwm_t PwmTable[] = {
#ifdef TIMER1_CH1_ENABLE
    {
        TIMER1_CH1, /* Id */

#ifdef TIMER1_CH1_SHIFT
        true,          /* IsShift */
        PWM_MAX_VALUE, /* CurrentLevel */
#else
        false, /* IsShift */
        0,     /* CurrentLevel */
#endif
#ifdef PWM_FADE_ENABLE
        0, /* GoalLevel */
        0, /* StepLevel */
        0, /* FadeCount */
#endif

        Timer1Ch1_Init,     /* TimerInit */
        Timer1Ch1_SetLevel, /* TimerSetLevel */
    },
#endif

#ifdef TIMER1_CH2_ENABLE
    {
        TIMER1_CH2, /* TimerId */

#ifdef TIMER1_CH2_SHIFT
        true,          /* IsShift */
        PWM_MAX_VALUE, /* CurrentLevel */
#else
        false, /* IsShift */
        0,     /* CurrentLevel */
#endif
#ifdef PWM_FADE_ENABLE
        0, /* GoalLevel */
        0, /* StepLevel */
        0, /* FadeCount */
#endif

        Timer1Ch2_Init,     /* TimerInit */
        Timer1Ch2_SetLevel, /* TimerSetLevel */
    },
#endif

#ifdef TIMER1_CH3_ENABLE
    {
        TIMER1_CH3, /* TimerId */

#ifdef TIMER1_CH3_SHIFT
        true,          /* IsShift */
        PWM_MAX_VALUE, /* CurrentLevel */
#else
        false, /* IsShift */
        0,     /* CurrentLevel */
#endif
#ifdef PWM_FADE_ENABLE
        0, /* GoalLevel */
        0, /* StepLevel */
        0, /* FadeCount */
#endif

        Timer1Ch3_Init,     /* TimerInit */
        Timer1Ch3_SetLevel, /* TimerSetLevel */
    },
#endif

#ifdef TIMER2_CH1_ENABLE
    {
        TIMER2_CH1, /* TimerId */

#ifdef TIMER2_CH1_SHIFT
        true,          /* IsShift */
        PWM_MAX_VALUE, /* CurrentLevel */
#else
        false, /* IsShift */
        0,     /* CurrentLevel */
#endif
#ifdef PWM_FADE_ENABLE
        0, /* GoalLevel */
        0, /* StepLevel */
        0, /* FadeCount */
#endif

        Timer2Ch1_Init,     /* TimerInit */
        Timer2Ch1_SetLevel, /* TimerSetLevel */
    },
#endif

#ifdef TIMER2_CH2_ENABLE
    {
        TIMER2_CH2, /* TimerId */

#ifdef TIMER2_CH2_SHIFT
        true,          /* IsShift */
        PWM_MAX_VALUE, /* CurrentLevel */
#else
        false, /* IsShift */
        0,     /* CurrentLevel */
#endif
#ifdef PWM_FADE_ENABLE
        0, /* GoalLevel */
        0, /* StepLevel */
        0, /* FadeCount */
#endif

        Timer2Ch2_Init,     /* TimerInit */
        Timer2Ch2_SetLevel, /* TimerSetLevel */
    },
#endif

#ifdef TIMER3_CH1_ENABLE
    {
        TIMER3_CH1, /* TimerId */

#ifdef TIMER3_CH1_SHIFT
        true,          /* IsShift */
        PWM_MAX_VALUE, /* CurrentLevel */
#else
        false, /* IsShift */
        0,     /* CurrentLevel */
#endif
#ifdef PWM_FADE_ENABLE
        0, /* GoalLevel */
        0, /* StepLevel */
        0, /* FadeCount */
#endif

        Timer3Ch1_Init,     /* TimerInit */
        Timer3Ch1_SetLevel, /* TimerSetLevel */
    },
#endif

#ifdef TIMER3_CH2_ENABLE
    {
        TIMER3_CH2, /* TimerId */

#ifdef TIMER3_CH2_SHIFT
        true,          /* IsShift */
        PWM_MAX_VALUE, /* CurrentLevel */
#else
        false, /* IsShift */
        0,     /* CurrentLevel */
#endif
#ifdef PWM_FADE_ENABLE
        0, /* GoalLevel */
        0, /* StepLevel */
        0, /* FadeCount */
#endif

        Timer3Ch2_Init,     /* TimerInit */
        Timer3Ch2_SetLevel, /* TimerSetLevel */
    },
#endif

#ifdef TIMER5_CH1_ENABLE
    {
        TIMER5_CH1, /* TimerId */

#ifdef TIMER5_CH1_SHIFT
        true,          /* IsShift */
        PWM_MAX_VALUE, /* CurrentLevel */
#else
        false, /* IsShift */
        0,     /* CurrentLevel */
#endif
#ifdef PWM_FADE_ENABLE
        0, /* GoalLevel */
        0, /* StepLevel */
        0, /* FadeCount */
#endif

        Timer5Ch1_Init,     /* TimerInit */
        Timer5Ch1_SetLevel, /* TimerSetLevel */
    },
#endif

#ifdef TIMER5_CH2_ENABLE
    {
        TIMER5_CH2, /* TimerId */

#ifdef TIMER5_CH2_SHIFT
        true,          /* IsShift */
        PWM_MAX_VALUE, /* CurrentLevel */
#else
        false, /* IsShift */
        0,     /* CurrentLevel */
#endif
#ifdef PWM_FADE_ENABLE
        0, /* GoalLevel */
        0, /* StepLevel */
        0, /* FadeCount */
#endif

        Timer5Ch2_Init,     /* TimerInit */
        Timer5Ch2_SetLevel, /* TimerSetLevel */
    },
#endif

    {
        MAX_TIMER_NUM, /* TimerId */

        false, /* IsShift */

        0, /* CurrentLevel */
#ifdef PWM_FADE_ENABLE
        0, /* GoalLevel */
        0, /* StepLevel */
        0, /* FadeCount */
#endif

        NULL, /* TimerInit */
        NULL, /* TimerSetLevel */
    },
};

void Pwm_Init(void)
{
  Pwm_t *pTimer = PwmTable;

  Pwm_InitTimer();
  while (pTimer->TimerId < MAX_TIMER_NUM)
  {
    if (pTimer->TimerInit)
    {
      pTimer->TimerInit();

      Timer_SetLevel(pTimer, ConvertToLevel(pTimer->IsShift, 0)); /* init PWM duty cycle to 0% */
    }

    pTimer++;
  }
  Pwm_EnableTimer();
}

OsErr_t Pwm_SetLevel(PwmTimerId_t timerId, float dutyCycle)
{
  Pwm_t *pTimer = FindTimerById(timerId);

  if (pTimer != NULL)
  {
#ifdef PWM_FADE_ENABLE
    Timer_Calculate(pTimer, dutyCycle);
#else
    Timer_SetLevel(pTimer, ConvertToLevel(pTimer->IsShift, dutyCycle));
#endif

    return OS_ERR_OK;
  }
  else
  {
    return OS_ERR_RANGE;
  }
}

OsErr_t Pwm_GetLevel(PwmTimerId_t timerId, float *level)
{
  Pwm_t *pTimer = FindTimerById(timerId);

  if (pTimer != NULL)
  {
    *level = ConvertToDutyCycle(pTimer->IsShift, pTimer->CurrentLevel);

    return OS_ERR_OK;
  }

  return OS_ERR_RANGE;
}

void Pwm_SetDutyCycle(u8 channel, float dutyCycle)
{
  Pwm_SetLevel((PwmTimerId_t)channel, dutyCycle);
}

float Pwm_GetDutyCycle(u8 channel)
{
  float dutyCycle;
  Pwm_GetLevel((PwmTimerId_t)channel, &dutyCycle);
  return dutyCycle;
}

#ifdef PWM_FADE_ENABLE
void Pwm_Process(void)
{
  Pwm_t *pTimer = PwmTable;

  while (pTimer->TimerId < MAX_TIMER_NUM)
  {
    Timer_Process(pTimer);

    pTimer++;
  }
}
static void Timer_Process(Pwm_t *pTimer)
{
  if (pTimer->FadeCount != 0)
  {
    pTimer->FadeCount--;
    if (pTimer->FadeCount != 0)
    {
      if (pTimer->GoalLevel > pTimer->CurrentLevel)
      {
        Timer_SetLevel(pTimer, pTimer->CurrentLevel + pTimer->StepLevel);
      }
      else
      {
        Timer_SetLevel(pTimer, pTimer->CurrentLevel - pTimer->StepLevel);
      }
    }
    else
    {
      Timer_SetLevel(pTimer, pTimer->GoalLevel);
    }
  }
}

static void Timer_Calculate(Pwm_t *pTimer, float duty_cycle)
{
  u16 changeLevel;

  pTimer->FadeCount = 0; /* stop former fade process */

  pTimer->GoalLevel = ConvertToLevel(pTimer->IsShift, duty_cycle);

  changeLevel = abs(pTimer->GoalLevel - pTimer->CurrentLevel);
  if (changeLevel >= PWM_FADE_SUM)
  {
    pTimer->FadeCount = PWM_FADE_SUM;
    pTimer->StepLevel = changeLevel / PWM_FADE_SUM;
  }
  else if (changeLevel > 0)
  {
    pTimer->FadeCount = changeLevel;
    pTimer->StepLevel = 1;
  }
}
#endif

static u16 ConvertToLevel(bool isShift, float duty_cycle)
{
  if (isShift)
  {
    return (u16)((float)PWM_MAX_VALUE * (1 - duty_cycle));
  }
  else
  {
    return (u16)((float)PWM_MAX_VALUE * duty_cycle);
  }
}

static float ConvertToDutyCycle(bool isShift, u16 level)
{
  if (isShift)
  {
    return 1 - (float)level / (float)PWM_MAX_VALUE;
  }
  else
  {
    return (float)level / (float)PWM_MAX_VALUE;
  }
}

static void Timer_SetLevel(Pwm_t *pTimer, u16 level)
{
  if (level != pTimer->CurrentLevel)
  {
    if (pTimer->TimerSetLevel)
    {
      pTimer->TimerSetLevel(level);
    }
    pTimer->CurrentLevel = level;
  }
}
static Pwm_t *FindTimerById(PwmTimerId_t timerId)
{
  Pwm_t *pTimer = PwmTable;

  while (pTimer->TimerId < MAX_TIMER_NUM)
  {
    if (pTimer->TimerId == timerId)
    {
      return pTimer;
    }

    pTimer++;
  }

  return NULL;
}

static void Pwm_InitTimer(void)
{
#if defined TIMER1_CH1_ENABLE || defined TIMER1_CH2_ENABLE || defined TIMER1_CH3_ENABLE
  Timer1_Init();
#endif

#if defined TIMER2_CH1_ENABLE || defined TIMER2_CH2_ENABLE
  Timer2_Init();
#endif

#if defined TIMER3_CH1_ENABLE || defined TIMER3_CH2_ENABLE
  Timer3_Init();
#endif

#if defined TIMER5_CH1_ENABLE || defined TIMER5_CH2_ENABLE
  Timer5_Init();
#endif
}

static void Pwm_EnableTimer(void)
{
#if defined TIMER1_CH1_ENABLE || defined TIMER1_CH2_ENABLE || defined TIMER1_CH3_ENABLE
  Timer1_Enable();
#endif

#if defined TIMER2_CH1_ENABLE || defined TIMER2_CH2_ENABLE
  Timer2_Enable();
#endif

#if defined TIMER3_CH1_ENABLE || defined TIMER3_CH2_ENABLE
  Timer3_Enable();
#endif

#if defined TIMER5_CH1_ENABLE || defined TIMER5_CH2_ENABLE
  Timer5_Enable();
#endif
}