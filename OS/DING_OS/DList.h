/*!*****************************************************************************
 * file		DList.h (Double List)
 * $Author: sunce.ding
 *******************************************************************************/

#ifndef DLIST_H_
#define DLIST_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "d_os.h"
#include "define.h"

  typedef struct DList
  {
    struct DList *next;
    struct DList *prev;
  } DList_t;

  void DList_Init(DList_t *list);

  void     DList_Add(DList_t *listHeader, DList_t *entry);
  void     DList_Push(DList_t *listHeader, DList_t *entry);
  DList_t *DList_Pop(DList_t *listHeader);
  bool     DList_IsEmpty(DList_t *listHeader);
  u16      DList_Len(const DList_t *listHeader);

  void DList_InsertBefore(DList_t *list, DList_t *entry);
  void DList_InsertAfter(DList_t *list, DList_t *entry);
  void DList_Remove(DList_t *entry);

  void DList_Splice(DList_t *list, DList_t *append);

#define DLIST_INIT(name)                                                                                     \
  {                                                                                                          \
    &(name), &(name)                                                                                         \
  }

#define DLIST(name) static DList_t name = DLIST_INIT(name)

#define PUBLIC_DLIST(name) DList_t name = DLIST_INIT(name)

#ifdef __cplusplus
}
#endif

#endif
