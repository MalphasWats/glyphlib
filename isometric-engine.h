#ifndef TILE_ENGINE_H
#define TILE_ENGINE_H

#include "GLYPH.h"

typedef struct Sprite {
    int x;
    int y;
    
    const byte __memx *tile;
} Sprite;

typedef struct Map {
    word cols;
    word rows;
    byte tiles[];
} Map;

/*typedef struct Image {
    word width;
    word height;
    byte data[];
} Image;*/

typedef struct Viewport {
    int x;
    int y;
} Viewport;

void draw_image(const Image __flash *image, int x, int y);
void draw_map(const Map __memx *map, const __flash Image *tileset[]);
void draw_sprite(Sprite *s);
bool visible(const Image __flash *image, int x, int y);

void set_viewport(int x, int y);

void center_on_sprite(Sprite *s, Map *map);

#endif