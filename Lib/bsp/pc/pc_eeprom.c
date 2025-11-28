#include "eeprom_func.h"
#include "memb.h"
#include "s_list.h"

typedef struct EeEntry
{
  LIST_HEADER;
  EeAdd_t add;
  u32     value;
} EeEntry_t;

#define EE_SUM 500

LIST(EeList);
MEMB(EeMem, struct EeEntry, EE_SUM);

void Ee_Init(void)
{
  List_Init(EeList);
  Memb_Init(&EeMem);
}

void Ee_Process(void)
{
}

void Ee_StartDirectWrite(void)
{
}

void Ee_StopDirectWrite(void)
{
}

static EeEntry_t *FindExistedEntry(u16 add)
{
  EeEntry_t *entry = List_Head(EeList);

  while (entry != NULL)
  {
    if (entry->add == add)
    {
      return entry;
    }

    entry = List_ItemNext(entry);
  }

  return NULL;
}

u8 Ee_Read8(EeAdd_t add)
{
  return (u8)Ee_Read32(add);
}

u16 Ee_Read16(EeAdd_t add)
{
  return (u16)Ee_Read32(add);
}

u32 Ee_Read32(EeAdd_t add)
{
  EeEntry_t *entry = FindExistedEntry(add);

  if (entry != NULL)
  {
    return entry->value;
  }

  return 0;
}

OsErr_t Ee_Write8(EeAdd_t add, u8 value)
{
  return Ee_Write32(add, value);
}

OsErr_t Ee_Write16(EeAdd_t add, u16 value)
{
  return Ee_Write32(add, value);
}

static OsErr_t AddEntryToList(u16 add, u16 data)
{
  EeEntry_t *entry = Memb_Alloc(&EeMem);

  if (entry != NULL)
  {
    entry->value = data;
    entry->add = add;
    List_Add(EeList, entry);
    return OS_ERR_OK;
  }

  return OS_ERR_ALLOC;
}

OsErr_t Ee_Write32(EeAdd_t add, u32 value)
{
  EeEntry_t *entry = FindExistedEntry(add);

  if (entry != NULL)
  {
    entry->value = value;
    return OS_ERR_OK;
  }

  return AddEntryToList(add, value);
}

bool Ee_IsEmpty(void)
{
  return false;
}
