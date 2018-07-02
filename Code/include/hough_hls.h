#include <inttypes.h>

#define WIDTH 400
#define HEIGHT 400
#define SIZE WIDTH * HEIGHT
#define INPUT_IMAGE "/home/dom/Bachelorarbeit/img/gen/canny.dat"

typedef struct{
	unsigned int x;
	unsigned int y;
	uint8_t r;
} circle;

void hough(uint8_t input[SIZE], circle* output, unsigned int* circleCount);
