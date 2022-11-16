#include "hardware.h"
#include <drivers/board.h>
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
#define TASK_CLOUD_PRIO              2u
static OS_TCB TASK_CLOUDTCB;
static CPU_STK TASK_CLOUDStk[TASK_CLOUD_STK_SIZE];
//-----------------------------------------

void App_Init (OS_Q* queue);
void App_Run (void);

static OS_Q msgqTest;
/******************************************************************************
 *                                 TASK NUBE                                  *
 ******************************************************************************/
static void TaskCloud(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    void* p_msg;
	OS_MSG_SIZE msg_size;

	while (1) {
		int i = 0;
		p_msg = OSQPend(&msgqTest, 0, OS_OPT_PEND_BLOCKING, &msg_size, NULL, &os_err);

		/*
		char ID = ((char*)p_msg)[0];
		switch(ID)
		{
			case 5:
				LED_R_TOGGLE();
				break;
			case 6: LED_G_TOGGLE(); break;
			case 7: LED_B_TOGGLE(); break;
		}
		*/

		//App_run_cloud();
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

    // Creo el semaforo
	OSQCreate(&msgqTest, "Msg Q Test", 16, &os_err);

	hw_DisableInterrupts();
	App_Init(&msgqTest);
	hw_EnableInterrupts();

	// Creo el Task Cloud
	OSTaskCreate(&TASK_CLOUDTCB, 			//tcb
				 "Task Cloud",				//name
				 TaskCloud,				//func
				  0u,					//arg
				  TASK_CLOUD_PRIO,			//prio
				 &TASK_CLOUDStk[0u],			//stack
				 TASK_CLOUD_STK_SIZE_LIMIT,	//stack limit
				 TASK_CLOUD_STK_SIZE,		//stack size
				  0u,
				  0u,
				  0u,
				 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				 &os_err);



    /********************************************************
     *             		 CORRO EL PROGRAMA 		            *
     ********************************************************/
    while (1) 
    {
        App_Run();
        //OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &os_err);
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


    /* RGB LED */
	SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK);
	LED_B_PORT->PCR[LED_B_PIN] = PORT_PCR_MUX(1);
	LED_G_PORT->PCR[LED_G_PIN] = PORT_PCR_MUX(1);
	LED_R_PORT->PCR[LED_R_PIN] = PORT_PCR_MUX(1);
	LED_B_GPIO->PDDR |= (1 << LED_B_PIN);
	LED_G_GPIO->PDDR |= (1 << LED_G_PIN);
	LED_R_GPIO->PDDR |= (1 << LED_R_PIN);
	LED_B_GPIO->PSOR |= (1 << LED_B_PIN);
	LED_G_GPIO->PSOR |= (1 << LED_G_PIN);
	LED_R_GPIO->PSOR |= (1 << LED_R_PIN);

    OSInit(&err);




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

