#ifndef _STM8_PWM_IMPL_H
#define _STM8_PWM_IMPL_H

#include "define.h"

#ifndef CLOCK_FREQUENCY
#define CLOCK_FREQUENCY (16E6) /* 16M */
#endif

#define PWM_AUTO_RELOAD_VALUE ((u16)(CLOCK_FREQUENCY / PWM_FREQUENCY - 1))
#define PWM_MAX_VALUE         ((u16)(CLOCK_FREQUENCY / PWM_FREQUENCY))

extern void Timer1_Init(void);
extern void Timer1_Enable(void);
extern void Timer1Ch1_Init(void);
extern void Timer1Ch2_Init(void);
extern void Timer1Ch3_Init(void);
extern void Timer1Ch1_SetLevel(u16 level);
extern void Timer1Ch2_SetLevel(u16 level);
extern void Timer1Ch3_SetLevel(u16 level);

extern void Timer2_Init(void);
extern void Timer2_Enable(void);
extern void Timer2Ch1_Init(void);
extern void Timer2Ch2_Init(void);
extern void Timer2Ch1_SetLevel(u16 level);
extern void Timer2Ch2_SetLevel(u16 level);

extern void Timer3_Init(void);
extern void Timer3_Enable(void);
extern void Timer3Ch1_Init(void);
extern void Timer3Ch2_Init(void);
extern void Timer3Ch1_SetLevel(u16 level);
extern void Timer3Ch2_SetLevel(u16 level);

extern void Timer5_Init(void);
extern void Timer5_Enable(void);
extern void Timer5Ch1_Init(void);
extern void Timer5Ch2_Init(void);
extern void Timer5Ch1_SetLevel(u16 level);
extern void Timer5Ch2_SetLevel(u16 level);

#endif
