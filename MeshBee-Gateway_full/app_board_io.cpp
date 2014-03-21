#include "app_board_io.h"
//#include "C12832_lcd.h"

//extern C12832_LCD lcd;

extern "C" void lcd_printf( const char* fmt, ...  )
{
    char buffer[ 64 ];
    
    va_list ap;
    va_start( ap, fmt );
    vsnprintf( buffer, 64, fmt, ap );
    va_end( ap );
    
    //lcd.cls();
    //lcd.locate( 0, 3 );
    //lcd.printf( buffer );
    //wait( 1.0 );
}