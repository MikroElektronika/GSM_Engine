#ifndef TFT_RESOURCES_H
#define TFT_RESOURCES_H

#include <built_in.h>

#define DISPLAY_FRAME(click_name, chip_name, board_name)                       \
        TFT_Set_Pen( CL_WHITE, 1 );                                            \
        TFT_Set_Brush( 1, CL_WHITE, 0, 0, 0, 0 );                              \
        TFT_Set_Font( TFT_defaultFont, CL_BLACK, FO_HORIZONTAL );              \
        TFT_Fill_Screen( CL_WHITE );                                           \
        TFT_Set_Pen( CL_BLACK, 1 );                                            \
        TFT_Line(  20,  46, 300,  46 );                                        \
        TFT_Line(  20,  70, 300,  70 );                                        \
        TFT_Line(  20, 220, 300, 220 );                                        \
        TFT_Set_Pen( CL_WHITE, 1 );                                            \
        TFT_Set_Font( &HandelGothic_BT21x22_Regular, CL_RED, FO_HORIZONTAL );  \
        TFT_Write_Text( click_name, 20, 14 );                                  \
        TFT_Set_Font( &Tahoma15x16_Bold, CL_BLUE, FO_HORIZONTAL );             \
        TFT_Write_Text( chip_name, 20, 50 );                                   \
        TFT_Set_Font( &Verdana12x13_Regular, CL_BLACK, FO_HORIZONTAL );        \
        TFT_Write_Text( board_name, 20, 223 );                                 \
        TFT_Set_Font( &Verdana12x13_Regular, CL_RED, FO_HORIZONTAL );          \
        TFT_Write_Text( "www.mikroe.com", 200, 223 );                          \
        TFT_Set_Font( &Tahoma15x16_Bold, CL_BLACK, FO_HORIZONTAL );            

#ifdef __MIKROC_PRO_FOR_FT90x__                                                 
#define DISPLAY_INIT(click_name, chip_name, board_name)                        \
{                                                                              \
    TFT_Set_Active(Set_Index, Write_Command, Write_Data);                      \
    TFT_Init_ILI9341_8bit( 320, 240 );                                         \
    DISPLAY_FRAME(click_name, chip_name, board_name)                           \
}
#else
#define DISPLAY_INIT(click_name, chip_name, board_name)                        \
{                                                                              \
    TFT_Init_ILI9341_8bit( 320, 240 );                                         \
    DISPLAY_FRAME(click_name, chip_name, board_name)                           \
}
#endif                                                                          


#ifdef __MIKROC_PRO_FOR_FT90x__


void Auxiliary_Code();

void Set_Index(unsigned short index);

void Write_Command(unsigned short cmd);

void Write_Data(unsigned int _data);
#endif

const code char HandelGothic_BT21x22_Regular[];
const code char Verdana12x13_Regular[];
const code char Tahoma15x16_Bold[];
const code char Arial16x18_Regular[];
const code char Arial16x19_Bold[];

#endif // TFT_RESOURCES_H