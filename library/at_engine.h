/*

    at_engine.h

    Copyright (c) 2011-2017 MikroElektronika.  All right reserved.

------------------------------------------------------------------------------*/

/**
 * @file                                            	at_engine.h
 * @brief                     AT Parser
 *
 * @mainpage AT Parser
 *
 * @{
 *
 * ### Library Description ###
 *
 * Generic AT Parser usable on various architectures.
 *
 * ### Features ###
 *
 *    - Timer based engine
 *    - External buffer
 *    - External handler storage
 *    - External defined HFC functions
 *
 * @note In case of hardware flow control (HFC) usage definition of the
 * external RTS/CTS control and check functions is needed.
 *
 * @}
 *
 * @defgroup AT
 * @brief                   AT Parser
 *
 * @{
 *
 * Global Library Prefix : **AT**
 *
 * @example C4G_Click_ARM_KIN.c
 * @example C4G_Click_ARM_STM.c
 * @example C4G_Click_PIC.c
 * @example C4G_Click_PIC32.c
 * @example C4G_Click_DSPIC.c
 *
 ******************************************************************************/

#ifndef _AT_PARSER_H_
#define _AT_PARSER_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

/*-------------------------- HAL POINTERS ------------------------------------*/

#if defined( __MIKROC_PRO_FOR_ARM__ )       || \
    defined( __MIKROC_PRO_FOR_DSPIC__ )     || \
    defined( __MIKROC_PRO_FOR_PIC32__ )
typedef void ( *T_AT_UART_Write )(unsigned int data_out);

#elif defined( __MIKROC_PRO_FOR_AVR__ )     || \
      defined( __MIKROC_PRO_FOR_PIC__ )     || \
      defined( __MIKROC_PRO_FOR_8051__ )    || \
      defined( __MIKROC_PRO_FOR_FT90x__ )
typedef void ( *T_AT_UART_Write )(unsigned char data_out);
#else
typedef void ( *T_AT_UART_Write )(unsigned char data_out);
#endif

/*----------------------------------------------------------------------------*/

/**
 * @name        Configuration defs
 *
 * Configuration preprocessors.
 *
 ******************************************************************************/
///@{

/** End of response time between characters in ms */
#define _AT_ST_TIMER                                5		
/** Max command size excluding command args */
#define _AT_CMD_MAXSIZE                             15		
/** 0 HFC disabled / 1 HFC enabled */
#define _AT_HFC_CONTROL                             0		

///@}
/**
 * @name                 AT Termintation Characters
 * 
 * Termintation characters - must be last character of the command string.
 *
 ******************************************************************************/
///@{

/** Global termintation character */
#define _AT_TERMINATE                               0x0D
/** Special termination character */
#define _AT_TERMINATE_ADD                           0x1A

/**
 * @name                 AT Command Types
 *
 * Command types provided as second argument during the handler call. 
 *
 * @note Use this values to decide which type of command made handler call.
 *
 ******************************************************************************/
///@{

#define _AT_UNKNOWN                                 (0)
#define _AT_TEST                                    (1)
#define _AT_SET                                     (2)
#define _AT_GET                                     (3)
#define _AT_URC                                     (4)
#define _AT_EXEC                                    (5)

///@}
/**
 * @name                 AT Parser Types
 ******************************************************************************/
///@{

/**
 * @typedef Handler Prototype
 */
typedef void ( *T_AT_handler )( char *buffer, uint8_t *type );

/**
 * @brief Parser Structure
 *
 * Struct is used for storing the command with timeout and callbacks.
 * Command strings are converted to the hash code for easiest comparision.
 */
typedef struct
{
    /** Command Length */
    uint16_t                    len;
    /** Command Hash Value */
    uint32_t                    hash;
    /** Command Timeout */
    uint32_t                    timeout;
    /** Get Callback */
    T_AT_handler                handler;

}T_AT_storage;

///@}
/**
 * @name                 AT Driver Functions
 ******************************************************************************/
///@{
#ifdef __cplusplus
extern "C"{
#endif

#if (_AT_HFC_CONTROL)
extern bool AT_getStateDCE();
extern void AT_setStateDTE(bool state);
#endif

/**
 * @brief AT Parser Initialization
 *
 * @param[in] pWrite            UARTx_Write function
 * @param[in] pHandler          default handler
 * @param[in] cmdTimeout        default timeout
 * @param[in] pBuffer           response buffer
 * @param[in] bufferSize        size of the buffer in bytes
 * @param[in] pStorage          handler storage
 * @param[in] storageSize       handler storage size
 *
 * Initialization should be executed before any other function. User can
 * execute this function later inside the application to reset AT Engine to
 * the default state.
 */
void AT_initParser
(
        T_AT_UART_Write     pWrite,
        T_AT_handler        pHandler,
        uint32_t            cmdTimeout,
        uint8_t*            pBuffer,
        uint16_t            bufferSize,
        T_AT_storage*       pStorage,
        uint16_t            storageSize
);

/**
 * @brief Receive
 *
 * @param[in] rxInput       char received from the module
 *
 * Function is used to populate response buffer with characters received from
 * the module. This function should be placed inside UART ISR function.It also
 * can be used inside function which constatnly poll the UART RX register.
 */
void AT_getChar( char rxInput );

/**
 * @brief Engine Tick
 *
 * Function must be placed inside the Timer ISR function which will be called
 * every one millisecond.
 */
void AT_tick();

/**
 * @brief Simple AT Command
 *
 * @param[in] pCmd          pointer to command string
 *
 * Function should be used in case of simple AT commands which have no
 * additional arguments expected. Most of the AT Commands uses this function.
 */
void AT_cmdSingle( char *pCmd );

/**
 * @brief Doble AT Command
 *
 * @param[in] pCmd          pointer to command string
 * @param[in] pArg1         pointer to the string used as additional argument
 *
 * Function should be used with AT commands which expects additional data after
 * the module responses with specific character. In most cases special character
 * is ">" like in example of AT command for SMS sending.
 */
void AT_cmdDouble( char *pCmd, char *pArg1 );

/**
 * @brief Triple AT Command
 *
 * @param[in] pCmd          pointer to command string
 * @param[in] pArg1         pointer to the string used as additional argument
 * @param[in] pArg2         pointer to the second additional argument
 *
 * Function should be used with AT command which excepts two additional
 * parameters after the execution of the command. This kind of AT functions are
 * very rare.
 */
void AT_cmdTriple( char *pCmd, char *pArg1, char *pArg2 );

/**
 * @brief Save AT Command to Storage
 *
 * @param[in] pCmd          command string
 * @param[in] timeout       timeout for provided command
 * @param[in] pHandler      handler for provided command
 *
 * Saves handlers and timeout for the particular command.
 */
int AT_saveHandler( char *pCmd, uint32_t timeout, T_AT_handler pHandler );

/**
 * @brief AT Engine State Machine
 *
 * This function should be placed inside the infinite while loop.
 */
void AT_process();


#ifdef __cplusplus
} // extern "C"
#endif
#endif
///@}
///@}
/*------------------------------------------------------------------------------

  at_engine.h

  Copyright (c) 2011-2017 MikroElektronika.  All right reserved.

    This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.

------------------------------------------------------------------------------*/
