/**
 * @file gsm_gnss_click.c
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

#include "at_engine.h"

#include "log.h"

/***************************************************************************//**
* Preprocessors
*******************************************************************************/

#define SMS_FINISHED_INITIALISATION_CODE 3
#define AT_BUFFER_SIZE 512
#define STR_BUFFER_SIZE 256
#define PHONE_NUMBER_SIZE 16
/***************************************************************************//**
* Typedefs
*******************************************************************************/

/***************************************************************************//**
* Variable Definitions
*******************************************************************************/
extern sfr sbit GSM_PWR;

static msg_recieved_callback_t on_msg_recieved = NULL;

static bool sim_ready = false;
static bool rdy_flag = false;

static char str_buffer[STR_BUFFER_SIZE] = { 0 };

static char phone_number[PHONE_NUMBER_SIZE] = { 0 };

static char at_buffer[AT_BUFFER_SIZE] = { 0 };

/***************************************************************************//**
* Function Prototypes
*******************************************************************************/
static void gsm_gnss_click_reset( void );

static bool gsm_gnss_click_response_ok( char* response);

static void gsm_gnss_click_default_event( char* response );
static void gsm_gnss_click_on_cmt( char* response );
static void gsm_gnss_click_on_qinistat( char *response );

static bool gsm_gnss_click_wait_ready( void );

static void gsm_gnss_click_get_param( char* response, char* out, uint8_t index);

static void gsm_gnss_click_config( void );

/***************************************************************************//**
* Private Functions
*******************************************************************************/

/***************************************************************************//**
 *
 */
static void gsm_gnss_click_reset( void )
{
    GSM_PWR = 1; 
    Delay_ms(10);

    GSM_PWR = 0; 
    Delay_ms(10);

    GSM_PWR = 1;
}

/***************************************************************************//**
 *
 */
static bool gsm_gnss_click_response_ok( char* response)
{
    char* ok_str = strstr(response, "OK");
    if(ok_str != NULL) return true;

    return false;
}

/***************************************************************************//**
 *
 */
static void gsm_gnss_click_default_event( char* response )
{
    LOG_INFO("Default event.");
    LOG_INFO(response);

    if(strstr(response, "RDY") != 0)
        rdy_flag = true;
}

/***************************************************************************//**
 *
 */
static void gsm_gnss_click_on_cmt( char* response )
{
    char* msg_start;
    char* msg_end;

    LOG_INFO("Message recieved.");

    gsm_gnss_click_get_param(response, phone_number, 0);

    msg_start = strstr(response + 2, "\r\n");
    msg_start += 2;
    msg_end = strstr(msg_start, "\r\n");

    if (msg_start != 0 && msg_end != 0)
    {
        memcpy(str_buffer, msg_start, msg_end - msg_start);
        str_buffer[msg_end - msg_start] = '\0';
        on_msg_recieved(str_buffer, phone_number);
    }
}

/***************************************************************************//**
 *
 */
static void gsm_gnss_click_on_qinistat( char *response )
{
    LOG_INFO(response);

    if( ! gsm_gnss_click_response_ok( response ) )
        return;

    gsm_gnss_click_get_param(response, str_buffer, 0);

    if (strcmp(str_buffer, SMS_FINISHED_INITIALISATION_CODE) == 0)
    {
        sim_ready = true;
    }
}

/***************************************************************************//**
 *  The function blocks until SIM card initialisation is done.
 *  The function returns when initialization of SIM card related functions has
 *  been finished.
 */
static void gsm_gnss_click_wait_ready( void )
{
    uint8_t index = 0;

    while( ! rdy_flag )
        at_process();

    LOG_INFO("Ready flag.");
    
    while( ! sim_ready )
    {
        at_cmd_single( "AT+QINISTAT" );
        Delay_ms(1000);
    }
    LOG_INFO("Ready done.");
}

/***************************************************************************//**
 *
 */
static void gsm_gnss_click_get_param( char* response, char* out, uint8_t index)
{
    uint8_t i = 0;
    uint8_t j = 0;
    char* comma;
    char* param = strchr(response, ':') + 2;

    while( index && param[i] != '\r' )
    {
        if(param[i++] == ',')
        {
            index--;
        }
    }

    while( param[i] != ',' && param[i] != '\r')
    {
        out[j++] = param[i++];
    }

    out[j] = '\0'; 
}

/***************************************************************************//**
 *  
 */
static void gsm_gnss_click_config( void )
{
    // Set SMS message format as text mode.
    at_cmd_single( "AT+CMGF=1" );
    // Set character set as GSM which is used by the TE.
    at_cmd_single( "AT+CSCS=\"GSM\"" );
    // SMS status report is supported under text mode if <fo> is set to 49.
    // code: +CDS.
    at_cmd_single( "AT+CNMI=2,2,0,1,0" );
}

/***************************************************************************//**
* Public Functions
*******************************************************************************/

/***************************************************************************//**
 *
 */
void gsm_gnss_click_init( msg_recieved_callback_t callback )
{
    on_msg_recieved = callback;

    LOG_INFO("Initialising AT engine...");
    at_init( 
        gsm_gnss_click_default_event, 
        UART2_Write, 
        at_buffer,
        AT_BUFFER_SIZE 
     );
    LOG_INFO("AT engine initialised.");

    at_cmd_save( "+CMT", 
        1000, 
        NULL, 
        NULL, 
        NULL, 
        gsm_gnss_click_on_cmt 
    );

    at_cmd_save( "+QINISTAT", 
        1000, 
        NULL, 
        NULL, 
        NULL, 
        gsm_gnss_click_on_qinistat 
    );


    gsm_gnss_click_reset();

    gsm_gnss_click_wait_ready();
    LOG_INFO("Ready.");
    at_cmd_single( "AT" );

    gsm_gnss_click_config();
    LOG_INFO("Click initialised.");
}

/***************************************************************************//**
 *
 */
void gsm_gnss_click_run( void )
{
    if( !on_msg_recieved ) return;

    while (1)
    {
        at_process();
    }
}

/***************************************************************************//**
 *
 */
void UART2_Interrupt() iv IVT_UART_2 ilevel 7 ics ICS_SRS
{
    if( IFS1 & ( 1 << U2RXIF ))
    {
        char tmp = UART2_Read();

        at_rx( tmp );
        UART5_Write(tmp);
        
        U2RXIF_bit = 0;
    }
}

/***************************************************************************//**
 *
 */
void Timer1_Interrupt() iv IVT_TIMER_1 ilevel 7 ics ICS_SRS 
{
    T1IF_bit = 0;
    at_tick();
}

/*************** END OF FUNCTIONS *********************************************/