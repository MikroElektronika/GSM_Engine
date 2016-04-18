/*******************************************************************************
* Title                 :   GSM HAL
* Filename              :   gsm_hal.c
* Author                :   MSV
* Origin Date           :   01/02/2016
* Notes                 :   None
*******************************************************************************/
/*************** MODULE REVISION LOG ******************************************
*
*    Date    Software Version    Initials       Description
*  01/02/16       1.0.0             MSV        Module Created.
*
*******************************************************************************/
/**
 * @file gsm_hal.c
 * @brief HAL layer
 */
/******************************************************************************
* Includes
*******************************************************************************/
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
#if defined( __MIKROC_PRO_FOR_ARM__ )       || \
    defined( __MIKROC_PRO_FOR_DSPIC__ )     || \
    defined( __MIKROC_PRO_FOR_PIC32__ )
static void         ( *write_uart_p )           ( unsigned int data_out );
static void         ( *write_uart_text_p )      ( unsigned char *text );

#elif defined( __MIKROC_PRO_FOR_AVR__ )     || \
      defined( __MIKROC_PRO_FOR_PIC__ )     || \
      defined( __MIKROC_PRO_FOR_8051__ )    || \
      defined( __MIKROC_PRO_FOR_FT90x__ )
static void         ( *write_uart_p )           ( unsigned char data_out );
static void         ( *write_uart_text_p )      ( unsigned char *text );
#endif

#if defined( __MIKROC_PRO_FOR_ARM__ )     || \
    defined( __MIKROC_PRO_FOR_AVR__ )     || \
    defined( __MIKROC_PRO_FOR_PIC__ )     || \
    defined( __MIKROC_PRO_FOR_PIC32__ )   || \
    defined( __MIKROC_PRO_FOR_DSPIC__ )   || \
    defined( __MIKROC_PRO_FOR_8051__ )    || \
    defined( __MIKROC_PRO_FOR_FT90x__ )
extern sfr sbit GSM_PWR;
extern sfr sbit GSM_RTS;
extern sfr sbit GSM_CTS;
#endif
/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
void gsm_hal_reset( void )
{
#if defined( __MIKROC_PRO_FOR_ARM__ )   || \
    defined( __MIKROC_PRO_FOR_AVR__ )   || \
    defined( __MIKROC_PRO_FOR_DSPIC__ ) || \
    defined( __MIKROC_PRO_FOR_PIC32__ ) || \
    defined( __MIKROC_PRO_FOR_8051__ )  || \
    defined( __MIKROC_PRO_FOR_FT90x__ ) || \
    defined( __MIKROC_PRO_FOR_PIC__ )
    GSM_PWR = 1;
    Delay_ms( 100 );
    GSM_PWR = 0;
    Delay_ms( 2500 );
    GSM_PWR = 1;
    Delay_ms( 100 );
#endif
}

bool gsm_tx_ctl()
{
#if defined( __MIKROC_PRO_FOR_ARM__ )   || \
    defined( __MIKROC_PRO_FOR_AVR__ )   || \
    defined( __MIKROC_PRO_FOR_DSPIC__ ) || \
    defined( __MIKROC_PRO_FOR_PIC32__ ) || \
    defined( __MIKROC_PRO_FOR_8051__ )  || \
    defined( __MIKROC_PRO_FOR_FT90x__ ) || \
    defined( __MIKROC_PRO_FOR_PIC__ )
    if ( GSM_CTS )
        return false;
    else
        return true;
#endif
}

void gsm_rx_ctl( bool state )
{
#if defined( __MIKROC_PRO_FOR_ARM__ )   || \
    defined( __MIKROC_PRO_FOR_AVR__ )   || \
    defined( __MIKROC_PRO_FOR_DSPIC__ ) || \
    defined( __MIKROC_PRO_FOR_PIC32__ ) || \
    defined( __MIKROC_PRO_FOR_8051__ )  || \
    defined( __MIKROC_PRO_FOR_FT90x__ ) || \
    defined( __MIKROC_PRO_FOR_PIC__ )
    if ( state )
        GSM_RTS = 0;
    else
        GSM_RTS = 1;
#endif
}

void gsm_hal_init()
{
#if defined( __MIKROC_PRO_FOR_ARM__ )   || \
    defined( __MIKROC_PRO_FOR_AVR__ )   || \
    defined( __MIKROC_PRO_FOR_DSPIC__ ) || \
    defined( __MIKROC_PRO_FOR_PIC32__ ) || \
    defined( __MIKROC_PRO_FOR_8051__ )  || \
    defined( __MIKROC_PRO_FOR_FT90x__ ) || \
    defined( __MIKROC_PRO_FOR_PIC__ )
    write_uart_p            = UART_Wr_Ptr;
    write_uart_text_p       = UART_Write_Text;
    
    GSM_PWR = 1;
    Delay_ms( 100 );
    GSM_PWR = 0;
    Delay_ms( 2500 );
    GSM_PWR = 1;
    Delay_ms( 12500 );
#endif
    gsm_rx_ctl( true );
}

void gsm_hal_write( char *buffer )
{
#if defined( __MIKROC_PRO_FOR_ARM__ )   || \
    defined( __MIKROC_PRO_FOR_AVR__ )   || \
    defined( __MIKROC_PRO_FOR_DSPIC__ ) || \
    defined( __MIKROC_PRO_FOR_PIC32__ ) || \
    defined( __MIKROC_PRO_FOR_8051__ )  || \
    defined( __MIKROC_PRO_FOR_FT90x__ ) || \
    defined( __MIKROC_PRO_FOR_PIC__ )
    write_uart_text_p( buffer );
#endif
}

/*************** END OF FUNCTIONS ***************************************************************************/