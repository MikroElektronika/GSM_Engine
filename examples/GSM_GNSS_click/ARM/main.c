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
 * -<b> MCU           </b> :    STM32F107VC
 * -<b> Dev. Board    </b> :    EasyMx PRO v7 for STM32
 * -<b> Ext. Modules  </b> :    GSM/GNSS click
 * -<b> SW            </b> :    mikroC PRO for ARM v.4.9.0
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

/***************************************************************************//**
* Typedefs
*******************************************************************************/

/***************************************************************************//**
* Variable Definitions
*******************************************************************************/

sbit GSM_PWR      at GPIOC_ODR.B2;    // out RST

sbit GSM_CTS      at GPIOD_IDR.B10;    // in INT

sbit GSM_RTS      at GPIOD_ODR.B13;     // out CS

sbit GSM_RING      at GPIOA_IDR.B0;    // in PWM

sbit GSM_STAT      at GPIOD_IDR.B4;     // in AN



/***************************************************************************//**
* Function Prototypes
*******************************************************************************/

static void system_init( void );

/***************************************************************************//**
* Private Functions
*******************************************************************************/

void InitTimer2()
{
    RCC_APB1ENR.TIM2EN = 1;
    TIM2_CR1.CEN = 0;
    TIM2_PSC = 1;
    TIM2_ARR = 35999;
    NVIC_IntEnable(IVT_INT_TIM2);
    TIM2_DIER.UIE = 1;
    TIM2_CR1.CEN = 1;
}

static void system_init( void )
{
    LOG_INIT(56000);
 
    GPIO_Digital_Output( &GPIOC_ODR, _GPIO_PINMASK_2 );
    GPIO_Digital_Output( &GPIOD_ODR, _GPIO_PINMASK_13 );
    GPIO_Digital_Input( &GPIOD_IDR, _GPIO_PINMASK_10 );
    GPIO_Digital_Input( &GPIOA_IDR, _GPIO_PINMASK_0 );
    GPIO_Digital_Input( &GPIOD_IDR, _GPIO_PINMASK_4 );

    // GSM/GNSS module communication.
    UART3_Init_Advanced( 
        GSM_GNSS_MODEM_BAUD_RATE, 
        _UART_8_BIT_DATA, 
        _UART_NOPARITY, 
        _UART_ONE_STOPBIT, 
         &_GPIO_MODULE_USART3_PD89
    );

    Delay_ms( 100 );
    LOG_INFO("UART3 initialised.");

    InitTimer2();

    RXNEIE_USART3_CR1_bit = 1;
    NVIC_IntEnable( IVT_INT_USART3 );

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