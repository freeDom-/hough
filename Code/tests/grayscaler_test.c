#include <inttypes.h>
#include <sys/time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../include/dimensions.h"

void grayscaler(uint32_t input[SIZE], uint8_t output[SIZE]);

/*
** Returns time in microseconds
*/
long getTime() {
    struct timeval timecheck;
    gettimeofday(&timecheck, NULL);
    return (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec / 1000;
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

int main(int argc, char **argv) {
	int x, y;
	long hwStartTime, hwEndTime;
	long swStartTime, swEndTime;
	unsigned int errCnt = 0;
	const char *path = "/home/dom/Bachelorarbeit/img/src/euro.png";

    SDL_Surface* img;
    SDL_Surface* grayImg;

    uint32_t *input;
    uint8_t *swResult = malloc(400*400);
    uint8_t *hwResult = malloc(400*400);

    // Initialize SDL and SDL Image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    // Load test image
	img = IMG_Load(path);
	if(!img) {
		fprintf(stderr, "ERROR: Could not load image: %s\n", IMG_GetError());
		exit(EXIT_FAILURE);
	}

	// Prepare image
	img = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_ARGB8888, 0);

	// Convert pixels to grayscale using SW
	swStartTime = getTime();
	input = img->pixels;
    for(y = 0; y < HEIGHT; y++) {
        for(x = 0; x < WIDTH; x++) {
			int index = y * WIDTH + x;
			uint8_t r, g, b;

			r = input[index] >> 16 & 0xFF;
			g = input[index] >> 8 & 0xFF;
			b = input[index] & 0xFF;

			swResult[index] = 0.3*r + 0.59*g + 0.11*b;
        }
    }
	swEndTime = getTime();
	printf("%li ms needed for grayscaler on SW.\n", swEndTime-swStartTime);

	// Create a Surface with 8 Bit depth
	grayImg = SDL_CreateRGBSurface(0, img->w, img->h, 8, 0, 0, 0, 0);
	if(grayImg == NULL) {
		fprintf(stderr, "ERROR: SDL_CreateRGBSurface() failed: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	SDL_SetSurfacePalette(grayImg, createPalette(0));

	// Convert pixels to grayscale using HW
	hwStartTime = getTime();
	grayscaler(img->pixels, hwResult);
	hwEndTime = getTime();
	printf("%li ms needed for grayscaler on HW.\n", hwEndTime-hwStartTime);
	SDL_FreeSurface(img);

	// Compare results
    for(y = 0; y < HEIGHT; y++) {
        for(x = 0; x < WIDTH; x++) {
        	if(hwResult[y * WIDTH + x] != swResult[y * WIDTH + x]) {
        		errCnt++;
        		fprintf(stderr, "ERROR: mismatch at position (%i, %i)\n", x, y);
        	}
        }
    }
    if(errCnt) {
    	fprintf(stderr, "ERROR: %i mismatches detected!\n", errCnt);
    }
    else fprintf(stderr, "Test passed!");

	free(swResult);
	free(hwResult);
}
