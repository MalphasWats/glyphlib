#include <stdlib.h>
#include "roguelike-engine.h"
#include "ASCII-SMALL.h"

Viewport viewport = {.x=0, .y=0, .offset_x=0, .offset_y=0};

Floater floaters[MAX_FLOATERS];

Window win_inventory;
const __flash Item* inventory_slots[MAX_INVENTORY];

int8_t inventory_cursor;

void init_engine( void )
{
    frame_timer = t+FRAME_DURATION;
    f = 0;

    button_timer = 0;

    map = malloc(2 + 2 + 3 + MAP_COLS_MAX*MAP_ROWS_MAX);

    for (uint8_t m=0 ; m<MAX_MOBS ; m++)
        mobs[m].alive = FALSE;

    for (uint8_t i=0 ; i<MAX_FLOATERS ; i++)
        floaters[i].counter = 255;

    init_windows();

    win_inventory = (Window){
        .x=3,
        .y=0,
        .w=10,
        .h=8,
        ._draw=draw_inventory,
    };

    for (uint8_t i=0 ; i<MAX_INVENTORY ; i++)
        inventory_slots[i] = 0;
    inventory_cursor = 2;
}

void gameloop( void )
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

    update_windows();
    update_floaters();

    _update();

    _draw();

    draw_floaters();
    draw_windows();
    draw();
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
            draw_tile(&map->tileset[map->tiles[map->cols * (row+y) + (col+x)]].data[0], &BLOCK_MASKS[OPAQUE], col*8-x_offset, row*8-y_offset, FALSE);
        }
    }
}


void draw_mob(Mob *s)
{
    int16_t x = s->x*8+s->offset_x;
    int16_t y = s->y*8+s->offset_y;
    x -= viewport.x*8+viewport.offset_x;
    y -= viewport.y*8+viewport.offset_y;

    draw_tile(&s->tileset[f].data[0], &BLOCK_MASKS[OPAQUE], x, y, s->flipped);
}

void add_floater(Floater f)
{
    for (uint8_t i=0 ; i<MAX_FLOATERS ; i++)
    {
        if (floaters[i].value == 0)
        {
            floaters[i] = f;
            return;
        }
    }
}

void update_floaters( void )
{
    for (uint8_t i=0 ; i<MAX_FLOATERS ; i++)
    {
        if (floaters[i].counter < 255)
        {
            if (floaters[i].timer <= t)
            {
                floaters[i].y -= 1;
                floaters[i].timer = t+FLOATER_DELAY;
                floaters[i].counter += 1;
            }
            if (floaters[i].counter > 6)
            {
                floaters[i].counter = 255;
            }
        }
    }
}

void draw_floaters( void )
{
    for (uint8_t i=0 ; i<MAX_FLOATERS ; i++)
    {
        if (floaters[i].counter < 255)
        {
            draw_tile( &floaters[i].tileset[floaters[i].value*8],
                       &floaters[i].masks[(floaters[i].value)*8],
                       floaters[i].x-viewport.x*8, floaters[i].y-viewport.y*8, FALSE
                     );
        }
    }
}

void move_viewport( void )
{
    if (player.x > viewport.x+SCREEN_COLUMNS-2)
    {
        viewport.x += 8;
        viewport.offset_x = -8*8;
    }
    if (player.x < viewport.x+1)
    {
        viewport.x -= 8;
        viewport.offset_x = 8*8;
    }

    if (player.y > viewport.y+SCREEN_ROWS-2)
    {
        viewport.y += 4;
        viewport.offset_y = -4*8;
    }
    if (player.y < viewport.y+1)
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
        _update = player_walk_ani;
    }
}

Tile get_tile_at(uint16_t x, uint16_t y)
{
    return map->tileset[map->tiles[ y * map->cols + x ]];
}

Mob* get_mob_at(uint16_t x, uint16_t y)
{
    for (uint8_t m=0 ; m<MAX_MOBS ; m++)
    {
        if (mobs[m].alive && mobs[m].x==x && mobs[m].y==y)
        {
            return &mobs[m];
        }
    }
    return 0;
}

void check_player_turn( void )
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
            player.flipped = TRUE;
            move_player(-1, 0);
        }
        if ( buttons & BTN_RIGHT )
        {
            player.flipped = FALSE;
            move_player(1, 0);
        }

        if ( buttons & BTN_B )
        {
            _update = update_inventory;
            show_window(&win_inventory, FIXED);
        }
    }
}

// TODO: use DIRX & DIRY
void move_player(int8_t dx, int8_t dy)
{
    int16_t px = player.x+dx;
    int16_t py = player.y+dy;

    // Simple InBounds
    if (px < 0 || px >= map->cols || py < 0 || py >= map->rows)
    {
        set_bump_ani(dx, dy);
    }

    Tile tile = get_tile_at(px, py);
    collide_mob = get_mob_at(px, py);
    if (collide_mob != 0)
    {
        set_bump_ani(dx, dy);

        click();

        hit_mob(&player, collide_mob);
    }
    else if (tile.flags & COLLIDE_FLAG)
    {
        set_bump_ani(dx, dy);
        collide_x = px;
        collide_y = py;

        if (tile.flags & FLAG_CHEST)
        {
            click();
            map->tiles[collide_y*map->cols+collide_x] += 1;
        }
    }
    else
    {
        player.x = px;
        player.offset_x = -dx*8;

        player.y = py;
        player.offset_y = -dy*8;

        move_viewport();
    }
}

