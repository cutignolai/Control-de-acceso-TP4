/***************************************************************************//**
  @file     user.h
  @brief    User Database
  @author   Ignacio Cutignola & Olivia De Vincenti
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
	bool is_inside;
	bool is_blocked;
} user_t;

/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/



/*******************************************************************************
 *                   FUNCTION PROTOTYPES WITH GLOBAL SCOPE                      *
 ******************************************************************************/

void loadDatabase();
bool checkUser(uint8_t id[], uint8_t pass[], uint8_t pass_len);
bool changeUserState(uint8_t index);
void blockUser(uint8_t index);
uint16_t getUserIndex(uint8_t id[]);
bool addUser(uint8_t* id, uint8_t* pass, uint8_t pass_len, uint8_t floor);

/*******************************************************************************
 ******************************************************************************/

#endif // _USER_H_
