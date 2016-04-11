/******************************************************************************
* Title                 :   AT CONFIGURARTION
* Filename              :   at_config.h
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
 * @file at_config.h
 * @brief GSM Engine and AT Parser Configuration
 */
#ifndef AT_CONFIG_H
#define AT_CONFIG_H
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
/******************************************************************************
* Preprocessor Constants
*******************************************************************************/
/**
 * AT Head */
#define AT_HEAD                                         "AT"
#define AT_HEAD_SIZE                                    2
/**
 * General Error Message */
#define AT_CMS_ERROR                                    "+CMS ERROR:"
#define AT_CMS_ERROR_SIZE                               11
/**
 * Size of AT command. This should represent maximum size of request header */
#define AT_HEADER_SIZE                                  15
/**
 * Size of command storage. This represent number of commands that can be
 * stored to the module and later recognized by the library. */
#define AT_STORAGE_SIZE                                 50
/**
 * Size of RX and TX buffers inside the
 * @link at_buffer.h @endlink
 * This is the maximum amount of characters that can be transfered per one
 * sequence in both directions. */
#define AT_TRANSFER_SIZE                                256
/**
 * Default timeout in milliseconds for the specific module */
#define DEFAULT_TIMEOUT                                 500
/******************************************************************************
* Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

#endif // AT_CONFIG_H
/*******************************End of File ***********************************/
