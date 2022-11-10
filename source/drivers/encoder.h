/***************************************************************************//**
  @file     encoder.h
  @brief    Encoder Driver
  @author   Micho
 ******************************************************************************/
#ifndef _ENCODER_H_
#define _ENCODER_H_

/*******************************************************************************
 *                      INCLUDE HEADER FILES                                    *
 ******************************************************************************/

#include  <os.h>


/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/

#define ENCODER_DEV_MODE


/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/
typedef enum{
  NONE_ENCODER = 0,
  LEFT,
  RIGHT
}encoderEvent_t;



/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/

/*******************************************************************************
 *                   FUNCTION PROTOTYPES WITH GLOBAL SCOPE                      *
 ******************************************************************************/


void initEncoder();                           //Inicialización del encoder

bool encoderGetStatus();                      //Me devuelve si hubo un cambio en la perilla

bool encoderSetStatus(bool change_state);     //Me setea el status para que no quede en evento

encoderEvent_t encoderGetEvent();             //Me devuelve si fue un giro hacia la derecha, izquierda, click o nada aun

OS_SEM* getEncoderSemPointer();



/*******************************************************************************
 ******************************************************************************/

#endif // _ENCODER_H_
