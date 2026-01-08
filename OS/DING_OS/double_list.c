/*!*****************************************************************************
 * file		double_list.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "double_list.h"
//-----------------------------------------------------------------------------------------------------------
void DbList_Init(Db_List_t *list)
{
  list->next = list->prev = list;
}
//-----------------------------------------------------------------------------------------------------------
static void Add(Db_List_t *entry, Db_List_t *prev, Db_List_t *next)
{
  prev->next = entry;
  entry->prev = prev;

  entry->next = next;
  next->prev = entry;
}
//-----------------------------------------------------------------------------------------------------------
void DbList_Push(Db_List_t *listHeader, Db_List_t *entry)
{
  // insert entry at beginning of a list header
  Add(entry, listHeader, listHeader->next);
}
//-----------------------------------------------------------------------------------------------------------
void DbList_InsertAfter(Db_List_t *list, Db_List_t *entry)
{
  // insert a entry after a list
  Add(entry, list, list->next);
}
//-----------------------------------------------------------------------------------------------------------
void DbList_Add(Db_List_t *listHeader, Db_List_t *entry)
{
  // insert entry at end of a list header
  Add(entry, listHeader->prev, listHeader);
}
//-----------------------------------------------------------------------------------------------------------
void DbList_InsertBefore(Db_List_t *list, Db_List_t *entry)
{
  // insert a entry before a list
  Add(entry, list->prev, list);
}
//-----------------------------------------------------------------------------------------------------------
static void Remove(Db_List_t *prev, Db_List_t *next)
{
  prev->next = next;
  next->prev = prev;
}
//-----------------------------------------------------------------------------------------------------------
void DbList_Remove(Db_List_t *entry)
{
  Remove(entry->prev, entry->next);
}
//-----------------------------------------------------------------------------------------------------------
Db_List_t *DbList_Pop(Db_List_t *listHeader)
{
  // Remove the first object on a list.
  Db_List_t *got = listHeader->next;
  if (got == listHeader)
    return NULL;

  DbList_Remove(got);
  return got;
}
//-----------------------------------------------------------------------------------------------------------
bool DbList_IsEmpty(Db_List_t *listHeader)
{
  return listHeader->next == listHeader;
}
//-----------------------------------------------------------------------------------------------------------
void DbList_Splice(Db_List_t *list, Db_List_t *append)
{
  // join two lists
  if (list == list->next) // empty list
    return;

  if (append == append->next) // empty append list
    return;

  Db_List_t *lastOfList = list->prev;
  Db_List_t *firstOfAppend = append->next;
  Db_List_t *lastOfAppend = append->prev;

  lastOfList->next = firstOfAppend;
  firstOfAppend->prev = lastOfList;

  lastOfAppend->next = list;
  list->prev = lastOfAppend;
}
//-----------------------------------------------------------------------------------------------------------
u16 DbList_Len(const Db_List_t *listHeader)
{
  unsigned int     len = 0;
  const Db_List_t *p = listHeader;
  while (p->next != listHeader)
  {
    p = p->next;
    len++;
  }

  return len;
}