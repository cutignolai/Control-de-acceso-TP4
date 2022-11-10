#include "hardware.h"
#include  <os.h>
#include <stdio.h>
#include <string.h>
#include <drivers/board.h>
#include <drivers/gpio.h>
#include <drivers/timer.h>
#include <drivers/display.h>
#include <drivers/leds.h>
#include <drivers/encoder.h>
#include <drivers/card_reader.h>
#include <drivers/button.h>
#include <drivers/user.h>
#include <drivers/message.h>

/* LEDs */
#define LED_R_PORT            PORTB
#define LED_R_GPIO            GPIOB
#define LED_G_PORT            PORTE
#define LED_G_GPIO            GPIOE
#define LED_B_PORT            PORTB
#define LED_B_GPIO            GPIOB
#define LED_R_PIN             22
#define LED_G_PIN             26
#define LED_B_PIN             21
#define LED_B_ON()           (LED_B_GPIO->PCOR |= (1 << LED_B_PIN))
#define LED_B_OFF()          (LED_B_GPIO->PSOR |= (1 << LED_B_PIN))
#define LED_B_TOGGLE()       (LED_B_GPIO->PTOR |= (1 << LED_B_PIN))
#define LED_G_ON()           (LED_G_GPIO->PCOR |= (1 << LED_G_PIN))
#define LED_G_OFF()          (LED_G_GPIO->PSOR |= (1 << LED_G_PIN))
#define LED_G_TOGGLE()       (LED_G_GPIO->PTOR |= (1 << LED_G_PIN))
#define LED_R_ON()           (LED_R_GPIO->PCOR |= (1 << LED_R_PIN))
#define LED_R_OFF()          (LED_R_GPIO->PSOR |= (1 << LED_R_PIN))
#define LED_R_TOGGLE()       (LED_R_GPIO->PTOR |= (1 << LED_R_PIN))


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

//----------- ESTADOS DEL MENU -----------
typedef enum{
    ESTADO_INIT,
    ESTADO_ID,
    ESTADO_PASS,
    ESTADO_BRILLO,
    ESTADO_VERIFICAR,
    ESTADO_OPEN,
    ESTADO_WRONG
} estadosDelMenu_t;
//----------------------------------------

//----------- EVENTOS DEL MENU -----------
typedef enum{
    EVENTO_NONE,
    EVENTO_IZQ,
    EVENTO_DER,
    EVENTO_CLICK,
    EVENTO_CLICK_2,
    EVENTO_CLICK_3,
    EVENTO_CLICK_LONG,
    EVENTO_TARJETA,
    EVENTO_MSG
} eventosDelMenu_t;
//----------------------------------------

//----------- COLORES DEL LED ------------
typedef enum{
    LED1,
    LED2,
    LED3
} colored_led_t;
//-----------------------------------------


#define MAX_UNIT_ID         8
#define MIN_UNIT_PASS		4
#define MAX_UNIT_PASS       5

#define READY               true
#define NOT_READY           !READY

#define USUARIO_VALIDO      true
#define USUARIO_INVALIDO    !USUARIO_VALIDO

#define ENTER_CLICK         1
#define BRILLO_CLICK        2
#define SUPR_CLICK          3
 
#define SI                  true
#define NO                  false

#define ACTIVADO            true
#define DESACTIVADO         false

#define OPEN_TIME           5000
#define SEC                 1000
#define WRONG_TIME_1        5
#define WRONG_TIME_2        30


/* Task Start */
#define TASKSTART_STK_SIZE 		512u
#define TASKSTART_PRIO 			2u
static OS_TCB TaskStartTCB;
static CPU_STK TaskStartStk[TASKSTART_STK_SIZE];

/* Task 2 */
#define TASK2_STK_SIZE			256u
#define TASK2_STK_SIZE_LIMIT	(TASK2_STK_SIZE / 10u)
#define TASK2_PRIO              3u
static OS_TCB Task2TCB;
static CPU_STK Task2Stk[TASK2_STK_SIZE];

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static estadosDelMenu_t idle(eventosDelMenu_t evento);
static estadosDelMenu_t modificar_id(eventosDelMenu_t evento);
static estadosDelMenu_t modificar_pass(eventosDelMenu_t evento);
static estadosDelMenu_t modificar_brillo(eventosDelMenu_t evento);
static estadosDelMenu_t verificar_estado (void);
static estadosDelMenu_t open_door(void);
static estadosDelMenu_t wrong_pin();
static void reset_all (void);
static void sec_callback(void);
static void show_input(digit_t *input_ptr, uint8_t input_len, uint8_t pos);
static void show_message(digit_t *msg_ptr, uint8_t msg_len);
static void show_pass(digit_t *pass_ptr, uint8_t pass_len);
// static void show_enter(digit_t *input_ptr, uint8_t input_len);
static void show_brightness();
static void id_reset();
static void pass_reset();

