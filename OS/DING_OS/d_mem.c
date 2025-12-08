/*!*****************************************************************************
 * file		d_mem.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "d_mem.h"
#include "osal.h"

#if (MEM_DEBUG == 1)
#define DMEM_ALLOC 0
#define DMEM_FREE  'F'
static u16 MemAlo = 0;
static u16 MemMaxAlo = 0;
static u16 MemFree = MEM_HEAP;
static u16 MemAloFailed = 0;
#endif

// add a null block at the end
static u8 TheHeap[MEM_HEAP + MEM_HEADER_SIZE]
#if defined PLATFORM_STM8 && defined _IAR_
    @0x180; // Avoid IAR alloc in the last position of RAM, which result in damaging stack.
#else
    ;
#endif

static MemHdr_t *FirstHeader;
static MemHdr_t *FreeHeader; // pointer to the lowest free block
static MemHdr_t *EndHeader;

static os_sem_t *MemSem;
//-----------------------------------------------------------------------------------------------------------
#if (MEM_DEBUG == 1)
static void DMem_DebugInit(void)
{
  memset(TheHeap, DMEM_FREE, sizeof(TheHeap));
  MemAlo = 0;
  MemMaxAlo = 0;
  MemAloFailed = 0;
  MemFree = MEM_HEAP;
}
//-----------------------------------------------------------------------------------------------------------
static void DMem_DebugAllocate(MemHdr_t *hdr)
{
  MemAlo += hdr->Len;
  if (MemMaxAlo < MemAlo)
    MemMaxAlo = MemAlo;

  MemFree -= hdr->Len;
}
//-----------------------------------------------------------------------------------------------------------
static void DMem_DebugAllocateFailed(void)
{
  MemAloFailed++;
}
//-----------------------------------------------------------------------------------------------------------
static void DMem_DebugFree(MemHdr_t *hdr)
{
  MemAlo -= hdr->Len;
  MemFree += hdr->Len;
  // set data in block as DMEM_FREE
  memset((u8 *)(hdr + 1), DMEM_FREE, (hdr->Len - MEM_HEADER_SIZE));
}
//-----------------------------------------------------------------------------------------------------------
static void DMem_DebugFreeHeader(MemHdr_t *hdr)
{
  // set header of block as DMEM_FREE
  memset((u8 *)hdr, DMEM_FREE, MEM_HEADER_SIZE);
}
//-----------------------------------------------------------------------------------------------------------
u16 DMem_GetAllocSize(void)
{
  return MemAlo;
}
//-----------------------------------------------------------------------------------------------------------
u16 DMem_GetFreeSize(void)
{
  return MemFree;
}
#endif
//-----------------------------------------------------------------------------------------------------------
void DMem_Init(void)
{
#if (MEM_DEBUG == 1)
  DMem_DebugInit();
#endif

  FirstHeader = (MemHdr_t *)TheHeap;
  FirstHeader->Len = MEM_HEAP;
  FirstHeader->InUse = false;

  EndHeader = (MemHdr_t *)(&TheHeap[MEM_HEAP]);
  EndHeader->Len = 0;
  EndHeader->InUse = true;

  FirstHeader->PreLen = 0;
  EndHeader->PreLen = MEM_HEAP;

  FreeHeader = FirstHeader;

  MemSem = os_sem_create(1);
}
//-----------------------------------------------------------------------------------------------------------
static void DMem_SplitBlock(MemHdr_t *hdr, u16 blockSize)
{
  u16 redundant = (u16)(hdr->Len - blockSize);
  if (redundant >= MEM_MIN_BLOCK_SIZE)
  {
    MemHdr_t *split = (MemHdr_t *)((u8 *)hdr + blockSize);

    MemHdr_t *next = (MemHdr_t *)((u8 *)hdr + hdr->Len);

    split->Len = redundant;
    split->InUse = false;

    hdr->Len = blockSize;
    hdr->InUse = true;

    split->PreLen = hdr->Len;
    next->PreLen = split->Len;
  }
  else
  {
    hdr->InUse = true;
  }
}
//-----------------------------------------------------------------------------------------------------------
static void UpdateFreeHeader(MemHdr_t *hdr)
{
  if (FreeHeader == hdr)
  {
    // Find next free block after hdr and update the lowest free pointer
    while (FreeHeader->InUse && FreeHeader != EndHeader)
      FreeHeader = (MemHdr_t *)((u8 *)FreeHeader + FreeHeader->Len);
  }
}
//-----------------------------------------------------------------------------------------------------------
void *DMem_Malloc(u16 size)
{
#if MEM_ALIGMENT_4BYTES
  size = ((size + 3) / 4) * 4;
#endif

  MemHdr_t *hdr = FreeHeader;
  u16       blockSize = size + MEM_HEADER_SIZE;

  if (size == 0)
    return NULL;

  os_sem_wait(MemSem, OS_WAIT_FOREVER);

  while (hdr != EndHeader)
  {
    if (hdr->InUse == false && hdr->Len >= blockSize)
      break;

    hdr = (MemHdr_t *)((u8 *)hdr + hdr->Len);
  }

  if (hdr == EndHeader)
  {
#if (MEM_DEBUG == 1)
    DMem_DebugAllocateFailed();
#endif

    os_sem_signal(MemSem);

    return NULL;
  }

  DMem_SplitBlock(hdr, blockSize);

  UpdateFreeHeader(hdr);
  memset((u8 *)(hdr + 1), 0, (hdr->Len - MEM_HEADER_SIZE));

#if (MEM_DEBUG == 1)
  DMem_DebugAllocate(hdr);
#endif

  hdr++;

  os_sem_signal(MemSem);

  return (void *)hdr;
}
//-----------------------------------------------------------------------------------------------------------
static void DMem_CoalesceBlock(MemHdr_t *hdr)
{
  MemHdr_t *curHdr = hdr;
  MemHdr_t *preHdr = (MemHdr_t *)((u8 *)hdr - hdr->PreLen);
  MemHdr_t *nextHdr = (MemHdr_t *)((u8 *)hdr + hdr->Len);
  if (nextHdr != hdr && nextHdr->InUse == false)
  {
    curHdr->Len = curHdr->Len + nextHdr->Len;
#if (MEM_DEBUG == 1)
    DMem_DebugFreeHeader(nextHdr);
#endif
    nextHdr = (MemHdr_t *)((u8 *)curHdr + curHdr->Len);
    nextHdr->PreLen = curHdr->Len;
  }

  if (preHdr != hdr && preHdr->InUse == false)
  {
    preHdr->Len = preHdr->Len + curHdr->Len;
#if (MEM_DEBUG == 1)
    DMem_DebugFreeHeader(curHdr);
#endif
    nextHdr->PreLen = preHdr->Len;
  }
}
//-----------------------------------------------------------------------------------------------------------
void DMem_Free(void *ptr)
{
  if ((u8 *)ptr < (u8 *)FirstHeader || (u8 *)ptr >= (u8 *)EndHeader)
    return;

  os_sem_wait(MemSem, OS_WAIT_FOREVER);

  MemHdr_t *hdr = (MemHdr_t *)ptr - 1;
  hdr->InUse = false;

  if (FreeHeader > hdr)
    FreeHeader = hdr;

#if (MEM_DEBUG == 1)
  DMem_DebugFree(hdr);
#endif

  DMem_CoalesceBlock(hdr);

  os_sem_signal(MemSem);
}
