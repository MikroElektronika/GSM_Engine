/**
 * @file display.c
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

#include "display.h"
#include <stdint.h>
#include "tft_resources.h"


/***************************************************************************//**
* Preprocessors
*******************************************************************************/

#define MAX_LINE_SIZE 8

/***************************************************************************//**
* Typedefs
*******************************************************************************/

/***************************************************************************//**
* Variable Definitions
*******************************************************************************/

char TFT_DataPort at GPIO_PORT_32_39;
sbit TFT_RST at GPIO_PIN42_bit;
sbit TFT_RS at GPIO_PIN40_bit;
sbit TFT_CS at GPIO_PIN41_bit;
sbit TFT_RD at GPIO_PIN46_bit;
sbit TFT_WR at GPIO_PIN47_bit;
sbit TFT_BLED at GPIO_PIN58_bit;

/***************************************************************************//**
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Private Functions
*******************************************************************************/

/***************************************************************************//**
* Public Functions
*******************************************************************************/

void display_init( void )
{
    DISPLAY_INIT("GSM/GNSS click", "MC60", "EasyFT90x v7");
    TFT_Set_Font(Arial16x19_Bold, CL_BLACK, FO_HORIZONTAL);
    TFT_Write_Text("NO MESSAGE.", 20, 80);

}

void display_print_msg( char* msg, char* sender_number )
{
    TFT_Set_Pen( CL_WHITE, 1 );                                            
    TFT_Set_Brush( 1, CL_WHITE, 0, 0, 0, 0 );  
    TFT_Rectangle(20, 71, 300, 219);  

    TFT_Set_Font(Arial16x19_Bold, CL_BLACK, FO_HORIZONTAL);
    TFT_Write_Text("FROM:", 20, 80);
    TFT_Write_Text("MESSAGE:", 20, 110);

    TFT_Set_Font(Arial16x18_Regular, CL_BLACK, FO_HORIZONTAL);
    TFT_Write_Text(sender_number, 80, 80);
    TFT_Write_Text(msg, 20, 130);
}

/*************** END OF FUNCTIONS *********************************************/