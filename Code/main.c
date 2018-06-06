/*
** Finds circles in a 32 bit color image
*/

#ifndef STDIO_H_INCLUDED
#define STIDO_H_INCLUDED
#include <stdio.h>
#endif /*STDIO_H_INCLUDED*/
#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif /*STDLIB_H_INCLUDED*/
#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif /*STRING_H_INCLUDED*/
#ifndef SYSTIME_H_INCLUDED
#define SYSTIME_H_INCLUDED
#include <sys/time.h>
#endif /*SYSTIME_H_INCLUDED*/
#ifndef UNISTD_H_INCLUDED
#define UNISTD_H_INCLUDED
#include <unistd.h>
#endif /*UNISTD_H_INCLUDED*/
#include <getopt.h>

#ifndef SDL2_H_INCLUDED
#define SDL2_H_INCLUDED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif /*SDL2_H_INCLUDED*/

#ifdef _OPENMP
#ifndef OMP_H_INCLUDED
#define OMP_H_INCLUDED
#include <omp.h>
#endif /*OMP_H_INCLUDED*/
#endif /*_OPENMP*/

#include "grayscaler.h"
#include "gauss.h"
#include "canny.h"
#include "hough.h"

/*
** Returns time in microseconds
*/
long getTime() {
    struct timeval timecheck;
    gettimeofday(&timecheck, NULL);
    return (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
}

/*
** Set a pixel to the given intensity
*/
void setPixel(uint8_t* input, unsigned int w, unsigned int h, int x, int y, uint8_t intensity) {
    if(x >= 0 && x < w && y >= 0 && y < h) {
        input[y * w + x] = intensity;
    }
}

/*
** Bresenham circle algorithm from Wikipedia
** Draws circles with a radius of r at position (x0, y0)
** Takes the pixel data of the image, its width and height as additional input
** Returns the updated pixel data
*/
uint8_t* drawCircle(uint8_t* input, unsigned int width, unsigned int height, unsigned int x0, unsigned int y0, uint8_t r) {
    int dy, dx;
    int x = r;
    int y = 0;
    int error = r;

    setPixel(input, width, height, x0, y0+r, 127);
    setPixel(input, width, height, x0, y0-r, 127);
    setPixel(input, width, height, x0+r, y0, 127);
    setPixel(input, width, height, x0-r, y0, 127);

    while(y < x) {
        dy = y*2 + 1;
        y += 1;
        error -= dy;
        if(error < 0) {
            dx = 1 - x*2;
            x -= 1;
            error -= dx;
        }
        setPixel(input, width, height, x0+x, y0+y, 127);
        setPixel(input, width, height, x0-x, y0+y, 127);
        setPixel(input, width, height, x0-x, y0-y, 127);
        setPixel(input, width, height, x0+x, y0-y, 127);
        setPixel(input, width, height, x0+y, y0+x, 127);
        setPixel(input, width, height, x0-y, y0+x, 127);
        setPixel(input, width, height, x0-y, y0-x, 127);
        setPixel(input, width, height, x0+y, y0-x, 127);
    }

    return input;
}

/*
** Creates a (grayscale) palette with 256 colors
** When mode = 1 red is added to the palette
*/
SDL_Palette* createPalette(uint8_t mode) {
    SDL_Palette* p = SDL_AllocPalette(256);
    SDL_Color* tmp;

    // Create grayscale-colors
    for(int i = 0; i < 256; i++) {
        tmp = &p->colors[i];
        tmp->r = i;
        tmp->g = i;
        tmp->b = i;
    }

    // Add red to the palette on position 127
    if(mode == 1) {
        tmp = &p->colors[127];
        tmp->r = 255;
        tmp->g = 0;
        tmp->b = 0;
    }

    return p;
}

/*
** Prints usage information to stderr
*/
void printUsage() {
    fprintf(stderr, "usage: hough [-i image_name] [-k kernel_size] [-t threshold] [-l low_threshold] [-h high_threshold] [-r radius] [-R radius_upper_bounds] [-H threshold]"
                #ifdef _OPENMP
                " [-o number_of_threads]"
                #endif
                "\n"
                "\t-i image_name: path to the image to process\n"
                "\t\t(default is test.png)\n"
                "\t-k kernel_size: filtersize for the gaussian kernel\n"
                "\t\t(default is 5)\n"
                "\t-t threshold: threshold for the canny edge detector when calculating the gradient\n"
                "\t\t(default is 0)\n"
                "\t-l low_threshold: lower threshold for the canny edge detectors hysteresis\n"
                "\t\t(default is 100)\n"
                "\t-h high_threshold: higher threshold for the canny edge detectors hysteresis\n"
                "\t\t(default is 200)\n"
                "\t-r radius: radius for the size of the circles searched in the hough transform\n"
                "\t\t(default is 15)\n"
                "\t-R radius_upper_bounds: upper bounds radius for the sizes of the circles searched in the hough transform\n"
                "\t\tIf this argument is given to the program, a range of radii from radius to radius_upper_bounds is searched in the hough transform\n"
                "\t\t(default is 25)\n"
                "\t-H threshold: threshold for the hough transform in percentage\n"
                "\t\t(default is 70)\n"
                #ifdef _OPENMP
                "\t-o number_of_threads: use OpenMP with a certain number of threads\n"
                #endif
                );
}

/*
** Prints an error message, shows usage and exits
*/
void integerArgumentError(char argument, char* end) {
    fprintf(stderr, "ERROR: Wrong argument for -%c\nInteger expected but was: %s\n", argument, end);
    printUsage();
    exit(EXIT_FAILURE);
}

/*
** Evaluates arguments and checks if they are correct
*/
void evaluateArguments(int argc, char** argv, char* path, const char* dir, uint8_t* kernelSize, uint8_t* threshold, uint8_t* lowThreshold, uint8_t* highThreshold, unsigned int* radius, unsigned int* radiusUpperBounds, unsigned int* houghThreshold) {
    int opt;
    char* end;
    #ifdef _OPENMP
    int threads;
    #endif

    // Loop through arguments
    while((opt = getopt(argc, argv, "i:k:t:l:h:r:R:H:"
        #ifdef _OPENMP
        "o:"
        #endif
        )) != - 1) {
        switch(opt) {
            case 'i':
                strcpy(path, dir);
                strcat(path, optarg);
                break;
            case 'k':
                *kernelSize = strtol(optarg, &end, 0);
                if(*end)
                    integerArgumentError('k', end);
                break;
            case 't':
                *threshold = strtol(optarg, &end, 0);
                if(*end)
                    integerArgumentError('t', end);
                break;
            case 'l':
                *lowThreshold = strtol(optarg, &end, 0);
                if(*end)
                    integerArgumentError('l', end);
                break;
            case 'h':
                *highThreshold = strtol(optarg, &end, 0);
                if(*end)
                    integerArgumentError('h', end);
                break;
            case 'r':
                *radius = strtol(optarg, &end, 0);
                if(*end)
                    integerArgumentError('r', end);
                break;
            case 'R':
                *radiusUpperBounds = strtol(optarg, &end, 0);
                if(*end)
                    integerArgumentError('R', end);
                break;
            case 'H':
                *houghThreshold = strtol(optarg, &end, 0);
                if(*end)
                    integerArgumentError('H', end);
                break;
            #ifdef _OPENMP
            case 'o':
                threads = strtol(optarg, &end, 0);
                if(*end)
                    integerArgumentError('o', end);
                omp_set_num_threads(threads);
                break;
            #endif
            default:
                printUsage();
                exit(EXIT_FAILURE);
        }
    }
    if(*highThreshold < *lowThreshold) {
        fprintf(stderr, "ERROR: high_threshold (%i) must not be smaller than low_threshold (%i)\n", *highThreshold, *lowThreshold);
        exit(EXIT_FAILURE);
    }
    if(*radiusUpperBounds < *radius) {
        fprintf(stderr, "ERROR: radius_upper_bounds (%i) must not be smaller than radius (%i)\n", *radiusUpperBounds, *radius);
        exit(EXIT_FAILURE);
    }
    if(*kernelSize > 29) {
        fprintf(stderr, "ERROR: kernel_size (%i) is too big. Only kernel_size's up to 29 are supported\n", *kernelSize);
        exit(EXIT_FAILURE);
    }
}

/*
** Finding circles in a 32 bit color image by
** 1) Transforming the image to a grayscale image
** 2) Applying a Gaussian filter
** 3) Converting the image to a binary image
** 4) Applying canny edge detector
** 5) Performing hough transform
** 6) Drawing found circles
*/
int main(int argc, char **argv) {
    // Default parameters
    uint8_t kernelSize = 5;
	uint8_t threshold = 0;
    uint8_t lowThreshold = 100;
    uint8_t highThreshold = 200;
    unsigned int radius = 15;
    unsigned int radiusUpperBounds = 25;
    unsigned int houghThreshold = 70;

    long startTime, endTime;
    long grayscalerTime;
    long gaussTime;
    long cannyTime;
    long houghTime;
    long totalTime = 0;
    unsigned int circleCount = 0;
    circle* circles = NULL;

    // Set dir and default path for images
    const char* dir = "../img/src/";
    char* path = malloc(255);
    strcpy(path, dir);
    strcat(path, "test.png");

    SDL_Surface* img;
    SDL_Surface* grayImg;

    // Initialize SDL and SDL Image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    // Evaluate arguments
    evaluateArguments(argc, argv, path, dir, &kernelSize, &threshold, &lowThreshold, &highThreshold, &radius, &radiusUpperBounds, &houghThreshold);

    // Load test image
    img = IMG_Load(path);
    free(path);
    if(!img) {
        fprintf(stderr, "ERROR: Could not load image: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }

    printf("Dimensions: (%ix%i)\n", img->w, img->h);

    // Prepare image and create a big Surface with 8 Bit depth
    img = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_ARGB8888, 0);
    grayImg = SDL_CreateRGBSurface(0, img->w, img->h, 8, 0, 0, 0, 0);
    if(grayImg == NULL) {
        fprintf(stderr, "ERROR: SDL_CreateRGBSurface() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_SetSurfacePalette(grayImg, createPalette(0));

    // Convert pixels to grayscale
    startTime = getTime();
    grayImg->pixels = grayscaler(img->pixels, img->w, img->h);
    endTime = getTime();
    grayscalerTime = endTime-startTime;
    totalTime += grayscalerTime;
    printf("%li ms needed for grayscaler.\n", grayscalerTime);
    IMG_SavePNG(grayImg, "../img/gen/grayscale.png");
    SDL_FreeSurface(img);

    // Apply gauss filter
    if(kernelSize != 0) {
        startTime = getTime();
        grayImg->pixels = gauss(grayImg->pixels, grayImg->w, grayImg->h, kernelSize);
        endTime = getTime();
        gaussTime = endTime-startTime;
        totalTime += gaussTime;
        printf("%li ms needed for gauss filter.\n", gaussTime);
        IMG_SavePNG(grayImg, "../img/gen/gauss.png");
    }

    // Apply canny edge detector
    startTime = getTime();
    grayImg->pixels = canny(grayImg->pixels, grayImg->w, grayImg->h, threshold, lowThreshold, highThreshold);
    endTime = getTime();
    cannyTime = endTime-startTime;
    totalTime += cannyTime;
    printf("%li ms needed for canny edge detector.\n", cannyTime);
    IMG_SavePNG(grayImg, "../img/gen/sobel.png");

    // Perform hough transform
    startTime = getTime();
    circles = hough(grayImg->pixels, grayImg->w, grayImg->h, radius, radiusUpperBounds, houghThreshold, &circleCount);
    endTime = getTime();
    houghTime = endTime-startTime;
    totalTime += houghTime;
    printf("%li ms needed for hough transform.\n", houghTime);
    printf("------------------------------------------\n");
    printf("%li ms needed total.\n", totalTime);
    printf("%i circles found.\n", circleCount);

    // Draw found circles in red
    SDL_SetSurfacePalette(grayImg, createPalette(1));
    for(int i = 0; i < circleCount; i++) {
        grayImg->pixels = drawCircle(grayImg->pixels, grayImg->w, grayImg->h, circles[i].x, circles[i].y, circles[i].r);
    }
    IMG_SavePNG(grayImg, "../img/gen/hough.png");
    
    // Free allocated memory and exit
    free(circles);
    SDL_FreePalette(grayImg->format->palette);
    SDL_FreeSurface(grayImg);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
