/*
** Applies Canny Edge Detector to a gaussian filtered grayscale image
*/

#include "canny.h"

/*
** Rounds an angle to -180, -90, 0, 90 or 180 degrees
*/
uint8_t roundAngle(uint8_t angle) {
	if(angle <= 22) return 0;
	else if(angle > 22 && angle <= 67) return 45;
	else if(angle > 67 && angle <= 112) return 90;
	else return 135;
}

uint8_t* canny(uint8_t* input, unsigned int width, unsigned int height, uint8_t threshold, uint8_t lowThreshold, uint8_t highThreshold) {
    uint8_t (*output)[width] = malloc(width * height * sizeof(uint8_t));
    uint8_t (*pixels)[width] = (uint8_t(*)[width]) input;
	int8_t g_x[height][width];
	int8_t g_y[height][width];
	uint8_t g[height][width];
	uint8_t delta[height][width];
	uint8_t h_filter[3] = {1, 0, -1};
	uint8_t v_filter[3] = {1, 2, 1};
	const uint8_t offset = 1;
	const uint8_t kernelSize = 3;

	/*
	** Calculate g_x
	*/

	// Apply horizontal Sobeloperator
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width-offset; x++) {
        	// Apply Sobeloperator on borders
        	if(x < offset) {
	            g[y][x] = 0;
	            g[y][width-1-x] = 0;
	            // Filter pixels on edges (offset-x) times
	            for(int i = 0; i < offset-x; i++) {
	                g[y][x] += h_filter[i] * pixels[y][0];
	                g[y][width-1-x] += h_filter[i] * pixels[y][width-1];
	            }
	            // Apply rest of the filter normally
	            for(int i = offset-x; i < kernelSize; i++) {
	                g[y][x] += h_filter[i] * pixels[y][x+i-offset];
	                g[y][width-1-x] += h_filter[i] * pixels[y][width-1-x-i+offset];
	            }
        	}
        	// Apply Sobeloperator on rest
        	else {
	            g[y][x] = 0;
	            for(int i = 0; i < kernelSize; i++) {
	                g[y][x] += h_filter[i] * pixels[y][x+i-offset];
	            }
	        }
        }
    }

	// Apply vertical Sobeloperator
    for(int x = 0; x < width; x++) {
        for(int y = 0; y < height-offset; y++) {
        	// Apply Sobelfilter on borders
        	if(y < offset) {
	            g_x[y][x] = 0;
	            g_x[height-1-y][x] = 0;
	            // Filter pixels on edges (offset-x) times
	            for(int i = 0; i < offset-y; i++) {
	                g_x[y][x] += v_filter[i] * g[0][x];
	                g_x[height-1-y][x] += v_filter[i] * g[height-1][x];
	            }
	            // Apply rest of the filter normally
	            for(int i = offset-y; i < kernelSize; i++) {
	                g_x[y][x] += v_filter[i] * g[y+i-offset][x];
	                g_x[height-1-y][x] += v_filter[i] * g[height-1-y-i+offset][x];
	            }
        	}
        	// Apply Sobelfilter on rest
        	else {
	            g_x[y][x] = 0;
	            for(int i = 0; i < kernelSize; i++) {
	                g_x[y][x] += v_filter[i] * g[y+i-offset][x];
	            }
        	}
        }
    }

	/*
	** Calculate g_y
	*/

	// Apply horizontal Sobeloperator
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width-offset; x++) {
        	// Apply Sobeloperator on borders
        	if(x < offset) {
	            g[y][x] = 0;
            	g[y][width-1-x] = 0;
	            // Filter pixels on edges (offset-x) times
	            for(int i = 0; i < offset-x; i++) {
	                g[y][x] += v_filter[i] * pixels[y][0];
	                g[y][width-1-x] += v_filter[i] * pixels[y][width-1];
	            }
	            // Apply rest of the filter normally
	            for(int i = offset-x; i < kernelSize; i++) {
	                g[y][x] += v_filter[i] * pixels[y][x+i-offset];
	                g[y][width-1-x] += v_filter[i] * pixels[y][width-1-x-i+offset];
	            }
        	}
        	// Apply Sobeloperator on rest
        	else {
	            g[y][x] = 0;
	            for(int i = 0; i < kernelSize; i++) {
	                g[y][x] += v_filter[i] * pixels[y][x+i-offset];
	            }
	        }
        }
    }

	// Apply vertical Sobeloperator
    for(int x = 0; x < width; x++) {
        for(int y = 0; y < height-offset; y++) {
        	// Apply Sobeloperator on borders
			if(y < offset) {
	            g_y[y][x] = 0;
	            g_y[height-1-y][x] = 0;
	            // Filter pixels on edges (offset-x) times
	            for(int i = 0; i < offset-y; i++) {
	                g_y[y][x] += h_filter[i] * g[0][x];
	                g_y[height-1-y][x] += h_filter[i] * g[height-1][x];
	            }
	            // Apply rest of the filter normally
	            for(int i = offset-y; i < kernelSize; i++) {
	                g_y[y][x] += h_filter[i] * g[y+i-offset][x];
	                g_y[height-1-y][x] += h_filter[i] * g[height-1-y-i+offset][x];
	            }
			}
			// Apply Sobeloperator on rest
			else {
	            g_y[y][x] = 0;
	            for(int i = 0; i < kernelSize; i++) {
	                g_y[y][x] += h_filter[i] * g[y+i-offset][x];
	            }
        	}
        }
    }

    // Calculate gradient
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			// Calculate gradients direction
			delta[y][x] = abs(atan2(g_y[y][x], g_x[y][x]) * 180 / PI);
			delta[y][x] = roundAngle(delta[y][x]);
			// Calculate gradients intensity - approximation
			g[y][x] = abs(g_x[y][x]) + abs(g_y[y][x]);
            // Exact formula
			//g[y][x] = sqrt(g_x[y][x]*g_x[y][x] + g_y[y][x]*g_y[y][x]);
			g[y][x] = fmax(g[y][x], threshold);
			if(g[y][x] == threshold) g[y][x] = 0;
			output[y][x] = 0;
    	}
    }

    // Non-maximum suppression
    //for(int y = 1; y < height-1; y++){
    for(int y = 0; y < height-1; y++){
	    //for(int x = 1; x < width-1; x++) {
    	for(int x = 0; x < width-1; x++) {
	    	// Compare neighbour pixel in gradient direction and delete all but the maximum
	    	switch(delta[y][x]) {
                // Edge is in east-west direction
	    		case 0:
                    if(g[y][x] < g[y][x-1] || g[y][x] < g[y][x+1]) g[y][x] = 0;
	    			break;
                // Edge is in northeast-southwest direction
	    		case 45:
	    			if(g[y][x] < g[y-1][x+1] || g[y][x] < g[y+1][x-1]) g[y][x] = 0;
	    			break;
                // Edge is in north-south direction
	    		case 90:
                    if(g[y][x] < g[y-1][x] || g[y][x] < g[y+1][x]) g[y][x] = 0;
	    			break;
                // Edge is in northwest-southeast direction
	    		case 135:
	    			if(g[y][x] < g[y-1][x-1] || g[y][x] < g[y+1][x+1]) g[y][x] = 0;
	    			break;
	    	}
	    }
	}

	// Hysterese
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			// Follow strong edges
			if(g[y][x] > highThreshold) {
				int i = y;
				int j = x;
				// Follow edge in positive direction
				while(i > 0 && i < height-1 && j > 0 && j < width-1 && g[i][j] > lowThreshold) {
                    output[i][j] = 255;
					switch(delta[i][j]) {
						case 0:
							i -= 1;
							break;
						case 45:
							i -= 1;
							j -= 1;
							break;
						case 90:
							j -= 1;
							break;
						case 135:
							i += 1;
							j -= 1;
							break;
					}
				}
				i = y;
				j = x;
				// Follow edge in negative direction
				while(i > 0 && i < height-1 && j > 0 && j < width-1 && g[i][j] > lowThreshold) {
					output[i][j] = 255;
					switch(delta[y][x]) {
						case 0:
							i += 1;
							break;
						case 45:
							i += 1;
							j += 1;
							break;
						case 90:
							j += 1;
							break;
						case 135:
							i -= 1;
							j += 1;
							break;
					}
				}
			}
		}
	}

	return *output;
}
