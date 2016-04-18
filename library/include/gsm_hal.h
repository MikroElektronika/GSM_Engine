/****************************************************************************
* Title                 :   GSM HAL
* Filename              :   gsm_hal.h
* Author                :   MSV
* Origin Date           :   01/02/2016
* Notes                 :   None
*****************************************************************************/
/**************************CHANGE LIST **************************************
*
*    Date    Software Version    Initials       Description
*  01/02/16       1.0.0             MSV        Module Created.
*
*****************************************************************************/
/**
 * @file gsm_hal.h
 * @brief HAL layer
 *
 * @par
 * HAL layer for GSM engine
 */
#ifndef GSM_HAL_H
#define GSM_HAL_H
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
/******************************************************************************
* Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Configuration Constants
*******************************************************************************/

/******************************************************************************
* Macros
*******************************************************************************/

/******************************************************************************
* Typedefs
*******************************************************************************/

/******************************************************************************
* Variables
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Hardware Reset
 *
 * @par
 * Resets the module via RST PIN
 */
void gsm_hal_reset( void );

/**
 * @brief RTS
 *
 * @par
 * Hardware flow control of sending data
 *
 * @retval RTS pin state
 */
bool gsm_tx_ctl( void );

/**
 * @brief CTS
 *
 * @par
 * Hardware flow control of incoming data
 *
 * @param[in] state of CTS
 */
void gsm_rx_ctl( bool state );

/**
 * @brief HAL Initialization
 *
 * Hal layer initialization. Must be called before any other function.
 */
void gsm_hal_init( void );

/**
 * @brief HAL Write
 *
 * @par
 * Writes data through UART bus
 *
 * @param[in] buffer
 */
void gsm_hal_write( char *buffer );

void gsm_hal_invert( bool invert );

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* GSM_HAL_H_ */
/*** End of File **************************************************************/