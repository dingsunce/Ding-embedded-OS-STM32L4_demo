/*!*****************************************************************************
 * file		osal.c
 * $Author: sunce.ding
 *******************************************************************************/

#include "osal.h"
#include "rtconfig.h"
#include "rthw.h"
#include <stdlib.h>

#define TICK_PERIOD_MS (1000 / RT_TICK_PER_SECOND)

static rt_err_t err;

#if defined(RT_USING_HEAP)
/*
 * Please modify RT_HEAP_SIZE if you enable RT_USING_HEAP
 * the RT_HEAP_SIZE max value = (sram size - ZI size), 1024 means 1024 bytes
 */
#define RT_HEAP_SIZE (20 * 1024)
static rt_uint8_t rt_heap[RT_HEAP_SIZE];

RT_WEAK void *rt_heap_begin_get(void)
{
  return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
  return rt_heap + RT_HEAP_SIZE;
}
#endif
//-----------------------------------------------------------------------------------------------------------
void rt_hw_board_init(void)
{
/* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
  rt_components_init();

  rt_components_board_init();
#endif

#if defined(RT_USING_HEAP)
  rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}
//-----------------------------------------------------------------------------------------------------------
void os_init(void)
{
  rt_hw_interrupt_disable();

  rt_hw_board_init();

  /* show RT-Thread version */
  rt_show_version();

  /* timer system initialization */
  rt_system_timer_init();

  /* scheduler system initialization */
  rt_system_scheduler_init();

#ifdef RT_USING_SIGNALS
  /* signal system initialization */
  rt_system_signal_init();
#endif /* RT_USING_SIGNALS */

  /* timer thread initialization */
  rt_system_timer_thread_init();

  /* idle thread initialization */
  rt_thread_idle_init();

#ifdef RT_USING_SMP
  rt_hw_spin_lock(&_cpus_lock);
#endif /* RT_USING_SMP */
}
//-----------------------------------------------------------------------------------------------------------
void os_start(void)
{
  /* start scheduler */
  rt_system_scheduler_start();
}
//-----------------------------------------------------------------------------------------------------------
void *os_malloc(size_t size)
{
  return rt_malloc(size);
}
//-----------------------------------------------------------------------------------------------------------
void os_free(void *ptr)
{
  rt_free(ptr);
}
//-----------------------------------------------------------------------------------------------------------
os_thread_t *os_thread_create(char *name, uint32_t priority, size_t stacksize,
                              void (*entry)(void *arg), void *arg)
{
  os_thread_t *thread =
      rt_thread_create(name, entry, arg, stacksize * sizeof(rt_uint32_t), priority, 20);

  rt_thread_startup(thread);
  return thread;
}
//-----------------------------------------------------------------------------------------------------------
void os_thread_destroy(os_thread_t *thread)
{
  rt_thread_delete(thread);
}
//-----------------------------------------------------------------------------------------------------------
os_mutex_t *os_mutex_create(void)
{
  rt_mutex_t mutex = rt_mutex_create("mutex", RT_IPC_FLAG_FIFO);
  return (os_mutex_t *)mutex;
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_lock(os_mutex_t *mutex)
{
  rt_mutex_take((rt_mutex_t)mutex, OS_WAIT_FOREVER);
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_unlock(os_mutex_t *mutex)
{
  rt_mutex_release((rt_mutex_t)mutex);
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_destroy(os_mutex_t *mutex)
{
  rt_mutex_delete((rt_mutex_t)mutex);
}
//-----------------------------------------------------------------------------------------------------------
os_sem_t *os_sem_create(size_t count)
{
  rt_sem_t sem = rt_sem_create("semaphore", count, RT_IPC_FLAG_FIFO);
  return (os_sem_t *)sem;
}
//-----------------------------------------------------------------------------------------------------------
bool os_sem_wait(os_sem_t *sem, uint32_t ms)
{
  if (ms == OS_WAIT_FOREVER)
    err = rt_sem_take((rt_sem_t)sem, RT_WAITING_FOREVER);
  else
    err = rt_sem_take((rt_sem_t)sem, ms / TICK_PERIOD_MS);

  if (err != RT_EOK)
    return false;

  return true;
}
//-----------------------------------------------------------------------------------------------------------
void os_sem_signal(os_sem_t *sem)
{
  rt_sem_release((rt_sem_t)sem);
}
//-----------------------------------------------------------------------------------------------------------
void os_sem_destroy(os_sem_t *sem)
{
  rt_sem_delete((rt_sem_t)sem);
}
//-----------------------------------------------------------------------------------------------------------
os_event_t *os_event_create(void)
{
  rt_event_t event = rt_event_create("event", RT_IPC_FLAG_FIFO);
  return (os_event_t *)event;
}
//-----------------------------------------------------------------------------------------------------------
bool os_event_wait(os_event_t *event, uint32_t mask, uint32_t *value, uint32_t ms)
{
  if (ms == OS_WAIT_FOREVER)
    err = rt_event_recv((rt_event_t)event, mask, RT_EVENT_FLAG_OR, RT_WAITING_FOREVER, value);
  else
    err = rt_event_recv((rt_event_t)event, mask, RT_EVENT_FLAG_OR, ms / TICK_PERIOD_MS, value);

  if (err != RT_EOK)
    return false;

  *value &= mask;

  return true;
}
//-----------------------------------------------------------------------------------------------------------
void os_event_set(os_event_t *event, uint32_t value)
{
  rt_event_send((rt_event_t)event, value);
}
//-----------------------------------------------------------------------------------------------------------
void os_event_clr(os_event_t *event, uint32_t value)
{
}
//-----------------------------------------------------------------------------------------------------------
void os_event_destroy(os_event_t *event)
{
  rt_event_delete(event);
}
//-----------------------------------------------------------------------------------------------------------
os_mbox_t *os_mbox_create(size_t size)
{
  return rt_mb_create("mbox", size, RT_IPC_FLAG_FIFO);
}
//-----------------------------------------------------------------------------------------------------------
bool os_mbox_fetch(os_mbox_t *mbox, void **msg, uint32_t ms)
{
  if (ms == OS_WAIT_FOREVER)
    err = rt_mb_recv(mbox, (rt_ubase_t *)msg, RT_WAITING_FOREVER);
  else
    err = rt_mb_recv(mbox, (rt_ubase_t *)msg, ms / TICK_PERIOD_MS);

  if (err != RT_EOK)
    return false;

  return true;
}
//-----------------------------------------------------------------------------------------------------------
bool os_mbox_post(os_mbox_t *mbox, void *msg, uint32_t ms)
{
  rt_err_t err = rt_mb_send(mbox, (rt_ubase_t)msg);

  if (err != RT_EOK)
    return false;

  return true;
}
//-----------------------------------------------------------------------------------------------------------
void os_mbox_destroy(os_mbox_t *mbox)
{
  rt_mb_delete(mbox);
}
//-----------------------------------------------------------------------------------------------------------
void os_usleep(uint32_t us)
{
  rt_thread_delay((us / 1000) / TICK_PERIOD_MS);
}
//-----------------------------------------------------------------------------------------------------------
void os_tick_sleep(os_tick_t tick)
{
  rt_thread_delay(tick);
}
//-----------------------------------------------------------------------------------------------------------
os_tick_t os_tick_current(void)
{
  return rt_tick_get();
}
//-----------------------------------------------------------------------------------------------------------
uint32_t os_get_current_time_us(void)
{
  return 1000 * (os_tick_current() / TICK_PERIOD_MS);
}
//-----------------------------------------------------------------------------------------------------------
os_tick_t os_tick_from_us(uint32_t us)
{
  return (us / 1000) / TICK_PERIOD_MS;
}
//-----------------------------------------------------------------------------------------------------------
static void os_timer_callback(void *parameter)
{
  os_timer_t *timer = (os_timer_t *)parameter;

  if (timer->fn)
    timer->fn(timer, timer->arg);
}
//-----------------------------------------------------------------------------------------------------------
os_timer_t *os_timer_create(uint32_t us, void (*fn)(os_timer_t *, void *arg), void *arg,
                            bool oneshot)
{
  os_timer_t *timer;

  timer = os_malloc(sizeof(os_timer_t));
  if (timer == NULL)
    return NULL;

  timer->fn = fn;
  timer->arg = arg;

  rt_timer_t rt_timer;
  if (oneshot)
    rt_timer = rt_timer_create("timer", os_timer_callback, timer, os_tick_from_us(us),
                               RT_TIMER_FLAG_ONE_SHOT);
  else
    rt_timer = rt_timer_create("timer", os_timer_callback, timer, os_tick_from_us(us),
                               RT_TIMER_FLAG_PERIODIC);

  timer->rt_timer = rt_timer;

  return (os_timer_t *)timer;
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_set(os_timer_t *timer, uint32_t us)
{
  timer->rt_timer->timeout_tick = os_tick_from_us(us);
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_start(os_timer_t *timer)
{
  rt_timer_start(timer->rt_timer);
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_stop(os_timer_t *timer)
{
  rt_timer_stop(timer->rt_timer);
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_destroy(os_timer_t *timer)
{
  rt_timer_delete(timer->rt_timer);
  os_free(timer);
}