void update_mobs( void )
{
    for (uint8_t m=0 ; m<MAX_MOBS ; m++)
    {
        if (mobs[m].alive)
        {
            if ( line_of_sight(mobs[m].x, mobs[m].y, player.x, player.y) )
            {
                if (!mobs[m].aggro)
                {
                    mobs[m].aggro = TRUE;
                    add_floater((Floater){
                        .x=mobs[m].x*8,
                        .y=mobs[m].y*8 - 4,
                        .counter=0,
                        .timer=t+FLOATER_DELAY,
                        .tileset=&STATUS_FLOATERS[0],
                        .masks=&STATUS_FLOATER_MASKS[0],
                        .value=FLOAT_STAT_AGGRO,
                    });
                }
            }
            else
            {
                mobs[m].aggro = FALSE;
            }
        }
    }

    _update = _update_return;
}

void set_bump_ani(int8_t dx, int8_t dy)
{
    player.offset_x = 0;
    player.offset_y = 0;
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
        _update = update_mobs;
    }
}

bool on_screen(uint8_t x, uint8_t y)
{
    return x >= viewport.x && x < viewport.x+SCREEN_COLUMNS && y >= viewport.y && y < viewport.y+SCREEN_ROWS;
}

bool line_of_sight(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
    // Are both points on-screen?
    if (!on_screen(x1, y1) || !on_screen(x2, y2))
        return FALSE;

    int8_t dx = abs(x2-x1);
    int8_t sx = 1;
    if (x2 < x1)
        sx = -1;

    int8_t dy = abs(y2-y1);
    int8_t sy = 1;
    if (y2 < y1)
        sy = -1;

    int8_t err;
    int8_t err2;

    if (dx > dy)
        err = dx>>1;
    else
        err = -(dy>>1);

    for(ever)
    {
        if (x1 == x2 && y1 == y2)
            return TRUE;
        Tile tile = get_tile_at(x1, y1);
        if (tile.flags & COLLIDE_FLAG)
        {
            return FALSE;
        }
        else
        {
            err2 = err;
            if (err2 > -dx)
            {
                err -= dy;
                x1 += sx;
            }
            if (err2 < dy)
            {
                err += dx;
                y1 += sy;
            }
        }
    }
    return TRUE;
}

void hit_mob(Mob* attacker, Mob* defender)
{
    //TODO: deal with defence
    defender->hp -= attacker->damage;

    add_floater((Floater){
        .x=defender->x*8,
        .y=defender->y*8 - 4,
        .counter=0,
        .timer=t+FLOATER_DELAY,
        .tileset=&DAMAGE_FLOATERS[0],
        .masks=&DAMAGE_FLOATER_MASKS[0],
        .value=attacker->damage,
    });

    if (defender->hp <= 0)
    {
        defender->alive = FALSE;
    }
}

void draw_small_string(const __memx char *string, int16_t x, int16_t y)
{
    for(uint8_t i=0 ; string[i] != '\0' ; i++)
    {
        draw_tile(&SMALL_ASCII[(string[i]-32)*8], &SMALL_CHAR_MASK[0], x+(i*4), y, FALSE);
    }
}

void draw_small_int(int16_t n, uint8_t width, int16_t x, int16_t y)
{
    //TODO: Negative numbers
    if (width == 0)
        width = num_digits(n);
    int32_t n_;
    for (uint8_t i=0 ; i<width ; i++)
    {
        n_ = (6554*(int32_t)n)>>16;
        draw_tile(&SMALL_DIGITS[(n - (n_*10))*8], &SMALL_CHAR_MASK[0], x+((width-i-1)*4), y, FALSE);
        n = (int16_t)n_;
    }
}

uint8_t num_digits(int16_t n)
{
    //TODO: negative numbers
    if (n < 10)
        return 1;
    if (n < 100)
        return 2;
    if (n < 1000)
        return 3;
    if (n < 10000)
        return 4;
    return 5;
}

