/*
 * Example for 4G LTE Click
 *
 * Date         Apr 2017.
 * Author       Milos V
 *
 * Test configuration KINETIS :
 *  MCU           :        MK64
 *  Dev. Board    :        HEXIWEAR
 *  SW            :        mikroC PRO for ARM v5.0.0
 *
 * Description :
 *    Include example.pld in your project.
 *
 ******************************************************************************/

#include "at_engine.h"

#define SMS_RECEIVER 		// phone number which will receive SMS

sbit C4G_STA_PIN              at PTB_PDIR.B2;
sbit C4G_PWK_PIN              at PTB_PDOR.B11;
sbit C4G_RI_PIN               at PTA_PDIR.B10;
sbit C4G_RTS_PIN              at PTC_PDOR.B4;
sbit C4G_CTS_PIN              at PTB_PDOR.B13;

static bool                      callFlag;
static uint8_t                   buffer[ 1024 ];
static char                      callerId[ 20 ];
static T_AT_storage              storage[ 20 ];

void system_init( void )
{
    GPIO_Digital_Input( &PTB_PDIR, _GPIO_PINMASK_2 );
    GPIO_Digital_Output( &PTB_PDOR, _GPIO_PINMASK_11 );
    GPIO_Digital_Input( &PTA_PDIR, _GPIO_PINMASK_10 );
    GPIO_Digital_Output( &PTC_PDOR, _GPIO_PINMASK_4 );
    GPIO_Digital_Input( &PTB_PDIR, _GPIO_PINMASK_13 );

    UART2_Init_Advanced( 115200, _UART_8_BIT_DATA, _UART_NOPARITY, 
                                 _UART_ONE_STOPBIT, &_GPIO_Module_UART2_PD3_2 );

    UART3_Init_Advanced( 115200, _UART_8_BIT_DATA, _UART_NOPARITY,
                                 _UART_ONE_STOPBIT, &_GPIO_Module_UART3_PC16_17 );
    
    SIM_SCGC6 |= ( 1 << PIT );
    PIT_MCR = 0x00;
    PIT_LDVAL0 = 59999;
    PIT_TCTRL0 |= 2;
    PIT_TCTRL0 |= 1;
    UART2_C2 |= 1 << 5;
    NVIC_IntEnable(IVT_INT_PIT0);
    NVIC_IntEnable(IVT_INT_UART2_RX_TX);
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
    Delay_ms( 100 );
    AT_cmdSingle( "AT+CSCS=\"GSM\"" );
    Delay_ms( 100 );
    AT_cmdSingle( "AT+CMGF=1" );
    Delay_ms( 100 );
    AT_cmdSingle( "AT+CSCA?" );
    Delay_ms( 100 );
    AT_cmdSingle( "AT+UGPIOC?" );
    Delay_ms( 100 );

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
void TICK_ISR() iv IVT_INT_PIT0
{
    PIT_TFLG0.TIF = 1;
    AT_tick();
}

/*
 * UART Interrupt
 */
void RX_ISR() iv IVT_INT_UART2_RX_TX ics ICS_AUTO
{
    if(  UART2_S1 & ( 1 << 5 )  )
    {
        char tmp = UART2_D;
        At_getChar( tmp );
    }
}

/************************************************************ END OF FILE *****/
