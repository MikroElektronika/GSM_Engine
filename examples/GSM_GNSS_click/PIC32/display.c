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

#define MAX_LINE_SIZE 15

/***************************************************************************//**
* Typedefs
*******************************************************************************/

/***************************************************************************//**
* Variable Definitions
*******************************************************************************/

// TFT display connections
char TFT_DataPort                   at LATE;
sbit TFT_RST                        at LATD7_bit;
sbit TFT_BLED                       at LATD2_bit;
sbit TFT_RS                         at LATD9_bit;
sbit TFT_CS                         at LATD10_bit;
sbit TFT_RD                         at LATD5_bit;
sbit TFT_WR                         at LATD4_bit;

char TFT_DataPort_Direction         at TRISE;
sbit TFT_RST_Direction              at TRISD7_bit;
sbit TFT_BLED_Direction             at TRISD2_bit;
sbit TFT_RS_Direction               at TRISD9_bit;
sbit TFT_CS_Direction               at TRISD10_bit;
sbit TFT_RD_Direction               at TRISD5_bit;
sbit TFT_WR_Direction               at TRISD4_bit;
// End of TFT display connections

static char line_buffer[MAX_LINE_SIZE];

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