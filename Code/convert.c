#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#define PATH "/home/dom/Bachelorarbeit/img/gen/"
#define DATA_PATH PATH "data.txt"
#define WIDTH 400
#define HEIGHT 400

void convertImage(char* path) {
    SDL_Surface* img;
    uint32_t* data;
    FILE* f;

    img = IMG_Load(path);
    if(!img) {
        fprintf(stderr, "ERROR: Could not load image: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }
    img = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_ARGB8888, 0);

    f = fopen(DATA_PATH, "w");
    if(f == NULL) {
        fprintf(stderr, "ERROR: Could not open file: " DATA_PATH);
        exit(EXIT_FAILURE);
    }

    data = img->pixels;
    for(int y = 0; y < img->h; y++) {
        for(int x = 0; x < img->w; x++) {
            fprintf(f, "%X ", data[y * img->w + x]);
        }
        fprintf(f, "\n");
    }

    fclose(f);
    free(img);
}

void convertData() {
    SDL_Surface* img;
    uint32_t* data = malloc(WIDTH*HEIGHT*sizeof(uint32_t));
    FILE* f;
    char buff[255];
    unsigned long index = 0;

    f = fopen(DATA_PATH, "r");
    if(f == NULL) {
        fprintf(stderr, "ERROR: Could not open file: " DATA_PATH);
        exit(EXIT_FAILURE);
    }

    while(fscanf(f, "%s ", buff) != EOF) {
        data[index] = strtol(buff, NULL, 16);
        index++;
    }
    img = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    img->pixels = data;
    IMG_SavePNG(img, PATH "data.png");
    
    fclose(f);
    SDL_FreeSurface(img);
}

void printUsage() {
    fprintf(stderr, "usage: convert mode name\n"
        "\tmode: 'data' or 'image'\n");
}

int main(int argc, char** argv) {
    // Initialize SDL and SDL Image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    if(strcmp(argv[1], "image") == 0) {
       if(argc != 3) {
            printUsage();
            exit(EXIT_FAILURE);
        }
        convertImage(argv[2]);
    }
    else if(strcmp(argv[1], "data") == 0) {
        if(argc != 2) {
            printUsage();
            exit(EXIT_FAILURE);
        }
        convertData();
    }
    else {
        printUsage();
        exit(EXIT_FAILURE);
    }

    IMG_Quit();
    SDL_Quit();
    return 0;
}