/*******************************************************************************
 * VARIABLES
 ******************************************************************************/

// ESTADOS
static estadosDelMenu_t estado = ESTADO_INIT;
static estadosDelMenu_t ultimo_estado = ESTADO_INIT;

// ID 
static uint8_t id[] = {0, 0, 0, 0, 0, 0, 0, 0};
static uint8_t posicion_id = 0;

// CONTRASENA
static uint8_t pass[] = {0, 0, 0, 0, 0};
static uint8_t posicion_pass = 0;

// MESSAGE COMPLETE
static bool ha_hecho_click = NO;
static bool user_is_ready = false;

// TIMERS
static tim_id_t sec_timer;
static uint8_t sec_count;
static uint8_t wrong_count;



/* Example semaphore */
static OS_SEM semTest;
static OS_SEM semCardReader;

//test variables
static uint8_t sem_counter;

static void Task2(void *p_arg) {        //en este ejemplo simboliza al lector de tarjeta
    (void)p_arg;
    OS_ERR os_err;

    while (1) {
        sem_counter++;                  //simula que pregunta si hay algo
        if (sem_counter == 10){         //solo en 1 de cada 10 entradas es que leyó la tarjeta
            OSSemPost(&semCardReader, OS_OPT_POST_1, &os_err);      //habilita al main diciendo que si la leyó
        }       
        OSTimeDlyHMSM(0u, 0u, 0u, 500u, OS_OPT_TIME_HMSM_STRICT, &os_err);  //ojo con los delays
        LED_R_TOGGLE();
    }
}


