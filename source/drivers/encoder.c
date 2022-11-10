/***************************************************************************//**
  @file     encoder.c
  @brief    Encoder Driver
  @author   Micho
 ******************************************************************************/

/*******************************************************************************
 *                              INCLUDE HEADER FILES                            *
 ******************************************************************************/

#include "board_encoder_display.h"
#include "encoder.h"
#include "timer.h"


/*******************************************************************************
 *                 CONSTANT AND MACRO DEFINITIONS USING #DEFINE                 *
 ******************************************************************************/

#define PIN_A   DIO_8
#define PIN_B   DIO_6
#ifdef ENCODER_DEV_MODE
	#define ENCODER_TEST_PIN	PTD0
#endif
#define ENCODER_TIME    1

/*******************************************************************************
 *                              ENUMS AND STRUCTURES                            *
 ******************************************************************************/

enum estados {START, LEFT_1, LEFT_2, LEFT_3, RIGHT_1, RIGHT_2, RIGHT_3};           //FSM


/*******************************************************************************
 *      FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE         *
 ******************************************************************************/

static encoderEvent_t event_coming(bool A, bool B);
void callback_encoder(void);

/*******************************************************************************
 *                                  VARIABLES                                   *
 ******************************************************************************/

static bool current_A;                      //Valor actual de A
static bool current_B;                      //Valor actual de B
static bool status;                         //Estado del encoder (para la FSM)
static encoderEvent_t encoder_event;        //Eveneto del encoder
static tim_id_t encoder_timer;              //timer

//Semáforo del encoder
static OS_SEM semEncoder;
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void initEncoder() {
	timerInit();                        // Inicializo Timer
	encoder_timer = timerGetId();

    //Pins Modes//
	gpioMode(PIN_A, INPUT);
	gpioMode(PIN_B, INPUT);

#ifdef ENCODER_DEV_MODE
	gpioMode(ENCODER_TEST_PIN, OUTPUT);
#endif

    encoder_event = NONE_ENCODER;       // Se inicializa con el evento nulo (que no hay)
    status = false;                     // Variable de cambio en falso

    // Periodic Interuption ---> encoder_callback (1ms)
	timerStart(encoder_timer, TIMER_MS2TICKS(ENCODER_TIME), TIM_MODE_PERIODIC, callback_encoder);

	//Creamos el semaforo
	OS_ERR os_err;
	OSSemCreate(&semEncoder, "Sem Encoder", 0u, &os_err);
}

bool encoderGetStatus(){            // Si hay un evento, devolveme true, sino devolveme un false
	if(status){
		status = false;
		return true;
	}
    else{
		return false;
	}
}

encoderEvent_t encoderGetEvent(){          // Getter del evento del encoder
	return encoder_event;
}

bool encoderSetStatus(bool change_state){            // Setter para que la app me lo pueda cambiar
	status = change_state;
	return status;
}


