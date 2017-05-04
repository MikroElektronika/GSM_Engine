/*

    at_engine.c

    Copyright (c) 2011-2017 MikroElektronika.  All right reserved.

--------------------------------------------------------------------------------

    Version : 0.1.1

    Revision Log :

- 0.0.1 (Apr/2016) Module created                   Milos Vidojevic
    * RX based engine
    * Timer not required
    * Write pointer assingment without user interaction
    * Internal buffer with fixed size
    * Internal handler storage

- 0.1.0 (Dec/2016)  Major changes					Milos Vidojevic
    * Timer based engine
    * Timer requierd
    * User provides write (tx) pointer
    * Buffer provided drunig initialization

- 0.1.1 (Apr/2017) Improvements						Milos Vidojevic
    * MikroC coding rules
    * Parsing improved - all known AT command types implemented 
    * Hanlder protype improved - handler provides response type
    * Handler storage moved to userspace
    * HFC implemented

*******************************************************************************/

#include "__AT_Parser.h"

// Serial write function prototypes depend on compiler/toolchain.
/*----------------------------- UART HAL -------------------------------------*/

#if defined( __MIKROC_PRO_FOR_ARM__ )       || \
    defined( __MIKROC_PRO_FOR_DSPIC__ )     || \
    defined( __MIKROC_PRO_FOR_PIC32__ )
static void ( *fpWriteUART )(unsigned int data_out);

#elif defined( __MIKROC_PRO_FOR_AVR__ )     || \
      defined( __MIKROC_PRO_FOR_PIC__ )     || \
      defined( __MIKROC_PRO_FOR_8051__ )    || \
      defined( __MIKROC_PRO_FOR_FT90x__ )
static void ( *fpWriteUART )(unsigned char data_out);
#else
static void ( *fpWriteUART )(unsigned char data_out);							
#endif

/*----------------------------- UART HAL END ---------------------------------*/

/* Write pointer */
static T_AT_UART_Write          fpWrite;

/* Engine flags and calls*/
static volatile bool            fCue;
static volatile bool            fBuffer;
static volatile bool            fResponse;
static volatile bool            fNoResponse;
#define AT_WAIT_RESPONSE()                                                      \
{ while( fCue ) AT_process(); }
#define AT_BLOCK()                                                              \
{ fNoResponse = false; fResponse = false; fCue = true; }
#define AT_UNBLOCK()                                                            \
{ fNoResponse = false; fResponse = false; fCue = false; }

/* Timer vars and calls */
static volatile bool            tfGt;
static volatile bool            tfSt;
static volatile uint32_t        tcGt;
static volatile uint32_t        tcSt;
static volatile uint32_t        tlGt;
static volatile uint32_t        tlSt;
#define AT_GT_START()                                                           \
{ tcGt = 0; tfGt = true; }
#define AT_GT_STOP()                                                            \
{ tfGt = false; }
#define AT_ST_START()                                                           \
{ tcSt = 0; tfSt = true; }
#define AT_ST_STOP()                                                            \
{ tfSt = false; }

/*
 * Response Storage
 *
 * counter / size / pointer
 *
 * Still simle array...
 */
static volatile uint16_t            rxIdx;
static uint16_t                     rxSize;
static volatile uint8_t*            rxStorage;
#define AT_CLEAN_BUFFER()                                                       \
{ rxIdx = 0; *rxStorage = 0; }

/* Handler vars */
static T_AT_handler                 fpHandler;
static int                          flags;

/*
 * Handlers Storage
 *      counter / size / pointer
 */
static uint16_t                     handlerIdx;
static uint16_t                     handlerSize;
static T_AT_storage*                handlerStorage;

/* Look up table for START MARK string, must have "" as 0 member */
#define MS_LUT_S      6
char MS_LUT[7][3] =
{
    "",             // Default - skipped by search
    "+",            // AT+...
    "#",            // AT#...
    "$",            // AT$...
    "%",            // AT%...
    "\\",           // AT\...
    "&"             // AT&...
};

/* Look up table for END MARK string, must have "" as 0 member */
/* ENDMARK represents the command type */
#define AT_DEFAULT      0
#define AT_TEST_F       1
#define AT_GET_F        2
#define AT_SET_F        3
#define AT_URC_F        4
#define AT_EXE_F        5

#define ME_LUT_S        6
char ME_LUT[6][3] =
{
    "",             // Default - skipped by search
    "=?",           // Test
    "?",            // Get
    "=",            // Set
    ":",            // URC 100%
    "\r"            // Exec
};


/*                 Private Function Prototypes
 ******************************************************************************/

/*
 * Transmission
 *
 * Transmission uses function pointer provided by the user as an argument
 * to the initialization of the engine.
 */
static void sendText( uint8_t *pInput, uint8_t delimiter );

/*
 * Simple Hash code generation
 *
 * Hash code is used to save the command to the storage in aim to have fixed
 * storage space for all functions.
 */
static uint32_t makeHash( char *pCmd );

