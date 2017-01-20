/**
 * @file at_engine.h
 * @brief                       AT Engine
 *
 * @defgroup ATE AT Engine
 * @brief Generic Driver for AT command based modules.
 *
 * @{
 *
 * Version : 0.0.2
 * @author Milos Vidojevic
 *
 * ### Revision Log : ###
 *
 * @version
 * 0.0.1 (Apr/2016)  Module created
 *  + RX based engine
 *  + Timer not required
 *  + Write pointer assingment without user interaction
 *  + Internal buffer with fixed size
 *  + Internal handler storage
 *
 * @version
 * 0.0.2 (Dec/2016)  More compatible
 *  + Timer based engine
 *  + Timer requierd
 *  + User provides write (tx) pointer
 *  + External user defined buffer provided drunig initialization
 *
 * ### To Do List : ###
 *
 * @todo
 * Implement Hardware flow control
 *  + Add dynamic enabling of HFC depend on HW implementation
 *  + Cover both possible HFC methods
 *
 * @todo
 * Improve Parser
 *  + Response parsing for now uses "+" as the begin of the command. However
 *      some AT commands starts with "#" or "$" or some other character.
 *
 * @todo
 * Buffer overflow check needed
 *  + In case of data transfers some responses might be bigger than the size of
 * the buffer - this situation is not handled yet.
 *
 * 
 *
 * ### Test configurations : ###
 *
 * + <b>STM32</b> :
 * - MCU            :   STM32F107VC
 * - Dev. Board     :   EasyMx Pro v7
 * - SW             :   MikroC PRO for ARM 4.9.0
 *
 * + <b>AVR</b> :
 * - MCU            :	ATMEGA32
 * - Dev. Board     :	EasyAVR v7
 * - SW             :	mikroC PRO for AVR v6.1.1
 *
 * + <b>PIC</b> :
 * - MCU            :   P18F87K22
 * - Dev. Board     :   EasyPIC PRO v7
 * - SW             :   mikroC PRO for PIC v7.0.0
 *
 * + <b>PIC32</b>
 * - MCU            :	P32MX795F512L
 * - Dev. Board     :	EasyPIC Fusion v7
 * - SW             :	mikroC PRO for PIC32 v3.6.0
 *
 *
 * @example Skywire_ARM.c
 * @example Skywire_PIC.c
 * @example Skywire_PIC32.c
 * @example Skywire_AVR.c
 * @example gsm_gnss_click.c
 */

#ifndef AT_ENGINE_H
#define AT_ENGINE_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

/**
 * @name                 Adjustable Properties
 ******************************************************************************/
///@{
/**
 * Termination characters */
#define AT_TERMINATE                                    0x0D
#define AT_TERMINATE_ADD                                0x1A
/**
 * Default timeout in milliseconds for the specific module */
#define AT_DEFAULT_TIMEOUT                              500
/**
 * Size of command storage. This represent number of commands that can be
 * stored to the module and later recognized by the library. */
#define AT_STORAGE_SIZE                                 50
///@}
/**
 * @name                 Handler prototype
 ******************************************************************************/
///@{
typedef void ( *at_cmd_cb )( char *buffer );
///@}
/**
 * @name                 Write pointer
 ******************************************************************************/
///@{
#if defined( __MIKROC_PRO_FOR_ARM__ )       || \
    defined( __MIKROC_PRO_FOR_DSPIC__ )     || \
    defined( __MIKROC_PRO_FOR_PIC32__ )
typedef void ( *at_write_p )( unsigned int in );
#elif defined( __MIKROC_PRO_FOR_AVR__ )     || \
      defined( __MIKROC_PRO_FOR_PIC__ )     || \
      defined( __MIKROC_PRO_FOR_8051__ )    || \
      defined( __MIKROC_PRO_FOR_FT90x__ )
typedef void ( *at_write_p )( unsigned char data_out );
#endif
///@}
/**
 * @name                 Functions
 ******************************************************************************/
///@{
#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Initialization
 *
 * @param[in] default_callback  default handler
 * @param[in] default_write     UARTx_Write function
 * @param[in] buffer_ptr        pointer to buffer
 * @param[in] buffer_size       size of the buffer in bytes
 *
 * Initialization should be executed before any other function. User can
 * execute this function later inside the application to reset AT Engine to
 * the default state.
 */
void at_init( at_cmd_cb default_callback, at_write_p default_write,
              uint8_t *buffer_ptr, uint16_t buffer_size );

/**
 * @brief Receive
 *
 * @param[in] rx_input      char received from the module
 *
 * Function is used to populate response buffer with characters received from
 * the module. This function should be placed inside UART ISR function.Is also
 * can be used inside function which constatnly poll the UART RX register.
 */
void at_rx( char rx_input );

/**
 * @brief Engine Tick
 *
 * Function must be placed inside the Timer ISR function which will be called
 * every one millisecond.
 */
void at_tick( void );

/**
 * @brief Simple AT Command
 *
 * @param[in] cmd           pointer to command string
 *
 * Function should be used in case of simple AT commands which have no
 * additional arguments expected. Most of the AT Commands uses this function.
 */
void at_cmd_single( char *cmd );

/**
 * @brief Doble AT Command
 *
 * @param[in] cmd           pointer to command string
 * @param[in] arg_1         pointer to the string used as additional argument
 *
 * Function should be used with AT commands which expects additional data after
 * the module responses with specific character. In most cases special character
 * is ">" like in example of AT command for SMS sending.
 */
void at_cmd_double( char *cmd, char *arg_1 );

/**
 * @brief Triple AT Command
 *
 * @param[in] cmd           pointer to command string
 * @param[in] arg_1         pointer to the string used as additional argument
 * @param[in] arg_2         pointer to the second additional argument
 *
 * Function should be used with AT command which excepts two additional
 * parameters after the execution of the command. This kind of AT functions are
 * very rare.
 */
void at_cmd_triple( char *cmd, char *arg_1, char *arg_2 );

/**
 * @brief Save AT Command to Storage
 *
 * @param[in] command       command string
 * @param[in] timeout       timeout for particular command
 * @param[in] getter        handler for GET command
 * @param[in] setter        handler for SET command
 * @param[in] tester        handler for TEST command
 * @param[in] executer      handler for EXECUTE command
 *
 * Saves handlers and timeout for the particular command.
 */
void at_cmd_save( char *cmd, uint32_t timeout,
                  at_cmd_cb getter, at_cmd_cb setter,
                  at_cmd_cb tester, at_cmd_cb executer );

/**
 * @brief AT Engine State Machine
 *
 * This function should be placed inside the infinite while loop.
 */
void at_process( void );

#ifdef __cplusplus
}
#endif
#endif
///@}
/**
 * @}                                                           End of File
 */
