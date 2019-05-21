#include "roguelike-engine.h"

Viewport viewport = {.x=0, .y=0};

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

/*
void draw_sprite(Sprite *s)
{
    uint16_t x = s->x - (s->width>>1);
    uint16_t y = s->y - (s->height>>1);

    for(uint8_t rows=0 ; rows<(s->height>>3) ; rows++)
    {
        for(uint8_t cols=0 ; cols<(s->width>>3) ; cols++)
        {
            draw_tile(s->tile + cols*8 + rows*s->width, s->mask, (x+(cols*8))-viewport.x, (y+(rows*8))-viewport.y);
        }
    }

    //draw_tile(s->tile, s->mask, s->x-viewport.x, s->y-viewport.y);
}

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

}

uint8_t tile_at_xy(const __memx Map* m, uint16_t x, uint16_t y)
{
    return m->tiles[ (y>>3) * m->cols + (x>>3) ];
}
*/
