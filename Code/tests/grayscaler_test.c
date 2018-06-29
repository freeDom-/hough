#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/grayscaler_hls.h"

int main(int argc, char** argv) {
	int x, y;
	unsigned int errCnt = 0;
	FILE* f;
	FILE* f_hw;
	FILE* f_sw;
	char buff[9];
	unsigned long index = 0;

    static uint32_t input[SIZE];
    static uint8_t swResult[SIZE];
    static uint8_t hwResult[SIZE];

    // Load test image
    f = fopen(INPUT_IMAGE, "r");
	if(f == NULL) {
		fprintf(stderr, "ERROR: Could not open file: " INPUT_IMAGE);
		exit(EXIT_FAILURE);
	}
	while(fscanf(f, "%s", buff) != EOF) {
		input[index] = strtol(buff, NULL, 16);
		index++;
	}
	fclose(f);

	// Convert pixels to grayscale using SW
    for(y = 0; y < HEIGHT; y++) {
        for(x = 0; x < WIDTH; x++) {
			int index = y * WIDTH + x;
			uint8_t r, g, b;

			r = input[index] >> 16 & 0xFF;
			g = input[index] >> 8 & 0xFF;
			b = input[index] & 0xFF;

			swResult[index] = 0.3*r + 0.59*g + 0.11*b;
        }
    }

	// Convert pixels to grayscale using HW
	grayscaler(input, hwResult);

	// Compare results and save
	f_hw = fopen(OUTPUT_HW, "w");
	f_sw = fopen(OUTPUT_SW, "w");
    for(y = 0; y < HEIGHT; y++) {
        for(x = 0; x < WIDTH; x++) {
        	fprintf(f_hw, "%X ", hwResult[y * WIDTH + x]);
        	fprintf(f_sw, "%X ", swResult[y * WIDTH + x]);
        	if(hwResult[y * WIDTH + x] != swResult[y * WIDTH + x]) {
        		errCnt++;
        		fprintf(stderr, "ERROR: mismatch at position (%i, %i)\n", x, y);
        	}
        }
        fprintf(f_hw, "\n");
        fprintf(f_sw, "\n");
    }
    if(errCnt) {
    	fprintf(stderr, "ERROR: %i mismatches detected!\n", errCnt);
    }
    else fprintf(stderr, "Test passed!\n");
    fclose(f_hw);
    fclose(f_sw);

	return errCnt;
}
