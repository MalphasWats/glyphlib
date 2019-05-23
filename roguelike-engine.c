#include <stdlib.h>
#include "roguelike-engine.h"

Viewport viewport = {.x=0, .y=0, .offset_x=0, .offset_y=0};

void init_engine( void )
{
    frame_timer = t+FRAME_DURATION;
    f = 0;

    button_timer = 0;

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
    int8_t x = viewport.x;
    int8_t y = viewport.y;
    uint8_t x_offset = viewport.offset_x;
    uint8_t y_offset = viewport.offset_y;

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
            draw_tile(&map->tileset[map->tiles[map->cols * (row+y) + (col+x)]*8].data[0], &BLOCK_MASKS[OPAQUE], col*8-x_offset, row*8-y_offset);
        }
    }
}


void draw_sprite(Sprite *s)
{
    int16_t x = s->x*8+s->offset_x;
    int16_t y = s->y*8+s->offset_y;
    x -= viewport.x*8+viewport.offset_x;
    y -= viewport.y*8+viewport.offset_y;

    draw_tile(&s->tileset[f].data[0], &BLOCK_MASKS[OPAQUE], x, y);
}

void move_viewport( void )
{
    if (player.x > viewport.x+SCREEN_WIDTH-3*8)
    {
        viewport.x += SCREEN_WIDTH-3*8;
        viewport.offset_x = -(SCREEN_WIDTH-3*8);
    }
    if (player.x < viewport.x+3*8)
    {
        viewport.x -= SCREEN_WIDTH-3*8;
        viewport.offset_x = SCREEN_WIDTH-3*8;
    }

    if (player.y > viewport.y+SCREEN_HEIGHT-3*8)
    {
        viewport.y += SCREEN_WIDTH-3*8;
        viewport.offset_y = -(SCREEN_HEIGHT-3*8);
    }
    if (player.y < viewport.y+3*8)
    {
        viewport.y -= SCREEN_HEIGHT-3*8;
        viewport.offset_y = SCREEN_HEIGHT-3*8;
    }

    if (viewport.x < 0)
    {
        viewport.x = 0;
        viewport.offset_x = 0;
    }
    if (viewport.y < 0)
    {
        viewport.y = 0;
        viewport.offset_y = 0;
    }

    if (viewport.x > map->cols*8 - SCREEN_WIDTH)
    {
        viewport.x = map->cols*8 - SCREEN_WIDTH;
        viewport.offset_x = 0;
    }
    if (viewport.y > map->rows*8 - SCREEN_HEIGHT)
    {
        viewport.y = map->rows*8 - SCREEN_HEIGHT;
        viewport.offset_y = 0;
    }

    _update = update_viewport_ani;
}

void update_viewport_ani( void )
{
    if ( viewport.offset_x < 0 )
    {
        viewport.offset_x += 1;
    }
    if ( viewport.offset_x > 0 )
    {
        viewport.offset_x -= 1;
    }
    if ( viewport.offset_y < 0 )
    {
        viewport.offset_y += 1;
    }
    if ( viewport.offset_y > 0 )
    {
        viewport.offset_y -= 1;
    }

    if ( viewport.offset_x == 0 && viewport.offset_y == 0)
    {
        _update = _update_return;
    }
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

Tile check_move( void )
{
    if (button_timer <= t)
    {
        button_timer = t+BUTTON_DELAY;

        uint8_t buttons;
        buttons = read_buttons();
        if ( buttons & BTN_UP )
        {
            return move_player(0, -1);
        }
        if ( buttons & BTN_DOWN )
        {
            return move_player(0, 1);
        }
        if ( buttons & BTN_LEFT )
        {
            return move_player(-1, 0);
        }
        if ( buttons & BTN_RIGHT )
        {
            return move_player(1, 0);
        }
    }
    return get_tile_at(map, player.x, player.y);
}

Tile move_player(int8_t dx, int8_t dy)
{
    int16_t px = player.x+dx;
    int16_t py = player.y+dy;

    //TODO: make a bump animation
    _update = player_walk_ani;

    // Simple InBounds
    if (px < 0 || px > 15 || py < 0 || py > 7)
    {
        player.offset_x = dx*8;
        player.offset_y = dy*8;
        return get_tile_at(map, player.x, player.y);
    }

    Tile tile = get_tile_at(map, px, py);
    if (tile.flags & COLLIDE_FLAG)
    {
        player.offset_x = dx*8;
        player.offset_y = dy*8;
    }
    else
    {
        player.x = px;
        player.offset_x = -dx*8;

        player.y = py;
        player.offset_y = -dy*8;

        //TODO: Player actually moved
        //TODO: scroll the map
    }
    return tile;
}

void player_walk_ani( void )
{
    if ( player.offset_x < 0 )
    {
        player.offset_x += 1;
    }
    if ( player.offset_x > 0 )
    {
        player.offset_x -= 1;
    }
    if ( player.offset_y < 0 )
    {
        player.offset_y += 1;
    }
    if ( player.offset_y > 0 )
    {
        player.offset_y -= 1;
    }

    if ( player.offset_x == 0 && player.offset_y == 0)
    {
        move_viewport();
    }
}
