/***************************************************************************//**
  @file     button.h
  @brief    Button Driver
  @author   Ignacio Cutignola
 ******************************************************************************/
#ifndef _USER_H_
#define _USER_H_

/*******************************************************************************
 *                      INCLUDE HEADER FILES                                    *
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/

#define MAX_ID         8
#define MAX_PASS       5



/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/
typedef struct {
	uint8_t index;
	uint8_t	id[MAX_ID];
	uint8_t	pass[MAX_PASS];
	uint8_t len;
	uint8_t	floor;
	bool	is_inside;
	bool	is_blocked;
} user_t;

/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/



/*******************************************************************************
 *                   FUNCTION PROTOTYPES WITH GLOBAL SCOPE                      *
 ******************************************************************************/

bool checkUser(uint8_t id[], uint8_t pass[], uint8_t pass_len);
void blockUser(uint8_t id[]);
uint8_t getUserIndex(uint8_t id[], uint8_t pass[], uint8_t pass_len);
bool changeUserState(uint8_t index);
uint8_t getFloorCount(uint8_t floor);

/*******************************************************************************
 ******************************************************************************/

#endif // _USER_H_
