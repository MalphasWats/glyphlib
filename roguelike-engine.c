#include <stdlib.h>
#include "roguelike-engine.h"

Viewport viewport = {.x=0, .y=0};

void init_engine( void )
{
    frame_timer = t+FRAME_DURATION;
    f = 0;

    map = malloc(2 + 2 + 3 + 32*16);
    map->cols = 32;
    map->rows = 16;
}

void update_engine( void )
{
    t = millis();
    if (frame_timer <= t)
    {
        f += 1;
        if ( f == 4 )
        {
            f = 0;
        }
        frame_timer = t+FRAME_DURATION;
    }
}

void draw_map(const Map __memx *map)
{
    int16_t x = viewport.x;
    int16_t y = viewport.y;

    for (uint8_t row=0 ; row<SCREEN_ROWS ; row++)
    {
        for (uint8_t col=0 ; col<SCREEN_COLUMNS ; col++)
        {
            draw_tile(&map->tileset[map->tiles[map->cols * (row+y) + (col+x)]].data[0], &BLOCK_MASKS[OPAQUE], col*8, row*8);
        }
    }
}


void draw_sprite(Sprite *s)
{
    draw_tile(&s->tileset[f].data[0], &BLOCK_MASKS[OPAQUE], s->x*8+s->offset_x, s->y*8+s->offset_y);
}
/*
void center_on_sprite(Sprite *s, const Map __memx *map)
{
    viewport.x = s->x - (SCREEN_WIDTH>>1);
    viewport.y = s->y - (SCREEN_HEIGHT>>1);

    if (viewport.x < 0)
        viewport.x = 0;
    if (viewport.y < 0)
        viewport.y = 0;

    if (viewport.x > map->cols*8 - SCREEN_WIDTH)
        viewport.x = map->cols*8 - SCREEN_WIDTH;
    if (viewport.y > map->rows*8 - SCREEN_HEIGHT)
        viewport.y = map->rows*8 - SCREEN_HEIGHT;

}*/

Tile get_tile_at(const __memx Map* m, uint16_t x, uint16_t y)
{
    return m->tileset[m->tiles[ y * m->cols + x ]];
}
