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
 * @date        04 Jan 2017
 * @author      Branislav Marton
 * @copyright   GNU Public License
 * @version     1.0.0 - Initial testing and verification
 */

/**
 * @page TEST_CFG Test Configurations
 *
 * ### Test configuration STM : ###
 * @par
 * -<b> MCU           </b> :    P18F87K22
 * -<b> Dev. Board    </b> :    EasyPIC PRO v7
 * -<b> Ext. Modules  </b> :    GSM/GNSS click
 * -<b> SW            </b> :    mikroC PRO for PIC v.7.0.0
 *
 */

/**
 * @page Dev. board setup
 *
 *  1. Insert GSM/GNSS click into mikroBUS 1 socket.
 */

/***************************************************************************//**
* Includes
*******************************************************************************/

#include "gsm_gnss_click.h"


#include "log.h"

/***************************************************************************//**
* Preprocessors
*******************************************************************************/

#define PIN_DIR_IN  1
#define PIN_DIR_OUT 0

/***************************************************************************//**
* Typedefs
*******************************************************************************/

/***************************************************************************//**
* Variable Definitions
*******************************************************************************/

sbit GSM_PWR      at RC0_bit;    // out RST
sbit GSM_PWR_DIR  at TRISC0_bit;    // out RST

sbit GSM_CTS      at RB0_bit;    // in INT
sbit GSM_CTS_DIR  at TRISB0_bit;    // in INT

sbit GSM_RTS      at RE0_bit;     // out CS
sbit GSM_RTS_DIR  at TRISE0_bit;     // out CS

sbit GSM_RING      at RC1_bit;    // in PWM
sbit GSM_RING_DIR  at TRISC1_bit;    // in PWM

sbit GSM_STAT      at RA0_bit;     // in AN
sbit GSM_STAT_DIR  at TRISA0_bit;     // in AN

// LCD module connections
sbit LCD_RS at RB4_bit;
sbit LCD_EN at RB5_bit;
sbit LCD_D4 at RB0_bit;
sbit LCD_D5 at RB1_bit;
sbit LCD_D6 at RB2_bit;
sbit LCD_D7 at RB3_bit;

sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB0_bit;
sbit LCD_D5_Direction at TRISB1_bit;
sbit LCD_D6_Direction at TRISB2_bit;
sbit LCD_D7_Direction at TRISB3_bit;
// End LCD module connections

/***************************************************************************//**
* Function Prototypes
*******************************************************************************/

static void system_init( void );

/***************************************************************************//**
* Private Functions
*******************************************************************************/

void InitTimer0()
{
    T0CON = 0x88;
    TMR0H = 0x9E;
    TMR0L = 0x58;
    GIE_bit = 1;
    TMR0IE_bit = 1;
}

static void system_init( void )
{   
    LOG_VAR;

    TRISH0_bit = PIN_DIR_OUT;
    RH0_bit = 0;

    // Initialise software UART for log.
    Soft_UART_Init(&PORTC, 4, 5, 9600, 0);
    LOG_INIT(9600);

    GSM_PWR_DIR = PIN_DIR_OUT;
    GSM_CTS_DIR = PIN_DIR_IN;
    GSM_RTS_DIR = PIN_DIR_OUT;

    // GSM/GNSS module communication.
    UART1_Init( GSM_MODEM_BAUD_RATE );
    Delay_ms( 100 );
    LOG_INFO("UART1 initialised.");

    RC1IE_bit = 1;
    RC1IF_bit = 0;
    PEIE_bit  = 1;
    GIE_bit   = 1;

    InitTimer0();

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