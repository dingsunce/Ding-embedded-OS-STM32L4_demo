#include "stm32ll.h"
#include "unique_id_func.h"

#define UNIQUE_ID_LENGTH 12

u8 *UniqueId_Get(u8 *pLength)
{
  *pLength = UNIQUE_ID_LENGTH;
  return (u8 *)UID_BASE_ADDRESS;
}

u8 UniqueId_GetLength(void)
{
  return UNIQUE_ID_LENGTH;
}