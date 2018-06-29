#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#define PATH8 "/home/dom/Bachelorarbeit/img/gen/"
#define PATH32 "/home/dom/Bachelorarbeit/img/src/"
#define PATH PATH8
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

void convertImage(char* name, uint8_t bitdepth) {
    SDL_Surface* img;
    uint32_t* data;
    FILE* f;
    char* path = malloc(255);
    if(bitdepth == 8) strcpy(path, PATH8);
    else if(bitdepth == 32) strcpy(path, PATH32);
    strcat(path, name);
    strcat(path, ".png");
    char* outPath = malloc(255);
    strcpy(outPath, PATH8);
    strcat(outPath, name);
    strcat(outPath, ".dat");

    img = IMG_Load(path);
    if(!img) {
        fprintf(stderr, "ERROR: Could not load image: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }
    img = SDL_ConvertSurfaceFormat(img, SDL_PIXELFORMAT_ARGB8888, 0);

    f = fopen(outPath, "w");
    if(f == NULL) {
        fprintf(stderr, "ERROR: Could not open file: %s\n", path);
        exit(EXIT_FAILURE);
    }

    data = img->pixels;
    for(int y = 0; y < img->h; y++) {
        for(int x = 0; x < img->w; x++) {
            if(bitdepth == 8) fprintf(f, "%X ", data[y * img->w + x] & 0xFF);
            else if(bitdepth == 32) fprintf(f, "%X ", data[y * img->w + x] & 0xFFFFFF);
        }
        fprintf(f, "\n");
    }

    fclose(f);
    free(img);
}

void convertData(char* name, uint8_t bitdepth) {
    SDL_Surface* img;
    uint8_t* data8;
    uint32_t* data32;
    FILE* f;
    char buff[255];
    unsigned long index = 0;
    char* path = malloc(255);
    strcpy(path, PATH);
    strcat(path, name);
    strcat(path, ".dat");
    char* outPath = malloc(255);
    strcpy(outPath, PATH);
    strcat(outPath, name);
    strcat(outPath, ".png");

    f = fopen(path, "r");
    if(f == NULL) {
        fprintf(stderr, "ERROR: Could not open file: %s\n", path);
        exit(EXIT_FAILURE);
    }

    if(bitdepth == 8) {
        data8 = malloc(WIDTH*HEIGHT*sizeof(uint8_t));
        while(fscanf(f, "%s", buff) != EOF) {
            data8[index] = strtol(buff, NULL, 16);
            index++;
        }
        img = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 8, 0, 0, 0, 0);
        SDL_SetSurfacePalette(img, createPalette(0));
        img->pixels = data8;
    }
    else if(bitdepth == 32) {
        data32 = malloc(WIDTH*HEIGHT*sizeof(uint32_t));
        while(fscanf(f, "%s", buff) != EOF) {
            data32[index] = strtol(buff, NULL, 16);
            index++;
        }
        img = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
        img->pixels = data32;
    }

    IMG_SavePNG(img, outPath);

    fclose(f);
    if(bitdepth == 8) SDL_FreePalette(img->format->palette);
    SDL_FreeSurface(img);
}

void printUsage() {
    fprintf(stderr, "usage: convert mode bitdepth filename\n"
        "\tmode: 'data' or 'image'\n");
}

int main(int argc, char** argv) {
    uint8_t bitdepth;

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
        convertImage(argv[3], bitdepth);
    }
    else if(strcmp(argv[1], "data") == 0) {
        if(argc != 4) {
            printUsage();
            exit(EXIT_FAILURE);
        }

        convertData(argv[3], bitdepth);
    }
    else {
        printUsage();
        exit(EXIT_FAILURE);
    }

    IMG_Quit();
    SDL_Quit();
    return 0;
}
