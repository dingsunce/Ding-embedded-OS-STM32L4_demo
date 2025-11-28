#ifndef _EEPROM_FUNC_H
#define _EEPROM_FUNC_H

#include "define.h"
#include "error.h"

typedef u16 EeAdd_t;

extern void Ee_Init(void);
extern void Ee_Process(void);

extern u8      Ee_Read8(EeAdd_t addr);
extern u16     Ee_Read16(EeAdd_t addr);
extern u32     Ee_Read32(EeAdd_t addr);
extern OsErr_t Ee_Read(EeAdd_t addr, u8 *pData, u16 length);

extern void    Ee_StartDirectWrite(void);
extern void    Ee_StopDirectWrite(void);
extern OsErr_t Ee_Write8(EeAdd_t addr, u8 value);
extern OsErr_t Ee_Write16(EeAdd_t addr, u16 value);
extern OsErr_t Ee_Write32(EeAdd_t addr, u32 value);
extern OsErr_t Ee_Write(EeAdd_t addr, u8 *pData, u16 length);

extern bool Ee_IsEmpty(void);

#endif
