/******************************************************************************
* Title                 :   GSM ENGINE
* Filename              :   gsm_engine.h
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
 * @file gsm_engine.h
 * @brief GSM Engine
 *
 * @par
 * Engine for GSM modules.
 */

/**
 * @page LIB_INFO Library Info
 * @date 10 Mar 2016
 * @author Milos Vidojevic
 * @copyright GNU Public License
 * @version 1.0.0 - Initial testing and verification
 */

/**
 * @page TEST_CFG Test Configurations
 * ### Test configuration 1 : ###
 * @par
 * - <b>MCU</b> :             STM32F107VC
 * - <b>Dev. Board</b> :      EasyMx Pro v7
 * - <b>Oscillator</b> :      72 Mhz internal
 * - <b>Ext. Modules</b> :    GSM Click
 * - <b>SW</b> :              MikroC PRO for ARM 4.7.1
 */

/**
 * @mainpage
 * ### General Description ###
 * @par
 * Generic GSM engine for GSM moules.
 */
#ifndef GSM_ENGINE_H
#define GSM_ENGINE_H
/******************************************************************************
* Includes
*******************************************************************************/
#include "at_config.h"
#include "at_parser.h"
#include "at_timer.h"
#include "gsm_adapter.h"
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
 * @brief AT Engine Initialization
 *
 * Initialization of engine should be done before execution of any other
 * function.
 */
void gsm_engine_init( at_cmd_cb default_callback );

/**
 * @brief AT Command
 *
 * @par
 * Executes provided AT command
 *
 * @param[in] input
 */
void at_cmd( char *input );

/**
 * @brief AT addition
 *
 * @par
 * Some AT commands requires additional data, this function is used to add the
 * data after the module resposes with specific output.
 *
 * @param[in] input
 */
void at_cmd_addition( char *input );

/**
 * @brief Save AT Command to Storage
 *
 * @par
 * Saves provided command with callbacks and timeout to the storage for later
 * parsing.
 *
 * @param[in] cmd
 * @param[in] timeout
 * @param[in] setter
 * @param[in] getter
 * @param[in] tester
 * @param[in] executer
 */
void at_cmd_save( char *cmd,
                  uint32_t timeout,
                  at_cmd_cb getter,
                  at_cmd_cb setter,
                  at_cmd_cb tester,
                  at_cmd_cb executer );

/**
 * @brief AT Process
 *
 * This function should be placed inside the infinite while loop.
 */
void gsm_process( void );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // GSM_ENGINE_H
/*******************************End of File ***********************************/
