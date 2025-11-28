/*!*****************************************************************************
 * file		osal.c
 * $Author: sunce.ding
 *******************************************************************************/

#include "osal.h"

#include <stdlib.h>

#define TM_TO_TICK(ms) ((ms == OS_WAIT_FOREVER) ? portMAX_DELAY : (ms) / portTICK_PERIOD_MS)

static BaseType_t xHigherPriorityTaskWoken;
//-----------------------------------------------------------------------------------------------------------
void os_init(void)
{
}
//-----------------------------------------------------------------------------------------------------------
void os_start(void)
{
  vTaskStartScheduler();
}
//-----------------------------------------------------------------------------------------------------------
void *os_malloc(size_t size)
{
  return pvPortMalloc(size);
}
//-----------------------------------------------------------------------------------------------------------
void os_free(void *ptr)
{
  vPortFree(ptr);
}
//-----------------------------------------------------------------------------------------------------------
os_thread_t *os_thread_create(char *name, uint32_t priority, size_t stacksize,
                              void (*entry)(void *arg), void *arg)
{
  TaskHandle_t xHandle = NULL;

  /* stacksize in freertos is not in bytes but in stack depth */
  if (xTaskCreate(entry, name, stacksize, arg, priority, &xHandle) == pdPASS)
    return (os_thread_t *)xHandle;

  return NULL;
}
//-----------------------------------------------------------------------------------------------------------
void os_thread_destroy(os_thread_t *thread)
{
  vTaskDelete((TaskHandle_t)thread);
}
//-----------------------------------------------------------------------------------------------------------
os_mutex_t *os_mutex_create(void)
{
  SemaphoreHandle_t handle = xSemaphoreCreateRecursiveMutex();
  return (os_mutex_t *)handle;
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_lock(os_mutex_t *mutex)
{
  if (xPortIsInsideInterrupt())
    return;

  xSemaphoreTakeRecursive((SemaphoreHandle_t)mutex, portMAX_DELAY);
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_unlock(os_mutex_t *mutex)
{
  if (!xPortIsInsideInterrupt())
  {
    xSemaphoreGiveRecursive((SemaphoreHandle_t)mutex);
  }
  else
  {
    xSemaphoreGiveFromISR((SemaphoreHandle_t)mutex, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
  }
}
//-----------------------------------------------------------------------------------------------------------
void os_mutex_destroy(os_mutex_t *mutex)
{
  vSemaphoreDelete((SemaphoreHandle_t)mutex);
}
//-----------------------------------------------------------------------------------------------------------
void os_usleep(uint32_t us)
{
  vTaskDelay((us / portTICK_PERIOD_MS) / 1000);
}
//-----------------------------------------------------------------------------------------------------------
uint32_t os_get_current_time_us(void)
{
  return 1000 * (xTaskGetTickCount() / portTICK_PERIOD_MS);
}
//-----------------------------------------------------------------------------------------------------------
os_tick_t os_tick_current(void)
{
  return xTaskGetTickCount();
}
//-----------------------------------------------------------------------------------------------------------
os_tick_t os_tick_from_us(uint32_t us)
{
  return us / (1000u * portTICK_PERIOD_MS);
}
//-----------------------------------------------------------------------------------------------------------
void os_tick_sleep(os_tick_t tick)
{
  vTaskDelay(tick);
}
//-----------------------------------------------------------------------------------------------------------
os_sem_t *os_sem_create(size_t count)
{
  SemaphoreHandle_t handle = xSemaphoreCreateCounting(UINT32_MAX, count);
  return (os_sem_t *)handle;
}
//-----------------------------------------------------------------------------------------------------------
bool os_sem_wait(os_sem_t *sem, uint32_t ms)
{
  if (xPortIsInsideInterrupt())
    return false;

  if (xSemaphoreTake((SemaphoreHandle_t)sem, TM_TO_TICK(ms)) != pdTRUE)
    return false;

  return true;
}
//-----------------------------------------------------------------------------------------------------------
void os_sem_signal(os_sem_t *sem)
{
  if (!xPortIsInsideInterrupt())
  {
    xSemaphoreGive((SemaphoreHandle_t)sem);
  }
  else
  {
    xSemaphoreGiveFromISR((SemaphoreHandle_t)sem, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
  }
}
//-----------------------------------------------------------------------------------------------------------
void os_sem_destroy(os_sem_t *sem)
{
  vSemaphoreDelete((SemaphoreHandle_t)sem);
}
//-----------------------------------------------------------------------------------------------------------
os_event_t *os_event_create(void)
{
  EventGroupHandle_t handle = xEventGroupCreate();
  return (os_event_t *)handle;
}
//-----------------------------------------------------------------------------------------------------------
bool os_event_wait(os_event_t *event, uint32_t mask, uint32_t *value, uint32_t ms)
{
  *value = xEventGroupWaitBits((EventGroupHandle_t)event, mask, pdFALSE, pdFALSE, TM_TO_TICK(ms));

  *value &= mask;
  return *value == 0;
}
//-----------------------------------------------------------------------------------------------------------
void os_event_set(os_event_t *event, uint32_t value)
{
  xEventGroupSetBits((EventGroupHandle_t)event, value);
}
//-----------------------------------------------------------------------------------------------------------
void os_event_clr(os_event_t *event, uint32_t value)
{
  xEventGroupClearBits((EventGroupHandle_t)event, value);
}
//-----------------------------------------------------------------------------------------------------------
void os_event_destroy(os_event_t *event)
{
  vEventGroupDelete((EventGroupHandle_t)event);
}
//-----------------------------------------------------------------------------------------------------------
os_mbox_t *os_mbox_create(size_t size)
{
  QueueHandle_t handle = xQueueCreate(size, sizeof(void *));
  return (os_mbox_t *)handle;
}
//-----------------------------------------------------------------------------------------------------------
bool os_mbox_fetch(os_mbox_t *mbox, void **msg, uint32_t ms)
{
  BaseType_t success;

  if (!xPortIsInsideInterrupt())
  {
    success = xQueueReceive((QueueHandle_t)mbox, msg, TM_TO_TICK(ms));
  }
  else
  {
    success = xQueueReceiveFromISR((QueueHandle_t)mbox, msg, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
  }

  return success != pdTRUE;
}
//-----------------------------------------------------------------------------------------------------------
bool os_mbox_post(os_mbox_t *mbox, void *msg, uint32_t ms)
{
  BaseType_t success;

  if (!xPortIsInsideInterrupt())
  {
    success = xQueueSendToBack((QueueHandle_t)mbox, &msg, TM_TO_TICK(ms));
  }
  else
  {
    success = xQueueSendToBackFromISR((QueueHandle_t)mbox, &msg, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
  }

  return success != pdTRUE;
}
//-----------------------------------------------------------------------------------------------------------
void os_mbox_destroy(os_mbox_t *mbox)
{
  vQueueDelete((QueueHandle_t)mbox);
}
//-----------------------------------------------------------------------------------------------------------
static void os_timer_callback(TimerHandle_t xTimer)
{
  os_timer_t *timer = pvTimerGetTimerID(xTimer);

  if (timer->fn)
    timer->fn(timer, timer->arg);
}
//-----------------------------------------------------------------------------------------------------------
os_timer_t *os_timer_create(uint32_t us, void (*fn)(os_timer_t *, void *arg), void *arg,
                            bool oneshot)
{
  os_timer_t *timer;

  timer = os_malloc(sizeof(*timer));
  if (timer == NULL)
    return NULL;

  timer->fn = fn;
  timer->arg = arg;
  timer->us = us;

  timer->handle = xTimerCreate("os_timer", (us / portTICK_PERIOD_MS) / 1000,
                               oneshot ? pdFALSE : pdTRUE, timer, os_timer_callback);

  if (timer->handle == NULL)
  {
    os_free(timer);
    return NULL;
  }

  return timer;
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_set(os_timer_t *timer, uint32_t us)
{
  timer->us = us;
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_start(os_timer_t *timer)
{
  /* Start timer by updating the period */
  xTimerChangePeriod(timer->handle, (timer->us / portTICK_PERIOD_MS) / 1000, portMAX_DELAY);
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_stop(os_timer_t *timer)
{
  xTimerStop(timer->handle, portMAX_DELAY);
}
//-----------------------------------------------------------------------------------------------------------
void os_timer_destroy(os_timer_t *timer)
{
  xTimerDelete(timer->handle, portMAX_DELAY);
  os_free(timer);
}
