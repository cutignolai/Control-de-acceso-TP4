/***************************************************************************//**
  @file     user.h
  @brief    User Database
  @author   Ignacio Cutignola & Olivia De Vincenti
 ******************************************************************************/

/*******************************************************************************
 *                      INCLUDE HEADER FILES                                    *
 ******************************************************************************/
#include <utils/utils.h>
#include "user.h"

/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/

#define USER_MAX_N		250
#define USER_N_INIT		11

/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/


/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/

static user_t user_db[USER_MAX_N];
static uint16_t user_num = USER_N_INIT;
static bool is_init = false;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void loadDatabase(){

	if (!is_init){

		user_t cuty = {.index = 0, .id = {2, 0, 6, 0, 2, 0, 9, 0}, .pass = {5, 9, 3, 3, 0}, .len = 5, .floor = 1, .is_inside = false, .is_blocked = false};
		user_t pedro = {.index = 1, .id = {3, 0, 0, 0, 7, 0, 5, 0}, .pass = {5, 9, 5, 0, 2}, .len = 5, .floor = 1, .is_inside = false, .is_blocked = false};
		user_t starbucks = {.index = 2, .id = {9, 0, 6, 0, 6, 0, 4, 0}, .pass = {1, 2, 3, 4}, .len = 4, .floor = 2, .is_inside = false, .is_blocked = false};
		user_t oli = {.index = 3, .id = {3, 0, 4, 0, 9, 0, 5, 0}, .pass = {6, 0, 3, 5, 4}, .len = 5, .floor = 2, .is_inside = false, .is_blocked = false};
		user_t micho = {.index = 4, .id = {1, 0, 4, 0, 7, 0, 6, 0}, .pass = {6, 0, 0, 9, 7}, .len = 5, .floor = 3, .is_inside = false, .is_blocked = false};
		user_t santander_pedro = {.index = 5, .id = {6, 0, 6, 0, 3, 0, 6, 0}, .pass = {7, 4, 2, 6}, .len = 4, .floor = 3, .is_inside = false, .is_blocked = false};
		user_t user0 = {.index = 6, .id = {0, 0, 0, 0, 0, 0, 0, 0}, .pass = {0, 0, 0, 0}, .len = 4, .floor = 1, .is_inside = false, .is_blocked = false};
		user_t user1 = {.index = 7, .id = {0, 0, 0, 0, 0, 0, 0, 1}, .pass = {0, 0, 0, 0, 1}, .len = 5, .floor = 1, .is_inside = false, .is_blocked = false};
		user_t user2 = {.index = 8, .id = {1, 2, 3, 4, 0, 0, 0, 0}, .pass = {1, 1, 4, 4}, .len = 4, .floor = 1, .is_inside = false, .is_blocked = false};
		user_t user3 = {.index = 9, .id = {3, 4, 8, 9, 0, 2, 2, 3}, .pass = {2, 3, 9, 1, 0}, .len = 5, .floor = 1, .is_inside = false, .is_blocked = false};
		user_t user4 = {.index = 10, .id = {1, 2, 3, 4, 5, 6, 7, 8}, .pass = {1, 2, 3, 4}, .len = 4, .floor = 1, .is_inside = false, .is_blocked = false};

		user_db[0] = cuty;					// TARJETA CUTY
		user_db[1] = pedro;					// TARJETA PEDRO
		user_db[2] = starbucks;				// TARJETA STARBUCKS
		user_db[3] = oli;					// TARJETA OLI
		user_db[4] = micho;					// TARJETA MICHO (UALA)
		user_db[5] = santander_pedro;		// TARJETA SANTANDER PEDRO
		user_db[6] = user0;
		user_db[7] = user1;
		user_db[8] = user2;
		user_db[9] = user3;
		user_db[10] = user4;

		is_init = true;

	}

}

bool checkUser(uint8_t id[], uint8_t pass[], uint8_t pass_len){

    bool answer = false;
    uint8_t i;
    for(i = 0; i < user_num; i++){
        if ( !user_db[i].is_blocked && arr_eq(user_db[i].id, MAX_ID, id, MAX_ID) && arr_eq(user_db[i].pass, user_db[i].len, pass, pass_len) ){
            answer = true;
            break;
        }
    }

    return answer;
}

bool changeUserState(uint8_t index){

	user_db[index].is_inside = !user_db[index].is_inside;
	return user_db[index].is_inside;
}

uint16_t getUserIndex(uint8_t id[])
{
	uint8_t i;
	for(i = 0; i < user_num; i++){
		if ( arr_eq(user_db[i].id, MAX_ID, id, MAX_ID) ){
			return i;
		}
	}
	return (uint16_t)(-1);
}

void blockUser(uint8_t index){
    user_db[index].is_blocked = true;
}

bool addUser(uint8_t* id, uint8_t* pass, uint8_t pass_len, uint8_t floor){
	bool r = false;
	if (user_num < USER_MAX_N){
		user_db[user_num].index = user_num;
		copy_arr(id, &user_db[user_num].id[0], MAX_ID);
		copy_arr(pass, &user_db[user_num].pass[0], MAX_PASS);
		user_db[user_num].len = pass_len;
		user_db[user_num].floor = floor;
		user_db[user_num].is_inside = false;
		user_db[user_num].is_blocked = false;
		user_num++;
		r = true;
	}
	return r;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


/*******************************************************************************
 ******************************************************************************/
