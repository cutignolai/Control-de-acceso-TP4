/***************************************************************************//**
  @file     utils.h
  @brief    Useful functions to share between modules
  @author   Olivia De Vincenti
 ******************************************************************************/
#ifndef _UTILS_H_
#define _UTILS_H_

/*******************************************************************************
 *                      INCLUDE HEADER FILES                                    *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 *          CONSTANT AND MACRO DEFINITIONS USING #DEFINE                        *
 ******************************************************************************/


/*******************************************************************************
 *               ENUMERATIONS AND STRUCTURES AND TYPEDEFS                       *
 ******************************************************************************/


/*******************************************************************************
 *                  VARIABLE PROTOTYPES WITH GLOBAL SCOPE                       *
 ******************************************************************************/



/*******************************************************************************
 *                   FUNCTION PROTOTYPES WITH GLOBAL SCOPE                      *
 ******************************************************************************/

/**
 * @brief Compares two arrays, checks if they ar equal
 * 
 * @param arr1 Pointer to first byte of array 1
 * @param arr1_len Array 1 length
 * @param arr2 Pointer to first byte of array 2
 * @param arr2_len Array 2 length
 * @return true: Both arrays are equal
 * @return false: Arrays are different
 */
bool arr_eq(uint8_t* arr1, uint8_t arr1_len, uint8_t * arr2, uint8_t arr2_len);

/**
 * @brief Copies an array into another
 * 
 * @param og Pointer to first byte of original array
 * @param copy Pointer to first byte of copy array
 * @param len Length of both arrays
 */
void copy_arr(uint8_t* og, uint8_t* copy, uint8_t len);

/*******************************************************************************
 ******************************************************************************/

#endif // _UTILS_H_
