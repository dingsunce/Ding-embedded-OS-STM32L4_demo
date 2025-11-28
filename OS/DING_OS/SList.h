/*!*****************************************************************************
 * file		SList.h (Single List)
 * $Author: sunce.ding
 *******************************************************************************/

#ifndef SLIST_H_
#define SLIST_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "d_os.h"
#include "define.h"

  typedef struct SList
  {
    struct SList *next;
  } SList_t;

  void     SList_Init(SList_t *list);
  void     SList_Add(SList_t *listHeader, SList_t *entry);
  void     SList_Push(SList_t *listHeader, SList_t *entry);
  SList_t *SList_Pop(SList_t *listHeader);
  SList_t *SList_Chop(SList_t *listHeader);
  bool     SList_IsEmpty(SList_t *listHeader);
  u16      SList_Len(const SList_t *listHeader);
  void     SList_Remove(SList_t *listHeader, SList_t *entry);

#define SLIST_INIT(name)                                                                                     \
  {                                                                                                          \
    NULL                                                                                                     \
  }

#define SLIST(name) static SList_t name = SLIST_INIT(name)

#define PUBLIC_SLIST(name) SList_t name = DLIST_INIT(name)

#ifdef __cplusplus
}
#endif

#endif
