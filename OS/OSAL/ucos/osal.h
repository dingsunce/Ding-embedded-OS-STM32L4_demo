/*!*****************************************************************************
 * file		osal.h
 * $Author: sunce.ding
 *******************************************************************************/

#ifndef OSAL_H
#define OSAL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <os.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

  typedef unsigned long long s64;
  typedef int32_t            s32;
  typedef int16_t            s16;
  typedef int8_t             s8;
  typedef long long          u64;
  typedef uint32_t           u32;
  typedef uint16_t           u16;
  typedef uint8_t            u8;

// 0 is highest priority ......
#define OS_INVERTED_PRIORITY 0

#define OS_WAIT_FOREVER 0xFFFFFFFF

  typedef OS_MUTEX    os_mutex_t;
  typedef OS_TCB      os_thread_t;
  typedef OS_SEM      os_sem_t;
  typedef OS_FLAG_GRP os_event_t;
  typedef OS_Q        os_mbox_t;
  typedef OS_TICK     os_tick_t;
  typedef OS_TMR      os_timer_t;

  typedef void os_return_t;
  typedef void (*os_entry_t)(void *arg);
#define OS_RETURN(thread)                                                                          \
  {                                                                                                \
  }

  void *os_malloc(u16 size);
  void  os_free(void *ptr);

  void os_init(void);
  void os_start(void);

  os_thread_t *os_thread_create(char *name, u16 priority, u16 stacksize, os_entry_t entry,
                                void *arg);
  void         os_thread_destroy(os_thread_t *thread);
  bool         os_thread_should_stop(os_thread_t *thread);

  os_mutex_t *os_mutex_create(void);
  void        os_mutex_lock(os_mutex_t *mutex);
  void        os_mutex_unlock(os_mutex_t *mutex);
  void        os_mutex_destroy(os_mutex_t *mutex);

  os_sem_t *os_sem_create(u16 count);
  bool      os_sem_wait(os_sem_t *sem, u32 ms);
  void      os_sem_signal(os_sem_t *sem);
  void      os_sem_destroy(os_sem_t *sem);

  os_event_t *os_event_create(void);
  bool        os_event_wait(os_event_t *event, u32 mask, u32 *value, u32 ms);
  void        os_event_set(os_event_t *event, u32 value);
  void        os_event_clr(os_event_t *event, u32 value);
  void        os_event_destroy(os_event_t *event);

  os_mbox_t *os_mbox_create(u32 size);
  bool       os_mbox_fetch(os_mbox_t *mbox, void **msg, u32 ms);
  bool       os_mbox_post(os_mbox_t *mbox, void *msg, u32 ms);
  void       os_mbox_destroy(os_mbox_t *mbox);

  void      os_msleep(u32 ms);
  u32       os_ms_current(void);
  os_tick_t os_tick_current(void);
  os_tick_t os_tick_from_ms(u32 ms);
  void      os_tick_sleep(os_tick_t tick);

  os_timer_t *os_timer_create(u32 ms, void (*fn)(os_timer_t *timer, void *arg), void *arg,
                              bool oneshot);
  void        os_timer_set(os_timer_t *timer, u32 ms);
  void        os_timer_start(os_timer_t *timer);
  void        os_timer_stop(os_timer_t *timer);
  void        os_timer_destroy(os_timer_t *timer);

#ifdef __cplusplus
}
#endif

#endif /* OSAL_H */
