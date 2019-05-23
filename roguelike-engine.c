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

void draw_map()
{
    int8_t x = viewport.x + (viewport.offset_x>>3);
    int8_t y = viewport.y + (viewport.offset_y>>3);
    uint8_t x_offset = viewport.offset_x&7;
    uint8_t y_offset = viewport.offset_y&7;

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
            draw_tile(&map->tileset[map->tiles[map->cols * (row+y) + (col+x)]].data[0], &BLOCK_MASKS[OPAQUE], col*8-x_offset, row*8-y_offset);

            //draw_tile(&map->tileset[map->tiles[map->cols * (row+y) + (col+x)]].data[0], &BLOCK_MASKS[OPAQUE], col*8-x_offset, row*8-y_offset);
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
    if (player.x > viewport.x+SCREEN_COLUMNS-3)
    {
        viewport.x += 8;
        viewport.offset_x = -8*8;
    }
    if (player.x < viewport.x+2)
    {
        viewport.x -= 8;
        viewport.offset_x = 8*8;
    }

    if (player.y > viewport.y+SCREEN_ROWS-3)
    {
        viewport.y += 4;
        viewport.offset_y = -4*8;
    }
    if (player.y < viewport.y+2)
    {
        viewport.y -= 4;
        viewport.offset_y = 4*8;
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

    if (viewport.x > map->cols - SCREEN_COLUMNS)
    {
        viewport.x = map->cols - SCREEN_COLUMNS;
        viewport.offset_x = 0;
    }
    if (viewport.y > map->rows - SCREEN_ROWS)
    {
        viewport.y = map->rows - SCREEN_ROWS;
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

Tile get_tile_at(uint16_t x, uint16_t y)
{
    return map->tileset[map->tiles[ y * map->cols + x ]];
}

void check_move( void )
{
    if (button_timer <= t)
    {
        button_timer = t+BUTTON_DELAY;

        uint8_t buttons;
        buttons = read_buttons();
        if ( buttons & BTN_UP )
        {
            move_player(0, -1);
        }
        if ( buttons & BTN_DOWN )
        {
            move_player(0, 1);
        }
        if ( buttons & BTN_LEFT )
        {
            move_player(-1, 0);
        }
        if ( buttons & BTN_RIGHT )
        {
            move_player(1, 0);
        }
    }
}

void move_player(int8_t dx, int8_t dy)
{
    int16_t px = player.x+dx;
    int16_t py = player.y+dy;

    _update = player_walk_ani;

    // Simple InBounds
    if (px < 0 || px >= map->cols || py < 0 || py >= map->rows)
    {
        set_bump_ani(dx, dy);

        return;
    }

    Tile tile = get_tile_at(px, py);
    if (tile.flags & COLLIDE_FLAG)
    {
        set_bump_ani(dx, dy);
    }
    else
    {
        player.x = px;
        player.offset_x = -dx*8;

        player.y = py;
        player.offset_y = -dy*8;
    }
}

void set_bump_ani(int8_t dx, int8_t dy)
{
    player.offset_x = 0;//dx*4;
    player.offset_y = 0;//dy*4;
    player.counter = 8;
    player.dx = dx;
    player.dy = dy;
    _update = player_bump_ani;
}

void player_bump_ani( void )
{
    player.offset_x += player.dx;
    player.offset_y += player.dy;
    player.counter -= 1;
    if (player.counter == 4)
    {
        player.dx *= -1;
        player.dy *= -1;
    }

    if (player.counter == 0)
        _update = _update_return;
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
