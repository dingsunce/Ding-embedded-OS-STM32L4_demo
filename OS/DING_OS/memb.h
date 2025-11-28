/*!*****************************************************************************
 * file		memb.c
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef MEMB_H_
#define MEMB_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "SList.h"
#include "define.h"
#include "osal.h"

  typedef struct Memblock
  {
    unsigned index : 15;
    unsigned InUse : 1;
    SList_t  list;
  } Memblock_t;

  typedef struct Memb
  {
    u16         Size;
    u16         Num;
    Memblock_t *block;
    SList_t     FreeList;
    void       *Mem;
    os_sem_t   *Sem;
  } Memb_t;

#define MEMB(name, structure, num)                                                                 \
  static Memblock_t name##Block[num];                                                              \
  static structure  name##Mem[num];                                                                \
  static Memb_t     name = {sizeof(structure), num, name##Block, {NULL}, (void *)name##Mem}

#define MEMB_STRUCT(name, structure, num)                                                          \
  Memblock_t name##Block[num];                                                                     \
  structure  name##Mem[num];                                                                       \
  Memb_t     name

#define MEMB_STRUCT_INIT(struct_ptr, name, structure, num)                                         \
  (struct_ptr)->name.Size = sizeof(structure);                                                     \
  (struct_ptr)->name.Num = num;                                                                    \
  (struct_ptr)->name.block = (struct_ptr)->name##Block;                                            \
  (struct_ptr)->name.Mem = (void *)(struct_ptr)->name##Mem;                                        \
  Memb_Init(&(struct_ptr)->name)

  void  Memb_Init(Memb_t *m);
  void *Memb_Alloc(Memb_t *m);
  s8    Memb_Free(Memb_t *m, void *ptr);
  bool  Memb_InMem(Memb_t *m, void *ptr);
  u16   Memb_NumFree(Memb_t *m);
  bool  Memb_HasFreeNum(Memb_t *m);

#ifdef __cplusplus
}
#endif

#endif