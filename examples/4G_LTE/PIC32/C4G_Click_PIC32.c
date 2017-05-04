/*
 * Example for 4G LTE click
 *
 * Date         Apr 2017.
 * Author       Milos Vidojevic
 *
 * Test configuration PIC32 :
 *  MCU           :	P32MX795F512L
 *  Dev. Board    :	EasyPIC Fusion v7
 *  SW            :	mikroC PRO for PIC32 v3.6.1
 *
 * Include example.pld to your project.
 *
 ******************************************************************************/

#include "at_engine.h"

#define SMS_RECEIVER 		// phone number which will receive SMS - "\"+xxxxxxxxx\"" format

sbit C4G_STA_PIN              at RB8_bit;
sbit C4G_PWK_PIN              at LATC1_bit;
sbit C4G_RI_PIN               at RD0_bit;
sbit C4G_RTS_PIN              at LATC2_bit;
sbit C4G_CTS_PIN              at RE8_bit;

static bool                      callFlag;
static uint8_t                   buffer[ 1024 ];
static char                      callerId[ 20 ];
static T_AT_storage              storage[ 20 ];

void system_init( void )
{
    AD1PCFG      = 0xFFFF;
    TRISB8_bit = 1;
    TRISC1_bit = 0;
    TRISD0_bit = 1;
    TRISC2_bit = 0;
    TRISE8_bit = 1;

    UART5_Init( 115200 );
    UART2_Init( 57600 );

    T1CON	     = 0x8010;
    T1IP0_bit	 = 1;
    T1IP1_bit	 = 1;
    T1IP2_bit	 = 1;
    T1IF_bit	 = 0;
    T1IE_bit	 = 1;
    PR1		     = 10000;
    TMR1		 = 0;
    U2IP0_bit    = 1;
    U2IP1_bit    = 1;
    U2IP2_bit    = 1;
    U2RXIE_bit   = 1;
    EnableInterrupts();
    LOG( "\r\n_______________________________\r\n System Initialized\r\n\r\n" );
}

void C4G_powerOn( void )
{
    C4G_RTS_PIN = 0;
    C4G_PWK_PIN = 0;
    Delay_ms( 50 );
    C4G_PWK_PIN = 1;
    Delay_ms( 50 );
    C4G_PWK_PIN = 0;
    Delay_ms( 10000 );
}

void C4G_defaultHandler( uint8_t *rsp, uint8_t *flag )
{
    LOG( "Response : \r\n" );
    LOG( rsp );
    LOG( "\r\n" );
    if( !strncmp( "\r\nRING", rsp, 6 ) )
        callFlag = true;
}

void C4G_callerIdHandler( char *rsp, uint8_t *flag )
{
    char *tmpStart;
    char *tmpEnd;

    tmpStart = strchr( rsp, '\"' );
    tmpEnd = strchr( tmpStart + 1, '\"' );
    strncpy( callerId, tmpStart, tmpEnd - tmpStart + 1 );
}

void C4G_replyToCaller()
{
    char tmp[ 30 ];
    char cmdContent[ 30 ];
    char replyContent[ 256 ];

    strcpy( cmdContent, "AT+CMGS=" );
    strcat( cmdContent, SMS_RECEIVER );
    strcpy( replyContent, "4G LTE by MikroE rocks!" );
    AT_cmdDouble( cmdContent, replyContent );
}

void main()
{
    system_init();
    C4G_powerOn();

    AT_initParser( UART3_Write, C4G_defaultHandler, 500, buffer, 1024, storage, 20 );
    AT_saveHandler( "+CLCC", 500, C4G_callerIdHandler );
    AT_cmdSingle( "AT" );
    AT_cmdSingle( "AT+CSCS=\"GSM\"" );
    AT_cmdSingle( "AT+CMGF=1" );
    AT_cmdSingle( "AT+CSCA?" );
    AT_cmdSingle( "AT+UGPIOC?" );

    while( 1 )
    {
        AT_process();
        if( callFlag )
        {
            AT_cmdSingle( "AT+CLCC" );
            AT_cmdSingle( "ATH" );
            Delay_ms( 3000 );                                                   // Delay needed after ATH
            C4G_replyToCaller();
            callFlag = false;
        }
    }
}

/*
 * Timer Interrupt
 */
void Timer1Interrupt() iv IVT_TIMER_1 ilevel 7 ics ICS_SRS 
{
    AT_tick();
    T1IF_bit	 = 0;
}

/*
 * UART Interrupt
 */
void RX_ISR() iv IVT_UART_2 ilevel 7 ics ICS_SRS
{
    if( IFS1 & ( 1 << U2RXIF ))
    {
        char tmp = UART2_Read();
        AT_getChar( tmp );
        U2RXIF_bit = 0;
    }
}

/************************************************************ END OF FILE *****/
