#include "hardware.h"
#include  <os.h>
#include <stdio.h>
#include <string.h>



/********************************************************************************************************
 *                          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                                *
 ********************************************************************************************************/
//----------- TASK PRINCIPAL --------------
#define TASK_MAIN_STK_SIZE 		512u
#define TASK_MAIN_PRIO 			2u
static OS_TCB TaskMainTCB;
static CPU_STK TaskMainStk[TASK_MAIN_STK_SIZE];
//-----------------------------------------

//-------------- TASK NUBE ----------------
#define TASK_CLOUD_STK_SIZE			256u
#define TASK_CLOUD_STK_SIZE_LIMIT	(TASK_CLOUD_STK_SIZE / 10u)
#define TASK_CLOUD_PRIO              3u
static OS_TCB TASK_CLOUDTCB;
static CPU_STK TASK_CLOUDStk[TASK_CLOUD_STK_SIZE];
//-----------------------------------------

void App_Init (void);
void App_Run (void);


/******************************************************************************
 *                                 TASK NUBE                                  *
 ******************************************************************************/
static void TaskCloud(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    while (1) {
    }
}

/******************************************************************************
 *                                 TASK MAIN                                  *
 ******************************************************************************/
static void TaskMain(void *p_arg) 
{
    (void)p_arg;
    OS_ERR os_err;

    /********************************************************
     *             		 INICILIZACION          		    *
     ********************************************************/
    // Inicializo la CPU 
    CPU_Init();



    /********************************************************
     *             		 CORRO EL PROGRAMA 		            *
     ********************************************************/
    while (1) 
    {
        App_Run();
    }
}















/******************************************************************************
 *                                 COMIENZO                                   *
 ******************************************************************************/
int main(void) 
{
    OS_ERR err;

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR  cpu_err;
#endif

    hw_Init();

    OSInit(&err);

    hw_DisableInterrupts();
	App_Init();
	hw_EnableInterrupts();


 #if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
	 /* Enable task round robin. */
	 OSSchedRoundRobinCfg((CPU_BOOLEAN)1, 0, &err);
 #endif
    OS_CPU_SysTickInit(SystemCoreClock / (uint32_t)OSCfg_TickRate_Hz);

    OSTaskCreate(&TaskMainTCB,
                 "App Task Main",
                  TaskMain,
                  0u,
                  TASK_MAIN_PRIO,
                 &TaskMainStk[0u],
                 (TASK_MAIN_STK_SIZE / 10u),
                  TASK_MAIN_STK_SIZE,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 &err);

    OSStart(&err);

	/* Should Never Get Here */
    while (1) {

    }
}




/*******************************************************************************
 ******************************************************************************/

