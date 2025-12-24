/*!*****************************************************************************
 * file		d_mem.h
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _d_mem_H
#define _d_mem_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "d_os.h"
#include "define.h"

  typedef struct MemHdr
  {
    unsigned Len : 15;
    unsigned InUse : 1;
    unsigned PreLen : 16;
  } MemHdr_t;

#define MEM_HEADER_SIZE    sizeof(MemHdr_t)
#define MEM_MIN_BLOCK_SIZE (MEM_HEADER_SIZE * 2)

  void  DMem_Init(void);
  void  DMem_Exit(void);
  void *DMem_Malloc(u16 size);
  void  DMem_Free(void *ptr);

#if (MEM_DEBUG == 1)
  u16 DMem_GetAllocSize(void);
  u16 DMem_GetFreeSize(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
