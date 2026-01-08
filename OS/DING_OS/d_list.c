/*!*****************************************************************************
 * file		d_list.c (an implementation of single List)
 * $Author: sunce.ding
 *******************************************************************************/
#include "d_list.h"

typedef struct ListContent
{
  struct ListContent *next;
} ListContent_t;

//-----------------------------------------------------------------------------------------------------------
void DList_Init(d_list_t list)
{
  *list = NULL;
}
//-----------------------------------------------------------------------------------------------------------
void *DList_Head(d_list_t list)
{
  return *list;
}
//-----------------------------------------------------------------------------------------------------------
void DList_Copy(d_list_t dest, d_list_t src)
{
  *dest = *src;
}
//-----------------------------------------------------------------------------------------------------------
void *DList_Tail(d_list_t list)
{
  ListContent_t *l;

  if (*list == NULL)
  {
    return NULL;
  }

  for (l = *list; l->next != NULL; l = l->next)
    ;

  return l;
}
//-----------------------------------------------------------------------------------------------------------
void DList_Add(d_list_t list, void *item)
{
  // Add an item to the end of the list.
  ListContent_t *l;

  if (item == NULL)
    return;

  ((ListContent_t *)item)->next = NULL;

  l = DList_Tail(list);

  if (l == NULL)
  {
    *list = item;
  }
  else
  {
    l->next = item;
  }
}
//-----------------------------------------------------------------------------------------------------------
void DList_Push(d_list_t list, void *item)
{
  if (item == NULL)
    return;

  ((ListContent_t *)item)->next = *list;
  *list = item;
}
//-----------------------------------------------------------------------------------------------------------
void *DList_Chop(d_list_t list)
{
  // Remove the last object on the list.
  ListContent_t *l, *r;

  if (*list == NULL)
    return NULL;

  if (((ListContent_t *)*list)->next == NULL)
  {
    l = *list;
    *list = NULL;
    return l;
  }

  for (l = *list; l->next->next != NULL; l = l->next)
    ;

  r = l->next;
  l->next = NULL;

  return r;
}
//-----------------------------------------------------------------------------------------------------------
void *DList_Pop(d_list_t list)
{
  // Remove the first object on a list.
  ListContent_t *l;
  l = *list;

  if (*list != NULL)
    *list = ((ListContent_t *)*list)->next;

  return l;
}
//-----------------------------------------------------------------------------------------------------------
void DList_Remove(d_list_t list, void *item)
{
  ListContent_t *l, *r;

  if (*list == NULL)
    return;

  r = NULL;
  for (l = *list; l != NULL; l = l->next)
  {
    if (l == item)
    {
      if (r == NULL)
        *list = l->next;
      else
        r->next = l->next;

      l->next = NULL;
      return;
    }

    r = l;
  }
}
//-----------------------------------------------------------------------------------------------------------
u32 DList_Length(d_list_t list)
{
  ListContent_t *l;
  u32            n = 0;

  for (l = *list; l != NULL; l = l->next)
    ++n;

  return n;
}
//-----------------------------------------------------------------------------------------------------------
bool DList_IsEmpty(d_list_t list)
{
  if (DList_Head(list) != NULL)
    return false;

  return true;
}
//-----------------------------------------------------------------------------------------------------------
void DList_Insert(d_list_t list, void *prevItem, void *newItem)
{
  if (prevItem == NULL)
  {
    DList_Push(list, newItem);
  }
  else
  {

    ((ListContent_t *)newItem)->next = ((ListContent_t *)prevItem)->next;
    ((ListContent_t *)prevItem)->next = newItem;
  }
}
//-----------------------------------------------------------------------------------------------------------
void *DList_ItemNext(void *item)
{
  return item == NULL ? NULL : ((ListContent_t *)item)->next;
}
