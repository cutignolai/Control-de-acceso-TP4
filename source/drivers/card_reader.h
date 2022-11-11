/***************************************************************************//**
  @file     card_reader.h
  @brief    card reader services
  @authors  Guillermo OM, Pedro DL
 ******************************************************************************/

#ifndef _CARD_READER_
#define _CARD_READER_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include  <os.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define CARD_DEV_MODE

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum {NO_ERROR,UNFINISHED_MESSAGE,DATA_AFTER_LRC,SS_EXPECTED,FS_EXPECTED, ES_EXPECTED, PARITY_ERROR, UNEXPECTED_CHARACTER, BAD_LRC, TO_ERROR}ERROR_TYPE;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief Initializes the card reader
 */
void initCardReader(void);

/**
 * @brief Resets the card reader data in case it's necessary
 */
void resetReader (void);

/**
 * @brief Checks if there's some information available regarding the card
 * @return -True if the information is ready
 */
bool CardReaderIsReady (void);

/**
 * @brief Gets which kind of error is registered in the variable error_type by the time this function is called
 * @return The error type
 */
ERROR_TYPE getError(void);

/**
 * @brief Asks for the data stored 
 * @return An array of 200 int in case there is no error stored, NULL otherwise
 */
uint8_t* getData (void);

/**
 * @brief process the 200 bits into the card information
 * @return A pointer to an 8 unsigned ints array corresponding to the ID
 */
uint8_t* processData (void);

OS_SEM* getCardSemPointer(void);

void printall(void);
/*******************************************************************************
 ******************************************************************************/

#endif // _CARD_READER_
