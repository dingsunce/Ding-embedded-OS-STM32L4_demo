/*!*****************************************************************************
 * file		d_list.h (another implementation of single List)
 * $Author: sunce.ding
 *******************************************************************************/

#ifndef d_list_H_
#define d_list_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "define.h"

  typedef void **d_list_t;

#define DLIST(name)                                                                                \
  static void    *name##List = NULL;                                                               \
  static d_list_t name = (d_list_t) & name##List

#define DLIST_STRUCT(name)                                                                         \
  void    *name##List;                                                                             \
  d_list_t name

#define DLIST_STRUCT_INIT(struct_ptr, name)                                                        \
  (struct_ptr)->name##List = NULL;                                                                 \
  (struct_ptr)->name = (d_list_t) & ((struct_ptr)->name##List);                                    \
  DList_Init((struct_ptr)->name)

#define DLIST_HEADER void *next

  void  DList_Init(d_list_t list);
  void *DList_Head(d_list_t list);
  void *DList_Tail(d_list_t list);
  void *DList_Pop(d_list_t list);
  void  DList_Push(d_list_t list, void *item);
  void *DList_Chop(d_list_t list);
  void  DList_Add(d_list_t list, void *item);
  void  DList_Remove(d_list_t list, void *item);
  bool  DList_IsEmpty(d_list_t list);
  u32   DList_Length(d_list_t list);
  void  DList_Copy(d_list_t dest, d_list_t src);
  void  DList_Insert(d_list_t list, void *prevItem, void *newItem);
  void *DList_ItemNext(void *item);

#ifdef __cplusplus
}
#endif

#endif
