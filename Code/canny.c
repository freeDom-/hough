/*
** Applies Canny Edge Detector to a gaussian filtered grayscale image
*/

#include "canny.h"

/*
** Rounds an angle to 0, 45, 90 or 135 degrees
*/
uint8_t roundAngle(uint8_t angle) {
	if(angle <= 22) return 0;
	else if(angle > 22 && angle <= 67) return 45;
	else if(angle > 67 && angle <= 112) return 90;
	else return 135;
}

uint8_t* canny(uint8_t* input, unsigned int width, unsigned int height, uint8_t threshold, uint8_t lowThreshold, uint8_t highThreshold) {
    const uint8_t offset = 1;
	const uint8_t kernelSize = 3;
    uint8_t *output = calloc(width * height, sizeof(uint8_t));	// allocate memory and initialize to 0
	uint8_t *delta = malloc(width * height * sizeof(uint8_t)); // allocate memory on heap instead of using the stack
	int8_t h_filter[3] = {1, 0, -1};
	int8_t v_filter[3] = {1, 2, 1};
	uint16_t *g = malloc(width * height * sizeof(uint16_t));   // allocate memory on heap instead of using the stack
	int16_t *g_x = malloc(width * height * sizeof(int16_t));   // allocate memory on heap instead of using the stack
	int16_t *g_y = malloc(width * height * sizeof(int16_t));   // allocate memory on heap instead of using the stack
	int16_t *tmp = malloc(width * height * sizeof(int16_t));   // allocate memory on heap instead of using the stack

	/*
	** Calculate g_x
	*/
	#ifdef _OPENMP
	#pragma omp parallel for
	#endif
	// Apply horizontal Sobeloperator and store in tmp
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width-offset; x++) {
        	// Apply Sobeloperator on borders
        	if(x < offset) {
                tmp[y * width + x] = 0;
	            tmp[y * width + (width-1-x)] = 0;
	            // Filter pixels on edges (offset-x) times
	            for(int i = 0; i < offset-x; i++) {
	                tmp[y * width + x] += h_filter[i] * input[y * width];
	                tmp[y * width + (width-1-x)] += h_filter[i] * input[y * width + (width-1)];
	            }
	            // Apply rest of the filter normally
	            for(int i = offset-x; i < kernelSize; i++) {
	                tmp[y * width + x] += h_filter[i] * input[y * width + (x+i-offset)];
	                tmp[y * width + (width-1-x)] += h_filter[i] * input[y * width + (width-1-x-i+offset)];
	            }
        	}
        	// Apply Sobeloperator on rest
        	else {
	            tmp[y * width + x] = 0;
	            for(int i = 0; i < kernelSize; i++) {
	                tmp[y * width + x] += h_filter[i] * input[y * width + (x+i-offset)];
	            }
	        }
        }
    }
	#ifdef _OPENMP
	#pragma omp parallel for
	#endif
	// Apply vertical Sobeloperator and store in g_x
    for(int x = 0; x < width; x++) {
        for(int y = 0; y < height-offset; y++) {
        	// Apply Sobelfilter on borders
        	if(y < offset) {
	            g_x[y * width + x] = 0;
	            g_x[(height-1-y) * width + x] = 0;
                // Filter pixels on edges (offset-x) times
	            for(int i = 0; i < offset-y; i++) {
	                g_x[y * width + x] += v_filter[i] * tmp[x];
	                g_x[(height-1-y) * width + x] += v_filter[i] * tmp[(height-1) * width + x];
	            }
	            // Apply rest of the filter normally
	            for(int i = offset-y; i < kernelSize; i++) {
	                g_x[y * width + x] += v_filter[i] * tmp[(y+i-offset) * width + x];
	                g_x[(height-1-y) * width + x] += v_filter[i] * tmp[(height-1-y-i+offset) * width + x];
	            }
        	}
        	// Apply Sobelfilter on rest
        	else {
	            g_x[y * width + x] = 0;
	            for(int i = 0; i < kernelSize; i++) {
	                g_x[y * width + x] += v_filter[i] * tmp[(y+i-offset) * width + x];
	            }
        	}
        }
    }

	/*
	** Calculate g_y
	*/
	#ifdef _OPENMP
	#pragma omp parallel for
	#endif
	// Apply horizontal Sobeloperator and store in tmp
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width-offset; x++) {
        	// Apply Sobeloperator on borders
        	if(x < offset) {
	            tmp[y * width + x] = 0;
            	tmp[y * width + (width-1-x)] = 0;
	            // Filter pixels on edges (offset-x) times
	            for(int i = 0; i < offset-x; i++) {
	                tmp[y * width + x] += v_filter[i] * input[y * width];
	                tmp[y * width + (width-1-x)] += v_filter[i] * input[y * width + (width-1)];
	            }
	            // Apply rest of the filter normally
	            for(int i = offset-x; i < kernelSize; i++) {
	                tmp[y * width + x] += v_filter[i] * input[y * width + (x+i-offset)];
	                tmp[y * width + (width-1-x)] += v_filter[i] * input[y * width + (width-1-x-i+offset)];
	            }
        	}
        	// Apply Sobeloperator on rest
        	else {
	            tmp[y * width + x] = 0;
	            for(int i = 0; i < kernelSize; i++) {
	                tmp[y * width + x] += v_filter[i] * input[y * width + (x+i-offset)];
	            }
	        }
        }
    };
	#ifdef _OPENMP
	#pragma omp parallel for
	#endif
	// Apply vertical Sobeloperator and store in g_y
    for(int x = 0; x < width; x++) {
        for(int y = 0; y < height-offset; y++) {
        	// Apply Sobeloperator on borders
			if(y < offset) {
	            g_y[y * width + x] = 0;
	            g_y[(height-1-y) * width + x] = 0;
	            // Filter pixels on edges (offset-x) times
	            for(int i = 0; i < offset-y; i++) {
	                g_y[y * width + x] += h_filter[i] * tmp[x];
	                g_y[(height-1-y) * width + x] += h_filter[i] * tmp[(height-1) * width + x];
	            }
	            // Apply rest of the filter normally
	            for(int i = offset-y; i < kernelSize; i++) {
	                g_y[y * width + x] += h_filter[i] * tmp[(y+i-offset) * width + x];
	                g_y[(height-1-y) * width + x] += h_filter[i] * tmp[(height-1-y-i+offset) * width + x];
	            }
			}
			// Apply Sobeloperator on rest
			else {
	            g_y[y * width + x] = 0;
	            for(int i = 0; i < kernelSize; i++) {
	                g_y[y * width + x] += h_filter[i] * tmp[(y+i-offset) * width + x];
	            }
        	}
        }
    }

	#ifdef _OPENMP
	#pragma omp parallel for
	#endif
    // Calculate gradient
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
            int index = y * width + x;
			// Calculate gradients direction
			delta[index] = abs(atan2(g_y[index], g_x[index]) * 180 / PI);
			delta[index] = roundAngle(delta[index]);
			// Calculate gradients intensity - approximation
			//g[y][x] = abs(g_x[y][x]) + abs(g_y[y][x]);
            // Exact formula
			g[index] = sqrt(g_x[index] * g_x[index] + g_y[index] * g_y[index]);
			g[index] = fmax(g[index], threshold);
			if(g[index] == threshold) g[index] = 0;
    	}
    }

	#ifdef _OPENMP
	#pragma omp parallel for
	#endif
    // Non-maximum suppression
    for(int y = 1; y < height-1; y++){
    //for(int y = 0; y < height-1; y++){
	    for(int x = 1; x < width-1; x++) {
    	//for(int x = 0; x < width-1; x++) {
            int index = y * width + x;
	    	// Compare neighbour pixel in gradient direction and delete all but the maximum
	    	switch(delta[index]) {
                // Edge is in east-west direction
	    		case 0:
                    if(g[index] < g[index - 1] || g[index] < g[index + 1]) g[index] = 0;
	    			break;
                // Edge is in northeast-southwest direction
	    		case 45:
	    			if(g[index] < g[index - width + 1] || g[index] < g[index + width - 1]) g[index] = 0;
	    			break;
                // Edge is in north-south direction
	    		case 90:
                    if(g[index] < g[index - width] || g[index] < g[index + width]) g[index] = 0;
	    			break;
                // Edge is in northwest-southeast direction
	    		case 135:
	    			if(g[index] < g[index - width - 1] || g[index] < g[index + width + 1]) g[index] = 0;
	    			break;
	    	}
	    }
	}

	#ifdef _OPENMP
	#pragma omp parallel for
	#endif
	// Hysteresis
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			// Follow strong edges if not already performed
			if(g[y * width + x] > highThreshold && output[y * width + x] != 255) {
				int i = y;
				int j = x;
				// Follow edge in positive direction
				while(i > 0 && i < height-1 && j > 0 && j < width-1 && g[i * width + j] > lowThreshold) {
                    output[i * width + j] = 255;
					switch(delta[i * width + j]) {
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
				while(i > 0 && i < height-1 && j > 0 && j < width-1 && g[i * width + j] > lowThreshold) {
					output[i * width + j] = 255;
					switch(delta[i * width + j]) {
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

	free(input);
	free(delta);
    free(g);
    free(g_x);
    free(g_y);
    free(tmp);
    return output;
}
