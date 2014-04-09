#ifndef __functions_H
#define __functions_H



#include "alientek.h"

#define toRad( Math_D )	((float)(Math_D)*0.0174532925f)
#define toDeg( Math_R )	((float)(Math_R)*57.2957795f)

extern 	int 			delay_buf;


void 		Int2Char(u16 number,char* buffer, int digitNumber );
int  		Hex2Dec(u8 return_value_double,u8 return_value_single);
int  		dec2Dec(u8 return_value_double,u8 return_value_single);
char 		checksum(char* buffer);
float 		degree2radians(float degree);
void 		DELAY_ms(int ms);
float 		arm_cos_f32(float x);
float   	arm_sin_f32(float x);
#endif

