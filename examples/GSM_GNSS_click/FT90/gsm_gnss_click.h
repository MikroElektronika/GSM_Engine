/**
 * @file gsm_gnss_click.h
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

#ifndef GSM_GNSS_CLICK_H
#define GSM_GNSS_CLICK_H


/***************************************************************************//**
* Includes
*******************************************************************************/

/***************************************************************************//**
* Preprocessors
*******************************************************************************/

#define GSM_GNSS_MODEM_BAUD_RATE  9600

/***************************************************************************//**
* Typedefs
*******************************************************************************/

typedef void ( *msg_recieved_callback_t )( char* msg, char* sender_number);

/***************************************************************************//**
* Variable Definitions
*******************************************************************************/

/***************************************************************************//**
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Private Functions
*******************************************************************************/

/***************************************************************************//**
* Public Functions
*******************************************************************************/

void gsm_gnss_click_init( msg_recieved_callback_t callback );
void gsm_gnss_click_run( void );

/*************** END OF FUNCTIONS *********************************************/

#endif // GSM_GNSS_CLICK_H