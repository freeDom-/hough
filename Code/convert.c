#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

#define PATH "/home/dom/Bachelorarbeit/img/src/"
#define DATA_PATH PATH "data.txt"
#define WIDTH 400
#define HEIGHT 400

void convertToData(uint8_t bitdepth, char* name) {
    SDL_Surface* img;
    uint8_t* data8;
    uint32_t* data32;
    FILE* f;
    char* path = malloc(strlen(PATH) + strlen(name) + 1);
    strcpy(path, PATH);
    strcat(path, name);

    img = IMG_Load(path);
    if(!img) {
        fprintf(stderr, "ERROR: Could not load image: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }
    if(bitdepth == 8) data8 = img->pixels;
    else if(bitdepth == 32) data32 = img->pixels;

    f = fopen(DATA_PATH, "w");
    if(f == NULL) {
        fprintf(stderr, "ERROR: Could not open file: " DATA_PATH);
        exit(EXIT_FAILURE);
    }

    for(int y = 0; y < img->h; y++) {
        for(int x = 0; x < img->w; x++) {
            if(bitdepth == 8) fprintf(f, "%X\t", data8[y * img->w + x]);
            else if(bitdepth == 32) fprintf(f, "%X\t", data32[y * img->w + x]);
        }
        fprintf(f, "\n");
    }

    fclose(f);
    free(path);
}

int convertToImage(uint8_t bitdepth, char* name) {
    SDL_Surface* img;
    FILE* f;
    char buff[255];

    // TODO: save/read width and height from file?
    img = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, bitdepth, 0, 0, 0, 0);

    f = fopen(DATA_PATH, "r");
    if(f == NULL) {
        fprintf(stderr, "ERROR: Could not open file: /home/dom/Bachelorarbeit/img/src/data.txt");
        exit(EXIT_FAILURE);
    }

    while(fscanf(f, "%s", buff) != EOF) {
        printf("%s\n", buff);
    }
    
    fclose(f);
}

void printUsage() {
    fprintf(stderr, "usage: convert mode bitdepth name\n"
        "\tmode: 'data' or 'image'\n");
}

int main(int argc, char** argv) {
    // Initialize SDL and SDL Image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    if(argc != 4) {
        printUsage();
        exit(EXIT_FAILURE);
    }
    if(strcmp(argv[1], "data") == 0) convertToData(atoi(argv[2]), argv[3]);
    else if(strcmp(argv[1], "image") == 0) convertToImage(atoi(argv[2]), argv[3]);
    else {
        printUsage();
        exit(EXIT_FAILURE);
    }

    IMG_Quit();
    SDL_Quit();
    return 0;
}
