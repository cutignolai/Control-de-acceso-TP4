#include "hardware.h"
#include <drivers/board.h>
#include <os.h>
#include <stdio.h>
#include <string.h>

/*********************************** LEDS ***********************************/
// Defino el led ROJO
#define LED_R_PORT            PORTB
#define LED_R_GPIO            GPIOB
#define LED_R_PIN             22
#define LED_R_ON()           (LED_R_GPIO->PCOR |= (1 << LED_R_PIN))
#define LED_R_OFF()          (LED_R_GPIO->PSOR |= (1 << LED_R_PIN))
#define LED_R_TOGGLE()       (LED_R_GPIO->PTOR |= (1 << LED_R_PIN))

// Defino el led AZUL
#define LED_B_PORT            PORTB
#define LED_B_GPIO            GPIOB
#define LED_B_PIN             21
#define LED_B_ON()           (LED_B_GPIO->PCOR |= (1 << LED_B_PIN))
#define LED_B_OFF()          (LED_B_GPIO->PSOR |= (1 << LED_B_PIN))
#define LED_B_TOGGLE()       (LED_B_GPIO->PTOR |= (1 << LED_B_PIN))


// Defino el led VERDE
#define LED_G_PORT            PORTE
#define LED_G_GPIO            GPIOE
#define LED_G_PIN             26
#define LED_G_ON()           (LED_G_GPIO->PCOR |= (1 << LED_G_PIN))
#define LED_G_OFF()          (LED_G_GPIO->PSOR |= (1 << LED_G_PIN))
#define LED_G_TOGGLE()       (LED_G_GPIO->PTOR |= (1 << LED_G_PIN))
/*****************************************************************************/

/********************************************************************************************************
 *                          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                                *
 ********************************************************************************************************/
//----------- TASK PRINCIPAL --------------
#define TASK_MAIN_STK_SIZE 		512u
#define TASK_MAIN_PRIO 			2u
static OS_TCB TaskMainTCB;
static CPU_STK TaskMainStk[TASK_MAIN_STK_SIZE];
//-----------------------------------------

// Task 2
#define TASK2_STK_SIZE			256u
#define TASK2_STK_SIZE_LIMIT	(TASK2_STK_SIZE / 10u)
#define TASK2_PRIO              3u
static OS_TCB Task2TCB;
static CPU_STK Task2Stk[TASK2_STK_SIZE];

// Task 3
#define TASK3_STK_SIZE			256u
#define TASK3_STK_SIZE_LIMIT	(TASK3_STK_SIZE / 10u)
#define TASK3_PRIO              3u
static OS_TCB Task3TCB;
static CPU_STK Task3Stk[TASK3_STK_SIZE];



void App_Init (OS_Q* queue);
void App_Run (void);

static OS_Q msgqTest;
/******************************************************************************
 *                                 TASK NUBE                                  *
 ******************************************************************************/
static void Task3(void *p_arg)
{

    (void)p_arg;
    OS_ERR os_err;


    void* p_msg;
    OS_MSG_SIZE msg_size;

    while (1) {
        p_msg = OSQPend(&msgqTest, 0, OS_OPT_PEND_BLOCKING, &msg_size, NULL, &os_err);
        LED_R_TOGGLE();
    }
}




/******************************************************************************
 *                                 TASK ALIVE                                 *
 ******************************************************************************/
static void Task2(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;
    char msg = 'B';

    while (1) {
    	LED_B_TOGGLE();
    	OSTimeDlyHMSM(0u, 0u, 0u, 1, OS_OPT_TIME_HMSM_STRICT, &os_err);
        LED_G_TOGGLE();
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

	// Creo el Task 2
	    OSTaskCreate(&Task2TCB, 			//tcb
	                 "Task 2",				//name
	                  Task2,				//func
	                  0u,					//arg
	                  TASK2_PRIO,			//prio
	                 &Task2Stk[0u],			//stack
	                  TASK2_STK_SIZE_LIMIT,	//stack limit
	                  TASK2_STK_SIZE,		//stack size
	                  0u,
	                  0u,
	                  0u,
	                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
	                 &os_err);

	    // Creo el Task 3
	        OSTaskCreate(&Task3TCB, 			//tcb
	                     "Task 3",				//name
	                      Task3,				//func
	                      0u,					//arg
	                      TASK3_PRIO,			//prio
	                     &Task3Stk[0u],			//stack
	                      TASK3_STK_SIZE_LIMIT,	//stack limit
	                      TASK3_STK_SIZE,		//stack size
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

