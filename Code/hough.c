/*
** Performs a hough transform on an edge image and returns the number of circles found
*/

#include "hough.h"

int hough(uint8_t* input, unsigned int width, unsigned int height, uint8_t radius, uint8_t range) {
    uint8_t (*pixels)[width] = (uint8_t(*)[width]) input;
    unsigned int* acc = calloc(height * width * radius * 2 + 1, sizeof(int));
    int h, w;
    unsigned int max;
    uint8_t maxRadius;

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            if(pixels[y][x] != 0) {
                for(int r = radius-range; r <= radius+range; r++) {
                    for(int t = 0; t <= 360; t++) {
                        w = x - r * cos(t * PI / 180);   // Polar coordinates of circles center
                        h = y - r * sin(t * PI / 180);   // Polar coordinates of circles center
                        if(w >= 0 && h >= 0) {
                            acc[h * w + r-radius+range] += 1;
                        }
                    }
                }
            }
        }
    }

    // Check numbers found 
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            maxRadius = 0;
            max = 0;
            for(int r = 0; r < 2*range+1; r++) {
                if(acc[y * x + r] > max) {
                    max = acc[y * x + r];
                    maxRadius = r;
                }
            }
            printf("A[%i][%i][%i]: %i\n", y, x, radius-range+maxRadius, max);
        }
    }

    free(acc);

    return 0;
}
