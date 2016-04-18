/****************************************************************************
* Title                 :   GSM ENGINE
* Filename              :   gsm_engine.c
* Author                :   MSV
* Origin Date           :   10/03/2016
* Notes                 :   None
*****************************************************************************/
/**************************CHANGE LIST **************************************
*
*    Date    Software Version    Initials   Description
*  10/03/16        1.0.0           MSV      Interface Created.
*
*****************************************************************************/
/**
 * @file gsm_engine.c
 * @brief GSM Engine
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "gsm_engine.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
/* TEMPS */
static uint32_t                 temp_timer;
static at_cmd_cb                temp_cb;
/* FLAGS */
volatile bool                   timeout_f;
volatile bool                   exception_f;
volatile bool                   response_f;
volatile bool                   cue_f;
/* CALLBACKS */
static volatile at_cmd_cb       cb;
static volatile at_cmd_cb       cb_default;
/* BUFFERS */
volatile char                   tx_buffer[ AT_TRANSFER_SIZE ];
volatile char                   rx_buffer[ AT_TRANSFER_SIZE ];
/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Private Function Definitions
*******************************************************************************/

/******************************************************************************
* Public Function Definitions
*******************************************************************************/
void gsm_engine_init( at_cmd_cb default_callback )
{
    cb_default = default_callback;
    
    at_adapter_init();
    at_timer_init();
    at_parser_init();

    at_cmd_save( "+CMS ERROR :", DEFAULT_TIMEOUT, default_callback, 
                                                  default_callback, 
                                                  default_callback, 
                                                  default_callback );

    exception_f   = false;
    response_f    = false;
    cue_f         = false;

    memset( ( void* )tx_buffer, 0, AT_TRANSFER_SIZE );
}

void at_cmd( char *input )
{
    char* temp_input = input;

    at_parse( temp_input, &temp_cb, &temp_timer );
    at_timer_load( temp_timer );

    while( cue_f )
        gsm_process();

    while( !at_adapter_tx( *( temp_input++ ) ) );

    at_adapter_reset();
    at_timer_start();
}

void at_cmd_addition( char *input )
{
    char* temp_input = input;

    while( !exception_f )
        gsm_process();

    at_timer_stop();

    while( !at_adapter_tx( *( temp_input++ ) ) );

    at_adapter_reset();
    at_timer_restart();
}

void at_cmd_save( char *cmd,
                  uint32_t timeout,
                  at_cmd_cb getter,
                  at_cmd_cb setter,
                  at_cmd_cb tester,
                  at_cmd_cb executer )
{
    if ( !setter )
        setter = cb_default;

    if( !getter )
        getter = cb_default;

    if( !tester )
        tester = cb_default;

    if( !executer )
        executer = cb_default;

    if( !timeout )
        timeout = DEFAULT_TIMEOUT;

    at_parser_store( cmd, timeout, getter, setter, tester, executer );
}

void gsm_process()
{
    if( response_f )
    {
        gsm_rx_ctl( false );
        at_timer_stop();
        at_parse( rx_buffer, &cb, &temp_timer );
        cb( rx_buffer );
        at_adapter_reset();

        timeout_f = false;
        exception_f = false;
        response_f = false;
        cue_f = false;
    }

    if( timeout_f )
    {
        gsm_rx_ctl( false );
        at_timer_stop();
        at_parse( rx_buffer, &cb, &temp_timer );
        cb( rx_buffer );
        at_adapter_reset();

        timeout_f = false;
        exception_f = false;
        response_f = true;
        cue_f = false;
    }
}
/*************** END OF FUNCTIONS *********************************************/