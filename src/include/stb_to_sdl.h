#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


static SDL_Texture* image_load(const char* filename, SDL_Renderer* renderer)
{
    int x, y, n;

    // Load image using stb_image
    unsigned char* data = stbi_load(filename, &x, &y, &n, 0);
    if (!data) {
        fprintf(stderr, "Failed to load image: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // Create SDL surface from stb_image data
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(data, x, y, 4, 4 * x, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        fprintf(stderr, "Failed to create SDL surface: %s\n", SDL_GetError());
        stbi_image_free(data);
        exit(EXIT_FAILURE);
    }

    // Create SDL texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        fprintf(stderr, "Failed to create SDL texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        stbi_image_free(data);
        exit(EXIT_FAILURE);
    }

    // Free stb_image data and SDL surface
    stbi_image_free(data);
    SDL_FreeSurface(surface);

    return texture;
}

