#include "ui.h"

void init_windows( void )
{
    for(uint8_t i=0 ; i<MAX_WINDOWS ; i++)
    {
        windows[i] = 0;
    }
}

bool add_window(Window* w)
{
    for(uint8_t i=0 ; i<MAX_WINDOWS ; i++)
    {
        if (windows[i] == 0)
        {
            windows[i] = w;
            return TRUE;
        }
    }
    return FALSE;
}

void update_windows( void )
{
    for(uint8_t i=0 ; i<MAX_WINDOWS ; i++)
    {
        if (windows[i]->timer <= millis())
        {
            windows[i]->timer = 0;
        }
    }
}

void draw_windows( void )
{
    for(uint8_t i=0 ; i<MAX_WINDOWS ; i++)
    {
        if (windows[i]->timer != 0)
        {
            draw_window(windows[i]);
        }
    }
}

void draw_window(Window* w)
{
    uint8_t tile;
    for(uint8_t y=0 ; y<w->h ; y++)
    {
        for(uint8_t x=0 ; x<w->w ; x++)
        {
            if (x==0 && y==0)
                tile = WIN_TL;
            else if (x==w->w-1 && y==0)
                tile = WIN_TR;
            else if (x==0 && y==w->h-1)
                tile = WIN_BL;
            else if (x==w->w-1 && y==w->h-1)
                tile = WIN_BR;
            else if (x==0)
                tile = WIN_L;
            else if (x==w->w-1)
                tile = WIN_R;
            else if (y==0)
                tile = WIN_T;
            else if (y==w->h-1)
                tile = WIN_B;
            else
                tile = WIN_EM;

            draw_tile(&WINDOW_TILES[tile], &WINDOW_TILE_MASKS[tile], (w->x+x)*8, (w->y+y)*8, FALSE);
        }
    }
    //TODO: deal with \n in content
    draw_string(w->content, (w->x+1)*8, (w->y+1)*8);
}
