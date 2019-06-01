#ifndef ROGUELIKE_ENGINE_H
#define ROGUELIKE_ENGINE_H

#include "GLYPH.h"

#define FRAME_DURATION 160
#define BUTTON_DELAY 120

#define COLLIDE_FLAG   0b10000000

#define MAX_MOBS 2
#define MAX_FLOATERS 5
#define FLOATER_DELAY 30

typedef struct Tile {
    uint8_t data[8];
    uint8_t flags;
} Tile;

typedef struct Mob {
    int16_t x;
    int16_t y;

    int8_t offset_x;
    int8_t offset_y;

    int8_t counter;
    int8_t dx;
    int8_t dy;

    bool flipped;

    const __flash Tile* tileset;

    bool alive;
    uint8_t type;

    int8_t hp;
    int8_t max_hp;

    uint8_t damage;
    uint8_t defence;
} Mob;

typedef struct Map {
    uint16_t cols;
    uint16_t rows;
    const __flash Tile* tileset;
    uint8_t tiles[];
} Map;

typedef struct Viewport {
    int8_t x;
    int8_t y;
    int8_t offset_x;
    int8_t offset_y;
} Viewport;

typedef struct Floater {
    uint8_t x;
    uint8_t y;
    uint8_t counter;
    uint32_t timer;
    const __flash uint8_t* tileset;
    uint8_t value;
} Floater;

typedef enum { NONE, BOUNDS, MAP, MOB } CollideType;

void init_engine( void );
void update_engine( void );
void draw_map();

void add_floater(Floater f);
void update_floaters( void );
void draw_floaters( void );

void draw_mob(Mob *s);

Tile get_tile_at(uint16_t x, uint16_t y);

CollideType check_move( void );
CollideType move_player(int8_t dx, int8_t dy);
void player_walk_ani( void );

void set_bump_ani(int8_t dx, int8_t dy);
void player_bump_ani( void );

void move_viewport( void );
void update_viewport_ani( void );


void (*_update)( void );
void (*_draw)( void );

void (*_update_return)( void );
void (*_draw_return)( void );

uint32_t t;
uint32_t frame_timer;
uint32_t button_timer;

uint8_t f;

Mob player;
Mob mobs[MAX_MOBS];

Map* map;

uint8_t collide_x;
uint8_t collide_y;
Mob* collide_mob;


static const __flash int8_t DIRX[] = { 0, 1, 0, -1,  1, 1, -1, -1};
static const __flash int8_t DIRY[] = {-1, 0, 1,  0, -1, 1,  1, -1};


#endif
