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
sbit SKYWIRE_EN           at LATA0_bit;
sbit SKYWIRE_RST          at LATC0_bit;

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
 * Data dump function
 */
void LOG( char *txt )
{
    char *ptr = txt;
    
    while( *ptr )
           Soft_UART_Write( *ptr++ );
}

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
    TRISA0_bit = 0;
    TRISC0_bit = 0;

    Soft_UART_Init( &PORTA, 1, 4, 9600, 0 );
    UART1_Init( 115200 );
    I2C1_Init( 400000 );
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
    LOG( "\r\n_______________________________\r\n System Initialized\r\n" );
}

/*
 * Power ON routine for HE910
 */
void skywire_power_on( void )
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
    I2C1_Wr( ( 0x48 << 1 ) | 0x00 );
    I2C1_Wr( 0x00 );
    I2C1_Repeated_Start();
    I2C1_Wr( ( 0x48 << 1 ) | 0x01 );
    tmp_data[ 0 ] = I2C1_Rd( 1 );
    tmp_data[ 1 ] = I2C1_Rd( 0 );
    I2C1_Stop();
    
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
    at_init( rsp_handler, UART1_Write, buffer, sizeof( buffer ) );
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
void interrupt()
{
   if( RC1IF_bit == 1 )
   {
       char tmp = UART1_Read();
       at_rx( tmp );
   }
   
   if( TMR1IF_bit )
   {
       at_tick();
       TMR1IF_bit = 0;
       TMR1H	  = 0xF0;
       TMR1L	  = 0x60;
   }
}