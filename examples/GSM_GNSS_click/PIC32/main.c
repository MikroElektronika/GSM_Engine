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
 * -<b> MCU           </b> :
 * -<b> Dev. Board    </b> :
 * -<b> Ext. Modules  </b> :
 * -<b> SW            </b> :
 *
 */

/**
 * @page Dev. board setup
 *
 */

/***************************************************************************//**
* Includes
*******************************************************************************/

#include "gsm_gnss_click.h"

#include "display.h"

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

sbit GSM_PWR      at RC1_bit;    // out RST
sbit GSM_PWR_DIR  at TRISC1_bit;    // out RST

sbit GSM_CTS      at RE8_bit;    // in INT
sbit GSM_CTS_DIR  at TRISE8_bit;    // in INT

sbit GSM_RTS      at RC2_bit;     // out CS
sbit GSM_RTS_DIR  at TRISC2_bit;     // out CS

sbit GSM_RING      at RD0_bit;    // in PWM
sbit GSM_RING_DIR  at TRISD0_bit;    // in PWM

sbit GSM_STAT      at RB8_bit;     // in AN
sbit GSM_STAT_DIR  at TRISB8_bit;     // in AN



/***************************************************************************//**
* Function Prototypes
*******************************************************************************/

static void system_init( void );

/***************************************************************************//**
* Private Functions
*******************************************************************************/

static void InitTimer1()
{
    T1CON = 0x8010;
    T1IP0_bit = 1;
    T1IP1_bit = 1;
    T1IP2_bit = 1;
    T1IF_bit = 0;
    T1IE_bit = 1;
    PR1	= 10000;
    TMR1 = 0;
}

static void system_init( void )
{
    AD1PCFG = 0xFFFF; 

    // LOG_INIT(56000);
    UART5_Init(56000);

    GSM_PWR_DIR = PIN_DIR_OUT;
    GSM_CTS_DIR = PIN_DIR_IN;
    GSM_RTS_DIR = PIN_DIR_OUT;

    // GSM/GNSS module communication.
    UART2_Init( GSM_MODEM_BAUD_RATE );
    Delay_ms( 100 );
    LOG_INFO("UART2 initialised.");


    U2IP0_bit = 1;
    U2IP1_bit = 1;
    U2IP2_bit = 1;
    U2RXIE_bit = 1;

    InitTimer1();

    T1IE_bit = 1; 
    ON__T1CON_bit = 1;       
    EnableInterrupts();

    LOG_INFO("Interrupt initialised.");

    display_init();
    LOG_INFO("Dsiplay initialised.");

    LOG_INFO("System initialised.");
}

void on_msg(char* msg, char* sender_number)
{
    LOG_INFO(sender_number);
    LOG_INFO(msg);

    display_print_msg(msg, sender_number);
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