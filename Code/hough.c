/*
** Performs a hough transform on an edge image and returns the number of circles found
*/

#include "hough.h"


void vote(unsigned int* acc, unsigned int w, unsigned int h, unsigned int rc, int x, int y, unsigned int r) {
    if(x >= 0 && x < w && y >= 0 && y < h && r < rc) {
        acc[y * w * rc + x * rc + r] += 1;
    }
}

circle* hough(uint8_t* input, unsigned int width, unsigned int height, unsigned int radius, unsigned int radiusUpperBounds, unsigned int threshold, unsigned int* circleCount) {
    unsigned int radiiCount = radiusUpperBounds - radius + 1;
    unsigned int *acc = calloc(height * width * radiiCount, sizeof(unsigned int));
    int maxCircles = 8;
    circle *circles = malloc(maxCircles * sizeof(circle));

    #ifdef _OPENMP
    #pragma omp parallel for
    #endif
    // Vote accumulator matrix
    for(int y0 = 0; y0 < height; y0++) {
        for(int x0 = 0; x0 < width; x0++) {
            if(input[y0 * width + x0] != 0) {
                for(unsigned int r = radius; r <= radiusUpperBounds; r++) {
                    // Bresenham
                    int dy, dx;
                    int x = r;
                    int y = 0;
                    int error = r;
                    vote(acc, width, height, radiiCount, x0, y0+r, r-radius);
                    vote(acc, width, height, radiiCount, x0, y0-r, r-radius);
                    vote(acc, width, height, radiiCount, x0+r, y0, r-radius);
                    vote(acc, width, height, radiiCount, x0-r, y0, r-radius);

                    while(y < x) {
                        dy = y*2 + 1;
                        y += 1;
                        error -= dy;
                        if(error < 0) {
                            dx = 1 - x*2;
                            x -= 1;
                            error -= dx;
                        }
                        vote(acc, width, height, radiiCount, x0+x, y0+y, r-radius);
                        vote(acc, width, height, radiiCount, x0-x, y0+y, r-radius);
                        vote(acc, width, height, radiiCount, x0-x, y0-y, r-radius);
                        vote(acc, width, height, radiiCount, x0+x, y0-y, r-radius);
                        vote(acc, width, height, radiiCount, x0+y, y0+x, r-radius);
                        vote(acc, width, height, radiiCount, x0-y, y0+x, r-radius);
                        vote(acc, width, height, radiiCount, x0-y, y0-x, r-radius);
                        vote(acc, width, height, radiiCount, x0+y, y0-x, r-radius);
                    }
                }
            }
        }
    }

    // Find biggest values in relation to their radius
    // TODO parallelize???
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            for(int r = radiiCount-1; r >= 0; r--) {
                int currentRadius = r+radius;
                if(acc[y * width * radiiCount + x * radiiCount + r] / (4*round(currentRadius*SQRT2)) * 100 > threshold) {
                    //printf("circle found at: (%i, %i) with radius %i\n", x, y, currentRadius);
                    // Add circle
                    circle tmp = {x, y, currentRadius};
                    if(*circleCount == maxCircles) {
                        maxCircles = maxCircles << 2;
                        circles = (circle*) realloc(circles, maxCircles * sizeof(circle));
                    }
                    circles[*circleCount] = tmp;
                    (*circleCount)++;

                    // Clear circle from hough space
                    for(int yy = y; yy < y+currentRadius; yy++) {
                        for(int xx = x-currentRadius; xx < x+currentRadius; xx++) {
                            for(int rr = 0; rr < radiiCount; rr++) {
                                acc[yy * width * radiiCount +  xx * radiiCount + rr] = 0;
                            }
                        }
                    }
                }
            }
        }
    }

    free(acc);
    return circles;
}
