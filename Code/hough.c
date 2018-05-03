/*
** Performs a hough transform on an edge image and returns the number of circles found
*/

#include "hough.h"

int hough(uint8_t* input, unsigned int width, unsigned int height, uint8_t radius, uint8_t range) {
    uint8_t (*pixels)[width] = (uint8_t(*)[width]) input;
    printf("check.1\n");
    int acc[height][width][2*range + 1];  // Accumulator matrix
    int h, w;

    printf("check.2\n");

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            if(pixels[y][x] != 0) {
                printf("edge found.\n");
                for(int r = radius-range; r <= radius+range; r++) {
                    for(int t = 0; t <= 360; t++) {
                        w = x - r * cos(t * PI / 180);   // Polar coordinates of circles center
                        h = y - r * sin(t * PI / 180);   // Polar coordinates of circles center
                        //printf("(w, h): (%i, %i)\n", w, h);
                        acc[h][w][r-radius-range] += 1;
                    }
                }
            }
        }
    }

    // Check numbers found
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            for(int r = 0; r < 2*range+1; r++) {
                printf("A[%i][%i][%i]: %i\n", y, x, radius-range+r, acc[y][x][r]);
            }
        }
    }

    return 0;
}
