/*
 * Example for Skywire click with HE910 module & Thermo 3 click
 *
 * Date         Dec 2016
 * Author       MilosV
 * Copyright    GNU Public License v2
 *
 * Test configuration STM32 :
 *  MCU           :        STM32F107VCT6
 *  Dev. Board    :        EasyMx PRO v7 for STM32 ARM
 *  SW            :        mikroC PRO for ARM v4.9.0
 *
 * Include example.pld file in your project.
 *
 ******************************************************************************/

#include "at_engine.h"

/*
 * Skywire  pins
 */
sbit SKYWIRE_EN           at GPIOA_ODR.B4;
sbit SKYWIRE_RST          at GPIOC_ODR.B2;

/*
 * Ring Flag
 */
bool measure_f;

/*
 * Response buffer
 */
uint8_t buffer[ 1024 ] = { 0 };

/*
 * Caller ID
 */
char caller_id[ 20 ] = { 0 };

/*
 * Thermo 3 measurement
 */
float temperature  = 0.0F;

/*
 * System Initialization
 *    - GPIO Initialization
 *        + Skywire EN pin
 *        + Skywire RST pin
 *    - Peripherals Initialization
 *        + UART1 for data dump
 *        + UART3 for HE910
 *        + I2C1 for Thermo 3 click
 *    - Interrupt Initialization
 *        + Timer
 *        + UART
 */
static void system_init( void )
{
    GPIO_Digital_Output( &GPIOA_ODR, _GPIO_PINMASK_4 );
    GPIO_Digital_Output( &GPIOC_ODR, _GPIO_PINMASK_2 );
    GPIO_Digital_Input( &GPIOD_IDR, _GPIO_PINMASK_0 );
    
    UART1_Init_Advanced( 115200, _UART_8_BIT_DATA,
                                 _UART_NOPARITY,
                                 _UART_ONE_STOPBIT,
                                 &_GPIO_MODULE_USART1_PA9_10 );
    UART3_Init_Advanced( 115200, _UART_8_BIT_DATA,
                                 _UART_NOPARITY,
                                 _UART_ONE_STOPBIT,
                                 &_GPIO_MODULE_USART3_PD89 );
    I2C1_Init_Advanced( 400000, &_GPIO_MODULE_I2C1_PB67 );
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
    LOG( "\r\n_______________________________\r\n System Initialized\r\n" );
}

/*
 * Power ON routine for HE910
 */
static void skywire_power_on( void )
{
    SKYWIRE_EN = 0;
    Delay_ms( 5000 );
    SKYWIRE_EN = 1;
    Delay_ms( 3000 );
    LOG( "\r\nSkywire Initialized\r\n" );
}

/*
 * Default response handler
 */
void rsp_handler( char *rsp )
{
    char tmp[ 25 ] = { 0 };

    LOG( "Response : \r\n" );
    LOG( rsp );
    LOG( "\r\n" );
    if( !strncmp( "\r\nRING", rsp, 6 ) )
        measure_f = true;
}

/*
 * Caller ID handler used to store caller ID in global var
 */
void callerid_handler( char *rsp )
{
    char *tmp_s;
    char *tmp_e;

    tmp_s = strchr( rsp, '\"' );
    tmp_e = strchr( tmp_s + 1, '\"' );
    strncpy( caller_id, tmp_s, tmp_e - tmp_s + 1 );
}

/*
 * Temperature measurement with Thermo 3
 */
void measure_temp( void )
{
    int raw_temp;
    uint8_t tmp_data[ 2 ] = { 0x00 };

    I2C1_Start();
    I2C1_Write( 0x48, tmp_data, 1, END_MODE_RESTART );
    I2C1_Read( 0x48, tmp_data, 2, END_MODE_STOP );
    raw_temp = ( ( tmp_data[ 0 ] << 8 ) | tmp_data[ 1 ] ) >> 4;

    if( raw_temp & ( 1 << 11 ) )
        raw_temp |= 0xF800;

    temperature = ( float )( raw_temp * 0.0625 );
}

/*
 * Composing Command and SMS content
 */
void reply_to_caller( void )
{
    char tmp[ 30 ] = { 0 };
    char cmd_content[ 30 ] = { 0 };                                             // Send SMS command
    char reply_content[ 256 ] = { 0 };                                          // SMS content
    
    strcat( cmd_content, "AT+CMGS=" );
    strcat( cmd_content, caller_id );
    strcat( reply_content, "Temperature : " );
    FloatToStr( temperature, tmp );
    strcat( reply_content, Ltrim( tmp ) );
    strcat( reply_content, " C" );
    at_cmd_double( cmd_content, reply_content );
}


void main()
{
    measure_f = false;
    system_init();
    skywire_power_on();
    at_init( rsp_handler, UART3_Write, buffer, sizeof( buffer ) );
    at_cmd_save( "+CLCC", 1000, NULL, NULL, NULL, callerid_handler );           // Assigning caller ID handler
    at_cmd_single( "AT" );
    at_cmd_single( "AT+CSCS=\"GSM\"" );
    at_cmd_single( "AT+CMGF=1" );

    while( 1 )
    {
        at_process();
        
        if( measure_f )
        {
            measure_temp();
            at_cmd_single( "AT+CLCC" );
            at_cmd_single( "ATH" );
            Delay_ms( 2000 );                                                   // Delay needed after ATH
            reply_to_caller();
            measure_f = false;
        }
    }
}

/*
 * Timer Interrupt
 */
void TICK_ISR() iv IVT_INT_TIM2 
{
    TIM2_SR.UIF = 0;
    at_tick();
}

/*
 * UART Interrupt
 */
void RX_ISR() iv IVT_INT_USART3 ics ICS_AUTO
{
    if( RXNE_USART3_SR_bit )
    {
        char tmp = USART3_DR;
        at_rx( tmp );
    }
}