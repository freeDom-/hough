/*
** Performs a hough transform on an edge image and returns the number of circles found
*/

#include "hough.h"

int hough(uint8_t* input, unsigned int width, unsigned int height, uint8_t radius, uint8_t range) {
    uint8_t (*pixels)[width] = (uint8_t(*)[width]) input;
    unsigned int* acc = calloc(height * width * (2*range + 1), sizeof(unsigned int));
    int h, w;
    unsigned int max;
    uint8_t maxRadius;
    int threshold = 900;

    // Vote accumulator matrix
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            if(pixels[y][x] != 0) {
                for(int r = radius-range; r <= radius+range; r++) {
                    for(int t = 0; t <= 360; t++) {
                        w = x - r * cos(t * PI / 180);   // Polar coordinates of circles center
                        h = y - r * sin(t * PI / 180);   // Polar coordinates of circles center
                        if(w >= 0 && w < width && h >= 0 && h < height) {
                            acc[h * w * (2*range + 1) + w * (2*range + 1) + r-(radius-range)] += 1;
                        }
                    }
                }
            }
        }
    }

    // Calculate maximal radii
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            maxRadius = 0;
            max = 0;
            for(int r = 0; r < 2*range + 1; r++) {
                if(acc[y * x * (2*range + 1)+ r] > max) {
                    max = acc[y * x * (2*range + 1) + x * (2*range + 1) + r];
                    maxRadius = r;
                }
            }
            acc[y * x * (2*range + 1) + x * (2*range + 1)] = max;
            acc[y * x * (2*range + 1) + x * (2*range + 1) + 1] = maxRadius;
        }
    }

    // Find local maximum
    for(int y = 1; y < height-1; y++) {
        for(int x = 1; x < width-1; x++) {
            unsigned int elem = acc[y * x * (2*range + 1) + x * (2*range + 1)];
            if(elem > threshold &&
               elem > acc[(y-1) * (x-1) * (2*range + 1) + (x-1) * (2*range + 1)] &&
               elem > acc[(y-1) *   x   * (2*range + 1) +   x   * (2*range + 1)] &&
               elem > acc[(y-1) * (x+1) * (2*range + 1) + (x+1) * (2*range + 1)] &&
               elem > acc[  y   * (x-1) * (2*range + 1) + (x-1) * (2*range + 1)] &&
               elem > acc[  y   * (x+1) * (2*range + 1) + (x+1) * (2*range + 1)] &&
               elem > acc[(y+1) * (x-1) * (2*range + 1) + (x-1) * (2*range + 1)] &&
               elem > acc[(y+1) *   x   * (2*range + 1) +   x   * (2*range + 1)] &&
               elem > acc[(y+1) * (x+1) * (2*range + 1) + (x+1) * (2*range + 1)]) {
                printf("A[%i][%i][%i]: %i\n", y, x, acc[y * x * (2*range + 1) + x * (2*range + 1) + 1], elem);
            }
        }
    }

    free(acc);

    return 0;
}
