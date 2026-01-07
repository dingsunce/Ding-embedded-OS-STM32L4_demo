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

  // data type u64 u32, u16... defined in linux/types.h

#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/types.h>

#ifndef LINUX_HIGH_RESOLUTION_TIMER
#define LINUX_HIGH_RESOLUTION_TIMER 1
#endif

#if (LINUX_HIGH_RESOLUTION_TIMER == 1)

#ifndef OS_TICK_PERIOD_MS
#define OS_TICK_PERIOD_MS 1
#endif

#else

// defaut CONFIG_HZ is 100, set CONFIG_HZ 1000 to achieve 1ms resolution
#ifndef OS_TICK_PERIOD_MS
#define OS_TICK_PERIOD_MS 10
#endif

#endif

// 0 is highest priority ......
#define OS_INVERTED_PRIORITY 1

#define OS_WAIT_FOREVER 0xFFFFFFFF

#ifndef OS_DEBUG
#define OS_DEBUG 0
#endif

#if OS_DEBUG == 0
#define OS_PRINT(...)
#else
#define OS_PRINT printk
#endif

  typedef struct mutex     os_mutex_t;
  typedef struct semaphore os_sem_t;
  typedef u32              os_tick_t;

  typedef int os_return_t;
  typedef int (*os_entry_t)(void *arg);
#define OS_RETURN(thread)                                                                          \
  {                                                                                                \
    complete(&thread->thread_exited);                                                              \
    os_free(thread);                                                                               \
    return 0;                                                                                      \
  }

  typedef struct os_thread
  {
    struct task_struct *pthread;
    struct completion   thread_exited;
    bool                should_stop;
    bool                exited;
  } os_thread_t;

  typedef struct os_mbox
  {
    os_sem_t   condition;
    os_mutex_t lock;
    size_t     r;
    size_t     w;
    size_t     count;
    size_t     size;
    void      *msg[];
  } os_mbox_t;

  typedef struct os_event
  {
    os_sem_t   condition;
    os_mutex_t lock;
    u32        flags;
  } os_event_t;

#if (LINUX_HIGH_RESOLUTION_TIMER == 1)
  typedef struct os_timer
  {
    struct hrtimer kernel_timer;
    void (*fn)(struct os_timer *, void *arg);
    void *arg;
    u32   ms;
    bool  oneshot;
  } os_timer_t;

#else

typedef struct os_timer
{
  struct timer_list kernel_timer;
  void (*fn)(struct os_timer *, void *arg);
  void *arg;
  u32   ms;
  bool  oneshot;
} os_timer_t;

#endif

  void *os_malloc(u16 size);
  void  os_free(void *ptr);

  void os_init(void);
  void os_start(void);

  os_thread_t *os_thread_create(char *name, u16 priority, u16 stacksize, os_entry_t entry,
                                void *arg);
  void         os_thread_destroy(os_thread_t *thread);
  bool         os_thread_should_stop(os_thread_t *thread);
  void         os_thread_wait_for_completion(os_thread_t *thread);

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
