/***************************************************************************//**
  @file     display.h
  @brief    Display Driver
  @author   Olivia De Vincenti
 ******************************************************************************/

/*******************************************************************************
 *                      INCLUDE HEADER FILES                                    *
 ******************************************************************************/
#include "gpio.h"
#include "board_encoder_display.h"
#include "board.h"
#include "display.h"
#include "timer.h"

/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/

#define SEGMENTS        8
#define ON      1
#define OFF     0
#define SEG_ON  1
#define SEG_OFF 0

#define BLINK_T         300
#define SCROLL_T        600
#define MAX_REFRESH_T   1
#define MAX_INTENSITY   4

/******* PINS *******/
#define SEGA    DIO_1
#define SEGB    DIO_3
#define SEGC    DIO_5
#define SEGD    DIO_7
#define SEGE    DIO_9
#define SEGF    DIO_11
#define SEGG    DIO_13
#define SEGDP   DIO_15

#define SEL0    DIO_2
#define SEL1    DIO_4

#define N_PINS  8

#ifdef DISPLAY_DEV_MODE
	#define DISPLAY_TEST_PIN	PTC4
#endif

/**** DIGITS ****/
#define DIG0    0
#define DIG1    1
#define DIG2    2
#define DIG3    3

#define GET_SEL0(x) (x & 1)
#define GET_SEL1(x) ((x >> 1) & 1)

/************ CHAR MASKS ************/
        //     aaaa
        //    f    b     Bits: DP g f e d c b a
        //    f    b
        //     gggg
        //    e    c
        //    e    c
        //     dddd    DP
                          //DPgfedcba
#define NUM_0       0x3F //0b00111111
#define NUM_1       0x06 //0b00000110
#define NUM_2       0x5B //0b01011011
#define NUM_3       0x4F //0b01001111
#define NUM_4       0x66 //0b01100110
#define NUM_5       0x6D //0b01101101
#define NUM_6       0x7D //0b01111101
#define NUM_7       0x07 //0b00000111
#define NUM_8       0x7F //0b01111111
#define NUM_9       0x67 //0b01100111
#define DASH        0x40 //0b01000000
#define LOW_DASH    0x08 //0b00001000
#define POINT       0x80 //0b10000000
#define CLEAR       0x00 //0b00000000
#define LET_A       0x77 //0b01110111
#define LET_a       0x5F //0b01011111
#define LET_b       0x7C //0b01111100
#define LET_C       0x39 //0b00111001
#define LET_d       0x5E //0b01011110
#define LET_E       0x79 //0b01111001
#define LET_e       0x7B //0b01111011
#define LET_F       0x71 //0b01110001
#define LET_G       NUM_6
#define LET_g       0x6F //0b01101111
#define LET_H       0x76 //0b01110110
#define LET_h       0x74 //0b01110100
#define LET_I       NUM_1
#define LET_J       0x1F //0b00011111
#define LET_j       0x1E //0b00011110
#define LET_l       NUM_1
#define LET_L       0x38 //0b00111000
#define LET_n       0x37 //0b00110111
#define LET_O       NUM_0
#define LET_P       0x73 //0b01110011
#define LET_Q       (NUM_0 | POINT)
#define LET_r       0x33 //0b00110011
#define LET_S       NUM_5
#define LET_t       0x78 //0b01111000
#define LET_U       0x3E //0b00111110
#define LET_X       0x75 //0b01110110
#define LET_Y       0x6E //0b01101110

/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/
typedef enum {
    DISPLAY_CLEAR = 0,
    DISPLAY_SCROLL = 1,
    DISPLAY_BLINK = 2,
    DISPLAY_STATIC = 3
} display_states_t;

/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void refresh_display(void);
void set_segment(uint8_t seg, uint8_t set);
void set_display(digit_t arr[]);
void set_digit(digit_t number, uint8_t position);
void set_buffer(digit_t *dig_arr, uint8_t arr_len);
void clear_buffer();
void blink_digits();
void scroll_buffer();
void scroll_message();
void display_blink(digit_t* arr);
void clear_display();
void auto_set_buffer_index();

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
digit_t char_arr[] = {NUM_0, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9, DASH, LOW_DASH, POINT, CLEAR, 
LET_A, LET_a, LET_b, LET_C, LET_d, LET_E, LET_e, LET_F, LET_G, LET_g, LET_H, LET_h, LET_I, LET_J, LET_j, LET_l, LET_L, LET_n, 
LET_O, LET_P, LET_Q, LET_r, LET_S, LET_t, LET_U, LET_X, LET_Y };