static void TaskStart(void *p_arg) 
{
    (void)p_arg;
    OS_ERR os_err;

    /* Initialize the uC/CPU Services. */
    CPU_Init();
    /* Initialize the Card Reader Services. */
    initCardReader();
    OSSemCreate(&semCardReader, "Sem Card Reader", 0u, &os_err);

	messageSetStatus(ACTIVADO);
	resetReader();




    while (1) {
        OSTimeDlyHMSM(0u, 0u, 0u, 1000u, OS_OPT_TIME_HMSM_STRICT, &os_err);
        LED_G_TOGGLE();
        OSSemPend(&semCardReader, 0, OS_OPT_PEND_BLOCKING, NULL, &os_err);
        uint8_t* data_ptr;
        data_ptr = processData();       // acá ya es chequear si efectivamente quedó guardado 01234567 en data_ptr

    }
}

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

    OSTaskCreate(&TaskStartTCB,
                 "App Task Start",
                  TaskStart,
                  0u,
                  TASKSTART_PRIO,
                 &TaskStartStk[0u],
                 (TASKSTART_STK_SIZE / 10u),
                  TASKSTART_STK_SIZE,
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
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static estadosDelMenu_t idle(eventosDelMenu_t evento)
{
	digit_t msg[] = {IDX_I, IDX_d, IDX_L, IDX_E};
	estadosDelMenu_t proximo_estado = ESTADO_ID;
	uint8_t *p;

    switch(evento)
    {
        case EVENTO_DER:
        case EVENTO_IZQ:
        case EVENTO_CLICK:

            proximo_estado = ESTADO_ID;
            messageSetStatus(ACTIVADO);

            break;

        case EVENTO_CLICK_2:
        case EVENTO_CLICK_3:

            proximo_estado = ESTADO_BRILLO;
            messageSetStatus(ACTIVADO);

            break;
            

        case EVENTO_TARJETA:
        	p = processData();
            if (getError() == NO_ERROR){
                uint8_t i;
                for (i=0; i<8; i++)
                {
                    id[i] = *(p+i);
                }
                resetReader();
                proximo_estado = ESTADO_PASS;
                posicion_id = 0;
            }
            else{
            	resetReader();
                reset_all();
                proximo_estado = ESTADO_INIT;
            }
			messageSetStatus(ACTIVADO);
            break;
        
        case EVENTO_MSG:
            show_message(&msg[0], 4);
        	messageSetStatus(ACTIVADO);
            break;
        
        default:
            break;

    }

	return proximo_estado;
}

static estadosDelMenu_t modificar_id(eventosDelMenu_t evento)
{
	estadosDelMenu_t proximo_estado = ESTADO_ID;
    digit_t msg[] = {IDX_U, IDX_S, IDX_e, IDX_r};
    uint8_t *p;
    clear_leds();

    switch(evento)
    {
        case EVENTO_DER:
            // Si estoy dentro del rango de digitos max
            if(posicion_id <= MAX_UNIT_ID)
            {
                id[posicion_id] += 1;

                // Si ya alcance el maximo, vuelvo a cero
                if (id[posicion_id] > 9)
                {
                    id[posicion_id] = 0;
                }
                
                show_input(&id[0], posicion_id + 1, posicion_id);
            }
            break;

        case EVENTO_IZQ:
            if(posicion_id <= MAX_UNIT_ID)
            {
                id[posicion_id] -= 1;

                // Si ya alcance el maximo, vuelvo a cero
                if (id[posicion_id] > 9)
                {
                    id[posicion_id] = 9;
                }

                show_input(&id[0], posicion_id + 1, posicion_id);
            }
            break;

        case EVENTO_CLICK:
        case EVENTO_CLICK_LONG:
            
            ha_hecho_click = SI;

            posicion_id += 1;

            if(posicion_id >= MAX_UNIT_ID)
            {
                posicion_id = 0;
                proximo_estado = ESTADO_PASS;
                messageSetStatus(ACTIVADO);
            }

            show_input(&id[0], posicion_id + 1, posicion_id);

            break;

        case EVENTO_CLICK_2:

            if( ha_hecho_click == NO )
            {
                // Guardo el estado actual para luego retomar desde aca
                ultimo_estado = ESTADO_ID;
                proximo_estado = ESTADO_BRILLO;
                messageSetStatus(ACTIVADO);
            }

            else if (ha_hecho_click == SI)
            {
                // Dejo en 0 el digito en donde estaba
                id[posicion_id] = 0;

                // Me ubico en el ultimo digito ingresado y lo pongo en 0
                posicion_id -= 1;
                id[posicion_id] = 0;
                if (!posicion_id) {
                	ha_hecho_click = NO;
                }
            }

            show_input(&id[0], posicion_id + 1, posicion_id);

            break;
        
        case EVENTO_CLICK_3:

            if( ha_hecho_click == SI )
            {
                reset_all();
                proximo_estado = ESTADO_INIT;
                messageSetStatus(ACTIVADO);
            }

            show_input(&id[0], posicion_id + 1, posicion_id);

            break;
            

        case EVENTO_TARJETA:
            p = processData();
            if (getError() == NO_ERROR){
                uint8_t i;
                for (i=0; i<8; i++)
                {
                    id[i] = *(p+i);
                }
                resetReader();
                proximo_estado = ESTADO_PASS;
                posicion_id = 0;
            }
            else{
                reset_all();
                proximo_estado = ESTADO_INIT;
            }
			messageSetStatus(ACTIVADO);
            break;

        case EVENTO_MSG:
            show_message(&msg[0], 4);
            break;
        
        default:
            break;
    
    
    }

	return proximo_estado;
}

static estadosDelMenu_t modificar_pass(eventosDelMenu_t evento)
{
	estadosDelMenu_t proximo_estado = ESTADO_PASS;
    digit_t msg[] = {IDX_P, IDX_A, IDX_S, IDX_S};
    clear_leds();

    switch(evento)
    {
        case EVENTO_DER:
            // Si estoy dentro del rango de digitos max
            if(posicion_pass < MAX_UNIT_PASS)
            {
                pass[posicion_pass] += 1;

                // Si ya alcance el maximo, vuelvo a cero
                if (pass[posicion_pass] > 9)
                {
                    pass[posicion_pass] = 0;
                }

                show_pass(&pass[0], posicion_pass + 1);
            }
            break;

        case EVENTO_IZQ:
            if(posicion_pass < MAX_UNIT_PASS)
            {
                pass[posicion_pass] -= 1;

                // Si ya alcance el maximo, vuelvo a cero
                if (pass[posicion_pass] > 9)
                {
                    pass[posicion_pass] = 9;
                }

                show_pass(&pass[0], posicion_pass + 1);
            }
            break;

      
        case EVENTO_CLICK:
            
            ha_hecho_click = SI;
            {
                posicion_pass += 1;

                if(posicion_pass >= MAX_UNIT_PASS)
                {
                	posicion_pass -= 1;
                    proximo_estado = ESTADO_VERIFICAR;
                    user_is_ready = READY;
                    messageSetStatus(ACTIVADO);
                }
            }

            show_pass(&pass[0], posicion_pass + 1);

            break;

        case EVENTO_CLICK_2:


            if(ha_hecho_click == SI)
            {
                // Dejo en 0 el digito en donde estaba
                pass[posicion_pass] = 0;

                // Me ubico en el ultimo digito ingresado y lo pongo en 0
                posicion_pass -= 1;
                pass[posicion_pass] = 0;

                show_pass(&pass[0], posicion_pass + 1);
            }

            break;

        case EVENTO_CLICK_3:

			reset_all();
			proximo_estado = ESTADO_INIT;

            break;

        case EVENTO_CLICK_LONG:
        	if(posicion_pass >= MIN_UNIT_PASS - 1)
			{
				proximo_estado = ESTADO_VERIFICAR;
				user_is_ready = READY;
	            messageSetStatus(ACTIVADO);
			} else {
				ha_hecho_click = SI;
				posicion_pass += 1;
				show_pass(&pass[0], posicion_pass + 1);
			}
        	break;

        case EVENTO_TARJETA:
            reset_all();
            proximo_estado = ESTADO_INIT;
            //messageSetStatus(ACTIVADO); 
            break;
    
        case EVENTO_MSG:
            show_message(&msg[0], 4);
            break;

        default:
            break;
    
    }

    return proximo_estado;

}	

static estadosDelMenu_t modificar_brillo(eventosDelMenu_t evento)
{
	estadosDelMenu_t proximo_estado = ESTADO_BRILLO;
    digit_t msg[] = {IDX_b, IDX_r, IDX_I, IDX_g, IDX_h, IDX_t, IDX_n, IDX_e, IDX_S, IDX_S};

    switch(evento)
    {
        case EVENTO_DER:

            upBrightness();
            show_brightness();
            break;

        case EVENTO_IZQ:
            
            downBrightness();
            show_brightness();
            break;

        case EVENTO_CLICK:
        case EVENTO_CLICK_LONG:
            
            proximo_estado = ultimo_estado;
            messageSetStatus(ACTIVADO);
            break;

        case EVENTO_MSG:
            show_message(&msg[0], 10);    
            break;
    
        default:
            break;
    }

	return proximo_estado;
}

static estadosDelMenu_t verificar_estado (void)
{
    estadosDelMenu_t proximo_estado = ESTADO_ID;
    
    uint8_t id_char[MAX_UNIT_ID];
    for(int i = 0 ; i < MAX_UNIT_ID ; i++)
    {
    	id_char[i] = (char)(id[i]);
    }

    uint8_t pass_char[posicion_pass + 1];
    for(int i = 0 ; i < posicion_pass + 1; i++)
    {
        pass_char[i] = (char)(pass[i]);
    }

    if( checkUser(id_char, pass_char, posicion_pass + 1) )
    {
		proximo_estado = ESTADO_OPEN;
    } 
    else
    {
    	proximo_estado = ESTADO_WRONG;
        wrong_count += 1;   
    }
    timerActivate(sec_timer);
    
    //reset_all();

    return proximo_estado;
}

static estadosDelMenu_t open_door(void)
{
    estadosDelMenu_t proximo_estado = ESTADO_OPEN;

    digit_t msg[] = {IDX_O, IDX_P, IDX_e, IDX_n};
    show_message(&msg[0], 4);

    set_led(LED3);
    toggle_led(LED1);
    clear_led(LED2);

    if (sec_count >= OPEN_TIME/SEC){
        timerReset(sec_timer);
        reset_all();
        proximo_estado = ESTADO_INIT;
        messageSetStatus(ACTIVADO);
    }

    return proximo_estado;
}

static estadosDelMenu_t wrong_pin()
{
    digit_t msg[] = {IDX_C, IDX_L, IDX_O, IDX_S, IDX_E, IDX_d};
    show_message(&msg[0], 6);

    toggle_led(LED1);
    clear_led(LED3);
    set_led(LED2);

    estadosDelMenu_t proximo_estado = ESTADO_WRONG;
    if ( (wrong_count == 1 && sec_count >= WRONG_TIME_1) || (wrong_count == 2 && sec_count >= WRONG_TIME_2)){
        timerReset(sec_timer);
        pass_reset();
        proximo_estado = ESTADO_PASS;
        messageSetStatus(ACTIVADO);

    } else if ( wrong_count >= 3 ){
    	uint8_t id_char[MAX_UNIT_ID];
		for(int i = 0 ; i < MAX_UNIT_ID ; i++)
		{
			id_char[i] = (uint8_t)(id[i]);
		}
        blockUser(&id_char[0]);
        reset_all();
        proximo_estado = ESTADO_INIT;
        messageSetStatus(ACTIVADO);
    }
    
    return proximo_estado;

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void reset_all (void)
{
    // RESETEO ID
    id_reset();

    // RESETEO PASSWORD
    pass_reset();

    // RESETEO ESTADOS
    estado = ESTADO_INIT;
    ultimo_estado = ESTADO_INIT;

    // RESETEO INTERFAZ
    setClearMode();
    clear_leds();
    ha_hecho_click = NO;
    timerReset(sec_timer);
    sec_count = 0;
    wrong_count = 0;

    messageSetStatus(ACTIVADO);

}

static void id_reset(){
for (int i = 0; i < MAX_UNIT_ID; i++)
    {
        id[i] = 0;
    }   
    posicion_id = 0;
}

static void pass_reset(){
    for (int i = 0; i < MAX_UNIT_PASS; i++)
    {
        pass[i] = 0;
    }   
    posicion_pass = 0;
}


static void sec_callback(void){
    sec_count++;
    messageSetStatus(ACTIVADO);
}

static void show_input(digit_t *input_ptr, uint8_t input_len, uint8_t pos){

    bool blink[] = {false, false, false, false};
    if (pos < DISPLAY_LEN) {
        blink[pos] = true;
    }
    else {
        blink[DISPLAY_LEN-1] = true;
    }

    setBlinkingDigits(&blink[0]);
    showLastDigits(true);
    loadBuffer(input_ptr, input_len);
    setBlinkMode();
}

static void show_message(digit_t *msg_ptr, uint8_t msg_len){

    loadBuffer(msg_ptr, msg_len);
    setScrollMode();
} 

// static void show_enter(digit_t *input_ptr, uint8_t input_len){

// 	loadBuffer(input_ptr, input_len);
//     showLastDigits(true);
//     setStaticMode();
// }

static void show_pass(digit_t *pass_ptr, uint8_t pass_len){

    digit_t pass[] = { IDX_DASH, IDX_DASH, IDX_DASH, IDX_DASH, IDX_DASH };
    pass[pass_len - 1] = *(pass_ptr + pass_len - 1);
    show_input(&pass[0], pass_len, pass_len - 1);

}

static void show_brightness(){
    switch (getBrightnessState()) {
        digit_t bmsg[BRIGHTNESS_HIGH + 1];
    case BRIGHTNESS_LOW:
        bmsg[0] = IDX_b;
        bmsg[1] = BRIGHTNESS_LOW + 1;
        show_message(&bmsg[0], 2);
        clear_led(LED1);
        clear_led(LED2);
        set_led(LED3);
        break;
    
    case BRIGHTNESS_MEDIUM:
        bmsg[0] = IDX_b;
        bmsg[1] = IDX_CLEAR;
        bmsg[2] = BRIGHTNESS_MEDIUM + 1;
        show_message(&bmsg[0], 3);
        clear_led(LED1);
        set_led(LED2);
        set_led(LED3);
        break;

    case BRIGHTNESS_HIGH:
        bmsg[0] = IDX_b;
        bmsg[1] = IDX_CLEAR;
        bmsg[2] = IDX_CLEAR;
        bmsg[3] = BRIGHTNESS_HIGH + 1;
        show_message(&bmsg[0], 4);
        set_led(LED1);
        set_led(LED2);
        set_led(LED3);
        break;

    default:
        break;
    }
}

/*******************************************************************************
 ******************************************************************************/

