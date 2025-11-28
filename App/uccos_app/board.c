
/*!*****************************************************************************
 * file		board.c
 * $Author: sunce.ding
 *******************************************************************************/

#include "os.h"
#include "SysTick.h"

void App_OS_TaskCreateHook(OS_TCB  *p_tcb)
{
  (void)&p_tcb;
}
//-----------------------------------------------------------------------------------------------------------
void App_OS_TaskDelHook(OS_TCB  *p_tcb)
{
  (void)&p_tcb;
}
//-----------------------------------------------------------------------------------------------------------
void App_OS_TaskReturnHook(OS_TCB  *p_tcb)
{
    (void)&p_tcb;
}
//-----------------------------------------------------------------------------------------------------------
void App_OS_IdleTaskHook(void)
{

}
//-----------------------------------------------------------------------------------------------------------
void App_OS_InitHook(void)
{

}
//-----------------------------------------------------------------------------------------------------------
void App_OS_StatTaskHook(void)
{

}
//-----------------------------------------------------------------------------------------------------------
void App_OS_TaskSwHook(void)
{

}
//-----------------------------------------------------------------------------------------------------------
void App_OS_TimeTickHook(void)
{
  SysTick_On();
}
//-----------------------------------------------------------------------------------------------------------
void ucos_set_hook(void)
{
#if OS_CFG_APP_HOOKS_EN > 0u
    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();
    OS_AppTaskCreateHookPtr = App_OS_TaskCreateHook;
    OS_AppTaskDelHookPtr    = App_OS_TaskDelHook;
    OS_AppTaskReturnHookPtr = App_OS_TaskReturnHook;

    OS_AppIdleTaskHookPtr   = App_OS_IdleTaskHook;
    OS_AppStatTaskHookPtr   = App_OS_StatTaskHook;
    OS_AppTaskSwHookPtr     = App_OS_TaskSwHook;
    OS_AppTimeTickHookPtr   = App_OS_TimeTickHook;
    CPU_CRITICAL_EXIT();
#endif
}


