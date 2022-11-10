/***************************************************************************//**
  @file     display.h
  @brief    Display Driver
  @author   Olivia De Vincenti
 ******************************************************************************/
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

/*******************************************************************************
 *                      INCLUDE HEADER FILES                                    *
 ******************************************************************************/
#include "gpio.h"

/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/

#define DISPLAY_DEV_MODE

#define BUFFER_MAX_LEN  16
#define DISPLAY_LEN     4

/********** CHAR INDEX **********/
#define IDX_0       0
#define IDX_1       1
#define IDX_2       2
#define IDX_3       3
#define IDX_4       4
#define IDX_5       5
#define IDX_6       6
#define IDX_7       7
#define IDX_8       8
#define IDX_9       9
#define IDX_DASH        10
#define IDX_LOW_DASH    11
#define IDX_POINT       12
#define IDX_CLEAR       13
#define IDX_A     14
#define IDX_a     15
#define IDX_b     16
#define IDX_C     17
#define IDX_d     18
#define IDX_E     19
#define IDX_e     20
#define IDX_F     21
#define IDX_G     22
#define IDX_g     23
#define IDX_H     24
#define IDX_h     25
#define IDX_I     26
#define IDX_J     27
#define IDX_j     28
#define IDX_l     29
#define IDX_L     30
#define IDX_n     31
#define IDX_O     32
#define IDX_P     33
#define IDX_Q     34
#define IDX_r     35
#define IDX_S     36
#define IDX_t     37
#define IDX_U     38
#define IDX_X     39
#define IDX_Y     40

/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/
typedef uint8_t digit_t;
typedef enum {
   BRIGHTNESS_LOW = 0,
   BRIGHTNESS_MEDIUM = 1,
   BRIGHTNESS_HIGH = 2
} brightness_states_t;

/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/

/*******************************************************************************
 *                   FUNCTION PROTOTYPES WITH GLOBAL SCOPE                      *
 ******************************************************************************/

// El display tiene 4 modos:
   // DISPLAY_CLEAR: Limpia el display
   // DISPLAY_SCROLL: El mensaje en buffer se desplazará por el display si tiene más de 4 dígitos
   // DISPLAY_BLINK: Los dígitos indicados con titilarán
   // DISPLAY_STATIC: Se mostraran los dígitos indicados, quietos

/**
 * @brief Inicializa el driver del Display 
 */
void initDisplay();   

/**
 * @brief Setea el modo DISPLAY_CLEAR
 * Limpia el display
 */
void setClearMode();

/**
 * @brief Setea el modo DISPLAY_SCROLL
 * El mensaje en buffer se desplazará por el display si tiene más de 4 dígitos
 */
void setScrollMode();

/**
 * @brief Setea el modo DISPLAY_BLINK
 * Los dígitos indicados con setBlinkingDigits() titilarán
 * Se puede elegir qué parte del mensaje en el buffer mostrar con setBufferIndex() o showLastDigits()
 */
void setBlinkMode();

/**
 * @brief Setea el modo DISPLAY_STATIC
 * Se mostraran los dígitos indicados con setBufferIndex() o showLastDigits()
 */
void setStaticMode();
                       
// Si está en modo DISPLAY_BLINK o DISPLAY_STATIC, llamar con showLastDigits()
/**
 * @brief Carga en el buffer para mostrar en el display un mensaje de entre 1 y 16 dígitos
 * @param msg Mensaje a cargar
 * @param msg_len Longitud del mensaje, si es mayor al tamaño máximo se trunca
 */
void loadBuffer(digit_t *msg, uint8_t msg_len);

/**
 * @brief Configura el display para mostar los últimos 4 dígitos del buffer
 * No aplica al modo DISPLAY_SCROLL
 */
void showLastDigits(bool b);

/**
 * @brief Setea la posición del buffer a partir de la cual se mostrarán los 4 dígitos
 * @param idx Posición en el buffer
 * No aplica al modo DISPLAY_SCROLL
 */
void setBufferIndex(uint8_t idx);

/**
 * @brief Configura los dígitos del display que titilarán en el modo DISPLAY_BLINK
 * @param arr Puntero a un arreglo de 4 bools, que indican si el dígito correspondiente a la posición titila o no
 */
void setBlinkingDigits(bool* arr);

/**
 * @brief Setea el nivel de brillo idicado
 * @param bright Intensidad de brillo, entre 1 y 4
 */
void setBrightness(brightness_states_t bright);

/**
 * @brief Aumenta en 1 el nivel de brillo, siempre que no supere al máximo
 */
void upBrightness();

/**
 * @brief Disminuye en 1 el nivel de brillo, siempre que no sea menor que el mínimo
 */
void downBrightness();

/**
 * @brief Devuelve el nivel de brillo actual del display
 * @return uint8_t brightness, puede ser BRIGHTNESS_LOW(0), BRIGHTNESS_MEDIUM(1) o BRIGHTNESS_HIGH(2)
 */
uint8_t getBrightnessState();
// /**
//  * @brief Muestra en el display un mensaje de longitud variable entre 1 y 16 dígitos
//  * 
//  * @param message puntero al arreglo que contiene el mensaje. ESTE ARREGLO TIENE QUE CONTENER LOS CHAR IDX
//  * @param message_len longitud del mensaje
//  */
// void showMessage(uint8_t* message, uint8_t message_len);

// /**
//  * @brief Muestra en el display los dígitos que se están ingresando
//  * 
//  * @param input dígitos que se van ingresando
//  * @param input_len cantidad de dígitos
//  */
// void showInput(uint8_t input);

// /**
//  * @brief Muestra en el display el dígito indicado en la posición indicada, rellena las posiciones anteriores con '-' (DASH)
//  * 
//  * @param char_idx char idx del caracter a mostrar
//  * @param position posición en la que mostrar el caracter
//  */
// void showPasswordDigit(uint8_t char_idx, uint8_t position);



/*******************************************************************************
 ******************************************************************************/

#endif // _DISPLAY_H_
