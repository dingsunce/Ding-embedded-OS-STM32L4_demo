#include "debug_func.h"
#include "io_func.h"

#ifdef ENABLE_DEBUG
#define DEBUG_IO_SUM 1

Io_t Debug_Ios[DEBUG_IO_SUM] = {{IO_PORT_B, IO_PIN0}};

void Debug_Init(void)
{
  u8 i;

  for (i = 0; i < DEBUG_IO_SUM; i++)
  {
    Io_Init(Debug_Ios[i].Port, Debug_Ios[i].Pin, IO_OUTPUT_PUSH_PULL_LOW);
  }
}

void Debug_ToggleIo(u8 id)
{
  if (id < DEBUG_IO_SUM)
  {
    Io_ToggleBits(Debug_Ios[id].Port, Debug_Ios[id].Pin);
  }
}

void Debug_SetIo(u8 id)
{
  if (id < DEBUG_IO_SUM)
  {
    Io_SetBits(Debug_Ios[id].Port, Debug_Ios[id].Pin);
  }
}

void Debug_ResetIo(u8 id)
{
  if (id < DEBUG_IO_SUM)
  {
    Io_ResetBits(Debug_Ios[id].Port, Debug_Ios[id].Pin);
  }
}

#endif