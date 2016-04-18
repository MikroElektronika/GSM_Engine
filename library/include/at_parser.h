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
*  10/03/16        1.0.0           MSV      Interface Created.
*
*******************************************************************************/
/**
 * @file at_parser.h
 * @brief AT Parser
 *
 * @par
 * Parser for AT commands.
 */
#ifndef AT_PARSER_H
#define AT_PARSER_H
/******************************************************************************
* Includes
*******************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
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
/**
 * @enum at_ctl_t
 * @brief AT Command Type
 */
typedef enum
{
    /**
     * Unknown command */
    AT_CMD_UNKNOWN                              = 0,
    /**
     * AT Get command */
    AT_CMD_GET                                  = 1,
    /**
     * AT Set command */
    AT_CMD_SET                                  = 2,
    /**
     * AT Test command */
    AT_CMD_TEST                                 = 3,
    /**
     * AT Execute command */
    AT_CMD_EXEC                                 = 4,

}at_type_t;

/**
 * @struct at_cmd_t
 * @brief Parser Structure
 *
 * @par
 * This struct is used for storing the command with timeout and callbacks.
 * Command strings are converted to the hash code for easiest comparision.
 */
typedef struct
{
    /**
     * @brief Command Hash Value */
    uint32_t                hash;
    /**
     * @brief Command Timeout */
    uint32_t                timeout;
    /**
     * @brief Get Callback */
    at_cmd_cb               getter;
    /**
     * @brief Set Callback */
    at_cmd_cb               setter;
    /**
     * @brief Test Callback */
    at_cmd_cb               tester;
    /**
     * @brief Execute Callback */
    at_cmd_cb               executer;

}at_cmd_t;
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
 * @brief AT Parser Initialization
 *
 * @note
 * Must be executed before any other operation.
 */
void at_parser_init();

/**
 * @brief Store Command
 *
 * @par
 * Stores the command and function pointers for command provided.
 *
 * @param[in] command
 * @param[in] timeout
 * @param[in] getter
 * @param[in] setter
 * @param[in] tester
 * @param[in] executer
 */
void at_parser_store( char *command,
                      uint32_t timeout,
                      at_cmd_cb getter,
                      at_cmd_cb setter,
                      at_cmd_cb tester,
                      at_cmd_cb execute );

/**
 * @brief Parse Command
 *
 * @par
 * Parses provided input and sets the callback and timeout for provided input.
 *
 * @param[in] cmd
 * @param[out] cb
 * @param[out] timeout
 */
void at_parse( char *input,
               at_cmd_cb *cb,
               uint32_t *timeout );

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* AT_PARSER_H */
/*** End of File **************************************************************/
