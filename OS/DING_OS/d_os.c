/*!*****************************************************************************
 * file		d_os.c
 * $Author: sunce.ding
 *******************************************************************************/
#include "d_os.h"
#include "SysTick.h"
#include "d_mem.h"
#include "d_task.h"
#include "message.h"
//-----------------------------------------------------------------------------------------------------------
void DOS_Init(void)
{
  SysTick_Init();
  DMem_Init();
  Process_Init();
  Msg_Init();
  DTask_Init();
}
//-----------------------------------------------------------------------------------------------------------
void DOS_Run1ms(void)
{
  // call DOS_Run1ms in main while loop 1ms, if we did not use a preemptive os like rt-thread
  Msg_Run1ms();
}
//-----------------------------------------------------------------------------------------------------------
void DOS_Run(void)
{
  /*
    call DOS_Run in main while loop as quickly as possible ,
    if we did not use a preemptive os like rt-thread
  */
  Process_Run();
  DTask_Run();
}