OS_SEM* getEncoderSemPointer(){
	return &semEncoder;
}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static encoderEvent_t event_coming(bool A, bool B){         // FSM: check if the user switch left or right

	static uint8_t current_state = START;

    encoderEvent_t turn = NONE_ENCODER;                             // Todavía no hay giro ni nada

    OS_ERR os_err;

    // Veo si hubo cambió (flanco descendente o ascendente)
    switch(current_state){          // RECORDAR QUE ES ACTIVO BAJO!!!!

        case START:                 // INICIO: DERECHA O IZQUIERDA
                if (A && !B){  
                    current_state = RIGHT_1;        // CON B EN 0 Y A EN 1 VOY PARA LA DERECHA
                }
                if (!A && B){
                    current_state = LEFT_1;         // CON A EN 0 Y B EN 1 VOY PARA LA IZQUIERDA
                }
        break;

        // IZQUIERDA
        case LEFT_1:                // SI SE DETECTÓ QUE GIRO PARA LA IZQUIERDA
                if(!A && !B){
                    current_state = LEFT_2;         // SI AMBOS SON 0, ENTONCES PASO AL PROXIMO ESTADO
                }
                else if(!A && B){
                    // SIGUE EL MISMO ESTADO
                }
                else{
                    current_state = START;          // SI NO TERMINA EL GIRO, VUELVO AL ESTADO INICIAL
                }
        break;
        case LEFT_2:                // SIGUE PARA LA IZQUIERDA
                if(A && !B){
                    current_state = LEFT_3;         // SI B ES 0 Y A ES 1, ENTONCES PASO AL PROXIMO ESTADO
                }
                else if(!A && !B){
                    // SIGUE EL MISMO ESTADO
                }
                else{
                    current_state = START;          // SI NO TERMINA EL GIRO, VUELVO AL ESTADO INICIAL
                }
        break;
        case LEFT_3:                // SIGUE PARA LA IZQUIERDA
                if(A && B){                     
                    current_state = START;          // VOLVI A ESTAR EN 1. QUE LA PERILLA ESTA EN REPOSO, ENTONCES VUELVO AL INICIO
                    turn = LEFT;              // UNA VEZ QUE SE HIZO EL MOVIMIENTO COMPLETO, CONFIRMO QUE ES IZQUIERDA
                    status = true;

                    OSSemPost(&semEncoder, OS_OPT_POST_1, &os_err);
                } 
                else if(A && !B){
                    // SIGUE EL MISMO ESTADO
                }
                else{
                    current_state = START;              // SI NO TERMINA EL GIRO, VUELVO AL ESTADO INICIAL
                }
        break;

        // DERECHA
        case RIGHT_1:               // SI YA SE DETECTÓ QUE GIRO PARA LA DERECHA
                if(!A && !B){
                    current_state = RIGHT_2;        // SI AMBOS SON 0, ENTONCES PASO AL PROXIMO ESTADO
                }
                else if(A && !B){
                    // SIGUE EL MISMO ESTADO
                }
                else{
                    current_state = START;          // SI NO TERMINA EL GIRO, VUELVO AL ESTADO INICIAL
                }
        break;
        case RIGHT_2:                   // SIGUE PARA LA DERECHA
                if(!A && B){
                    current_state = RIGHT_3;        // SI A ES 0 Y B ES 1, ENTONCES PASO AL PROXIMO ESTADO
                }
                else if(!A && !B){
                    //SIGUE EL MISMO ESTADO
                }
                else{
                    current_state = START;         // SI NO TERMINA EL GIRO, VUELVO AL ESTADO INICIAL
                }
        break;
        case RIGHT_3:                   // SIGUE PARA LA DERECHA
                if(A && B){                     // VOLVI A ESTAR EN 1. QUE LA PERILLA ESTA EN REPOSO, ENTONCES VUELVO AL INICIO
                    current_state = START;
                    turn = RIGHT;         // UNA VEZ QUE SE HIZO EL MOVIMIENTO COMPLETO, CONFIRMO QUE ES DERECHA
                    status = true;

                    OSSemPost(&semEncoder, OS_OPT_POST_1, &os_err);

                } 
                else if(!A && B){
                    // SIGUE EL MISMO ESTADO
                }
                else{
                    current_state = START;         // SI NO TERMINA EL GIRO, VUELVO AL ESTADO INICIAL
                }
        break;
        default: break;
    }
    return turn;                              // DEVUELVO EL RESULTADO: IZQ O DERECHA
}


void callback_encoder(void){  

#ifdef ENCODER_DEV_MODE
	gpioWrite(ENCODER_TEST_PIN, 1);
#endif

	current_A = gpioRead(PIN_A);							//Me fijo valores actuales de los pines de A y B
	current_B = gpioRead(PIN_B);
    encoder_event = event_coming(current_A, current_B);     //Me fijo si hubo un cambio en A o en B

#ifdef ENCODER_DEV_MODE
	gpioWrite(ENCODER_TEST_PIN, 0);
#endif

}

/*******************************************************************************
 ******************************************************************************/
