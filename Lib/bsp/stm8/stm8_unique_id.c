#include "stm8l15x_conf.h"

#define UNIQUE_ID_ADD    0x4926
#define UNIQUE_ID_LENGTH 12

u8 *UniqueId_Get(u8 *pLength)
{
  *pLength = UNIQUE_ID_LENGTH;
  return (u8 *)UNIQUE_ID_ADD;
}

u8 UniqueId_GetLength(void)
{
  return UNIQUE_ID_LENGTH;
}