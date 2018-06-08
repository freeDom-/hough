/*
** Applies a Gaussian filter to a 8-Bit greyscale image
** The function needs and returns only pixel data without any kind of
** compression.
*/

#include "gauss.h"

// KERNELS - hardcoded Kernels for speed improvements?
// 1/16 = 2^4
int filter3[3] = {4, 8, 4};
int sum3 = 16;
// 1/256 = 2^8
int filter5[5] = {16, 64, 96, 64, 16};
int sum5 = 256;
// 1/4096 = 2^12
int filter7[7] = {64, 384, 960, 1280, 960, 384, 64};
int sum7 = 4096;

/*
** Calculates (n+1)th row of pascals triangle
*/
unsigned long* pascal(unsigned long n) {
    unsigned long* line = malloc((n+1) * sizeof(unsigned long));
    line[0] = 1;

    for(int i = 0; i < n; i++) {
        line[i+1] = line[i] * (n-i) / (i+1);
    }
    return line;
}

/*
** Creates a gaussian kernel
*/
unsigned long* kernelGenerator(unsigned long kernelSize) {
    unsigned long* kernel = calloc(kernelSize, sizeof(unsigned long)); // reserve memory and initialize with 0
    unsigned long* pascalRow = pascal(kernelSize-1);

    for(int i = 0; i < kernelSize; i++) {
        for(int j = 0; j < kernelSize; j++) {
            kernel[i] += pascalRow[i] * pascalRow[j];
        }
    }
    free(pascalRow);
    return kernel;
}

uint8_t* gauss(uint8_t* pixelData, unsigned int width, unsigned int height, uint8_t kernelSize) {
    uint8_t offset = kernelSize >> 1;
    uint8_t *temp = malloc(width * height * sizeof(uint8_t));
    unsigned long *filter;
    unsigned long temp1, temp2;
    uint8_t shift = (kernelSize-1) << 1; // division by the sum of all kernel elements equals rightshifting by (kernelsize-1)*2

    filter = kernelGenerator(kernelSize);

    #ifdef _OPENMP
    #pragma omp parallel for private(temp1, temp2)
    #endif
    // Apply horizontal filter on pixels and save in temp
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width-offset; x++) {
            // Apply filter on borders
            if(x < offset) {
                temp1 = 0;
                temp2 = 0;
                // Filter pixels on edges (offset-x) times
                for(int i = 0; i < offset-x; i++) {
                    temp1 += filter[i] * pixelData[y * width];
                    temp2 += filter[i] * pixelData[y * width + (width-1)];
                }
                // Apply rest of the filter normally
                for(int i = offset-x; i < kernelSize; i++) {
                    temp1 += filter[i] * pixelData[y * width + (x+i-offset)];
                    temp2 += filter[i] * pixelData[y * width + (width-1-x-i+offset)];
                }
                temp[y * width + x] = temp1 >> shift;
                temp[y * width + (width-1-x)] = temp2 >> shift;
            }
            // Apply filter on rest
            else {
                temp1 = 0;
                for(int i = 0; i < kernelSize; i++) {
                    temp1 += filter[i] * pixelData[y * width + (x+i-offset)];
                }
                temp[y * width + x] = temp1 >> shift;
            }
        }
    }

    #ifdef _OPENMP
    #pragma omp parallel for private(temp1, temp2)
    #endif
    // Apply vertical filter on temp and save in pixelData
    for(int x = 0; x < width; x++) {
        for(int y = 0; y < height-offset; y++) {
            // Apply filter on borders
            if(y < offset) {
                temp1 = 0;
                temp2 = 0;
                // Filter pixels on edges (offset-x) times
                for(int i = 0; i < offset-y; i++) {
                    temp1 += filter[i] * temp[x];
                    temp2 += filter[i] * temp[(height-1) * width + x];
                }
                // Apply rest of the filter normally
                for(int i = offset-y; i < kernelSize; i++) {
                    temp1 += filter[i] * temp[(y+i-offset) * width + x];
                    temp2 += filter[i] * temp[(height-1-y-i+offset) * width + x];
                }
                pixelData[y * width + x] = temp1 >> shift;
                pixelData[(height-1-y) * width + x] = temp2 >> shift;
            }
            // Apply filter on rest
            else {
                temp1 = 0;
                for(int i = 0; i < kernelSize; i++) {
                    temp1 += filter[i] * temp[(y+i-offset) * width + x];
                }
                pixelData[y * width + x] = temp1 >> shift;
            }
        }
    }

    free(temp);
    free(filter);
    return pixelData;
}