uint8_t seg_arr[] = {SEGA, SEGB, SEGC, SEGD, SEGE, SEGF, SEGG, SEGDP};

display_states_t display_state = DISPLAY_CLEAR;
uint8_t display_idx = 0;        // Índice que indica qué dígito del display configurar

digit_t buffer[BUFFER_MAX_LEN + 1] = {IDX_CLEAR, IDX_CLEAR, IDX_CLEAR, IDX_CLEAR, IDX_CLEAR, IDX_CLEAR, IDX_CLEAR, IDX_CLEAR, IDX_CLEAR, IDX_CLEAR, IDX_CLEAR, IDX_CLEAR, IDX_CLEAR, IDX_CLEAR, IDX_CLEAR, IDX_CLEAR, IDX_CLEAR};
uint8_t buffer_len;         // Cantidad de caracteres seteados del buffer
uint8_t buffer_idx = 0;         // Caracter a partir de la cual mostrar

uint8_t brightness = BRIGHTNESS_LOW;
uint8_t brightness_count = 0;
uint8_t intensity = MAX_INTENSITY >> BRIGHTNESS_LOW;

uint8_t scroll_idx;         // Índice que indica el dígito del buffer que se va a mostrar en la posición 1 en ese instante de scroll

bool show_last = false;

bool blink = false;
bool blinking_digits[DISPLAY_LEN] = { false, false, false, false};

// TIMERS
tim_id_t refresh_timer;
tim_id_t blink_timer;
tim_id_t scroll_timer;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
// INIT
void initDisplay(){

	gpioMode(SEL0, OUTPUT);
    gpioMode(SEL1, OUTPUT);
    
    // INIT PINS
    uint8_t i;
    for(i = 0; i < N_PINS; i++){
        gpioMode(seg_arr[i], OUTPUT);
    }

#ifdef DISPLAY_DEV_MODE
	gpioMode(DISPLAY_TEST_PIN, OUTPUT);
#endif

    // CREATE TIMERS
    timerInit();
    refresh_timer = timerGetId();
    timerCreate(refresh_timer, TIMER_MS2TICKS(MAX_REFRESH_T), TIM_MODE_PERIODIC, refresh_display);
    blink_timer = timerGetId();
    timerCreate(blink_timer, TIMER_MS2TICKS(BLINK_T), TIM_MODE_PERIODIC, blink_digits);
    scroll_timer = timerGetId();
    timerCreate(scroll_timer, TIMER_MS2TICKS(SCROLL_T), TIM_MODE_PERIODIC, scroll_buffer);

    setBrightness(BRIGHTNESS_MEDIUM);

    // INIT 7 SEGMENTS
    buffer_len = BUFFER_MAX_LEN;
    clear_buffer();
    buffer[BUFFER_MAX_LEN] = IDX_CLEAR;
    clear_display();

    // START DISPLAY
    timerActivate(refresh_timer);
}                     

void setClearMode(){
    if (display_state != DISPLAY_CLEAR){
        timerReset(blink_timer);
        timerReset(scroll_timer);
    }
    display_state = DISPLAY_CLEAR;
}

void setScrollMode(){
    if (display_state != DISPLAY_SCROLL){
        timerReset(blink_timer);
        timerRestart(scroll_timer);
        scroll_idx = 0;
    }
    display_state = DISPLAY_SCROLL;
}

void setStaticMode(){
    if (display_state != DISPLAY_STATIC){
        timerReset(blink_timer);
        timerReset(scroll_timer);
    }
    display_state = DISPLAY_STATIC;
}

void setBlinkMode(){
    if (display_state != DISPLAY_BLINK){
        timerReset(blink_timer);
        timerReset(scroll_timer);
        blink = false;
        timerRestart(blink_timer);
    }
    display_state = DISPLAY_BLINK;
}

void loadBuffer(digit_t *msg, uint8_t msg_len){
    if (msg_len > BUFFER_MAX_LEN) { msg_len = BUFFER_MAX_LEN; }
    set_buffer(msg, msg_len);
    if (show_last) { auto_set_buffer_index(); }
}

void showLastDigits(bool b){
    show_last = b;
    if (show_last) { auto_set_buffer_index(); }
}

