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

#define SMS_FINISHED_INITIALISATION_CODE "3"
#define AT_BUFFER_SIZE 512
#define STR_BUFFER_SIZE 256
#define PHONE_NUMBER_SIZE 20
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

static bool navigation_ready = false;
static bool send_navigation = false;
static char navigation_url_buffer[128]; 

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

static void gsm_gnss_click_on_qgnssrd( char *response );

/***************************************************************************//**
* Private Functions
*******************************************************************************/

/***************************************************************************//**
 *
 */
static void gsm_gnss_click_reset( void )
{
    GSM_PWR = 1; 
    Delay_ms(20);

    GSM_PWR = 0; 
    Delay_ms(20);

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
    LOG_VAR;
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
    LOG_VAR;
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
        send_navigation = true;
    }
}

/***************************************************************************//**
 *
 */
static void gsm_gnss_click_on_qinistat( char *response )
{   
    LOG_VAR;
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
    
    while( ! sim_ready )
    {
        at_cmd_single( "AT+QINISTAT" );
        Delay_ms(1000);
    }
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

    // Enable GNSS.
    at_cmd_single( "AT+QGNSSC=1" );
    at_cmd_single( "AT+QIFGCNT=2" );
    at_cmd_single( "AT+QICSGP=1,\"CMNET\"" );
    at_cmd_single( "AT+QGNSSTS?" );
    at_cmd_single( "AT+QGNSSEPO=1" );
    at_cmd_single( "AT+QGEPOAID" );
}

/***************************************************************************//**
* Public Functions
*******************************************************************************/

/***************************************************************************//**
 *
 */
void gsm_gnss_click_init( msg_recieved_callback_t callback )
{
    LOG_VAR;
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

    at_cmd_save( "+QGNSSRD", 
        1000, 
        NULL, 
        gsm_gnss_click_on_qgnssrd, 
        NULL, 
        NULL
    );

    memset(phone_number, 0x00, PHONE_NUMBER_SIZE);
    str_buffer[0] = 0;

    LOG_INFO("Reseting...");
    gsm_gnss_click_reset();

    LOG_INFO("Wait for ready...");
    gsm_gnss_click_wait_ready();
    LOG_INFO("Ready.");

    at_cmd_single( "AT" );

    gsm_gnss_click_config();
    LOG_INFO("Click initialised.");
}

/***************************************************************************//**
 *  
 */
void gsm_gnss_click_read_nav( void )
{
    // Read GNSS navigationin formation. Get GLL sentence.
    at_cmd_single("AT+QGNSSRD=\"NMEA/GLL\"");
    while ( ! navigation_ready )
        at_process();

    navigation_ready = false;    
}

/***************************************************************************//**
 *  
 */
void gsm_gnss_click_send_nav( void )
{
    char cmd_buffer[32];

    memset(cmd_buffer, 0x00, 32);
    memset(str_buffer, 0x00, STR_BUFFER_SIZE);

    strcat(str_buffer, "Position: \n");
    strcat(str_buffer, navigation_url_buffer);

    strcat(cmd_buffer, "AT+CMGS=");
    strcat(cmd_buffer, phone_number);

    Delay_ms(1000);
    at_cmd_double(cmd_buffer, str_buffer);
}

/***************************************************************************//**
 *
 */
static void gsm_gnss_click_on_qgnssrd( char *response )
{
    LOG_VAR;
    char deg[4] = { 0 };
    char min[3] = { 0 };
    char dec[5] = { 0 };

    float  latitude;
    float  longitude;

    char latitude_buffer[16] = { 0 };
    char longitude_buffer[16] = { 0 };

    LOG_INFO("GET NAVIGATION.");
    LOG_INFO(response);

    gsm_gnss_click_get_param( response, str_buffer, 1 );

    strncpy(deg, str_buffer, 2);
    strncpy(min, str_buffer + 2, 2);
    strncpy(dec, str_buffer + 5, 4);

    latitude = atoi(deg) + (atoi(min) + (atoi(dec) / 10000.0f)) / 60.0f;
    
    gsm_gnss_click_get_param( response, str_buffer, 2 );

    if(str_buffer[0] == 'S') latitude *= -1.0f;

    gsm_gnss_click_get_param( response, str_buffer, 3 );

    strncpy(deg, str_buffer, 3);
    strncpy(min, str_buffer + 3, 2);
    strncpy(dec, str_buffer + 6, 4);

    longitude = atoi(deg) + (atoi(min) + atoi(dec) / 10000.0f) / 60.0f;

    gsm_gnss_click_get_param( response, str_buffer, 4 );

    if(str_buffer[0] == 'W') longitude *= -1.0f;

    FloatToStr(latitude, latitude_buffer);
    FloatToStr(longitude, longitude_buffer);

    sprintf(
        navigation_url_buffer,
        "https://www.google.rs/maps/place/%s,%s",
        Ltrim(latitude_buffer), Ltrim(longitude_buffer)
    );

    navigation_ready = true;
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

        if(send_navigation)
        {
            gsm_gnss_click_read_nav();
            gsm_gnss_click_send_nav();
            send_navigation = false;
        }
    }
}

/***************************************************************************//**
 *
 */
void UART_RX_ISR() iv IVT_UART2_IRQ ics ICS_AUTO
{
    if( UART2_LSR & ( 1 << DATA_RDY ) )
    {
        char tmp_data = UART2_RBR;
        GPIO_PORT_64_66 = 3;
        at_rx(tmp_data);
    }
}

/***************************************************************************//**
 *
 */
void TimerInterrupt() iv IVT_TIMERS_IRQ
{
    at_tick();
    if (TIMER_INT_A_bit)
    { 
        TIMER_INT = (TIMER_INT & 0xAA) | (1 << 0);
    }
}

/*************** END OF FUNCTIONS *********************************************/