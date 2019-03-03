#ifndef TILE_ENGINE_H
#define TILE_ENGINE_H

#include "GLYPH.h"

typedef struct Sprite {
    int16_t x;
    int16_t y;
    
    const uint8_t __memx *tile;
} Sprite;

typedef struct Map {
    uint16_t cols;
    uint16_t rows;
    uint8_t tiles[];
} Map;

/*typedef struct Image {
    word width;
    word height;
    byte data[];
} Image;*/

typedef struct Viewport {
    int16_t x;
    int16_t y;
} Viewport;

void draw_image(const Image __flash *image, int16_t x, int16_t y);
void draw_map(const Map __memx *map, const __flash Image *tileset[]);
void draw_sprite(Sprite *s);
bool visible(const Image __flash *image, int16_t x, int16_t y);

void set_viewport(int16_t x, int16_t y);

void center_on_sprite(Sprite *s, Map *map);

#endif