
/*!*****************************************************************************
 * file		board.c
 * $Author: sunce.ding
 *******************************************************************************/

#include <rthw.h>
#include <rtthread.h>
#include "SysTick.h"
//-----------------------------------------------------------------------------------------------------------
void rt_os_tick_callback(void)
{
  rt_interrupt_enter();
  
  rt_tick_increase();

  rt_interrupt_leave();
}
//-----------------------------------------------------------------------------------------------------------
void rt_thread_set_hook(void)
{
  rt_interrupt_enter();
  
  rt_tick_sethook(SysTick_On);

  rt_interrupt_leave();
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_Handler(void)
{
  rt_os_tick_callback();
}
//-----------------------------------------------------------------------------------------------------------
#ifdef RT_USING_CONSOLE

static int uart_init(void)
{
  return 0;
}
INIT_BOARD_EXPORT(uart_init);

void rt_hw_console_output(const char *str)
{

}

#endif

