/****************************************************************************
* Title                 :   GSM ADAPTER
* Filename              :   gsm_adapter.c
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
 * @file gsm_adapter.c
 * @brief GSM Adapter
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "gsm_adapter.h"
#include "gsm_hal.h"
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
static volatile uint16_t        rx_idx;
static volatile uint16_t        tx_idx;
/* Fragment flags */
static volatile bool            head_f;
static volatile bool            data_f;
static volatile bool            summ_f;
/* Fragment triggers */
static volatile bool            head_t;
static volatile bool            data_t;
static volatile bool            summ_t;
/* Sentence flags */
static volatile bool            term_f;
static volatile bool            frag_f;
/* Error check variables */
static volatile uint8_t         err_c;
static volatile bool            err_f;
static char*                    data_ptr;
static char                     error[ AT_HEADER_SIZE ];
/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Private Function Definitions
*******************************************************************************/

/******************************************************************************
* Public Function Definitions
*******************************************************************************/
void at_adapter_init( void )
{
    gsm_hal_init();
    at_adapter_reset();

    err_c       = 0;
    err_f       = false;

    strcpy( error, ( char* )AT_CMS_ERROR );

    memset( ( void* )tx_buffer, 0, AT_TRANSFER_SIZE );
    memset( ( void* )rx_buffer, 0, AT_TRANSFER_SIZE );
}

void at_adapter_reset( void )
{
    rx_idx      = 0;

    term_f      = false;
    frag_f      = false;

    head_f      = false;
    data_f      = false;
    summ_f      = false;

    head_t      = false;
    data_t      = false;
    summ_t      = false;

    gsm_rx_ctl( true );
}

int at_adapter_tx( char tx_input )
{
    // TX IMPEMENTATION


}

void at_adapter_rx( char rx_input )
{
    // RX IMPLEMENTATION


}

/*************** END OF FUNCTIONS *********************************************/