/*
 * Search handler storage for provided command
 *
 * Function search the storage based on sting length and hash code.
 * If function returns zero command does not exists in storage area.
 */
static uint16_t findHandler( char* pCmd );

/*
 * Search input for strings from LUT table.
 * LUT table must be 2 dimensional char array.
 *
 * Depend of flag returned value is :
 * - index of found string at LUT
 * - found string offset inside input
 * - (-1) no match
 */
static int searchLut( char* pInput, char (*pLut)[ 3 ], int lutSize, int flag );

/*
 * Parsing
 *
 * @param[in] char* input - AT Command
 * @param[out] at_cmd_cb* cb - handler pointer for the particular command
 * @param[out] uint32_t* timeout - timeout for the particular command
 *
 * Function parses provided raw command string and returns previously saved
 * handler and timeout for the particular command. If command is not found
 * the default handler and default timeout will be returned.
 */
static uint8_t parseInput( char *pInput, T_AT_handler *pHandler, uint32_t *timeout );

/*                 Private Function Definitions
 ******************************************************************************/

static void sendText( uint8_t *pInput, uint8_t delimiter )
{
#ifdef _DEBUG_
        LOG( "< WR >\r\n" );
#endif
    while(*pInput)
    {
#if( _AT_HFC_CONTROL )
        while(!AT_getStateDCE())
            ;
#endif
#ifdef _DEBUG_
        LOG_CH(*tx_input);
#endif
        fpWrite(*pInput++);
    }

    fpWrite(delimiter);
    fpWrite('\n');
#ifdef _DEBUG_
    LOG_CH(delimiter);
    LOG("\r\n");
#endif
}

static uint32_t makeHash( char *pCmd )
{
    uint16_t ch;
    uint32_t hash;

    ch = 0;
    hash = 0x05;
    while((ch = *pCmd++))
        hash += (ch << 1);
    return hash;
}

static uint16_t findHandler( char* pCmd )
{
    uint8_t     len;
    uint16_t    idx;
    uint32_t    hash;

    idx = 0;
    len = strlen(pCmd);
    hash = makeHash(pCmd);
    for(idx = 1; idx < handlerIdx; idx++)
        if(handlerStorage[idx].len == len)
            if(handlerStorage[idx].hash == hash)
                return idx;
    return _AT_UNKNOWN;
}

/* Flags */
#define LUT_IDX         0
#define IN_OFF          1
static int searchLut( char* pInput, char (*pLut)[ 3 ], int lutSize, int flag )
{
    uint8_t     inLen;
    uint8_t     inOff;
    uint8_t     lutLen;
    uint8_t     lutIdx;

    inLen = 0;
    inOff = 0;
    lutLen = 0;
    lutIdx = 0;
    if((inLen = strlen(pInput)) > _AT_CMD_MAXSIZE)
        inLen = _AT_CMD_MAXSIZE;
    for(lutIdx = 1; lutIdx < lutSize; lutIdx++)
    {
        lutLen = strlen(pLut[lutIdx]);
        for(inOff = 0; inOff < inLen; inOff++)
        {
            if(!strncmp(pLut[lutIdx], pInput + inOff, lutLen))
            {
                if(flag == LUT_IDX)
                    return lutIdx;
                else if(flag == IN_OFF)
                    return inOff;
            }
        }
    }
    return -1;
}

static uint8_t parseInput( char *pInput, T_AT_handler *pHandler, uint32_t *timeout )
{
    uint8_t hIdx;
    int     startOff;
    int     endOff;
    int     endIdx;
    char    tmp[ _AT_CMD_MAXSIZE + 1 ];

    hIdx = 0;
    startOff = 0;
    endOff = 0;
    endIdx = 0;
    memset( tmp, 0, _AT_CMD_MAXSIZE + 1 );
    if((startOff = searchLut(pInput, MS_LUT, MS_LUT_S, IN_OFF)) == -1)
        startOff = 0;
    if((endOff = searchLut(pInput, ME_LUT, ME_LUT_S, IN_OFF)) == -1)
        endOff = _AT_CMD_MAXSIZE;
    if((endIdx = searchLut(pInput, ME_LUT, ME_LUT_S, LUT_IDX)) == -1)
        endIdx = 0;
    strncpy(tmp, pInput + startOff, endOff - startOff);
    hIdx = findHandler(tmp);
    *pHandler = handlerStorage[hIdx].handler;
    *timeout = handlerStorage[hIdx].timeout;
    return endIdx;
}

/*                Public Function Definitions
 ******************************************************************************/

