#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif /*STDIO_H_INCLUDED*/
#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif /*STDLIB_H_INCLUDED*/
#ifndef INTTYPES_H_INCLUDED
#define INTTYPES_H_INCLUDED
#include <inttypes.h>
#endif /*INTTYPES_H_INCLUDED*/
#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED
#include <math.h>
#define PI 3.14159265
#endif /*MATH_H_INCLUDED*/

extern int circleCount;

typedef struct{
	unsigned int x;
	unsigned int y;
	uint8_t r;
} circle;

circle* hough(uint8_t* input, unsigned int width, unsigned int height, uint8_t radius, uint8_t range, unsigned int threshold);
