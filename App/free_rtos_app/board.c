
/*!*****************************************************************************
 * file		board.c
 * $Author: sunce.ding
 *******************************************************************************/

#include "SysTick.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

extern void xPortSysTickHandler( void );
//-----------------------------------------------------------------------------------------------------------
void vApplicationTickHook( void )
{
  SysTick_On();
}
//-----------------------------------------------------------------------------------------------------------
void SysTick_Handler(void)
{
  if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED )
    xPortSysTickHandler();
}
//-----------------------------------------------------------------------------------------------------------
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName)
{
  while(1)
  {
  }
}
