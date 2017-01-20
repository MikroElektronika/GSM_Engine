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
 * @date        28 Dec 2016
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

#include "display.h"

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

// TFT module connections
unsigned int TFT_DataPort at GPIOE_ODR;
sbit TFT_RST at GPIOE_ODR.B8;
sbit TFT_RS at GPIOE_ODR.B12;
sbit TFT_CS at GPIOE_ODR.B15;
sbit TFT_RD at GPIOE_ODR.B10;
sbit TFT_WR at GPIOE_ODR.B11;
sbit TFT_BLED at GPIOE_ODR.B9;
// End TFT module connections

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
    DISPLAY_INIT("GSM/GNSS click", "MC60", "EasyPIC Fusion v7");
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