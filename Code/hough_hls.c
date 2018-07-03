#include "include/hough_hls.h"

void vote(unsigned int* acc, int x, int y, unsigned int r) {
    if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT && r < RADIICOUNT) {
        acc[y * WIDTH * RADIICOUNT + x * RADIICOUNT + r] += 1;
    }
}

void hough(uint8_t input[SIZE], circle output[MAXCIRCLES], unsigned int* circleCount) {
    static unsigned int acc[SIZE * RADIICOUNT] = {0};

    // Vote accumulator matrix
    for(int y0 = 0; y0 < HEIGHT; y0++) {
        for(int x0 = 0; x0 < WIDTH; x0++) {
            if(input[y0 * WIDTH + x0] != 0) {
                for(unsigned int r = RADIUS; r <= RADIUS_UPPER_BOUNDS; r++) {
                    // Bresenham
                    int dy, dx;
                    int x = r;
                    int y = 0;
                    int error = r;
                    vote(acc, x0, y0+r, r-RADIUS);
                    vote(acc, x0, y0-r, r-RADIUS);
                    vote(acc, x0+r, y0, r-RADIUS);
                    vote(acc, x0-r, y0, r-RADIUS);

                    while(y < x) {
                        dy = y*2 + 1;
                        y += 1;
                        error -= dy;
                        if(error < 0) {
                            dx = 1 - x*2;
                            x -= 1;
                            error -= dx;
                        }
                        vote(acc, x0+x, y0+y, r-RADIUS);
                        vote(acc, x0-x, y0+y, r-RADIUS);
                        vote(acc, x0-x, y0-y, r-RADIUS);
                        vote(acc, x0+x, y0-y, r-RADIUS);
                        vote(acc, x0+y, y0+x, r-RADIUS);
                        vote(acc, x0-y, y0+x, r-RADIUS);
                        vote(acc, x0-y, y0-x, r-RADIUS);
                        vote(acc, x0+y, y0-x, r-RADIUS);
                    }
                }
            }
        }
    }

    // Find biggest values in relation to their RADIUS
    // TODO parallelize???
    for(int y = 0; y < HEIGHT; y++) {
        for(int x = 0; x < WIDTH; x++) {
            for(int r = RADIICOUNT-1; r >= 0; r--) {
                int currentRadius = r+RADIUS;
                if(acc[y * WIDTH * RADIICOUNT + x * RADIICOUNT + r] / (4*round(currentRadius*SQRT2)) * 100 > THRESHOLD) {
                    // Add circle
                    if(*circleCount < MAXCIRCLES) {
						circle tmp = {x, y, currentRadius};
						output[*circleCount] = tmp;
						(*circleCount)++;
                    }

                    // Clear circle from hough space
                    for(int yy = y; yy < y+currentRadius; yy++) {
                        for(int xx = x-currentRadius; xx < x+currentRadius; xx++) {
                            for(int rr = 0; rr < RADIICOUNT; rr++) {
                                acc[yy * WIDTH * RADIICOUNT +  xx * RADIICOUNT + rr] = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}
