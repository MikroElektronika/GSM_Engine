/****************************************************************************
* Title                 :   GSM ADAPTER
* Filename              :   gsm_adapter.h
* Author                :   MSV
* Origin Date           :   10/03/2016
* Notes                 :   None
*****************************************************************************/
/**************************CHANGE LIST **************************************
*
*    Date    Software Version    Initials   Description
*  10/03/16    XXXXXXXXXXX         MSV      Interface Created.
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

#ifdef GSM1_CLICK
    gsm_pwr_ctl( true );
    Delay_ms( 2500 );
    gsm_pwr_ctl( false );
    Delay_ms( 12500 );
#endif
#ifdef GSM2_CLICK
    gsm_pwr_ctl( true );
    Delay_ms( 100 );
    gsm_pwr_ctl( false );
    Delay_ms( 2500 );
    gsm_pwr_ctl( true );
    Delay_ms( 12500 );
#endif
#ifdef GSM3_CLICK

#endif

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
#ifdef GSM1_CLICK

#endif
#ifdef GSM2_CLICK
    if( tx_input != '\0' )
    {
        tx_buffer[ tx_idx++ ] = tx_input;

    } else {

        if( !exception_f )
        {
            tx_buffer[ tx_idx++ ] = '\r';
            tx_buffer[ tx_idx++ ] = '\n';
            tx_buffer[ tx_idx ]   = '\0';
            
        } else {
        
            tx_buffer[ tx_idx++ ] = 0x1A;
            tx_buffer[ tx_idx++ ] = '\r';
            tx_buffer[ tx_idx ]   = '\0';
        }

        while( !gsm_tx_ctl() );

        gsm_hal_write( tx_buffer );
        tx_idx = 0;

        exception_f = false;
        response_f = false;
        cue_f = true;

        return 1;
    }

    if ( tx_idx == AT_TRANSFER_SIZE )
    {
        tx_buffer[ tx_idx ] = '\0';

        while( !gsm_tx_ctl() );

        gsm_hal_write( tx_buffer );
        tx_idx = 0;

        return 0;
    }
    return 0;
#endif
#ifdef GSM_CLICK_3

#endif
}

void at_adapter_rx( char rx_input )
{
#ifdef GSM1_CLICK

#endif
#ifdef GSM2_CLICK
    if( rx_input == '\r' )
        term_f = true;

    if( rx_input == '\n' && term_f )
    {
        term_f = false;
        frag_f = true;
    }

    if( !frag_f )
    {
        if( !head_f && !data_f && !summ_f )
        {
            if( rx_input == 'A' ) {

                head_f = true;

            } else if( rx_input == 'O' || rx_input == 'E' ||
                       rx_input == 'B' || rx_input == 'C' ||
                       rx_input == 'R' || rx_input == 'N' || rx_input == 'P' ) {

                summ_f = true;

            } else if ( rx_input == '+' || data_t ) {

                data_f = true;
            }
        }

        if( rx_input == '>' )
        {
            exception_f = true;
        }
    }

    if( head_f )
    {
        if( !head_t )
            head_t = true;

        rx_buffer[ rx_idx++ ] = rx_input;
    }

    if( data_f )
    {
        if( !data_t )
        {
            data_t = true;
            data_ptr = &rx_buffer[ rx_idx ];
        }

        rx_buffer[ rx_idx++ ] = rx_input;
    }

    if( summ_f )
    {
        if( !summ_t )
            summ_t = true;

        rx_buffer[ rx_idx++ ] = rx_input;
    }

    if( frag_f )
    {
        if( head_f )
            head_f = false;

        if( data_f )
        {
            while ( *error )
            {
                if( error[ err_c ] == *( data_ptr + err_c++ ) )
                {
                    err_f = true;

                } else {

                    err_f = false;
                    break;
                }
            }

            if( err_f )
            {
                rx_buffer[ rx_idx ] = '\0';
                response_f = true;
            }

            if( !head_t )
            {
                rx_buffer[ rx_idx ] = '\0';
                response_f = true;
            }

            err_c = 0;
            data_f = false;
        }

        if( summ_f )
        {
            rx_buffer[ rx_idx ] = '\0';
            response_f = true;
        }

        frag_f = false;
    }

    if( rx_idx == AT_TRANSFER_SIZE )
    {
        rx_buffer[ rx_idx ] = '\0';
        response_f = true;
    }
#endif
#ifdef GSM3_CLICK

#endif
}
/*************** END OF FUNCTIONS *********************************************/
