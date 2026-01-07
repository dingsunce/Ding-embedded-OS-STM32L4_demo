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
void DOS_RunOneTick(void)
{
  /*
    call DOS_RunOneTick in main while loop per tick time,
    if we did not embed DOS in a preemptive os like rt-thread
  */
  Msg_RunOneTick();
}
//-----------------------------------------------------------------------------------------------------------
void DOS_Run(void)
{
  /*
    call DOS_Run in main while loop as quickly as possible ,
    if we did not embed DOS in a preemptive os like rt-thread
  */
  Process_Run();
  DTask_Run();
}
//-----------------------------------------------------------------------------------------------------------
void DOS_Exit(void)
{
  DTask_Exit();
  DProcess_Exit();
  Msg_Exit();
  DMem_Exit();
}