#ifndef _ASSERT_FUNC_H
#define _ASSERT_FUNC_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef ENABLE_ASSERT
#define Assert(expr) ((expr) ? (void)0 : (void)Assert_Fail(#expr, __FILE__, __LINE__))
#else
#define Assert(expr) ((void)0)
#endif

  extern void Assert_Fail(const char *expr, const char *file, int line);

#ifdef __cplusplus
}
#endif

#endif
