/*!*****************************************************************************
 * file		d_memb.c
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef D_MEMB_H_
#define D_MEMB_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "SList.h"
#include "define.h"
#include "osal.h"

  typedef struct DMemblock
  {
    unsigned index : 15;
    unsigned InUse : 1;
    SList_t  list;
  } DMemblock_t;

  typedef struct DMemb
  {
    u16          Size;
    u16          Num;
    DMemblock_t *block;
    SList_t      FreeList;
    void        *Mem;
    os_sem_t    *Sem;
  } DMemb_t;

#define DMEMB(name, structure, num)                                                                \
  static DMemblock_t name##Block[num];                                                             \
  static structure   name##Mem[num];                                                               \
  static DMemb_t     name = {sizeof(structure), num, name##Block, {NULL}, (void *)name##Mem}

#define DMEMB_STRUCT(name, structure, num)                                                         \
  DMemblock_t name##Block[num];                                                                    \
  structure   name##Mem[num];                                                                      \
  DMemb_t     name

#define DMEMB_STRUCT_INIT(struct_ptr, name, structure, num)                                        \
  (struct_ptr)->name.Size = sizeof(structure);                                                     \
  (struct_ptr)->name.Num = num;                                                                    \
  (struct_ptr)->name.block = (struct_ptr)->name##Block;                                            \
  (struct_ptr)->name.Mem = (void *)(struct_ptr)->name##Mem;                                        \
  DMemb_Init(&(struct_ptr)->name)

  void  DMemb_Init(DMemb_t *m);
  void *DMemb_Alloc(DMemb_t *m);
  s8    DMemb_Free(DMemb_t *m, void *ptr);
  bool  DMemb_InMem(DMemb_t *m, void *ptr);
  u16   DMemb_NumFree(DMemb_t *m);
  bool  DMemb_HasFreeNum(DMemb_t *m);

#ifdef __cplusplus
}
#endif

#endif