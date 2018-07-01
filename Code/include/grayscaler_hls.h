#include <inttypes.h>

#define WIDTH 400
#define HEIGHT 400
#define SIZE WIDTH * HEIGHT
#define INPUT_IMAGE "/home/dom/Bachelorarbeit/img/gen/euro.dat"
#define SW_IMAGE "/home/dom/Bachelorarbeit/img/gen/grayscaler.dat"
#define OUTPUT_HW "/home/dom/Bachelorarbeit/img/gen/grayscaler_hw.dat"

void grayscaler(uint32_t input[SIZE], uint8_t output[SIZE]);
