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
 * -<b> MCU           </b> :    FT900
 * -<b> Dev. Board    </b> :    EasyFT90x v7
 * -<b> Ext. Modules  </b> :    GSM/GNSS click
 * -<b> SW            </b> :    mikroC PRO for FT90x v.2.0.0
 *
 */

/**
 * @page Dev. board setup
 *  1. Put GSM/GNSS click into mikroBUS 1 slot.
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

/***************************************************************************//**
* Typedefs
*******************************************************************************/

/***************************************************************************//**
* Variable Definitions
*******************************************************************************/

sbit GSM_PWR    at GPIO_PIN1_bit;   // out RST

sbit GSM_CTS    at GPIO_PIN3_bit;   // in INT

sbit GSM_RTS    at GPIO_PIN28_bit;  // out CS

sbit GSM_RING   at GPIO_PIN56_bit;  // in PWM

sbit GSM_STAT   at GPIO_PIN9_bit;   // in AN

// Software UART module connections
sbit Soft_Uart_RX_Pin at GPIO_PIN30_Bit;
sbit Soft_Uart_TX_Pin at GPIO_PIN29_Bit;
// End Software UART module connections

/***************************************************************************//**
* Function Prototypes
*******************************************************************************/

static void system_init( void );

/***************************************************************************//**
* Private Functions
*******************************************************************************/

void InitTimerA() 
{ 
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
} 

static void system_init( void )
{
    LOG_VAR;
    Soft_UART_Init(56000, 0); 

    GPIO_Digital_Output( &GPIO_PORT_00_07, _GPIO_PINMASK_1 );
    GPIO_Digital_Output( &GPIO_PORT_24_31, _GPIO_PINMASK_4 );
    GPIO_Digital_Input( &GPIO_PORT_00_07, _GPIO_PINMASK_3);
    GPIO_Digital_Input( &GPIO_PORT_08_15, _GPIO_PINMASK_1 );
    GPIO_Digital_Input( &GPIO_PORT_56_63, _GPIO_PINMASK_0 );

    // GSM/GNSS module communication.
    UART2_Init( GSM_GNSS_MODEM_BAUD_RATE );

    Delay_ms( 100 );
    LOG_INFO("UART2 initialised.");

    InitTimerA();

    UART2_FCR |= ( 1 << FIFO_EN ) | ( 1 << RCVR_RST );
    UART2_INT_ENABLE |= ( 1 << RX_AVL_EN );
    IRQ_CTRL &= ~( 1 << GLOBAL_INTERRUPT_MASK );

    LOG_INFO("Interrupt initialised.");

    display_init();

    LOG_INFO("System initialised.");
}

void on_msg(char* msg, char* sender_number)
{
    LOG_VAR;
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