void setBufferIndex(uint8_t idx){
    if (idx < BUFFER_MAX_LEN - DISPLAY_LEN){
        buffer_idx = idx;
    }
}

void setBlinkingDigits(bool* arr){
    uint8_t i;
    for (i = 0; i < DISPLAY_LEN; i++){
        blinking_digits[i] = *(arr + i);
    }
}

void setBrightness(brightness_states_t bright){
    if ( BRIGHTNESS_LOW <= bright && bright <= BRIGHTNESS_HIGH ){
        brightness = bright;
        intensity = MAX_INTENSITY >> brightness;
    }
}

void upBrightness(){
    setBrightness(brightness + 1);
}

void downBrightness(){
    setBrightness(brightness - 1);
}

uint8_t getBrightnessState(){
    return brightness;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void refresh_display()
{

#ifdef DISPLAY_DEV_MODE
	gpioWrite(DISPLAY_TEST_PIN, 1);
#endif

    if( ! ( brightness_count % intensity ) ){

        switch (display_state)
        {
            case DISPLAY_SCROLL:
                if (buffer_len <= DISPLAY_LEN){
                    set_digit(buffer[display_idx], display_idx);
                } else {
                    scroll_message();
                }
                break;

            case DISPLAY_BLINK:
                if (blink && blinking_digits[display_idx]){
                    set_digit(IDX_CLEAR, display_idx);
                } else {
                set_digit(buffer[buffer_idx + display_idx], display_idx);
                }
                break;

            case DISPLAY_STATIC:
                set_digit(buffer[buffer_idx + display_idx], display_idx);
                break;

            case DISPLAY_CLEAR:
            default:
                clear_display();
                break;
        }
    } else {
    	set_digit(IDX_CLEAR, DISPLAY_LEN - 1);
    }
    
	display_idx++;

    if ( display_idx == DISPLAY_LEN){ 
        display_idx = 0; 
        brightness_count++;
        if ( brightness_count >= MAX_INTENSITY ){
            brightness_count = 0;
        }
    }

#ifdef DISPLAY_DEV_MODE
	gpioWrite(DISPLAY_TEST_PIN, 0);
#endif

}

void scroll_message() {

    set_digit(buffer[(scroll_idx + display_idx) % (buffer_len + 1)], display_idx);
    if (scroll_idx == buffer_len + 1){
    	scroll_idx = 0;
    }
    //set_digit(buffer[CLEAR], display_idx);
    if (display_idx == DISPLAY_LEN){
    	scroll_idx++;
    }
}

void clear_display(){
    clear_buffer();
    buffer_idx = 0;
}

void set_display(digit_t* arr){
    uint8_t i;
    for (i = 0; i < DISPLAY_LEN; i++){
        set_digit(*(arr + i), i);
    }
}

void set_digit(digit_t number, uint8_t position){

    digit_t character = char_arr[number];
    
    // Select digit
    gpioWrite(SEL0, GET_SEL0(position));
    gpioWrite(SEL1, GET_SEL1(position));

    // Segments ON
    uint8_t idx;
    for (idx = 0; idx < SEGMENTS; idx++){
        uint8_t set = character & 1;
        set_segment(seg_arr[idx], set);
        character = character >> 1;
    }
}

void set_segment(uint8_t seg, uint8_t set){
    gpioWrite(seg, set);
}

void set_buffer(digit_t *dig_arr, uint8_t arr_len){
    uint8_t i;
    clear_buffer();
    clear_display();
    for (i = 0; i < arr_len; i++){
        buffer[i] = *(dig_arr + i);
    }
    buffer_len = arr_len;
}

void auto_set_buffer_index(){
    if (buffer_len > DISPLAY_LEN){
        buffer_idx = buffer_len - DISPLAY_LEN;
    } else {
        buffer_idx = 0;
    }
}

void clear_buffer(void)
{
    uint8_t i;
    for (i = 0; i < buffer_len; i++){
    	buffer[i] = IDX_CLEAR;
    }
    buffer_len = 0;
}

void display_blink(digit_t* arr){
    uint8_t i;
    for (i = 0; i < DISPLAY_LEN; i++){
        if (blink && blinking_digits[i]){
            set_digit(IDX_CLEAR, i);
        } else {
            set_digit(*(arr + i), i);
        }
    }
}

// callback scroll
void scroll_buffer(){
    scroll_idx++;
}

// callback blink
void blink_digits(){
    blink = !blink;
}

/*******************************************************************************
 ******************************************************************************/
