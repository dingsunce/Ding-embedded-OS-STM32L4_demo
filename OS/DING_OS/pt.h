/*!*****************************************************************************
 * file		pt.h (Proto Process implementation.)
 * $Author: sunce.ding
 *******************************************************************************/
#ifndef _PT_H_
#define _PT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "define.h"

  typedef struct Pt
  {
    u16 Lc;
  } Pt_t;

#define LC_INIT(s) s = 0;

#define LC_RESUME(s)                                                                                         \
  switch (s)                                                                                                 \
  {                                                                                                          \
  case 0:

#define LC_SET(s)                                                                                            \
  s = __LINE__;                                                                                              \
  case __LINE__:

#define LC_END(s) }

#define PT_WAITING 0
#define PT_YIELDED 1
#define PT_EXITED  2
#define PT_ENDED   3

#define PT_INIT(pt) LC_INIT((pt)->Lc)

#define PT_BEGIN(pt)                                                                                         \
  {                                                                                                          \
    char PT_YIELD_FLAG = 1;                                                                                  \
    if (PT_YIELD_FLAG)                                                                                       \
    {                                                                                                        \
      ;                                                                                                      \
    }                                                                                                        \
    LC_RESUME((pt)->Lc)

#define PT_END(pt)                                                                                           \
  LC_END((pt)->Lc);                                                                                          \
  PT_YIELD_FLAG = 0;                                                                                         \
  PT_INIT(pt);                                                                                               \
  return PT_ENDED;                                                                                           \
  }

#define PT_EXIT(pt)                                                                                          \
  do                                                                                                         \
  {                                                                                                          \
    PT_INIT(pt);                                                                                             \
    return PT_EXITED;                                                                                        \
  } while (0)

#define PT_YIELD(pt)                                                                                         \
  do                                                                                                         \
  {                                                                                                          \
    PT_YIELD_FLAG = 0;                                                                                       \
    LC_SET((pt)->Lc);                                                                                        \
    if (PT_YIELD_FLAG == 0)                                                                                  \
    {                                                                                                        \
      return PT_YIELDED;                                                                                     \
    }                                                                                                        \
  } while (0)

#define PT_YIELD_UNTIL(pt, cond)                                                                             \
  do                                                                                                         \
  {                                                                                                          \
    PT_YIELD_FLAG = 0;                                                                                       \
    LC_SET((pt)->Lc);                                                                                        \
    if ((PT_YIELD_FLAG == 0) || !(cond))                                                                     \
    {                                                                                                        \
      return PT_YIELDED;                                                                                     \
    }                                                                                                        \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif