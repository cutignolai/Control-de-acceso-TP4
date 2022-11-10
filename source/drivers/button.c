/***************************************************************************//**
  @file     button.c
  @brief    Button Driver
  @author   Micho
 ******************************************************************************/

/*******************************************************************************
 *                              INCLUDE HEADER FILES                            *
 ******************************************************************************/

#include "board_encoder_display.h"
#include "button.h"
#include "timer.h"
#include  <os.h>

/*******************************************************************************
 *                 CONSTANT AND MACRO DEFINITIONS USING #DEFINE                 *
 ******************************************************************************/

#define PIN_C   DIO_10
#define ON      LOW         //Activo bajo ---> Prendido
#define OFF     HIGH        //Activo bajo ---> Apagado
#ifdef BUTTON_DEV_MODE
	#define BUTTON_TEST_PIN		PTD2
#endif

/*******************************************************************************
 *                              ENUMS AND STRUCTURES                            *
 ******************************************************************************/


/*******************************************************************************
 *      FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE         *
 ******************************************************************************/

static void event_coming(bool C);
static void callback_click(void);
static void callback_button(void);

/*******************************************************************************
 *                                  VARIABLES                                   *
 ******************************************************************************/
static int click_counter = 0;               // Cantidad de clicks
static bool last_state_button = false;      // el switch arranca en false        
static bool current_C;                      // Valor actual de C
static bool status;                         // Estado del button
static buttonEvent_t turn = NONE_CLICK;
static buttonEvent_t button_event;          // Evento del button
static tim_id_t button_timer;               // timer
static bool click_en;	
static ttick_t click_cnt;                	// timer div
static bool long_click_en;	
static ttick_t long_click_cnt;           	// timer div


//Semáforo del encoder
static OS_SEM semButton;
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initButton() {
	timerInit();                        // Inicializo Timer
	button_timer = timerGetId();

	gpioMode(PIN_C, INPUT);             // Inicializo pin

#ifdef BUTTON_DEV_MODE
	gpioMode(BUTTON_TEST_PIN, OUTPUT);
#endif

    button_event = NONE_CLICK;          // Se inicializa con el evento nulo (que no hay)
    status = false;                     // Variable de cambio en false

    //Periodic Interuption ---> button_callback (1ms)
	button_timer = timerGetId();
	timerStart(button_timer, TIMER_MS2TICKS(PERIODIC_BUTTON_TIME), TIM_MODE_PERIODIC, callback_button);

	//Creamos el semaforo
	OS_ERR os_err;
	OSSemCreate(&semButton, "Sem Button", 0u, &os_err);
}

bool buttonGetStatus(){            //Si hay un evento, devolveme true, sino devolveme un false
	if(status){
		status = false;
		return true;
	}
    else{
		return false;
	}
}

buttonEvent_t buttonGetEvent(){          //Getter del evento del button
	return button_event;
}

void buttonSetStatus(bool change_status){            //Setter para que la app me lo pueda cambiar
	status = change_status;
}

OS_SEM* getButtonSemPointer(void)
{
	return &semButton;
}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void event_coming(bool C){

    // Veo si hubo cambió (flanco descendente)
    bool current_state_button = (current_C == ON);
    if (!last_state_button && current_state_button){	// Si los estados de ambos son distintos, entonces hubo un cambio (se pulso el botton)
        click_counter += 1;
        if (click_counter == 1){		// Primer click
            long_click_en = true;                         
			long_click_cnt = 0;
			click_en = true;
			click_cnt = SINGLESHOT_CLICK_TIME;
        }
        else if (click_counter == 2){	// Segundo click
			click_en = true;
			click_cnt = SINGLESHOT_CLICK_TIME;
        }
        else {							// Tercer click: Maximo de clicks
			click_cnt = 0;
        }
    }

    if (click_counter == 1){  
        if (last_state_button && !current_state_button && long_click_en){     // Si no fue click long, entonces fue click normal
            long_click_en = false;
            long_click_cnt = 0;
        }
    }   

    last_state_button = current_state_button;               // Cambio variable para que quede arriba
}


static void callback_button(void){ 

#ifdef BUTTON_DEV_MODE
	gpioWrite(BUTTON_TEST_PIN, 1);
#endif

	if (click_en){					// Click timer habilitado?
		if (long_click_en){				// Long click timer habilitado?
			long_click_cnt += PERIODIC_BUTTON_TIME;             // Sumo counter del long click
		}
		if (!click_cnt){
			callback_click();
		} else {
			click_cnt--;
		}
	}
    current_C = gpioRead(PIN_C);             // Me fijo el valor actual de C
	event_coming(current_C);                 // Me fijo si hubo un cambio

#ifdef BUTTON_DEV_MODE
	gpioWrite(BUTTON_TEST_PIN, 0);
#endif

}

static void callback_click(void){ 

	OS_ERR os_err;
	click_en = false;			// Desactivo contador

    if (long_click_cnt >= MAX_LONG_CLICK){
        turn = CLICK_LONG;              		// Si se apreto mucho tiempo, tengo un click sostenido
        status = true;                  		// Hubo un cambio
        click_counter = 0;
        long_click_cnt = 0;
        OSSemPost(&semButton, OS_OPT_POST_1, &os_err);
    }

    if (click_counter == 1){      // Si se apreto una vez
        turn = CLICK;
        status = true;            // Hubo un cambio
        click_counter = 0;
        long_click_cnt = 0;
        OSSemPost(&semButton, OS_OPT_POST_1, &os_err);
    }
    else if (click_counter == 2){   // Si se apreto dos veces
        turn = CLICK_2;
        status = true;              // Hubo un cambio
        click_counter = 0;
        long_click_cnt = 0;
        OSSemPost(&semButton, OS_OPT_POST_1, &os_err);
    }
    else if (click_counter >= 3){
        turn = CLICK_3;             // Si se apreto mas de dos veces, se asume como 3
        status = true;              // Hubo un cambio
        click_counter = 0;
        long_click_cnt = 0;
        OSSemPost(&semButton, OS_OPT_POST_1, &os_err);
    }
    button_event = turn;
}

/*******************************************************************************
 ******************************************************************************/
