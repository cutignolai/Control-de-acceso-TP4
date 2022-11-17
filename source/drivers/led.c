/***************************************************************************//**
  @file     led.c
  @brief    FRDM Led Driver
  @author   Olivia De Vincenti
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"
#include "led.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define PIN_LED_RED     PORTNUM2PIN(PB, 22)	  // PTB22
#define PIN_LED_GREEN   PORTNUM2PIN(PE, 26)	  // PTE26
#define PIN_LED_BLUE    PORTNUM2PIN(PB, 21)	  // PTB21
#define LED_ACTIVE      LOW

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static rgb_t rgb;
static bool is_init;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void LedInit(){

	if (!is_init){

		gpioMode(PIN_LED_RED, OUTPUT);
		rgb.red = true;
		gpioMode(PIN_LED_GREEN, OUTPUT);
		rgb.green = true;
		gpioMode(PIN_LED_BLUE, OUTPUT);
		rgb.blue = true;
		LedOff();
		
		is_init = true;
	}
	
}

void LedRGB(rgb_t rgb_on){
	rgb = rgb_on;
}

void LedOn(){
	gpioWrite(PIN_LED_RED, !rgb.red);
	gpioWrite(PIN_LED_GREEN, !rgb.green);
	gpioWrite(PIN_LED_BLUE, !rgb.blue);
}

void LedOff(){
	gpioWrite(PIN_LED_RED, !LED_ACTIVE);
	gpioWrite(PIN_LED_GREEN, !LED_ACTIVE);
	gpioWrite(PIN_LED_BLUE, !LED_ACTIVE);
}

void LedToggle(){
	bool red_state = gpioRead(PIN_LED_RED);
	bool green_state = gpioRead(PIN_LED_GREEN);
	bool blue_state = gpioRead(PIN_LED_BLUE);

	if ((red_state || green_state || blue_state) == LED_ACTIVE){
		LedOff();
	} else {
		LedOn();
	}
}

void LedRedOn(){
	rgb.red = true;
	rgb.green = false;
	rgb.blue = false;
	LedOn();
}

void LedGreenOn(){
	rgb.red = false;
	rgb.green = true;
	rgb.blue = false;
	LedOn();
}

void LedBlueOn(){
	rgb.red = false;
	rgb.green = false;
	rgb.blue = true;
	LedOn();
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/******************************************************************************/
