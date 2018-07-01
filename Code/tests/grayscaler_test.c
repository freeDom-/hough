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

	// Load software result
	f_sw = fopen(SW_IMAGE, "r");
	if(f == NULL) {
		fprintf(stderr, "ERROR: Could not open file: " SW_IMAGE);
		exit(EXIT_FAILURE);
	}
	index = 0;
	while(fscanf(f_sw, "%s", buff) != EOF) {
		swResult[index] = strtol(buff, NULL, 16);
		index++;
	}
	fclose(f_sw);

	// Convert pixels to grayscale using HW
	grayscaler(input, hwResult);

	// Compare results and save
	f_hw = fopen(OUTPUT_HW, "w");
    for(y = 0; y < HEIGHT; y++) {
        for(x = 0; x < WIDTH; x++) {
        	fprintf(f_hw, "%X ", hwResult[y * WIDTH + x]);
        	index = y * WIDTH + x;
        	if(abs(hwResult[index] - swResult[index]) > 1) {
        		errCnt++;
        		fprintf(stderr, "ERROR: mismatch at position (%i, %i)\n", x, y);
        	}
        }
        fprintf(f_hw, "\n");
    }
    if(errCnt) {
    	fprintf(stderr, "ERROR: %i mismatches detected!\n", errCnt);
    }
    else fprintf(stderr, "Test passed!\n");
    fclose(f_hw);

	return errCnt;
}
