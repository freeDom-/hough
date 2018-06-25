#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#define DATA_PATH "/home/dom/Bachelorarbeit/img/src/data.txt"

void convertToData(char* path) {
    SDL_Surface* img;
    uint32_t* data;
    FILE* f;

    img = IMG_Load(path);
    if(!img) {
        fprintf(stderr, "ERROR: Could not load image: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }
    data = img->pixels;

    f = fopen(DATA_PATH, "w");
    if(f == NULL) {
        fprintf(stderr, "ERROR: Could not open file: " DATA_PATH);
        exit(EXIT_FAILURE);
    }

    for(int y = 0; y < img->h; y++) {
        for(int x = 0; x < img->w; x++) {
            fprintf(f, "%uli\t", data[y * img->w + x]);
        }
        fprintf(f, "\n");
    }

    fclose(f);
}

int convertToImage(char* path) {
    SDL_Surface* img;
    FILE* f;

    f = fopen(DATA_PATH, "r");
    if(f == NULL) {
        fprintf(stderr, "ERROR: Could not open file: /home/dom/Bachelorarbeit/img/src/data.txt");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv) {
    // Initialize SDL and SDL Image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    if(argc != 3) return 1;
    if(argv[1] == "-d") convertToData(argv[2]);
    else if(argv[1] == "-i") convertToImage(argv[2]);
    else return 1;

    IMG_Quit();
    SDL_Quit();
    return 0;
}
