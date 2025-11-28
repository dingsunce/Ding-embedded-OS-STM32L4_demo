/*!*****************************************************************************
 * file		s_list.h (another implementation of single List)
 * $Author: sunce.ding
 *******************************************************************************/

#ifndef s_list_H_
#define s_list_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "define.h"

  typedef void **s_list_t;

#define LIST(name)                                                                                 \
  static void    *name##List = NULL;                                                               \
  static s_list_t name = (s_list_t) & name##List

#define LIST_STRUCT(name)                                                                          \
  void    *name##List;                                                                             \
  s_list_t name

#define LIST_STRUCT_INIT(struct_ptr, name)                                                         \
  (struct_ptr)->name##List = NULL;                                                                 \
  (struct_ptr)->name = (s_list_t) & ((struct_ptr)->name##List);                                    \
  List_Init((struct_ptr)->name)

#define LIST_HEADER void *next

  void  List_Init(s_list_t list);
  void *List_Head(s_list_t list);
  void *List_Tail(s_list_t list);
  void *List_Pop(s_list_t list);
  void  List_Push(s_list_t list, void *item);
  void *List_Chop(s_list_t list);
  void  List_Add(s_list_t list, void *item);
  void  List_Remove(s_list_t list, void *item);
  bool  List_IsEmpty(s_list_t list);
  u32   List_Length(s_list_t list);
  void  List_Copy(s_list_t dest, s_list_t src);
  void  List_Insert(s_list_t list, void *prevItem, void *newItem);
  void *List_ItemNext(void *item);

#ifdef __cplusplus
}
#endif

#endif
