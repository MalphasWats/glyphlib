#include "isometric-engine.h"

Viewport viewport = {.x=0, .y=0};

void draw_image(const Image __flash *image, int16_t x, int16_t y)
{
    //x -= image->width>>1;
    //y -= image->height-8;
    
    //if (!visible(image, x, y))
    //    return;
    
    uint8_t rows = image->height>>3;
    uint8_t cols = image->width>>3;
    
    uint16_t mask_offset = rows * image->width;
    
    for(uint8_t r=0 ; r<rows ; r++)
    {
        for(uint8_t c=0 ; c<cols ; c++)
        {
            draw_tile(&image->data[(r*cols+c)*8], &image->data[ ((r*cols+c)*8) + mask_offset ], x+(c*8), y+(r*8));
        }
    }
}

void draw_map(const Map __memx *map, const __flash Image *tileset[])
{   
    int first_col = (viewport.y >> 3) + ((viewport.x-64)>>4);
    int first_row = (viewport.y >> 3) - ((viewport.x+64)>>4);
    if (first_col<0) first_col=0;
    if (first_row<0) first_row=0;
    if (first_col>map->cols-16) first_col=map->cols-16;
    if (first_row>map->rows-16) first_row=map->rows-16;
    
    for(uint16_t c=first_col ; c<first_col+16 ; c++)
    //for(word c=0 ; c<map->cols ; c++)
    {
        for(uint16_t r=first_row ; r<first_row+16 ; r++)
        //for(word r=0 ; r<map->rows ; r++)
        {
            const __flash Image *i = tileset[map->tiles[r*map->cols+c]];
            //TODO: VIEWPORT
            //int x = ((c*8+(64 - r*8)) - (i->width>>1)) - viewport.x;
            //int y = ((r*4+(c*4)) - (i->height-8)) - viewport.y;
            
            int16_t x = (((c - r) * 8) + 64) - (i->width>>1) - viewport.x;
            int16_t y = (((c + r) * 8) >> 1) - (i->height-8) - viewport.y;
            
            if ( visible(i, x, y) )
                draw_image(i, x, y);
                
            //draw_image(tileset[map->tiles[r*map->cols+c]], c*8+(64 - r*8), r*4+(c*4));
        }
    }
}

bool visible(const Image __flash *image, int16_t x, int16_t y)
{
    //TODO: need to include viewport
    //x += viewport.x;
    //y += viewport.y;
    if (x < SCREEN_WIDTH &&
        x >= 0-image->width &&
        y < SCREEN_HEIGHT &&
        y >= 0-image->height)
        return TRUE;
    
    return FALSE;
}

void draw_sprite(Sprite *s)
{
    draw_tile(s->tile, s->tile, s->x, s->y);
}

void set_viewport(int16_t x, int16_t y)
{
    viewport.x = x;
    viewport.y = y;
}

void center_on_sprite(Sprite *s, Map *map)
{

}