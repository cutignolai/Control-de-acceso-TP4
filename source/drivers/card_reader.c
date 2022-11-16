/***************************************************************************//**
  @file     card_reader.c
  @brief    card reader services
  @authors   Guillermo OM, Pedro DL
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include  <os.h>
#include "MK64F12.h"
#include "gpio.h"
#include "board.h"
#include "card_reader.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
 //Pins from the card reader
#define PIN_CR_DATA      PTB11     //Card reader data entry, LS bit comes out first
#define PIN_CR_CLOCK     PTC11     //Card reader clock entry, data changes on positive edge
#define PIN_CR_ENABLE    PTC10     //Card reader enable, low while card is sliding
#ifdef CARD_DEV_MODE
	#define CARD_TEST_PIN	PTB10
#endif

//Characters from the card
#define SS ';'
#define FS '='
#define ES '?'


#define MAX_BITS 4
#define MAX_DATA 200
#define MAX_TRACK 40
#define MAX_ID 8
#define PAN 16
#define AD_DD 37
#define SPECIAL_CHAR 1

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static ERROR_TYPE error_type;

typedef struct{
    uint8_t parity      :1;
	uint8_t data        :4;
	uint8_t loaded_bits :3;
} card_char;
static card_char current_char;

static char Track[MAX_TRACK]; 
static uint16_t track_index;

static uint16_t stored_ID[MAX_ID];

static uint8_t data[MAX_DATA];
static uint16_t index;

static bool data_pin;

static bool data_was_stored;
static bool volatile enable_interrupt;
static bool SS_arrived;

/*******************************************************************************
 *         FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
// Interruptions
static void irq_enable (void);
static void irq_clk_falling_edge (void);
// Data saving
static void readCard (void);
static void pin2data (bool pin, uint8_t index);
static void write_data (bool pin);
// Data processing
static void orderData (uint8_t pin);
static bool validateParity (void);
static void validate_LRC(uint8_t new_char);
static void add2track (uint8_t new_char);
static void add2ID (uint8_t index, uint8_t new_char);

//Semáforo del encoder
static OS_SEM semCard;
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void initCardReader(void){
    gpioMode(PIN_CR_DATA, INPUT);
    gpioMode(PIN_CR_CLOCK, INPUT);
    gpioMode(PIN_CR_ENABLE, INPUT);
    gpioIRQ(PIN_CR_CLOCK, GPIO_IRQ_MODE_FALLING_EDGE, irq_clk_falling_edge);
    gpioIRQ(PIN_CR_ENABLE, GPIO_IRQ_MODE_BOTH_EDGES, irq_enable);
#ifdef CARD_DEV_MODE
	gpioMode(CARD_TEST_PIN, OUTPUT);
#endif
	OS_ERR os_err;
	OSSemCreate(&semCard, "Sem Card", 0u, &os_err);

}

void resetReader (void){
    error_type = NO_ERROR;
    data_pin = false;
    index = 0;

    data_was_stored = false;
    enable_interrupt = false;
    SS_arrived = false;

    current_char.parity = 0b0;
	current_char.data = 0x0;
	current_char.loaded_bits = 0;

    track_index = 0;

    uint8_t i;
    for (i = 0; i<MAX_ID; i++){
		stored_ID[i] = 0;
	}
    for (i = 0; i<MAX_TRACK; i++){
		Track[i] = '0';
	}
    for (i = 0; i<MAX_DATA; i++){
        data[i] = 0;
    }

}

bool CardReaderIsReady (void){
	 return data_was_stored;
}

ERROR_TYPE getError (void){
	return error_type;
}

uint8_t* getData (void){
    if (getError() == NO_ERROR)
        return &data[0]; 
    else
        return NULL;
}

uint8_t* processData (void){
    track_index = 0;
    current_char.parity = 0b0;
	current_char.data = 0x0;
	current_char.loaded_bits = 0;
    uint8_t data_index;
    for (data_index = 0; data_index < MAX_DATA; data_index++){      //the 200 bits of the array
        orderData(data[data_index]);
    }
    return (uint8_t*) &stored_ID[0];
}



OS_SEM* getCardSemPointer(void)
{
	return &semCard;
}
/*******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static void irq_enable () {
#ifdef CARD_DEV_MODE
	gpioWrite(CARD_TEST_PIN, 1);
#endif

	if ((gpioRead(PIN_CR_ENABLE) == 0) && (!data_was_stored)){		//Falling edge and it's not reading
		enable_interrupt = true;									//avails to read
	}
    else if ((gpioRead(PIN_CR_ENABLE) == 1) && data_was_stored){    //rising edge and it's read
        enable_interrupt = false;	
    }
    else {                                                          //any other case
        enable_interrupt = false;
    }

#ifdef CARD_DEV_MODE
	gpioWrite(CARD_TEST_PIN, 0);
#endif

}
static void irq_clk_falling_edge () {	

#ifdef CARD_DEV_MODE
	gpioWrite(CARD_TEST_PIN, 1);
#endif

    if (enable_interrupt && (!data_was_stored)){
        readCard();
    }

#ifdef CARD_DEV_MODE
	gpioWrite(CARD_TEST_PIN, 0);
#endif

}

static void readCard (void){		//CUANDO PASO LA TARJETA ENTRA MAS DE 200 VECES ACA

    data_pin = !gpioRead(PIN_CR_DATA);				//ACA ESTA EL PROBLEMA, CUANDO LEE NUNCA ES 0
    OS_ERR os_err;
    if(index<MAX_DATA){
        pin2data(data_pin, index);          //thus, it proceeds to read incomming data
    }
    else if (index == MAX_DATA){
        data_was_stored = true;
        OSSemPost(&semCard, OS_OPT_POST_1, &os_err);

    }
    else{
        //do nothing
    }
}

static void pin2data (bool pin, uint8_t index){
    if(!SS_arrived){
        if (pin == true){
        	SS_arrived = true;
            write_data(pin);
        }
    }
    else{       
        write_data(pin);
    }
}

void write_data (bool pin){         
    data[index] = pin;
    index++;
}


static void orderData (uint8_t pin){
    if (current_char.loaded_bits < MAX_BITS){
            if (pin == 1)
                current_char.data |= 0b1<<current_char.loaded_bits;
            else
                current_char.data |= 0b0<<current_char.loaded_bits;
            current_char.loaded_bits++;
        }
        else if (current_char.loaded_bits >= MAX_BITS){
    	    current_char.parity |= pin;
             
    	    if (track_index <= (AD_DD + SPECIAL_CHAR)){
                if (validateParity()){
                    uint8_t new_char = (current_char.data & 0x0F) | '0';
                    add2track(new_char);
                }
                else{
                    error_type = PARITY_ERROR;
                }
            }
            else{                                                       //LRC doesn't check parity!
                uint8_t new_char = (current_char.data & 0x0F) | '0';
                add2track(new_char);
            }
            current_char.data = 0x0;
            current_char.loaded_bits = 0;
            current_char.parity = 0;       
        }
        else{
    	//do nothing
        }
}

static bool validateParity (void){
    uint8_t counter = 0;
    uint8_t zero;                                   //how many zeros does it have?
    uint8_t i;
    for (i = 0; i<MAX_BITS; i++){
        zero = (current_char.data & (1<<i)) & 0x0F;
        if (zero == 0x00)
            counter++;
    }                                           
    if ((!current_char.parity) == (counter%2))
        return true;
    else
        return false;
}

static void validate_LRC(uint8_t new_char){
	uint8_t LRC = current_char.data;
    uint8_t xor_char = 0;
    uint8_t i;
	for (i = 0; i<track_index;i++)
		xor_char ^= Track[i];
	if((uint8_t)(LRC & 0x0F) == (uint8_t)(xor_char & 0x0F))
		Track[(MAX_TRACK-1)] = new_char;
	else{
		error_type = BAD_LRC;
	}
}


static void add2track (uint8_t new_char){
    if (track_index == 0){                      //SS
        if (new_char == SS){
            Track[track_index] = new_char;
            track_index++;
        }          
        else{
            Track[track_index] = new_char;
            track_index++;
            error_type = SS_EXPECTED;
        }
    }  
    else if (track_index <= PAN){                //PAN
        if (new_char >= '0' && new_char <= '9'){
            Track[track_index] = new_char;
            add2ID(track_index, new_char);
            track_index++;
        }
        else{
            Track[track_index] = new_char;
            add2ID(track_index, new_char);
            track_index++;
            error_type = UNEXPECTED_CHARACTER;
        }        
    }
    else if (track_index == (PAN + SPECIAL_CHAR)){                //FS
        if (new_char == FS){
            Track[track_index] = new_char;
            track_index++;
        }          
        else{
            Track[track_index] = new_char;
            track_index++;
            error_type = FS_EXPECTED;
        }
    }
    else if (track_index <= AD_DD){                //AD & DD
        Track[track_index] = new_char;
        track_index++;                
    }
    else if (track_index == (AD_DD + SPECIAL_CHAR)){                //ES
        if (new_char == ES){
            Track[track_index] = new_char;
            track_index++;
        }          
        else{
            Track[track_index] = new_char;
            track_index++;
            error_type = ES_EXPECTED;
        }
    }
    else{                                       //LRC
        validate_LRC(new_char);
        Track[track_index] = new_char;
        track_index++;   
    }
   
}

static void add2ID (uint8_t index, uint8_t new_char){
    if (index > (PAN - MAX_ID)){                                 //takes the last 8 digits of the PAN
        uint8_t converted_char = (new_char - '0');   //Converts from char to uint8_t the current number
        stored_ID[index - (MAX_ID + SPECIAL_CHAR)] = converted_char;
    }
}

void printall(void){
    uint8_t i;
    printf("\n ID: ");
    for (i=0; i<8; i++){
        printf("%d", stored_ID[i]);
    }
    printf("\n Track: ");
    for (i=0; i<40; i++){
        printf("%c", Track[i]);
    }
    printf("\n Error type: %d", getError());
    /*printf("\n Data: ");
    for (i=0; i<200; i++){
        printf("%d", data[i]);
    }*/
}


/*******************************************************************************
 ******************************************************************************/

