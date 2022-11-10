/***************************************************************************//**
  @file     button.h
  @brief    Button Driver
  @author   Micho
 ******************************************************************************/
#ifndef _BUTTON_H_
#define _BUTTON_H_

/*******************************************************************************
 *                      INCLUDE HEADER FILES                                    *
 ******************************************************************************/
#include  <os.h>

/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/

#define BUTTON_DEV_MODE
#define PERIODIC_BUTTON_TIME    4     //ms
#define SINGLESHOT_CLICK_TIME   150   //ms
#define PERIODIC_LONG_CLICK_TIME   1  //ms
#define MAX_LONG_CLICK   600 //ms


/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/

typedef enum{
  NONE_CLICK,
  NOT_FOR_ME1,
  NOT_FOR_ME2,
  CLICK,
  CLICK_2,
  CLICK_3,
  CLICK_LONG,
}buttonEvent_t;


/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/

/*******************************************************************************
 *                   FUNCTION PROTOTYPES WITH GLOBAL SCOPE                      *
 ******************************************************************************/


void initButton();                           //Inicialización del encoder

bool buttonGetStatus();                      //Me devuelve si hubo un cambio en la perilla

void buttonSetStatus(bool change_state);     //Me setea el status para que no quede en evento

buttonEvent_t buttonGetEvent();              //Me devuelve si fue un giro hacia la derecha, izquierda, click o nada aun

OS_SEM* getButtonSemPointer();
/*******************************************************************************
 ******************************************************************************/

#endif // _BUTTON_H_
