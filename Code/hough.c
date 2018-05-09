/*
** Performs a hough transform on an edge image and returns the number of circles found
*/

#include "hough.h"

int circleCount = 0;

circle* hough(uint8_t* input, unsigned int width, unsigned int height, uint8_t radius, uint8_t range, unsigned int threshold) {
    unsigned int rangeSize = 2*range + 1;
    uint8_t (*pixels)[width] = (uint8_t(*)[width]) input;
    unsigned int* acc = calloc(height * width * rangeSize, sizeof(unsigned int));
    int maxCircles = 8;
    circle* circles = (circle*) malloc(maxCircles * sizeof(circle));
    int h, w;
    unsigned int max;
    unsigned int current;
    uint8_t maxRadius[height][width];

    // Vote accumulator matrix
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            if(pixels[y][x] != 0) {
                for(int r = radius-range; r <= radius+range; r++) {
                    for(int t = 0; t <= 180; t++) {
                        w = x - r * cos(t * PI / 180);   // Polar coordinates of circles center
                        h = y - r * sin(t * PI / 180);   // Polar coordinates of circles center
                        if(w >= 0 && w < width && h >= 0 && h < height) {
                            acc[h * width * rangeSize + w * rangeSize + r-(radius-range)] += 1;
                        }
                    }
                }
            }
        }
    }

    // Calculate maximal radii
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            maxRadius[y][x] = 0;
            max = 0;
            for(int r = 0; r < rangeSize; r++) {
                current = acc[y * width * rangeSize + x * rangeSize + r];
                if(current > max) {
                    max = current;
                    maxRadius[y][x] = r;
                }
            }
            acc[y * width * rangeSize + x * rangeSize] = max;
        }
    }

    // Find local maximum
    for(int y = 1; y < height-1; y++) {
        for(int x = 1; x < width-1; x++) {
            current = acc[y * width * rangeSize + x * rangeSize];
            if(current > threshold &&
               current > acc[(y-1) * width * rangeSize + (x-1) * rangeSize] &&
               current > acc[(y-1) * width * rangeSize +   x   * rangeSize] &&
               current > acc[(y-1) * width * rangeSize + (x+1) * rangeSize] &&
               current > acc[  y   * width * rangeSize + (x-1) * rangeSize] &&
               current > acc[  y   * width * rangeSize + (x+1) * rangeSize] &&
               current > acc[(y+1) * width * rangeSize + (x-1) * rangeSize] &&
               current > acc[(y+1) * width * rangeSize +   x   * rangeSize] &&
               current > acc[(y+1) * width * rangeSize + (x+1) * rangeSize]) {
                circle tmp = {x, y, (radius-range) + maxRadius[y][x]};
                if(circleCount == maxCircles) {
                    maxCircles = maxCircles << 2;
                    circles = (circle*) realloc(circles, maxCircles * sizeof(circle));
                }
                circles[circleCount] = tmp;
                circleCount++;
            }
        }
    }

    free(acc);

    return circles;
}
