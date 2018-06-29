#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#define PATH "/home/dom/Bachelorarbeit/img/gen/"
#define DATA_PATH PATH "data.dat"
#define WIDTH 400
#define HEIGHT 400

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

void convertImage8(char* path) {
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
        fprintf(stderr, "ERROR: Could not open file: " DATA_PATH "\n");
        exit(EXIT_FAILURE);
    }

    data = img->pixels;
    for(int y = 0; y < img->h; y++) {
        for(int x = 0; x < img->w; x++) {
            fprintf(f, "%X ", data[y * img->w + x] & 0xFF);
        }
        fprintf(f, "\n");
    }

    fclose(f);
    free(img);
}

void convertImage32(char* path) {
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
        fprintf(stderr, "ERROR: Could not open file: " DATA_PATH "\n");
        exit(EXIT_FAILURE);
    }

    data = img->pixels;
    for(int y = 0; y < img->h; y++) {
        for(int x = 0; x < img->w; x++) {
            fprintf(f, "%X ", data[y * img->w + x] & 0xFFFFFF);
        }
        fprintf(f, "\n");
    }

    fclose(f);
    free(img);
}

void convertData8(char* path) {
    SDL_Surface* img;
    uint8_t* data = malloc(WIDTH*HEIGHT*sizeof(uint8_t));
    FILE* f;
    char buff[255];
    unsigned long index = 0;

    f = fopen(path, "r");
    if(f == NULL) {
        fprintf(stderr, "ERROR: Could not open file: %s\n", path);
        exit(EXIT_FAILURE);
    }

    while(fscanf(f, "%s", buff) != EOF) {
        data[index] = strtol(buff, NULL, 16);
        index++;
    }
    img = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 8, 0, 0, 0, 0);
    SDL_SetSurfacePalette(img, createPalette(0));
    img->pixels = data;
    IMG_SavePNG(img, PATH "data.png");
    
    fclose(f);
    SDL_FreePalette(img->format->palette);
    SDL_FreeSurface(img);
}

void convertData32(char* path) {
    SDL_Surface* img;
    uint32_t* data = malloc(WIDTH*HEIGHT*sizeof(uint32_t));
    FILE* f;
    char buff[255];
    unsigned long index = 0;

    f = fopen(path, "r");
    if(f == NULL) {
        fprintf(stderr, "ERROR: Could not open file: %s\n", path);
        exit(EXIT_FAILURE);
    }

    while(fscanf(f, "%s", buff) != EOF) {
        data[index] = strtol(buff, NULL, 16);
        index++;
    }
    img = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
    img->pixels = data;
    IMG_SavePNG(img, PATH "data.png");
    
    fclose(f);
    SDL_FreeSurface(img);
}

void printUsage() {
    fprintf(stderr, "usage: convert mode bitdepth name\n"
        "\tmode: 'data' or 'image'\n");
}

int main(int argc, char** argv) {
    uint8_t bitdepth;
    char* path = DATA_PATH;

    // Initialize SDL and SDL Image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    if(argc < 3) {
        printUsage();
        exit(EXIT_FAILURE);
    }
    bitdepth = atoi(argv[2]);

    if(strcmp(argv[1], "image") == 0) {
       if(argc != 4) {
            printUsage();
            exit(EXIT_FAILURE);
        }
        if(bitdepth == 8) convertImage8(argv[3]);
        else if(bitdepth == 32) convertImage32(argv[3]);
    }
    else if(strcmp(argv[1], "data") == 0) {
        if(argc == 4) path = argv[3];
        else if(argc != 3) {
            printUsage();
            exit(EXIT_FAILURE);
        }

        if(bitdepth == 8) convertData8(path);
        else if(bitdepth == 32) convertData32(path);
    }
    else {
        printUsage();
        exit(EXIT_FAILURE);
    }

    IMG_Quit();
    SDL_Quit();
    return 0;
}
