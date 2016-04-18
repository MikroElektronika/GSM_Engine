/******************************************************************************
* Title                 :   AT TIMER
* Filename              :   at_timer.h
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
 * @file at_tmier.h
 * @brief AT Timer
 *
 * @par
 * Timer for GSM engine
 */
#ifndef AT_TIMER_H
#define AT_TIMER_H
/******************************************************************************
* Includes
*******************************************************************************/
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
extern volatile bool                            timeout_f;
/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief AT Timer Initialisation
 *
 * @par
 * Must be executed before any other timer operation
 */
void at_timer_init( void );

/**
 * @brief Tick ISR
 *
 * @par
 * This must be placed inside the ISR function that is going to be executed
 * every one millisecond
 */
void at_tick_isr( void );

/**
 * @brief AT Timer Start
 *
 * @par
 * Starts the timer
 */
void at_timer_start( void );

/**
 * @brief AT Timer Restart
 *
 * @par
 * Restart the timer
 */
void at_timer_restart( void );

/**
 * @brief AT Timer Stop
 *
 * @par
 * Stops the timer
 */
void at_timer_stop( void );

/**
 * @brief AT Timer Load
 *
 * @par
 * Load new timer
 *
 * @param[in] timeout ( milliseconds)
 */
void at_timer_load( uint32_t timeout );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // AT_SUB_ENGINE_H
/*******************************End of File ***********************************/
