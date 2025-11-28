/*!*****************************************************************************
 * file		SList.c  (Single List)
 * $Author: sunce.ding
 *******************************************************************************/
#include "SList.h"
//-----------------------------------------------------------------------------------------------------------
void SList_Init(SList_t *list)
{
  list->next = NULL;
}
//-----------------------------------------------------------------------------------------------------------
void SList_Add(SList_t *listHeader, SList_t *entry)
{
  if (entry == NULL)
    return;

  SList_t *node = listHeader;
  while (node->next)
    node = node->next;

  // append the entry to the tail
  node->next = entry;
  entry->next = NULL;
}
//-----------------------------------------------------------------------------------------------------------
void SList_Push(SList_t *listHeader, SList_t *entry)
{
  if (entry == NULL)
    return;

  // insert an entry at beginning of a list header
  entry->next = listHeader->next;
  listHeader->next = entry;
}
//-----------------------------------------------------------------------------------------------------------
SList_t *SList_Pop(SList_t *listHeader)
{
  // Remove the first object on a list.
  SList_t *got = listHeader->next;
  if (got != NULL)
    listHeader->next = got->next;

  return got;
}
//-----------------------------------------------------------------------------------------------------------
SList_t *SList_Chop(SList_t *listHeader)
{
  // Remove the last object on the list.
  SList_t *list = listHeader->next;
  SList_t *left = NULL;
  while (list != NULL)
  {
    if (list->next == NULL)
    {
      if (left == NULL)
        listHeader->next = list->next;
      else
        left->next = list->next;

      list->next = NULL;
      return list;
    }

    left = list;
    list = list->next;
  }

  return NULL;
}
//-----------------------------------------------------------------------------------------------------------
bool SList_IsEmpty(SList_t *listHeader)
{
  return listHeader->next == NULL;
}
//-----------------------------------------------------------------------------------------------------------
u16 SList_Len(const SList_t *listHeader)
{
  u16            len = 0;
  const SList_t *list = listHeader->next;
  while (list != NULL)
  {
    list = list->next;
    len++;
  }

  return len;
}
//-----------------------------------------------------------------------------------------------------------
void SList_Remove(SList_t *listHeader, SList_t *entry)
{
  SList_t *list = listHeader->next;
  SList_t *left = NULL;
  while (list != NULL)
  {
    if (list == entry)
    {
      if (left == NULL)
        listHeader->next = entry->next;
      else
        left->next = entry->next;

      entry->next = NULL;
      return;
    }

    left = list;
    list = list->next;
  }
}