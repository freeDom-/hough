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
#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED
#include <time.h>
#endif /*TIME_H_INCLUDED*/

#ifndef SDL2_H_INCLUDED
#define SDL2_H_INCLUDED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif /*SDL2_H_INCLUDED*/

#include "grayscaler.h"
#include "gauss.h"
#include "canny.h"
#include "hough.h"

/*
** Midpoint circle algorithm from Wikipedia
** Draws circles with a radius of r at position (x0, y0)
** Takes the pixel data of the image, its width and height as additional input
** Returns the updated pixel data
*/
uint8_t* drawCircle(uint8_t* input, unsigned int width, unsigned int height, unsigned int x0, unsigned int y0, uint8_t r) {
    uint8_t (*pixels)[width] = (uint8_t(*)[width]) input;
    unsigned int x = r-1;
    unsigned int y = 0;
    unsigned int dx = 1;
    unsigned int dy = 1;
    int err = dx - (r << 1);

    while(x >= y) {
        if((int)(x0 + x) < width && (int)(y0 + y) < height) {
            pixels[y0 + y][x0 + x] = 127;
        }
        if((int)(x0 + y) < width && (int)(y0 + x) < height) {
            pixels[y0 + x][x0 + y] = 127;
        }
        if((int)(x0 - y) >= 0 && (int)(y0 + x) < height) {
            pixels[y0 + x][x0 - y] = 127;
        }
        if((int)(x0 - x) >= 0 && (int)(y0 + y) < height) {
            pixels[y0 + y][x0 - x] = 127;
        }
        if((int)(x0 - x) >= 0 && (int)(y0 - y) >= 0) {
            pixels[y0 - y][x0 - x] = 127;
        }
        if((int)(x0 - y) >= 0 && (int)(y0 - x) >= 0) {
            pixels[y0 - x][x0 - y] = 127;
        }
        if((int)(x0 + y) < width && (int)(y0 - x) >= 0) {
            pixels[y0 - x][x0 + y] = 127;
        }
        if((int)(x0 + x) < width && (int)(y0 - y) >= 0) {
            pixels[y0 - y][x0 + x] = 127;
        }

        if(err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }

        if(err > 0) {
            x--;
            dx += 2;
            err += dx - (r << 1);
        }
    }
    pixels[y0][x0] = 127;

    return *pixels;
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
** Loads an image and checks if it was successfull
** Returns a SDL_Surface or NULL
*/
SDL_Surface* loadImage(char* path) {
    SDL_Surface* img = IMG_Load(path);

    if(!img) {
        printf("Could not load image: %s\n", IMG_GetError());
        return NULL;
    }
    return img;
}

/*
** Creates a surface and checks if it was successfull
** Returns a SDL_Surface or NULL
*/
SDL_Surface* createSurface(int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
    SDL_Surface* img = SDL_CreateRGBSurface(0, width, height, depth, Rmask, Gmask, Bmask, Amask);
    
    if(img == NULL) {
        printf("SDL_CreateRGBSurface() failed: %s\n", SDL_GetError());
        return NULL;
    }
    return img;
}

/*
** Prints usage information to stderr
*/
void printUsage() {
    fprintf(stderr, "usage: hough [-i image_name] [-k kernel_size] [-ct threshold] [-lt low_threshold] [-ht high_threshold] [-r radius] [-rub radius_upper_bounds] [-ht threshold]\n"
                "\t-i image_name: path to the image to process\n"
                "\t\t(default is test.png)\n"
                "\t-k kernel_size: filtersize for the gaussian kernel\n"
                "\t\t(default is 5)\n"
                "\t-ct threshold: threshold for the canny edge detector when calculating the gradient\n"
                "\t\t(default is 0)\n"
                "\t-lt low_threshold: lower threshold for the canny edge detectors hysteresis\n"
                "\t\t(default is 100)\n"
                "\t-ht high_threshold: higher threshold for the canny edge detectors hysteresis\n"
                "\t\t(default is 200)\n"
                "\t-r radius: radius for the size of the circles searched in the hough transform\n"
                "\t\t(default is 15)\n"
                "\t-rub radius_upper_bounds: upper bounds radius for the sizes of the circles searched in the hough transform\n"
                "\t\tIf this argument is given to the program, a range of radii from radius to radius_upper_bounds is searched in the hough transform\n"
                "\t\t(default is 25)\n"
                "\t-hot threshold: threshold for the hough transform when calculating local maxima\n"
                "\t\t(default is 100)\n");
}

/*
** Evaluates arguments and checks if they are correct
*/
void evaluateArguments(int argc, char** argv, char* path, char* dir, uint8_t* kernelSize, uint8_t* threshold, uint8_t* lowThreshold, uint8_t* highThreshold, uint8_t* radius, uint8_t* radiusUpperBounds, unsigned int* houghThreshold) {
    if(argc % 2 == 0) { // as long as each optional argument takes exactly one additional argument this is ok
        fprintf(stderr, "Wrong number of arguments.\n");
        printUsage();
        exit(EXIT_FAILURE);
    }

    // Loop through arguments
    char* end;
    for(int i = 3; i <= argc; i += 2) { // as long as each optional argument takes exactly one additional argument this is ok
        if(strcmp(argv[i-2],"-i") == 0) {
            free(path);
            path = malloc(sizeof(dir) + sizeof(argv[i-1]) + 1);
            strcpy(path, dir);
            strcat(path, argv[i-1]);
        }
        else if(strcmp(argv[i-2],"-k") == 0) {
            *kernelSize = strtol(argv[i-1], &end, 0);
            if(*end) {
                fprintf(stderr, "ERROR: Wrong argument for -k\nInteger expected but was: %s\n", end);
                printUsage();
                exit(EXIT_FAILURE);
            }
            if(*kernelSize != 0 && *kernelSize % 2 == 0) {
                fprintf(stderr, "ERROR: Wrong argument for -k\nkernel_size must be uneven.\n");
                printUsage();
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(argv[i-2],"-ct") == 0) {
            *threshold = strtol(argv[i-1], &end, 0);
            if(*end) {
                fprintf(stderr, "ERROR: Wrong argument for -ct\nInteger expected but was: %s\n", end);
                printUsage();
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(argv[i-2],"-lt") == 0) {
            *lowThreshold = strtol(argv[i-1], &end, 0);
            if(*end) {
                fprintf(stderr, "ERROR: Wrong argument for -lt\nInteger expected but was: %s\n", end);
                printUsage();
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(argv[i-2],"-ht") == 0) {
            *highThreshold = strtol(argv[i-1], &end, 0);
            if(*end) {
                fprintf(stderr, "ERROR: Wrong argument for -ht\nInteger expected but was: %s\n", end);
                printUsage();
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(argv[i-2], "-r") == 0) {
            *radius = strtol(argv[i-1], &end, 0);
            if(*end) {
                fprintf(stderr, "ERROR: Wrong argument for -r\nInteger expected but was: %s\n", end);
                printUsage();
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(argv[i-2], "-rub") == 0) {
            *radiusUpperBounds = strtol(argv[i-1], &end, 0);
            if(*end) {
                fprintf(stderr, "ERROR: Wrong argument for -rub\nInteger expected but was: %s\n", end);
                printUsage();
                exit(EXIT_FAILURE);
            }
        }
        else if(strcmp(argv[i-2], "-hot") == 0) {
            *houghThreshold = strtol(argv[i-1], &end, 0);
            if(*end) {
                fprintf(stderr, "ERROR: Wrong argument for -hot\nInteger expected but was: %s\n", end);
                printUsage();
                exit(EXIT_FAILURE);
            }
        }
        else {
            fprintf(stderr, "ERROR: Wrong argument %s\n", argv[i-2]);
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
    uint8_t radius = 15;
    uint8_t radiusUpperBounds = 25;
    unsigned int houghThreshold = 100;

    clock_t startTime, endTime;
    circle* circles = NULL;

    // Set dir and default path for images
    char* dir = "../img/";
    char* path = malloc(sizeof(dir) + sizeof("test.png") + 1);
    strcpy(path, dir);
    strcat(path, "../img/test.png");

    SDL_Surface* img;
    SDL_Surface* grayImg;

    // Initialize SDL and SDL Image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    // Evaluate arguments
    evaluateArguments(argc, argv, path, dir, &kernelSize, &threshold, &lowThreshold, &highThreshold, &radius, &radiusUpperBounds, &houghThreshold);

    // Load test image
    img = loadImage(path);
    if(img == NULL)
        return 1;

    // Prepare image and create a big Surface with 8 Bit depth
    img = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_ARGB8888, 0);
    grayImg = createSurface(img->w, img->h, 8, 0, 0, 0, 0);
    SDL_SetSurfacePalette(grayImg, createPalette(0));

    // Convert pixels to grayscale
    startTime = clock();
    grayImg->pixels = grayscaler(img->pixels, img->w, img->h);
    endTime = clock();
    printf("%i clock ticks needed for grayscaler.\n", (int)(endTime-startTime));
    IMG_SavePNG(grayImg, "../img/grayscale.png");
    SDL_FreeSurface(img);

    // Apply gauss filter
    if(kernelSize != 0) {
        startTime = clock();
        grayImg->pixels = gauss(grayImg->pixels, grayImg->w, grayImg->h, kernelSize);
        endTime = clock();
        printf("%i clock ticks needed for gauss filter.\n", (int)(endTime-startTime));
        IMG_SavePNG(grayImg, "../img/gauss.png");
    }

    // Apply canny edge detector
    startTime = clock();
    grayImg->pixels = canny(grayImg->pixels, grayImg->w, grayImg->h, threshold, lowThreshold, highThreshold);
    endTime = clock();
    printf("%i clock ticks needed for canny edge detector.\n", (int)(endTime-startTime));
    IMG_SavePNG(grayImg, "../img/sobel.png");

    // Perform hough transform
    startTime = clock();
    circles = hough(grayImg->pixels, grayImg->w, grayImg->h, radius, radiusUpperBounds, houghThreshold);
    endTime = clock();
    printf("%i clock ticks needed for hough transform.\n", (int)(endTime-startTime));

    // Draw found circles in red
    SDL_SetSurfacePalette(grayImg, createPalette(1));
    for(int i = 0; i < circleCount; i++) {
        grayImg->pixels = drawCircle(grayImg->pixels, grayImg->w, grayImg->h, circles[i].x, circles[i].y, circles[i].r);
    }
    IMG_SavePNG(grayImg, "../img/hough.png");
    // Free memory for circles
    free(circles);

    SDL_FreeSurface(grayImg);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
