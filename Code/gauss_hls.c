#include "include/gauss_hls.h"

/*
** Calculates (n+1)th row of pascals triangle
*/
unsigned long* pascal(unsigned long n) {
    unsigned long* line = malloc((n+1) * sizeof(unsigned long));
    line[0] = 1;

    for(int i = 0; i < n; i++) {
        line[i+1] = line[i] * (n-i) / (i+1);
    }
    return line;
}

/*
** Creates a gaussian kernel
*/
unsigned long* kernelGenerator(unsigned long kernelSize) {
    unsigned long* kernel = calloc(kernelSize, sizeof(unsigned long)); // reserve memory and initialize with 0
    unsigned long* pascalRow = pascal(kernelSize-1);

    for(int i = 0; i < kernelSize; i++) {
        for(int j = 0; j < kernelSize; j++) {
            kernel[i] += pascalRow[i] * pascalRow[j];
        }
    }
    free(pascalRow);
    return kernel;
}

void gauss(uint8_t input[SIZE], uint8_t output[SIZE]) {
	static uint8_t temp[SIZE];
	unsigned long *filter = malloc(WIDTH * HEIGHT * sizeof(uint8_t));
	unsigned long temp1, temp2;
	uint8_t offset = KERNELSIZE >> 1;
	uint8_t shift = (KERNELSIZE-1) << 1;

	filter = kernelGenerator(KERNELSIZE);

    // Apply horizontal filter on pixels and save in temp
    for(int y = 0; y < HEIGHT; y++) {
        for(int x = 0; x < WIDTH-offset; x++) {
            // Apply filter on borders
            if(x < offset) {
                temp1 = 0;
                temp2 = 0;
                // Filter pixels on edges (offset-x) times
                for(int i = 0; i < offset-x; i++) {
                    temp1 += filter[i] * input[y * WIDTH];
                    temp2 += filter[i] * input[y * WIDTH + (WIDTH-1)];
                }
                // Apply rest of the filter normally
                for(int i = offset-x; i < KERNELSIZE; i++) {
                    temp1 += filter[i] * input[y * WIDTH + (x+i-offset)];
                    temp2 += filter[i] * input[y * WIDTH + (WIDTH-1-x-i+offset)];
                }
                temp[y * WIDTH + x] = temp1 >> shift;
                temp[y * WIDTH + (WIDTH-1-x)] = temp2 >> shift;
            }
            // Apply filter on rest
            else {
                temp1 = 0;
                for(int i = 0; i < KERNELSIZE; i++) {
                    temp1 += filter[i] * input[y * WIDTH + (x+i-offset)];
                }
                temp[y * WIDTH + x] = temp1 >> shift;
            }
        }
    }
}
