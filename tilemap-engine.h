#ifndef TILEMAP_ENGINE_H
#define TILEMAP_ENGINE_H

#include "SPIKE.h"

typedef struct Sprite {
    int16_t x;
    int16_t y;
    
    const uint8_t __flash *tile;
    const uint8_t __flash *mask;
} Sprite;

typedef struct Map {
    uint16_t cols;
    uint16_t rows;
    uint8_t tileset;
    uint8_t tiles[];
} Map;

typedef struct Viewport {
    int16_t x;
    int16_t y;
} Viewport;

void draw_map(const Map __memx *map, const __flash uint8_t *tileset);
void draw_sprite(Sprite *s);

void center_on_sprite(Sprite *s, const Map __memx *map);

#endif