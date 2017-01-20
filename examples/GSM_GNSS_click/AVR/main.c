/**
 * @file main.c
 * @brief <h2> GSM/GNSS click Example </h2>
 *
 * @par
 * Example for <a href="http://www.mikroe.com">MikroElektronika's</a>
 * GSM/GNSS click board.
 *
 *
 *
 ******************************************************************************/

/**
 * @page Example
 * @date        27 Dec 2016
 * @author      Branislav Marton
 * @copyright   GNU Public License
 * @version     1.0.0 - Initial testing and verification
 */

/**
 * @page TEST_CFG Test Configurations
 *
 * ### Test configuration STM : ###
 * @par
 * -<b> MCU           </b> :    ATMEGA32
 * -<b> Dev. Board    </b> :    EasyAVR v7
 * -<b> Ext. Modules  </b> :    GSM/GNSS click, LCD
 * -<b> SW            </b> :    mikroC PRO for AVR v.6.1.2
 *
 */

/**
 * @page Dev. board setup
 *  1) Insert GSM/GNSS click into mikroBUS 1 slot.
 *
 */

/***************************************************************************//**
* Includes
*******************************************************************************/

#include "gsm_gnss_click.h"


#include "log.h"

/***************************************************************************//**
* Preprocessors
*******************************************************************************/

#define PIN_DIR_IN  0
#define PIN_DIR_OUT 1

/***************************************************************************//**
* Typedefs
*******************************************************************************/

/***************************************************************************//**
* Variable Definitions
*******************************************************************************/

sbit GSM_PWR      at PORTA6_bit;    // out RST
sbit GSM_PWR_DIR  at DDA6_bit;      // out RST

sbit GSM_CTS      at PIND2_bit;     // in INT
sbit GSM_CTS_DIR  at DDD2_bit;      // in INT

sbit GSM_RTS      at PORTA5_bit;    // out CS
sbit GSM_RTS_DIR  at DDA5_bit;      // out CS

sbit GSM_RING     at PIND4_bit;     // in PWM
sbit GSM_RING_DIR at DDD4_bit;      // in PWM

sbit GSM_STAT     at PINA7_bit;     // in AN
sbit GSM_STAT_DIR at DDA7_bit;      // in AN

// LCD module connections
sbit LCD_RS at PORTA2_bit;
sbit LCD_EN at PORTD6_bit;
sbit LCD_D4 at PORTC4_bit;
sbit LCD_D5 at PORTC5_bit;
sbit LCD_D6 at PORTC6_bit;
sbit LCD_D7 at PORTC7_bit;

sbit LCD_RS_Direction at DDA2_bit;
sbit LCD_EN_Direction at DDD6_bit;
sbit LCD_D4_Direction at DDC4_bit;
sbit LCD_D5_Direction at DDC5_bit;
sbit LCD_D6_Direction at DDC6_bit;
sbit LCD_D7_Direction at DDC7_bit;
// End LCD module connections

/***************************************************************************//**
* Function Prototypes
*******************************************************************************/

static void system_init( void );

/***************************************************************************//**
* Private Functions
*******************************************************************************/

void InitTimer1()
{
    SREG_I_bit = 1; 
    TCCR1A = 0x80;
    TCCR1B = 0x09;
    OCR1AH = 0x1F; 
    OCR1AL = 0x3F; 
    OCIE1A_bit = 1; 
}

static void system_init( void )
{
    LOG_VAR;

    DDRB = 0xFF;

PORTB++;

    Soft_UART_Init(&PORTB, 2, 3, 9600, 0);
    LOG_INIT(9600);

    GSM_PWR_DIR = PIN_DIR_OUT;
    GSM_CTS_DIR = PIN_DIR_IN;
    GSM_RTS_DIR = PIN_DIR_OUT;
    GSM_RING_DIR = PIN_DIR_IN;
    GSM_STAT_DIR = PIN_DIR_IN;

    // GSM/GNSS module communication.
    UART1_Init(GSM_GNSS_MODEM_BAUD_RATE);

    Delay_ms( 100 );
    LOG_INFO("UART1 initialised.");

    UCSRB |= ( 1 << RXCIE );
    SREG  |= ( 1 << SREG_I );

     InitTimer1();


    LOG_INFO("Interrupt initialised.");

    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    Lcd_Out(1, 1, "NO MESSAGE");
    LOG_INFO("Dsiplay initialised.");

    LOG_INFO("System initialised.");
}

void on_msg(char* msg, char* sender_number)
{
    LOG_VAR;
    LOG_INFO(sender_number);
    LOG_INFO(msg);

    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    
    Lcd_Out(1, 1, sender_number);
    Lcd_Out(2, 1, msg);
}

/***************************************************************************//**
* Public Functions
*******************************************************************************/

void main()
{
    system_init();

    gsm_gnss_click_init(on_msg);
    gsm_gnss_click_run();
}

/*************** END OF FUNCTIONS *********************************************/