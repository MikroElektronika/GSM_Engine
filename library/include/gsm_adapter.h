/******************************************************************************
* Title                 :   GSM ADAPTER
* Filename              :   gsm_adapter.h
* Author                :   MSV
* Origin Date           :   10/03/2016
* Notes                 :   None
*******************************************************************************/
/**************************CHANGE LIST ****************************************
*
*    Date    Software Version    Initials   Description
*  10/03/16        1.0.0           MSV      Interface Created.
*
*******************************************************************************/
/**
 * @file gsm_adapter.h
 * @brief GSM Adapter
 *
 * @par
 * Adapter for specific GSM module.
 */
#ifndef GSM_ADAPTER_H
#define GSM_ADAPTER_H
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
 * @brief Adapter Init
 *
 * @par
 * Must be called before any usage of adapter functions
 */
void at_adapter_init( void );

/**
 * @brief GSM Adapter Reset
 *
 * @par
 * Resets the receive flags. This function must be executed after any
 * successfull or unsuccessfull transmision on RX.
 */
void at_adapter_reset( void );

/**
 * @brief GSM Transmit
 *
 * @param[in] tx_input - character that is going to be transmiter through the
 * UART bus
 */
int at_adapter_tx( char tx_input );

/**
 * @brief GSM Receive
 *
 * @par
 * This function should be placed inside the UART RX interrupt routine.
 *
 * @param[in] rx_input - character received through UART bus
 */
void at_adapter_rx( char rx_input );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // GSM_ADAPTER_H
/*** End of File **************************************************************/
