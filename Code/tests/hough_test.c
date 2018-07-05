#include "../include/hough_hls.h"

void vote(unsigned int *acc, int x, int y, unsigned int r) {
    if(x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
    	acc[y * WIDTH * RADIICOUNT + x * RADIICOUNT + r] += 1;
    }
}

void houghSoftwareResult(uint8_t input[SIZE], circle output[MAXCIRCLES], unsigned int* circleCount) {
	unsigned int acc[SIZE * RADIICOUNT];

	// Initialize accumulator
	acc_init_loop:for(int i=0; i < SIZE * RADIICOUNT; i++) {
		acc[i] = 0;
	}

	// Vote accumulator matrix
    vote_outer_loop:for(int y0 = 0; y0 < HEIGHT; y0++) {
        vote_mid_loop:for(int x0 = 0; x0 < WIDTH; x0++) {
            if(input[y0 * WIDTH + x0] != 0) {
                vote_inner_loop:for(unsigned int r = RADIUS; r <= RADIUS_UPPER_BOUNDS; r++) {
                    // Bresenham
                    int dy, dx;
                    int x = r;
                    int y = 0;
                    int error = r;
                    vote(acc, x0, y0+r, r-RADIUS);
                    vote(acc, x0, y0-r, r-RADIUS);
                    vote(acc, x0+r, y0, r-RADIUS);
                    vote(acc, x0-r, y0, r-RADIUS);

                    bresenham_while:while(y < x) {
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
            find_radii_loop: for(int r = RADIICOUNT-1; r >= 0; r--) {
                int currentRadius = r+RADIUS;
                if(acc[y * WIDTH * RADIICOUNT + x * RADIICOUNT + r] / ((4*currentRadius*SQRT2) / SQRT2FACTOR) * 100 > THRESHOLD) {
                    // Add circle
                    if(*circleCount < MAXCIRCLES) {
						circle tmp = {x, y, currentRadius};
						output[*circleCount] = tmp;
						(*circleCount)++;
                    }

                    // Clear circle from hough space
                    outer_clearing_loop:for(int yy = y; yy < y+currentRadius; yy++) {
                        mid_clearing_loop:for(int xx = x-currentRadius; xx < x+currentRadius; xx++) {
                            inner_clearing_loop:for(int rr = 0; rr < RADIICOUNT; rr++) {
                                acc[yy * WIDTH * RADIICOUNT +  xx * RADIICOUNT + rr] = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv) {
	unsigned int errCnt = 0;
	FILE* f;
	char buff[3];

	static uint8_t input[SIZE];
	unsigned int swCirclesFound = 0;
	unsigned int hwCirclesFound = 0;

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


}
