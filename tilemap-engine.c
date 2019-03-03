#include "tilemap-engine.h"

Viewport viewport = {.x=0, .y=0};

void draw_map(const Map __memx *map, const __flash uint8_t *tileset)
{   
    int16_t x = viewport.x;
    int16_t y = viewport.y;
    uint8_t x_offset = x & 7; // x % 8
    x >>= 3;
    
    uint8_t y_offset = y & 7; // y % 8
    y >>= 3;
    
    uint8_t NUM_ROWS = SCREEN_ROWS;
    if (y_offset > 0)
        NUM_ROWS += 1;
    
    uint8_t NUM_COLS = SCREEN_COLUMNS;
    if (x_offset > 0)
        NUM_COLS += 1;
    
    for (uint8_t row=0 ; row<NUM_ROWS ; row++)
    {
        for (uint8_t col=0 ; col<NUM_COLS ; col++)
        {
            draw_tile(&tileset[map->tiles[map->cols * (row+y) + (col+x)]*8], &BLOCK_MASKS[OPAQUE], col*8-x_offset, row*8-y_offset); 
        }
    }
}

void draw_sprite(Sprite *s)
{
    draw_tile(s->tile, s->mask, s->x-viewport.x, s->y-viewport.y);
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