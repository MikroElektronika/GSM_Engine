/*
 * Example for 4G LTE click
 *
 * Date         Apr 2017
 * Author       Milos V
 *
 * Test configuration PIC :
 *  MCU           :	P18F87K22
 *  Dev. Board    :	EasyPIC PRO v7
 *  SW            :	mikroC PRO for PIC v7.0.0
 *
 * Include example.pld to your project.
 *
 ******************************************************************************/

#include "at_engine.h"

#define SMS_RECEIVER 		// phone number which will receive SMS - "\"+xxxxxxxxx\"" format

sbit C4G_STA_PIN              at RA0_bit;
sbit C4G_PWK_PIN              at LATC0_bit;
sbit C4G_RI_PIN               at RC1_bit;
sbit C4G_RTS_PIN              at LATE0_bit;
sbit C4G_CTS_PIN              at RB0_bit;

static bool                      callFlag;
static uint8_t                   buffer[ 1024 ];
static char                      callerId[ 20 ];
static T_AT_storage              storage[ 20 ];

void LOG(char *x)
{ 
    while( *x != 0 )
        Soft_UART_Write( *x++ );
}

void system_init( void )
{
    TRISA0_bit = 1;
    TRISC0_bit = 0;
    TRISC1_bit = 1;
    TRISE0_bit = 0;
    TRISB0_bit = 1;

    UART1_Init( 115200 );
    Soft_UART_Init( &PORTH, 1, 0, 9600, 0 );

    T1CON	 = 0x01;
    TMR1IF_bit	 = 0;
    TMR1H	 = 0xF0;
    TMR1L	 = 0x60;
    TMR1IE_bit	 = 1;
    INTCON	 = 0xC0;
    RC1IE_bit    = 1;
    RC1IF_bit    = 0;
    PEIE_bit     = 1;
    GIE_bit      = 1;
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

    AT_initParser( UART1_Write, C4G_defaultHandler, 500, buffer, 1024, storage, 20 );
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
void interrupt()
{
   if( RC1IF_bit == 1 )
   {
       char tmp = UART1_Read();
       AT_getChar( tmp );
   }

   if( TMR1IF_bit )
   {
       AT_tick();
       TMR1IF_bit = 0;
       TMR1H	  = 0xF0;
       TMR1L	  = 0x60;
   }
}

/************************************************************ END OF FILE *****/