void update_inventory( void )
{
    if (button_timer <= t)
    {
        button_timer = t+BUTTON_DELAY;

        uint8_t buttons;
        buttons = read_buttons();
        if ( buttons & BTN_UP )
        {
            if (inventory_cursor <= 4)
                inventory_cursor = 0;
            else
                inventory_cursor = 1;
        }
        if ( buttons & BTN_DOWN )
        {
            if (inventory_cursor <= 1)
                inventory_cursor = 2;
            else
                inventory_cursor = 7;
        }
        if ( buttons & BTN_LEFT )
        {
            inventory_cursor -= 1;
        }
        if ( buttons & BTN_RIGHT )
        {
            inventory_cursor += 1;
        }

        if (inventory_cursor < 0)
            inventory_cursor = 0;
        if (inventory_cursor > 7)
            inventory_cursor = 7;

        if (inventory_slots[0] != 0)
        {
            player.damage = 1 + inventory_slots[0]->value;
        }
        if (inventory_slots[1] != 0)
        {
            player.defence = inventory_slots[1]->value;
        }

        if ( buttons & BTN_B )
        {
            click();
            close_window(&win_inventory);
            _update = _update_return;
        }
        if ( buttons & BTN_A )
        {
            if (inventory_slots[inventory_cursor] != 0)
            {
                if (inventory_slots[inventory_cursor]->type == WEAPON)
                {
                    const __flash Item* tmp=inventory_slots[inventory_cursor];
                    inventory_slots[inventory_cursor] = inventory_slots[0];
                    inventory_slots[0]=tmp;
                }
                if (inventory_slots[inventory_cursor]->type == ARMOUR)
                {
                    const __flash Item* tmp=inventory_slots[inventory_cursor];
                    inventory_slots[inventory_cursor] = inventory_slots[1];
                    inventory_slots[1]=tmp;
                }
            }
        }
        if ( buttons & BTN_D )
        {
            inventory_slots[inventory_cursor] = 0;
        }
    }
}

void draw_inventory( Window* w )
{
    draw_tile(&BUTTON_GLYPHS[BTN_GLYPH_B], &BLOCK_MASKS[BUTTON_GLYPH], 11*8, 0, FALSE);

    draw_tile(&BUTTON_GLYPHS[BTN_GLYPH_A], &BLOCK_MASKS[BUTTON_GLYPH], 4*8, 7*8, FALSE);
    draw_tile(&BUTTON_GLYPHS[BTN_GLYPH_C], &BLOCK_MASKS[BUTTON_GLYPH], 5*8, 7*8, FALSE);
    draw_tile(&BUTTON_GLYPHS[BTN_GLYPH_D], &BLOCK_MASKS[BUTTON_GLYPH], 7*8, 7*8, FALSE);

    draw_small_string("ATK:", 5*8, 1*8+2);
    draw_small_string("DEF:", 9*8, 1*8+2);

    draw_small_int(player.damage, 0, 7*8, 1*8+2);
    draw_small_int(player.defence, 0, 11*8, 1*8+2);

    draw_tile(&INV_BRACKETS[0], &BLOCK_MASKS[OPAQUE], 5*8, 2*8, FALSE);
    draw_tile(&INV_BRACKETS[0], &BLOCK_MASKS[OPAQUE], 6*8, 2*8, TRUE);
    draw_tile(&INV_BRACKETS[8], &BLOCK_MASKS[OPAQUE], 5*8, 3*8, FALSE);
    draw_tile(&INV_BRACKETS[8], &BLOCK_MASKS[OPAQUE], 6*8, 3*8, TRUE);

    draw_tile(&INV_BRACKETS[0], &BLOCK_MASKS[OPAQUE], 9*8, 2*8, FALSE);
    draw_tile(&INV_BRACKETS[0], &BLOCK_MASKS[OPAQUE], 10*8, 2*8, TRUE);
    draw_tile(&INV_BRACKETS[8], &BLOCK_MASKS[OPAQUE], 9*8, 3*8, FALSE);
    draw_tile(&INV_BRACKETS[8], &BLOCK_MASKS[OPAQUE], 10*8, 3*8, TRUE);

    uint8_t cx, cy;

    for (uint8_t i=0 ; i<MAX_INVENTORY ; i++)
    {

        if (i == 0)
        {
            cx = 5*8+4;
            cy = 3*8-4;
        }
        else if (i == 1)
        {
            cx = 9*8+4;
            cy = 3*8-4;
        }
        else
        {
            cx = (3+i)*8;
            cy = 5*8;
        }
        if (inventory_slots[i] != 0)
            draw_tile(&inventory_slots[i]->tile[0], &BLOCK_MASKS[OPAQUE], cx, cy, FALSE);
        if (i == inventory_cursor)
            draw_tile(&INV_BRACKETS[CURSOR], &BLOCK_MASKS[TRANSPARENT], cx, cy, FALSE);
    }

    /*if (inventory_cursor == 0)
    {
        cx = 5*8+4;
        cy = 3*8-4;
    }
    else if (inventory_cursor == 1)
    {
        cx = 9*8+4;
        cy = 3*8-4;
    }
    else
    {
        cx = (3+inventory_cursor)*8;
        cy = 5*8;
    }
    draw_tile(&INV_BRACKETS[CURSOR], &BLOCK_MASKS[TRANSPARENT], cx, cy, FALSE);*/

    if (inventory_slots[inventory_cursor] != 0)
    {
        draw_small_string(inventory_slots[inventory_cursor]->name, 4*8, 6*8+2);
    }
}

void give_item(const __flash Item* item)
{
    // First 2 slots are special.
    for (uint8_t i=2 ; i<MAX_INVENTORY ; i++)
    {
        if (inventory_slots[i] == 0)
        {
            inventory_slots[i] = item;
            break;
            //TODO: message item name
        }
        //TODO: else message "inventory full"
    }
}