void AT_initParser
(
        T_AT_UART_Write     pWrite,
        T_AT_handler        pHandler,
        uint32_t            cmdTimeout,
        uint8_t*            pBuffer,
        uint16_t            bufferSize,
        T_AT_storage*       pStorage,
        uint16_t            storageSize
)
{
    T_AT_storage cmd;

    tlSt            = _AT_ST_TIMER;
    cmd.handler     = pHandler;
    cmd.timeout     = cmdTimeout;
    cmd.hash        = makeHash("");
    cmd.len         = 0;
    fpWrite         = pWrite;
    rxIdx           = 0;
    rxSize          = bufferSize;
    rxStorage       = pBuffer;
    handlerIdx      = 0;
    handlerSize     = storageSize;
    handlerStorage  = pStorage;
    memset((void*)rxStorage, 0, rxSize);
    memset((void*)handlerStorage, 0, handlerSize * sizeof(T_AT_storage));
    handlerStorage[handlerIdx] = cmd;
    handlerIdx++;
    AT_BLOCK();
    AT_UNBLOCK();
    AT_GT_START();
    AT_GT_STOP();
    AT_ST_START();
    AT_ST_STOP();
}

void AT_getChar( char rxInput )
{
    AT_BLOCK();
    AT_ST_START();
    *(rxStorage + rxIdx++) = rxInput;
}

void AT_tick()
{
    if(tfGt)
    {
        if(tcGt++ > tlGt)
        {
            tfGt = false;
            tcGt = 0;
            *(rxStorage + rxIdx) = 0;
            fNoResponse = true;
        }
    }
    if(tfSt)
    {
        if(tcSt++ > tlSt)
        {
            tfSt = false;
            tcSt = 0;
            *(rxStorage + rxIdx) = 0;
            fResponse = true;
        }
    }
}

int AT_saveHandler( char *pCmd, uint32_t timeout, T_AT_handler pHandler )
{
    T_AT_storage cmd;

    if(!pHandler)
        pHandler = handlerStorage[0].handler;
    if(!timeout)
        timeout = handlerStorage[0].timeout;
    cmd.len = strlen(pCmd);
    if(cmd.len >= _AT_CMD_MAXSIZE)
        return 0;
    if(handlerIdx == handlerSize)
        return 0;
    if(findHandler(pCmd))
        return 0;
    cmd.hash        = makeHash(pCmd);
    cmd.timeout     = timeout;
    cmd.handler     = pHandler;
    handlerStorage[handlerIdx] = cmd;
    handlerIdx++;
    return (handlerSize - handlerIdx);
}

void AT_cmdSingle( char *pCmd )
{
    T_AT_handler tmpCb;

    AT_WAIT_RESPONSE();
    parseInput(pCmd, &tmpCb, &tlGt);
    sendText(pCmd, _AT_TERMINATE);
    AT_BLOCK();
    AT_GT_START();
    AT_WAIT_RESPONSE();
    Delay_ms( 100 );
}

void AT_cmdDouble( char *pCmd, char *pArg1 )
{
    T_AT_handler tmpCb;

    AT_WAIT_RESPONSE();
    parseInput(pCmd, &tmpCb, &tlGt);
    sendText(pCmd, _AT_TERMINATE);
    AT_BLOCK();
    AT_GT_START();
    AT_WAIT_RESPONSE();
    sendText(pArg1, _AT_TERMINATE_ADD);
    AT_BLOCK();
    AT_GT_START();
    AT_WAIT_RESPONSE();
}

void AT_cmdTriple( char *pCmd, char *pArg1, char *pArg2 )
{
    T_AT_handler tmpCb;

    AT_WAIT_RESPONSE();
    parseInput(pCmd, &tmpCb, &tlGt);
    sendText(pCmd, _AT_TERMINATE );
    AT_BLOCK();
    AT_GT_START();
    AT_WAIT_RESPONSE();
    sendText(pArg1, _AT_TERMINATE_ADD);
    AT_BLOCK();
    AT_GT_START();
    AT_WAIT_RESPONSE();
    sendText(pArg2, _AT_TERMINATE_ADD);
    AT_BLOCK();
    AT_GT_START();
    AT_WAIT_RESPONSE();
}

void AT_process()
{
    /*
     * TODO: Check free space in buffer and handle situation if
     * buffer is near the end.
     *
     * Hardware flow control handle
     */
    if(fBuffer)
    {



    }

    if(fResponse)
    {
#if( _AT_HFC_CONTROL )
            AT_setStateDTE(false);
#endif
        AT_ST_STOP();
        AT_GT_STOP();
        flags = parseInput(rxStorage, &fpHandler, &tlGt);
        fpHandler(rxStorage, &flags);
        AT_CLEAN_BUFFER();
        AT_UNBLOCK();
#if( _AT_HFC_CONTROL )
        AT_setStateDTE(true);
#endif
    }

    if(fNoResponse)
    {
#if( _AT_HFC_CONTROL )
        AT_setStateDTE(false);
#endif
        AT_ST_STOP();
        AT_GT_STOP();
        flags = parseInput(rxStorage, &fpHandler, &tlGt);
        fpHandler(rxStorage, &flags);
        AT_CLEAN_BUFFER();
        AT_UNBLOCK();
#if( _AT_HFC_CONTROL )
        AT_setStateDTE(true);
#endif
    }
}

/*------------------------------------------------------------------------------

  at_engine.c

  Copyright (c) 2011-2017 MikroElektronika.  All right reserved.

    This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.

------------------------------------------------------------------------------*/
