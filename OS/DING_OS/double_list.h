/*!*****************************************************************************
 * file		double_list.h (Double List)
 * $Author: sunce.ding
 *******************************************************************************/

#ifndef DOUBLE_LIST_H_
#define DOUBLE_LIST_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "d_os.h"
#include "define.h"

  typedef struct Db_List
  {
    struct Db_List *next;
    struct Db_List *prev;
  } Db_List_t;

  void       DbList_Init(Db_List_t *list);
  void       DbList_Add(Db_List_t *listHeader, Db_List_t *entry);
  void       DbList_Push(Db_List_t *listHeader, Db_List_t *entry);
  Db_List_t *DbList_Pop(Db_List_t *listHeader);
  bool       DbList_IsEmpty(Db_List_t *listHeader);
  u16        DbList_Len(const Db_List_t *listHeader);
  void       DbList_InsertBefore(Db_List_t *list, Db_List_t *entry);
  void       DbList_InsertAfter(Db_List_t *list, Db_List_t *entry);
  void       DbList_Remove(Db_List_t *entry);
  void       DbList_Splice(Db_List_t *list, Db_List_t *append);

#define DB_LIST_INIT(name) {&(name), &(name)}

#define DB_LIST(name) static Db_List_t name = DB_LIST_INIT(name)

#define PUBLIC_DB_LIST(name) Db_List_t name = DB_LIST_INIT(name)

#ifdef __cplusplus
}
#endif

#endif
