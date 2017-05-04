/*
 * Example for 4G LTE click
 *
 * Date         Apr 2017.
 * Author       Milos Vidojevic
 *
 * Test configuration STM32 :
 *  MCU           :        STM32F107VCT6
 *  Dev. Board    :        EasyMx PRO v7 for STM32 ARM
 *  SW            :        mikroC PRO for ARM v5.0.0
 *
 * Include example.pld to your project.
 *
 ******************************************************************************/

#include "at_engine.h"

#define SMS_RECEIVER 		// phone number which will receive SMS - "\"+xxxxxxxxx\"" format

sbit C4G_STA_PIN              at GPIOA_IDR.B4;
sbit C4G_PWK_PIN              at GPIOC_ODR.B2;
sbit C4G_RI_PIN               at GPIOA_IDR.B0;
sbit C4G_RTS_PIN              at GPIOD_ODR.B13;
sbit C4G_CTS_PIN              at GPIOD_IDR.B10;

static bool                      callFlag;
static uint8_t                   buffer[ 1024 ];
static char                      callerId[ 20 ];
static T_AT_storage              storage[ 20 ];

void system_init( void )
{
    GPIO_Digital_Input( &GPIOA_IDR, _GPIO_PINMASK_4 | _GPIO_PINMASK_0 );
    GPIO_Digital_Input( &GPIOD_IDR, _GPIO_PINMASK_10 );
    GPIO_Digital_Output( &GPIOC_ODR, _GPIO_PINMASK_2 );
    GPIO_Digital_Output( &GPIOD_ODR, _GPIO_PINMASK_13 );

    UART1_Init_Advanced( 115200, _UART_8_BIT_DATA,
                                 _UART_NOPARITY,
                                 _UART_ONE_STOPBIT,
                                 &_GPIO_MODULE_USART1_PA9_10 );
                                 
    UART3_Init_Advanced( 115200, _UART_8_BIT_DATA,
                                 _UART_NOPARITY,
                                 _UART_ONE_STOPBIT,
                                 &_GPIO_MODULE_USART3_PD89 );
    RCC_APB1ENR.TIM2EN = 1;
    TIM2_CR1.CEN = 0;
    TIM2_PSC = 1;
    TIM2_ARR = 35999;
    TIM2_DIER.UIE = 1;
    TIM2_CR1.CEN = 1;;
    RXNEIE_USART3_CR1_bit = 1;
    NVIC_IntEnable( IVT_INT_TIM2 );
    NVIC_IntEnable( IVT_INT_USART3 );
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
void TICK_ISR() iv IVT_INT_TIM2
{
    TIM2_SR.UIF = 0;
    AT_tick();
}

/*
 * UART Interrupt
 */
void RX_ISR() iv IVT_INT_USART3 ics ICS_AUTO
{
    if( RXNE_USART3_SR_bit )
    {
        char tmp = USART3_DR;
        AT_getChar( tmp );
    }
}

/************************************************************ END OF FILE *****/
