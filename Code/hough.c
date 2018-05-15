/*
** Performs a hough transform on an edge image and returns the number of circles found
*/

#include "hough.h"

int circleCount = 0;

circle* hough(uint8_t* input, unsigned int width, unsigned int height, uint8_t radius, uint8_t radiusUpperBounds, unsigned int threshold) {
    uint8_t (*pixels)[width] = (uint8_t(*)[width]) input;
    uint8_t maxRadius[height][width];   // storing the radius with the highest voting for each x and y
    unsigned int radiiCount = radiusUpperBounds - radius + 1;
    unsigned int* acc = calloc(height * width * radiiCount, sizeof(unsigned int));
    unsigned int current;
    unsigned int max;
    int maxCircles = 8;
    circle* circles = (circle*) malloc(maxCircles * sizeof(circle));

#pragma omp parallel
    {
#pragma omp for
        // Vote accumulator matrix
        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                if(pixels[y][x] != 0) {
                    for(int r = radius; r <= radiusUpperBounds; r++) {
                        for(int t = 0; t <= 180; t++) {
                            int h, w;
                            w = x - r * cos(t * PI / 180);   // Polar coordinates of circles center
                            h = y - r * sin(t * PI / 180);   // Polar coordinates of circles center
                            if(w >= 0 && w < width && h >= 0 && h < height) {
                                acc[h * width * radiiCount + w * radiiCount + r-radius] += 1;
                            }
                        }
                    }
                }
            }
        }
    }

    // Find biggest radius
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            maxRadius[y][x] = 0;
            max = 0;
            for(int r = 0; r < radiiCount; r++) {
                current = acc[y * width * radiiCount + x * radiiCount + r];
                if(current > max) {
                    max = current;
                    maxRadius[y][x] = r;
                }
            }
            acc[y * width * radiiCount + x * radiiCount] = max;
        }
    }

    // Find local maximum
    for(int y = 1; y < height-1; y++) {
        for(int x = 1; x < width-1; x++) {
            current = acc[y * width * radiiCount + x * radiiCount];
            if(current > threshold &&
               current > acc[(y-1) * width * radiiCount + (x-1) * radiiCount] &&
               current > acc[(y-1) * width * radiiCount +   x   * radiiCount] &&
               current > acc[(y-1) * width * radiiCount + (x+1) * radiiCount] &&
               current > acc[  y   * width * radiiCount + (x-1) * radiiCount] &&
               current > acc[  y   * width * radiiCount + (x+1) * radiiCount] &&
               current > acc[(y+1) * width * radiiCount + (x-1) * radiiCount] &&
               current > acc[(y+1) * width * radiiCount +   x   * radiiCount] &&
               current > acc[(y+1) * width * radiiCount + (x+1) * radiiCount]) {
                circle tmp = {x, y, radius + maxRadius[y][x]};
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
