/*
 * Example for PWM click
 *
 * Date         Apr 2017.
 * Author       Milos Vidojevic
 *
 * Test configuration FT900x :
 *  MCU           :        FT900
 *  Dev. Board    :        EasyFT90x
 *  SW            :        mikroC PRO for FT90x v2.2.0
 *
 ******************************************************************************/

#include "at_engine.h"

#define SMS_RECEIVER 		// phone number which will receive SMS - "\"+xxxxxxxxx\"" format

sbit C4G_STA_PIN              at GPIO_PIN9_bit;
sbit C4G_PWK_PIN              at GPIO_PIN1_bit;
sbit C4G_RI_PIN               at GPIO_PIN56_bit;
sbit C4G_RTS_PIN              at GPIO_PIN28_bit;
sbit C4G_CTS_PIN              at GPIO_PIN3_bit;

static bool                      callFlag;
static uint8_t                   buffer[ 1024 ];
static char                      callerId[ 20 ];
static T_AT_storage              storage[ 20 ];

void system_init( void )
{
    GPIO_Digital_Output( &GPIO_PORT_00_07, _GPIO_PINMASK_1 );
    GPIO_Digital_Output( &GPIO_PORT_24_31, _GPIO_PINMASK_4 );
    GPIO_Digital_Input( &GPIO_PORT_00_07, _GPIO_PINMASK_3 );
    GPIO_Digital_Input( &GPIO_PORT_08_15, _GPIO_PINMASK_1 );
    GPIO_Digital_Input( &GPIO_PORT_56_63, _GPIO_PINMASK_0 );

    UART1_Init( 115200 );
    UART2_Init( 115200 );
    
    TIMER_CONTROL_0 = 2;
    TIMER_SELECT = 0;
    TIMER_PRESC_LS = 80;
    TIMER_PRESC_MS = 195;
    TIMER_WRITE_LS = 1;
    TIMER_WRITE_MS = 0;
    TIMER_CONTROL_3 = 0;
    TIMER_CONTROL_4 |= 17;
    TIMER_CONTROL_2 |= 16;
    TIMER_INT |= 2;
    TIMER_CONTROL_1 |= 1;
    UART2_FCR |= ( 1 << FIFO_EN ) | ( 1 << RCVR_RST );
    UART2_INT_ENABLE |= ( 1 << RX_AVL_EN );
    IRQ_CTRL &= ~( 1 << GLOBAL_INTERRUPT_MASK );
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

/*
 * Timer Interrupt
 */
void TimerInterrupt() iv IVT_TIMERS_IRQ
{
    AT_tick();
    if (TIMER_INT_A_bit)
    {
        TIMER_INT = (TIMER_INT & 0xAA) | (1 << 0);
    }
}

/*
 * UART Interrupt
 */
void UART_RX_ISR() iv IVT_UART2_IRQ ics ICS_AUTO
{
    if( UART2_LSR & ( 1 << DATA_RDY ) )
    {
        char tmp = UART2_RBR;
        GPIO_PORT_64_66 = 3;
        AT_getChar(tmp);
    }
}

/************************************************************ END OF FILE *****/
