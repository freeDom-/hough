#include <inttypes.h>
#include <stdlib.h>

#define WIDTH 400
#define HEIGHT 400
#define SIZE WIDTH * HEIGHT
#define MAXCIRCLES 256
#define RADIUS 44
#define RADIUS_UPPER_BOUNDS 50
#define RADIICOUNT RADIUS_UPPER_BOUNDS - RADIUS + 1
#define THRESHOLD 33
#define INPUT_IMAGE "/home/dom/Bachelorarbeit/img/gen/canny.dat"

#define SQRT2 1.414213562

typedef struct{
	unsigned int x;
	unsigned int y;
	uint8_t r;
} circle;

void hough(uint8_t input[SIZE], circle* output, unsigned int* circleCount);
