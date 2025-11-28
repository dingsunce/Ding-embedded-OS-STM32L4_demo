#ifndef _IO_FUNC_H
#define _IO_FUNC_H

#include "define.h"

#if defined PLATFORM_STM8

typedef u8 IoBits_t;
typedef enum
{
  IO_PORT_A,
  IO_PORT_B,
  IO_PORT_C,
  IO_PORT_D,
  IO_PORT_E,
  IO_PORT_F,
} IoPort_t;

#elif defined PLATFORM_STM32

typedef u16 IoBits_t;
typedef enum
{
  IO_PORT_A,
  IO_PORT_B,
  IO_PORT_C,
  IO_PORT_D,
#ifdef GPIOE
  IO_PORT_E,
#endif
#ifdef GPIOF
  IO_PORT_F,
#endif
} IoPort_t;

#else

typedef u16 IoBits_t;
typedef enum
{
  IO_PORT_A,
  IO_PORT_B,
  IO_PORT_C,
  IO_PORT_D,
  IO_PORT_E,
  IO_PORT_F,
} IoPort_t;

#endif

#ifdef PLATFORM_STM8
typedef enum
{
  IO_PIN0 = 0x01,
  IO_PIN1 = 0x02,
  IO_PIN2 = 0x04,
  IO_PIN3 = 0x08,
  IO_PIN4 = 0x10,
  IO_PIN5 = 0x20,
  IO_PIN6 = 0x40,
  IO_PIN7 = 0x80,
} IoPin_t;
#else
typedef enum
{
  IO_PIN0 = 0x0001,
  IO_PIN1 = 0x0002,
  IO_PIN2 = 0x0004,
  IO_PIN3 = 0x0008,
  IO_PIN4 = 0x0010,
  IO_PIN5 = 0x0020,
  IO_PIN6 = 0x0040,
  IO_PIN7 = 0x0080,
  IO_PIN8 = 0x0100,
  IO_PIN9 = 0x0200,
  IO_PIN10 = 0x0400,
  IO_PIN11 = 0x0800,
  IO_PIN12 = 0x1000,
  IO_PIN13 = 0x2000,
  IO_PIN14 = 0x4000,
  IO_PIN15 = 0x8000,
} IoPin_t;
#endif

typedef enum
{
  IO_LOW,
  IO_HIGH,
} IoLevel_t;

typedef struct
{
  IoPort_t Port;
  IoPin_t  Pin;
} Io_t;

typedef enum
{
  IO_INPUT_FLOATING,        // Floating input.
  IO_INPUT_PULL_UP,         // Pull up input.
  IO_OUTPUT_OPEN_DRAIN,     // Open drain output.
  IO_OUTPUT_PUSH_PULL_LOW,  // Push output low.
  IO_OUTPUT_PUSH_PULL_HIGH, // Push output high.
} IoMode_t;

extern void     Io_Init(IoPort_t port, IoBits_t pins, IoMode_t mode);
extern IoBits_t Io_GetBits(IoPort_t port, IoBits_t pins);
extern void     Io_WriteBits(IoPort_t port, IoBits_t pins, IoLevel_t level);
extern void     Io_SetBits(IoPort_t port, IoBits_t pins);
extern void     Io_ResetBits(IoPort_t port, IoBits_t pins);
extern void     Io_ToggleBits(IoPort_t port, IoBits_t pin);

#endif