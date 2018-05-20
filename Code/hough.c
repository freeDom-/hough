/*
** Performs a hough transform on an edge image and returns the number of circles found
*/

#include "hough.h"

unsigned int circleCount = 0;
unsigned int* a;
unsigned int* w;
unsigned int* h;
unsigned int* rc;

unsigned int getCircleCount() {
    return circleCount;
}

void vote(int x, int y, unsigned int r) {
    if(x >= 0 && x < *w && y >= 0 && y < *h && r < *rc) {
        a[y * *w * *rc + x * *rc + r] += 1;
    }
}

circle* hough(uint8_t* input, unsigned int width, unsigned int height, unsigned int radius, unsigned int radiusUpperBounds, unsigned int threshold) {
    uint8_t (*pixels)[width] = (uint8_t(*)[width]) input;
    uint8_t maxRadius[height][width];   // storing the radius with the highest voting for each x and y
    unsigned int radiiCount = radiusUpperBounds - radius + 1;
    unsigned int* acc = calloc(height * width * radiiCount, sizeof(unsigned int));
    unsigned int current;
    unsigned int max;
    int maxCircles = 8;
    circle* circles = (circle*) malloc(maxCircles * sizeof(circle));
    // Set convenience pointers
    a = &acc[0];
    w = &width;
    h = &height;
    rc = &radiiCount;

#pragma omp parallel for
    // Vote accumulator matrix
    for(int y0 = 0; y0 < height; y0++) {
        for(int x0 = 0; x0 < width; x0++) {
            if(pixels[y0][x0] != 0) {
                for(unsigned int r = radius; r <= radiusUpperBounds; r++) {
                    // takes 4-5 times longer than Bresenham
                    /*for(int t = 0; t <= 180; t++) {
                        int h, w;
                        w = x0 - r * cos(t * PI / 180);   // Polar coordinates of circles center
                        h = y0 - r * sin(t * PI / 180);   // Polar coordinates of circles center
                        if(w >= 0 && w < width && h >= 0 && h < height) {
                            acc[h * width * radiiCount + w * radiiCount + r-radius] += 1;
                        }
                    }*/
                    // Bresenham
                    int dy, dx;
                    int x = r;
                    int y = 0;
                    int error = r;
                    vote(x0, y0+r, r-radius);
                    vote(x0, y0-r, r-radius);
                    vote(x0+r, y0, r-radius);
                    vote(x0-r, y0, r-radius);

                    while(y < x) {
                        dy = y*2 + 1;
                        y += 1;
                        error -= dy;
                        if(error < 0) {
                            dx = 1 - x*2;
                            x -= 1;
                            error -= dx;
                        }
                        vote(x0+x, y0+y, r-radius);
                        vote(x0-x, y0+y, r-radius);
                        vote(x0-x, y0-y, r-radius);
                        vote(x0+x, y0-y, r-radius);
                        vote(x0+y, y0+x, r-radius);
                        vote(x0-y, y0+x, r-radius);
                        vote(x0-y, y0-x, r-radius);
                        vote(x0+y, y0-x, r-radius);
                    }
                }
            }
        }
    }
    
#pragma omp parallel for private(max, current)
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
