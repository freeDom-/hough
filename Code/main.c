/*
** Finds circles in a 32 bit color image
** usage: ./hough imagename threshold
*/

#ifndef SDL2_H_INCLUDED
#define SDL2_H_INCLUDED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif /*SDL2_H_INCLUDED*/

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif /*STRING_H_INCLUDED*/
#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED
#include <time.h>
#endif /*TIME_H_INCLUDED*/

#include "grayscaler.h"
#include "gauss.h"
#include "canny.h"
#include "hough.h"

/*
** Creates a (grayscale) palette with 256 colors
*/
SDL_Palette* createPalette(uint8_t mode) {
    SDL_Palette* p = SDL_AllocPalette(256);
    SDL_Color* tmp;

    for(int i = 0; i < 256; i++) {
        tmp = &p->colors[i];
        tmp->r = i;
        tmp->g = i;
        tmp->b = i;
    }

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
** Finding circles in a 32 bit color image by
** 1) Transforming the image to a grayscale image
** 2) Applying a Gaussian filter
** 3) Converting the image to a binary image 
*/
int main(int argc, char **argv) {
	uint8_t threshold = 0;
    uint8_t kernelSize = 5;
    uint8_t highThreshold = 90;
    uint8_t lowThreshold = 70;
    uint8_t radius = 70;
    uint8_t radiusRange = 10;
    unsigned int houghThreshold = 300;
    clock_t startTime, endTime;
    circle* circles = NULL;

    // Set default path for images
    char* dir = "../img/";
    char* path = malloc(sizeof(dir) + sizeof("../img/test.png") + 1);
    strcpy(path, dir);
    strcat(path, "../img/test.png");

    SDL_Surface* img;
    SDL_Surface* newImg;

    // Initialize SDL and SDL Image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    // Evaluate arguments
    if(argc % 2 == 0) {
        printf("Wrong number of arguments.\nUsage: ./hough -k 'kernelSize' -i 'imagename' -t 'threshold' -lt 'lowThreshold' -ht 'highThreshold -r 'radius' -rr 'radiusRange' -htr 'houghThreshold'\n");
        return 1;
    }
    for(int i = 3; i <= argc; i += 2) {
        if(strcmp(argv[i-2],"-i") == 0) {
            free(path);
            path = malloc(sizeof(dir) + sizeof(argv[i-1]) + 1);
            strcpy(path, dir);
            strcat(path, argv[i-1]);
        }
        else if(strcmp(argv[i-2],"-t") == 0) {
            threshold = atoi(argv[i-1]);
        }
        else if(strcmp(argv[i-2],"-k") == 0) {
            kernelSize = atoi(argv[i-1]);
            if(kernelSize != 0 && kernelSize % 2 == 0) {
                printf("kernelSize must be uneven.\n");
                return 1;
            }
        }
        else if(strcmp(argv[i-2],"-ht") == 0) {
            highThreshold = atoi(argv[i-1]);
        }
        else if(strcmp(argv[i-2],"-lt") == 0) {
            lowThreshold = atoi(argv[i-1]);
        }
        else if(strcmp(argv[i-2], "-r") == 0) {
            radius = atoi(argv[i-1]);
        }
        else if(strcmp(argv[i-2], "-rr") == 0) {
            radiusRange = atoi(argv[i-1]);
        }
        else if(strcmp(argv[i-2], "-htr") == 0) {
            houghThreshold = atoi(argv[i-1]);
        }
    }

    // Load test image
    img = loadImage(path);
    if(img == NULL)
        return 1;

    // Prepare image and create a big Surface with 8 Bit depth
    img = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_ARGB8888, 0);
    newImg = createSurface(img->w, img->h, 8, 0, 0, 0, 0);
    SDL_SetSurfacePalette(newImg, createPalette(0));

    // Convert pixels to grayscale
    startTime = clock();
    newImg->pixels = grayscaler(img->pixels, img->w, img->h);
    endTime = clock();
    printf("%i clock ticks needed for grayscaler.\n", (int)(endTime-startTime));
    IMG_SavePNG(newImg, "../img/grayscale.png");
    SDL_FreeSurface(img);

    // Apply gauss filter
    if(kernelSize != 0) {
        startTime = clock();
        newImg->pixels = gauss(newImg->pixels, newImg->w, newImg->h, kernelSize);
        endTime = clock();
        printf("%i clock ticks needed for gauss filter.\n", (int)(endTime-startTime));
        IMG_SavePNG(newImg, "../img/gauss.png");
    }

    // Apply canny edge detector
    startTime = clock();
    newImg->pixels = canny(newImg->pixels, newImg->w, newImg->h, threshold, lowThreshold, highThreshold);
    endTime = clock();
    printf("%i clock ticks needed for canny edge detector.\n", (int)(endTime-startTime));
    IMG_SavePNG(newImg, "../img/sobel.png");

    // Perform hough transform
    startTime = clock();
    circles = hough(newImg->pixels, newImg->w, newImg->h, radius, radiusRange, houghThreshold);
    endTime = clock();
    printf("%i clock ticks needed for hough transform.\n", (int)(endTime-startTime));

    // Draw found circles in red
    SDL_SetSurfacePalette(newImg, createPalette(1));
    for(int i = 0; i < circleCount; i++) {
        ((uint8_t*) newImg->pixels)[circles[i].y * newImg->w + circles[i].x] = 127;
    }
    IMG_SavePNG(newImg, "../img/hough.png");
    // Free memory for circles
    free(circles);

    SDL_FreeSurface(newImg);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
