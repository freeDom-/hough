#include <inttypes.h>

#define WIDTH 400
#define HEIGHT 400
#define SIZE WIDTH * HEIGHT
#define INPUT_IMAGE "/home/dom/Bachelorarbeit/img/gen/data.dat"
#define OUTPUT_HW "/home/dom/Bachelorarbeit/img/gen/grayscaler_hw.dat"
#define OUTPUT_SW "/home/dom/Bachelorarbeit/img/gen/grayscaler_sw.dat"

void grayscaler(uint32_t input[SIZE], uint8_t output[SIZE]);
