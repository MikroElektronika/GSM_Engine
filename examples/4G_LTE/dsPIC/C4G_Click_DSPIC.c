/*
 * Example for 4G LTE click
 *
 * Date         Dec 2016
 * Author       Milos V.
 *
 * Test configuration dsPIC :
 *  MCU           :	P33FJ256GP710A
 *  Dev. Board    :	EasyPIC Fusion v7
 *  SW            :	mikroC PRO for dsPIC v7.0.0
 *
 * Include example.pld file in your project.
 *
 ******************************************************************************/

#include "at_engine.h"

#define SMS_RECEIVER 		// phone number which will receive SMS - "\"+xxxxxxxxx\"" format

sbit C4G_STA_PIN              at RB8_bit;
sbit C4G_PWK_PIN              at LATC1_bit;
sbit C4G_RI_PIN               at RD0_bit;
sbit C4G_RTS_PIN              at LATC2_bit;
//sbit C4G_CTS_PIN              at RE8_bit;

static bool                      callFlag;
static uint8_t                   buffer[ 1024 ];
static char                      callerId[ 20 ];
static T_AT_storage              storage[ 20 ];

void system_init( void )
{
    TRISB8_bit = 1;
    TRISC1_bit = 0;
    TRISD0_bit = 1;
    TRISC2_bit = 0;
    //TRISE8_bit = 1;

    UART2_Init( 9600 );
    UART1_Init( 9600 );
    
    U2RXIF_bit   = 0;
    U2RXIE_bit   = 1;
    T1CON	 = 0x8000;
    T1IE_bit	 = 1;
    T1IF_bit	 = 0;
    IPC0	 = IPC0 | 0x1000;
    PR1		 = 4000;

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

    AT_initParser( UART2_Write, C4G_defaultHandler, 500, buffer, 1024, storage, 20 );
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

void UART_RX() iv IVT_ADDR_U2RXINTERRUPT 
{
    if( U2RXIF_bit )
    {
        char tmp = UART2_Read ();
        AT_getChar(tmp);
        U2RXIF_bit = 0;
    }
}

void Timer1Interrupt() iv IVT_ADDR_T1INTERRUPT
{
    T1IF_bit = 0;
    AT_tick();
}
/************************************************************ END OF FILE *****/
