/*
 * Example for Skywire click with HE910 module & Thermo 3 click
 *
 * Date         Dec 2016
 * Author       MilosV
 * Copyright    GNU Public License v2
 *
 * Test configuration PIC :
 *  MCU           :        P18F87K22
 *  Dev. Board    :        EasyPIC PRO v7
 *  SW            :        mikroC PRO for PIC v7.0.0
 *
 ******************************************************************************/

#include "at_engine.h"

/*
 * Skywire  pins
 */
sbit SKYWIRE_EN           at LATB8_bit;
sbit SKYWIRE_RST          at LATC1_bit;

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
 *        + SoftUART for data dump
 *        + UART1 for HE910
 *        + I2C1 for Thermo 3 click
 *    - Interrupt Initialization
 *        + Timer
 *        + UART
 */
void system_init( void )
{
    TRISB8_bit = 0;
    TRISC1_bit = 0;

    UART5_Init( 115200 );
    UART2_Init( 115200 );
    I2C2_Init( 400000 );
    AD1PCFG = 0xFFFF;
    T2CON	 = 0x8010;
    T2IE_bit	 = 1;
    T2IF_bit	 = 0;
    T2IP0_bit	 = 0;
    T2IP1_bit	 = 1;
    T2IP2_bit	 = 1;
    PR2		 = 40000;
    TMR2         = 0;
    U2IP0_bit    = 1;
    U2IP1_bit    = 1;
    U2IP2_bit    = 1;
    U2RXIE_bit   = 1;
    EnableInterrupts();
    LOG( "\r\n_______________________________\r\n System Initialized\r\n" );
}

/*
 * Power ON routine for HE910
 */
void skywire_power_on( void )
{
    SKYWIRE_RST = 0;
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

    I2C2_Start();
    I2C2_Write( ( 0x48 << 1 ) | 0x00 );
    I2C2_Write( 0x00 );
    I2C2_Restart();
    I2C2_Write( ( 0x48 << 1 ) | 0x01 );
    tmp_data[ 0 ] = I2C2_Read( 0 );
    tmp_data[ 1 ] = I2C2_Read( 1 );
    I2C2_Stop();

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
    at_init( rsp_handler, UART2_Write, buffer, sizeof( buffer ) );
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
void Timer2Interrupt() iv IVT_TIMER_2 ilevel 6 ics ICS_AUTO
{
    at_tick();
    T2IF_bit	 = 0;
}

/*
 * UART Interrupt
 */
void UART2interrupt() iv IVT_UART_2 ilevel 7 ics ICS_SRS
{
      if( IFS1 & ( 1 << U2RXIF ))
      {
           char tmp = UART2_Read();
           at_rx( tmp );
           U2RXIF_bit = 0;
      }
}