/*!*****************************************************************************
 * file		memb.c(Implementation of memory block)
 * $Author: sunce.ding
 *******************************************************************************/
#include "memb.h"

//-----------------------------------------------------------------------------------------------------------
void Memb_Init(Memb_t *m)
{
  SList_Init(&m->FreeList);
  memset(m->Mem, 0, m->Size * m->Num);
  for (u16 i = 0; i < m->Num; i++)
  {
    Memblock_t *block = m->block + i;
    SList_Init(&block->list);
    block->index = i;
    block->InUse = false;
    SList_Push(&m->FreeList, &block->list);
  }

  m->Sem = os_sem_create(1);
}
//-----------------------------------------------------------------------------------------------------------
void *Memb_Alloc(Memb_t *m)
{
  os_sem_wait(m->Sem, OS_WAIT_FOREVER);

  SList_t *myList = SList_Pop(&m->FreeList);
  if (myList != NULL)
  {
    Memblock_t *block = ContainerOf(myList, Memblock_t, list);
    block->InUse = true;

    os_sem_signal(m->Sem);

    return (void *)((u8 *)m->Mem + (block->index * m->Size));
  }

  os_sem_signal(m->Sem);

  return NULL;
}
//-----------------------------------------------------------------------------------------------------------
s8 Memb_Free(Memb_t *m, void *ptr)
{
  if (ptr < m->Mem || ptr > (void *)((u8 *)m->Mem + (m->Num * m->Size)))
    return -1;

  u16 index = (u16)(((uintptr_t)ptr - (uintptr_t)m->Mem) / m->Size);
  u16 offset = (u16)(((uintptr_t)ptr - (uintptr_t)m->Mem) % m->Size);
  if (offset != 0)
    return -1;

  os_sem_wait(m->Sem, OS_WAIT_FOREVER);

  Memblock_t *block = m->block + index;
  if (block->InUse)
  {
    block->InUse = false;
    SList_Push(&m->FreeList, &block->list);

    os_sem_signal(m->Sem);

    return 0;
  }

  os_sem_signal(m->Sem);

  return -1;
}
//-----------------------------------------------------------------------------------------------------------
bool Memb_InMem(Memb_t *m, void *ptr)
{
  if (ptr < m->Mem || ptr > (void *)((u8 *)m->Mem + (m->Num * m->Size)))
    return false;

  u16 index = (u16)(((uintptr_t)ptr - (uintptr_t)m->Mem) / m->Size);
  u16 offset = (u16)(((uintptr_t)ptr - (uintptr_t)m->Mem) % m->Size);
  if (offset != 0)
    return false;

  Memblock_t *block = m->block + index;

  return block->InUse;
}
//-----------------------------------------------------------------------------------------------------------
u16 Memb_NumFree(Memb_t *m)
{
  u16      numFree = 0;
  SList_t *tmp = m->FreeList.next;
  while (tmp != NULL)
  {
    tmp = tmp->next;
    ++numFree;
  }

  return numFree;
}
//-----------------------------------------------------------------------------------------------------------
bool Memb_HasFreeNum(Memb_t *m)
{
  SList_t *tmp = m->FreeList.next;
  while (tmp != NULL)
  {
    tmp = tmp->next;
    return true;
  }

  return false;
}
