/*!*****************************************************************************
 * file		DList.c  (Double List)
 * $Author: sunce.ding
 *******************************************************************************/
#include "DList.h"
//-----------------------------------------------------------------------------------------------------------
void DList_Init(DList_t *list)
{
  list->next = list->prev = list;
}
//-----------------------------------------------------------------------------------------------------------
static void Add(DList_t *entry, DList_t *prev, DList_t *next)
{
  prev->next = entry;
  entry->prev = prev;

  entry->next = next;
  next->prev = entry;
}
//-----------------------------------------------------------------------------------------------------------
void DList_Push(DList_t *listHeader, DList_t *entry)
{
  // insert entry at beginning of a list header
  Add(entry, listHeader, listHeader->next);
}
//-----------------------------------------------------------------------------------------------------------
void DList_InsertAfter(DList_t *list, DList_t *entry)
{
  // insert a entry after a list
  Add(entry, list, list->next);
}
//-----------------------------------------------------------------------------------------------------------
void DList_Add(DList_t *listHeader, DList_t *entry)
{
  // insert entry at end of a list header
  Add(entry, listHeader->prev, listHeader);
}
//-----------------------------------------------------------------------------------------------------------
void DList_InsertBefore(DList_t *list, DList_t *entry)
{
  // insert a entry before a list
  Add(entry, list->prev, list);
}
//-----------------------------------------------------------------------------------------------------------
static void Remove(DList_t *prev, DList_t *next)
{
  prev->next = next;
  next->prev = prev;
}
//-----------------------------------------------------------------------------------------------------------
void DList_Remove(DList_t *entry)
{
  Remove(entry->prev, entry->next);
}
//-----------------------------------------------------------------------------------------------------------
DList_t *DList_Pop(DList_t *listHeader)
{
  // Remove the first object on a list.
  DList_t *got = listHeader->next;
  if (got == listHeader)
    return NULL;

  DList_Remove(got);
  return got;
}
//-----------------------------------------------------------------------------------------------------------
bool DList_IsEmpty(DList_t *listHeader)
{
  return listHeader->next == listHeader;
}
//-----------------------------------------------------------------------------------------------------------
void DList_Splice(DList_t *list, DList_t *append)
{
  // join two lists
  if (list == list->next) // empty list
    return;

  if (append == append->next) // empty append list
    return;

  DList_t *lastOfList = list->prev;
  DList_t *firstOfAppend = append->next;
  DList_t *lastOfAppend = append->prev;

  lastOfList->next = firstOfAppend;
  firstOfAppend->prev = lastOfList;

  lastOfAppend->next = list;
  list->prev = lastOfAppend;
}
//-----------------------------------------------------------------------------------------------------------
u16 DList_Len(const DList_t *listHeader)
{
  unsigned int   len = 0;
  const DList_t *p = listHeader;
  while (p->next != listHeader)
  {
    p = p->next;
    len++;
  }

  return len;
}