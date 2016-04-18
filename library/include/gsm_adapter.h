/******************************************************************************
* Title                 :   AT PARSER
* Filename              :   at_parser.h
* Author                :   MSV
* Origin Date           :   10/03/2016
* Notes                 :   None
*******************************************************************************/
/**************************CHANGE LIST ****************************************
*
*    Date    Software Version    Initials   Description
*  10/03/16    XXXXXXXXXXX         MSV      Interface Created.
*
*******************************************************************************/
/**
 * @file at_engine.h
 * @brief AT Engine
 *
 * @par
 * Engine for AT commands.
 */
#ifndef AT_RECEIVER_H
#define AT_RECEIVER_H
/******************************************************************************
* Includes
*******************************************************************************/
#include "gsm_hal.h"
#include "at_config.h"
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
extern volatile bool            exception_f;
extern volatile bool            response_f;
extern volatile bool            cue_f;

extern volatile char            tx_buffer[ AT_TRANSFER_SIZE ];
extern volatile char            rx_buffer[ AT_TRANSFER_SIZE ];
/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief at_receiver_init
 */
void at_adapter_init( void );

/**
 * @brief at_receiver_reset
 */
void at_adapter_reset( void );

/**
 * @brief at_tx
 * @param tx_input
 */
int at_adapter_tx( char tx_input );

/**
 * @brief AT feed
 *
 * This function should be placed inside the UART RX interrupt routine.
 *
 * @param[in] rx_input - character received through UART bus
 */
void at_adapter_rx( char rx_input );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // AT_RECEIVER_H
/*** End of File **************************************************************/
