#ifndef ROGUELIKE_ENGINE_H
#define ROGUELIKE_ENGINE_H

#include "GLYPH.h"

#define FRAME_DURATION 160

void (*_update)( void );
void (*_draw)( void );

uint32_t t;
uint32_t frame_timer;

uint8_t f;

typedef struct Tile {
    uint8_t data[8];
    uint8_t flags;
} Tile;

typedef struct Sprite {
    int16_t x;
    int16_t y;

    uint8_t width;
    uint8_t height;

    const uint8_t __flash *tile;
    const uint8_t __flash *mask;
} Sprite;

typedef struct Map {
    uint16_t cols;
    uint16_t rows;
    const Tile __flash *tileset;
    uint8_t tiles[];
} Map;

typedef struct Viewport {
    int16_t x;
    int16_t y;
} Viewport;

void update_engine( void );
void draw_map(const Map __memx *map);
//void draw_sprite(Sprite *s);

//void center_on_sprite(Sprite *s, const Map __memx *map);

//uint8_t tile_at_xy(const __memx Map* m, uint16_t x, uint16_t y);

#endif
