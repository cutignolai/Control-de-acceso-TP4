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
#define MAX_ENTRIES			100
#define MAX_PEOPLE_FLOOR_COUNT		100
#define ENTRIES_INIT		6

/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/

typedef struct{
	uint8_t 	index;
	uint8_t  	id[MAX_ID];
	uint8_t		floor;
	uint8_t		is_inside;
	// uint32_t	timestamp;
} people_t;

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

static bool is_init = false;
static people_t people_db[MAX_ENTRIES];
static uint16_t people_count = ENTRIES_INIT;
static uint16_t entry_count = 0;
static uint16_t floor1_count = 0;
static uint16_t floor2_count = 0;
static uint16_t floor3_count = 0;

static uint16_t* floor_counts[] = { &floor1_count, &floor2_count, &floor3_count };

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void loadDataBase(){

	if (!is_init){

		people_t cuty = {.index = 0, .id = {2, 0, 6, 0, 2, 0, 9, 0}, .floor = 1, .is_inside = false};
		people_t pedro = {.index = 1, .id = {3, 0, 0, 0, 7, 0, 5, 0}, .floor = 1, .is_inside = false};
		people_t starbucks = {.index = 2, .id = {9, 0, 6, 0, 6, 0, 4, 0}, .floor = 2, .is_inside = false};
		people_t oli = {.index = 3, .id = {3, 0, 4, 0, 9, 0, 5, 0}, .floor = 2, .is_inside = false};
		people_t micho = {.index = 4, .id = {1, 0, 4, 0, 7, 0, 6, 0}, .floor = 3, .is_inside = false};
		people_t santander_pedro = {.index = 5, .id = {6, 0, 6, 0, 3, 0, 6, 0}, .floor = 3, .is_inside = false};

		people_db[0] = cuty;				// TARJETA CUTY
		people_db[1] = pedro;				// TARJETA PEDRO
		people_db[2] = starbucks;			// TARJETA STARBUCKS
		people_db[3] = oli;					// TARJETA OLI
		people_db[4] = micho;				// TARJETA MICHO (UALA)
		people_db[5] = santander_pedro;		// TARJETA SANTANDER PEDRO

		is_init = true;

	}
}


bool newEntry(uint8_t index){
	
	bool r = false;
	if ( index < people_count ){
		entry_count++;
		people_db[index].is_inside = !people_db[index].is_inside;
		if (people_db[index].is_inside){
			if (*(floor_counts[people_db[index].floor - 1]) < MAX_PEOPLE_FLOOR_COUNT){
				people_count++;
				*(floor_counts[people_db[index].floor - 1]) = *(floor_counts[people_db[index].floor - 1]) + 1;
			}
		} else {
			if (*(floor_counts[people_db[index].floor - 1])){
				people_count--;
				*(floor_counts[people_db[index].floor - 1]) = *(floor_counts[people_db[index].floor - 1]) - 1;
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
