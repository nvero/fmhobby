/*

                         \\\|///
                       \\  - -  //
                        (  @ @  )
+---------------------oOOo-(_)-oOOo-------------------------+
|                                                           |
|                       MyType.h                            |
|                     by Xiaoran Liu                        |
|                        2005.3.16                          |
|                                                           |
|                    ZERO research group                    |
|                        www.the0.net                       |
|                                                           |
|                            Oooo                           |
+----------------------oooO--(   )--------------------------+
                      (   )   ) /
                       \ (   (_/
                        \_)     

*/
#ifndef __ZT018_H__
#define __ZT018_H__

#include "Types.h"

void LCD_I2C0Init(void);
void LCD_BMP( U8 x0, U8 y0, U8 x1, U8 y1 , U8 * Bmp );
void LCD_Pixel( U8 x, U8 y, U16 Color );
void LCD_Line( U8 x0, U8 y0, U8 x1, U8 y1 , U16 Color );
void LCD_Rectangle( U8 x0, U8 y0, U8 x1, U8 y1 , U16 Color );
void LCD_Circle( U8 x0, U8 y0, U8 r, U16 Color );
void LCD_RectangleFill( U8 x0, U8 y0, U8 x1, U8 y1 , U16 Color );
void LCD_CircleFill( U8 x0, U8 y0, U8 r, U16 Color );
void LCD_Ellipse( U8 x0, U8 y0, U8 x1, U8 y1 , U16 Color );
void LCD_EllipseFill( U8 x0, U8 y0, U8 x1, U8 y1 , U16 Color );
void LCD_Pieslice( U8 x0, U8 y0, U8 r, U16 start, U16 end, U16 Color );

void LCD_MoveTo( U8 x, U8 y );
void LCD_PrintU8( U8 data );
void LCD_PrintX8( U8 data );
void LCD_SetFont( U8 data );

U8 LCD_Status_Read (U8 Cmd);

void LCD_Clear_Screen( U16 B );
void PutString( U8 x, U8 y, char *s, U16 F, U16 B );

void putch( char c );
void printStr( char *s );


#endif
