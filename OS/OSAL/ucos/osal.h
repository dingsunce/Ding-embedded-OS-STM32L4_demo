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

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <os.h>

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

  void *os_malloc(size_t size);
  void  os_free(void *ptr);

  void os_init(void);
  void os_start(void);

  os_thread_t *os_thread_create(char *name, uint32_t priority, size_t stacksize,
                                void (*entry)(void *arg), void *arg);
  void         os_thread_destroy(os_thread_t *thread);

  os_mutex_t *os_mutex_create(void);
  void        os_mutex_lock(os_mutex_t *mutex);
  void        os_mutex_unlock(os_mutex_t *mutex);
  void        os_mutex_destroy(os_mutex_t *mutex);

  os_sem_t *os_sem_create(size_t count);
  bool      os_sem_wait(os_sem_t *sem, uint32_t ms);
  void      os_sem_signal(os_sem_t *sem);
  void      os_sem_destroy(os_sem_t *sem);

  os_event_t *os_event_create(void);
  bool        os_event_wait(os_event_t *event, uint32_t mask, uint32_t *value, uint32_t ms);
  void        os_event_set(os_event_t *event, uint32_t value);
  void        os_event_clr(os_event_t *event, uint32_t value);
  void        os_event_destroy(os_event_t *event);

  os_mbox_t *os_mbox_create(size_t size);
  bool       os_mbox_fetch(os_mbox_t *mbox, void **msg, uint32_t ms);
  bool       os_mbox_post(os_mbox_t *mbox, void *msg, uint32_t ms);
  void       os_mbox_destroy(os_mbox_t *mbox);

  void      os_usleep(uint32_t us);
  uint32_t  os_get_current_time_us(void);
  os_tick_t os_tick_current(void);
  os_tick_t os_tick_from_us(uint32_t us);
  void      os_tick_sleep(os_tick_t tick);

  os_timer_t *os_timer_create(uint32_t us, void (*fn)(os_timer_t *timer, void *arg), void *arg,
                              bool oneshot);
  void        os_timer_set(os_timer_t *timer, uint32_t us);
  void        os_timer_start(os_timer_t *timer);
  void        os_timer_stop(os_timer_t *timer);
  void        os_timer_destroy(os_timer_t *timer);

#ifdef __cplusplus
}
#endif

#endif /* OSAL_H */
