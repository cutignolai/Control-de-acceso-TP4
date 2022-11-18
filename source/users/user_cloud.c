/***************************************************************************//**
  @file     user_cloud.c
  @brief    User data base for cloud process
  @author   Olivia De Vincenti
 ******************************************************************************/

/*******************************************************************************
 *                      INCLUDE HEADER FILES                                    *
 ******************************************************************************/
#include "user_cloud.h"
#include <utils/utils.h>

/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/
#define MAX_ENTRIES			250
#define MAX_FLOOR_COUNT		100
#define ENTRIES_INIT		0

/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/

typedef struct{
	uint16_t 	entry_nr;
	uint8_t  	id[MAX_ID];
	uint8_t		floor;
	uint8_t		is_inside;
	// uint32_t	timestamp;
} entry_t;

/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
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

static entry_t entry_db[MAX_ENTRIES];
static uint16_t entry_count = ENTRIES_INIT;
static uint16_t people_count = 0;
static uint16_t floor1_count = 0;
static uint16_t floor2_count = 0;
static uint16_t floor3_count = 0;

static uint16_t* floor_counts[] = { &floor1_count, &floor2_count, &floor3_count };

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool newEntry(share_user_t* user){
	bool r = false;
	if (entry_count < MAX_ENTRIES){
		entry_db[entry_count].entry_nr = entry_count;
		copy_arr(&entry_db[entry_count].id[0], user->id, MAX_ID);
		entry_db[entry_count].floor = user->floor;
		entry_db[entry_count].is_inside = user->is_inside;
		entry_count++;
		if (user->is_inside){
			if (*(floor_counts[user->floor - 1]) < MAX_FLOOR_COUNT){
				people_count++;
				*(floor_counts[user->floor - 1]) = *(floor_counts[user->floor - 1]) + 1;
			}
		} else {
			if (*(floor_counts[user->floor - 1])){
				people_count--;
				*(floor_counts[user->floor - 1]) = *(floor_counts[user->floor - 1]) - 1;
			}
		}
		r = true;
	}
	return r;
}

uint16_t getFloorCount(uint8_t floor){
	return *(floor_counts[floor - 1]);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
