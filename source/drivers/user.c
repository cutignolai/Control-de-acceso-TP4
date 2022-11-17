/***************************************************************************//**
  @file     user.h
  @brief    user data base
  @author   Ignacio Cutignola
 ******************************************************************************/

/*******************************************************************************
 *                      INCLUDE HEADER FILES                                    *
 ******************************************************************************/
#include "user.h"

/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/
#define USER_N_INIT		11

/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/


/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/

static user_t user_db[USER_N_INIT];
/*
 = {  user1, 
						user2, 
						user3,
						user4,
						user5,
						cuty,				// TARJETA CUTY
						pedro,				// TARJETA PEDRO
						starbucks,		 	// TARJETA STARBUCKS
						oli,				// TARJETA OLI
						micho,				// TARJETA MICHO (UALA)
						santander_pedro		// TARJETA SANTANDER PEDRO
					};
*/
static uint8_t user_num = USER_N_INIT;
static bool is_init = false;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
bool arr_eq(uint8_t* arr1, uint8_t arr1_len, uint8_t * arr2, uint8_t arr2_len);
void blockUser(uint8_t id[]);

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

void loadDataBase(){

	if (!is_init){
		
		user_t user0 = {.index = 0, .id = {0, 0, 0, 0, 0, 0, 0, 0}, .pass = {0, 0, 0, 0}, .len = 4, .floor = 1, .is_inside = false, .is_blocked = false};
		user_t user1 = {.index = 1, .id = {0, 0, 0, 0, 0, 0, 0, 1}, .pass = {0, 0, 0, 0, 1}, .len = 5, .floor = 1, .is_inside = false, .is_blocked = false};
		user_t user2 = {.index = 2, .id = {1, 2, 3, 4, 0, 0, 0, 0}, .pass = {1, 1, 4, 4}, .len = 4, .floor = 1, .is_inside = false, .is_blocked = false};
		user_t user3 = {.index = 3, .id = {3, 4, 8, 9, 0, 2, 2, 3}, .pass = {2, 3, 9, 1, 0}, .len = 5, .floor = 1, .is_inside = false, .is_blocked = false};
		user_t user4 = {.index = 4, .id = {1, 2, 3, 4, 5, 6, 7, 8}, .pass = {1, 2, 3, 4}, .len = 4, .floor = 1, .is_inside = false, .is_blocked = false};
		user_t cuty = {.index = 5, .id = {2, 0, 6, 0, 2, 0, 9, 0}, .pass = {5, 9, 3, 3, 0}, .len = 5, .floor = 1, .is_inside = false, .is_blocked = false};
		user_t pedro = {.index = 6, .id = {3, 0, 0, 0, 7, 0, 5, 0}, .pass = {5, 9, 5, 0, 2}, .len = 5, .floor = 1, .is_inside = false, .is_blocked = false};
		user_t starbucks = {.index = 7, .id = {9, 0, 6, 0, 6, 0, 4, 0}, .pass = {1, 2, 3, 4}, .len = 4, .floor = 2, .is_inside = false, .is_blocked = false};
		user_t oli = {.index = 8, .id = {3, 0, 4, 0, 9, 0, 5, 0}, .pass = {6, 0, 3, 5, 4}, .len = 5, .floor = 2, .is_inside = false, .is_blocked = false};
		user_t micho = {.index = 9, .id = {1, 0, 4, 0, 7, 0, 6, 0}, .pass = {6, 0, 0, 9, 7}, .len = 5, .floor = 3, .is_inside = false, .is_blocked = false};
		user_t santander_pedro = {.index = 10, .id = {6, 0, 6, 0, 3, 0, 6, 0}, .pass = {7, 4, 2, 6}, .len = 4, .floor = 3, .is_inside = false, .is_blocked = false};

		user_db[0] = user0;
		user_db[1] = user1;
		user_db[2] = user2;
		user_db[3] = user3;
		user_db[4] = user4;
		user_db[5] = cuty;					// TARJETA CUTY
		user_db[6] = pedro;					// TARJETA PEDRO
		user_db[7] = starbucks;				// TARJETA STARBUCKS
		user_db[8] = oli;					// TARJETA OLI
		user_db[9] = micho;					// TARJETA MICHO (UALA)
		user_db[10] = santander_pedro;		// TARJETA SANTANDER PEDRO

		is_init = true;

	}

}

bool checkUser(uint8_t id[], uint8_t pass[], uint8_t pass_len)
{
    bool answer = false;

    uint8_t i;
    for(i = 0; i < user_num; i++)
    {
        if ( !user_db[i].is_blocked && arr_eq(user_db[i].id, MAX_ID, id, MAX_ID) && arr_eq(user_db[i].pass, user_db[i].len, pass, pass_len) )
        {
            answer = true;
            break;
        }
    }

    return answer;

}

uint8_t getUserIndex(uint8_t id[], uint8_t pass[], uint8_t pass_len)
{
	uint8_t i;
	for(i = 0; i < user_num; i++)
	{
		if ( arr_eq(user_db[i].id, MAX_ID, id, MAX_ID) )
		{
			return i;
		}
	}
	return (uint8_t)(-1);
}

void blockUser(uint8_t id[]){
    uint8_t i;
    for (i = 0; i < user_num; i++){
        if (arr_eq(user_db[i].id, MAX_ID, id, MAX_ID)){
            user_db[i].is_blocked = true;
            break;
        }
    }
}

uint8_t getFloorCount(uint8_t floor){
	uint8_t floor_count = 0;
	uint8_t i;
    for (i = 0; i < user_num; i++){
		if (user_db[i].floor == floor && user_db[i].is_inside){
			floor_count++;
		}
	}
	return floor_count;
}

bool changeUserState(uint8_t index){
	user_db[index].is_inside = !user_db[index].is_inside;
	return user_db[index].is_inside;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

bool arr_eq(uint8_t* arr1, uint8_t arr1_len, uint8_t * arr2, uint8_t arr2_len){
	bool r = false;
	uint8_t i;
	if (arr1_len == arr2_len){
		for (i = 0; i < arr1_len; i++){
			if (*(arr1 + i) == *(arr2 + i)){
				r = true;
			} else {
				r = false;
				break;
			}
		}
	}
	return r;
}

/*******************************************************************************
 ******************************************************************************/
