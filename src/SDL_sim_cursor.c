/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2023 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include "SDL_sim_cursor.h"

struct SDL_Cursor
{
    SDL_Cursor *next;
    SDL_Surface *surface;
    int hot_x;
    int hot_y;
};


typedef struct {
    SDL_Cursor *cursors;
    SDL_Cursor *def_cursor;
    SDL_Cursor *cur_cursor;
    SDL_bool cursor_shown;
} SDL_SIM_Mouse;

typedef struct {
    int width;
    int height;
    int hot_x;
    int hot_y;
    SDL_Cursor *cursor;
    const unsigned char *data;
    const unsigned char *mask;
} BuiltinCursor;

/* Include our builtin cursors */
#include "builtin_cursors.h"

/* The mouse state */
static SDL_SIM_Mouse SDL_sim_mouse;
SDL_SIM_Mouse *SDL_SIM_GetMouse(void);


void SDL_SIM_MouseInit(void)
{
    SDL_SIM_Mouse *mouse = SDL_SIM_GetMouse();

    SDL_zerop(mouse);

    mouse->cursor_shown = SDL_TRUE;
    mouse->def_cursor = SDL_SIM_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
}


void SDL_SIM__FreeCursor(SDL_Cursor *cursor)
{
    SDL_FreeSurface(cursor->surface);
    SDL_free(cursor);
}


void SDL_SIM_MouseQuit(void)
{
    SDL_Cursor *cursor, *next;
    SDL_SIM_Mouse *mouse = SDL_SIM_GetMouse();

    cursor = mouse->cursors;
    while (cursor) {
        next = cursor->next;
        SDL_SIM__FreeCursor(cursor);
        cursor = next;
    }
    mouse->cursors = NULL;
    mouse->cur_cursor = NULL;

    if (mouse->def_cursor) {
        SDL_SIM__FreeCursor(mouse->def_cursor);
        mouse->def_cursor = NULL;
    }

    /* Clear out these old pointers. */
    for (int i=0; i < (sizeof(BUILTIN_CURSORS) / sizeof(BUILTIN_CURSORS[0])); i++) {
        if (BUILTIN_CURSORS[i]) {
            BUILTIN_CURSORS[i]->cursor = NULL;
        }
    }
}


SDL_SIM_Mouse *SDL_SIM_GetMouse(void)
{
    return &SDL_sim_mouse;
}


/* Public functions */
void SDL_SIM_SetDefaultCursor(SDL_Cursor *cursor)
{
    SDL_SIM_Mouse *mouse = SDL_SIM_GetMouse();

    mouse->def_cursor = cursor;
    if (!mouse->cur_cursor) {
        SDL_SIM_SetCursor(cursor);
    }
}

SDL_Cursor *SDL_SIM_CreateCursor(const Uint8 *data, const Uint8 *mask,
                 int w, int h, int hot_x, int hot_y)
{
    SDL_Surface *surface;
    SDL_Cursor *cursor;
    int x, y;
    Uint32 *pixel;
    Uint8 datab = 0, maskb = 0;
    const Uint32 black = 0xFF000000;
    const Uint32 white = 0xFFFFFFFF;
    const Uint32 transparent = 0x00000000;

    /* Make sure the width is a multiple of 8 */
    w = ((w + 7) & ~7);

    /* Create the surface from a bitmap */
    surface = SDL_CreateRGBSurface(0, w, h, 32,
                                   0x00FF0000,
                                   0x0000FF00,
                                   0x000000FF,
                                   0xFF000000);
    if (surface == NULL) {
        return NULL;
    }
    for (y = 0; y < h; ++y) {
        pixel = (Uint32 *)((Uint8 *)surface->pixels + y * surface->pitch);
        for (x = 0; x < w; ++x) {
            if ((x % 8) == 0) {
                datab = *data++;
                maskb = *mask++;
            }
            if (maskb & 0x80) {
                *pixel++ = (datab & 0x80) ? black : white;
            } else {
                *pixel++ = (datab & 0x80) ? black : transparent;
            }
            datab <<= 1;
            maskb <<= 1;
        }
    }

    cursor = SDL_SIM_CreateColorCursor(surface, hot_x, hot_y);

    SDL_FreeSurface(surface);

    return cursor;
}

SDL_Cursor *SDL_SIM_CreateColorCursor(SDL_Surface *surface, int hot_x, int hot_y)
{
    SDL_SIM_Mouse *mouse = SDL_SIM_GetMouse();
    SDL_Surface *temp = NULL;
    SDL_Cursor *cursor;

    if (surface == NULL) {
        SDL_InvalidParamError("surface");
        return NULL;
    }

    /* Sanity check the hot spot */
    if ((hot_x < 0) || (hot_y < 0) ||
        (hot_x >= surface->w) || (hot_y >= surface->h)) {
        SDL_SetError("Cursor hot spot doesn't lie within cursor");
        return NULL;
    }

    if (surface->format->format != SDL_PIXELFORMAT_ARGB8888) {
        temp = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
        if (temp == NULL) {
            return NULL;
        }
        surface = temp;
    }
    else {
        temp = SDL_DuplicateSurface(surface);
    }

    cursor = SDL_static_cast(SDL_Cursor*, SDL_malloc(sizeof(SDL_Cursor)));
    cursor->surface = temp;
    cursor->hot_x = hot_x;
    cursor->hot_y = hot_y;

    if (cursor) {
        cursor->next = mouse->cursors;
        mouse->cursors = cursor;
    }

    return cursor;
}

SDL_Cursor *SDL_SIM_CreateSystemCursor(SDL_SystemCursor id)
{
    BuiltinCursor *builtin_cursor;

    if (id >= (sizeof(BUILTIN_CURSORS) / sizeof(BUILTIN_CURSORS[0]))) {
        // TODO: maybe actually say which cursor was asked for.
        SDL_SetError("Unsupported SDL_SystemCursor.");

        return NULL;
    }

    builtin_cursor = BUILTIN_CURSORS[id];
    if (!builtin_cursor) {
        // TODO: maybe actually say which cursor was asked for.
        SDL_SetError("Unsupported SDL_SystemCursor.");

        return NULL;        
    }

    if (!builtin_cursor->cursor) {
        builtin_cursor->cursor = SDL_SIM_CreateCursor(
            builtin_cursor->data,
            builtin_cursor->mask,
            builtin_cursor->width,
            builtin_cursor->height, 
            builtin_cursor->hot_x, 
            builtin_cursor->hot_y);
    }

    return builtin_cursor->cursor;
}

/* SDL_SetCursor(NULL) can be used to force the cursor redraw,
   if this is desired for any reason.  This is used when setting
   the video mode and when the SDL window gains the mouse focus.
 */
void SDL_SIM_SetCursor(SDL_Cursor *cursor)
{
    SDL_SIM_Mouse *mouse = SDL_SIM_GetMouse();

    /* Return immediately if setting the cursor to the currently set one (fixes #7151) */
    if (cursor == mouse->cur_cursor) {
        return;
    }

    /* Set the new cursor */
    if (cursor) {
        /* Make sure the cursor is still valid for this mouse */
        if (cursor != mouse->def_cursor) {
            SDL_Cursor *found;
            for (found = mouse->cursors; found; found = found->next) {
                if (found == cursor) {
                    break;
                }
            }
            if (found == NULL) {
                SDL_SetError("Cursor not associated with the current mouse");
                return;
            }
        }
        mouse->cur_cursor = cursor;
    } else {
        cursor = mouse->def_cursor;
    }
}

SDL_Cursor *SDL_SIM_GetCursor(void)
{
    SDL_SIM_Mouse *mouse = SDL_SIM_GetMouse();

    if (mouse == NULL) {
        return NULL;
    }

    return mouse->cur_cursor;
}

SDL_Cursor *SDL_SIM_GetDefaultCursor(void)
{
    SDL_SIM_Mouse *mouse = SDL_SIM_GetMouse();

    if (mouse == NULL) {
        return NULL;
    }

    return mouse->def_cursor;
}

void SDL_SIM_FreeCursor(SDL_Cursor *cursor)
{
    SDL_SIM_Mouse *mouse = SDL_SIM_GetMouse();
    SDL_Cursor *curr, *prev;

    if (cursor == NULL) {
        return;
    }

    if (cursor == mouse->def_cursor) {
        return;
    }

    if (cursor == mouse->cur_cursor) {
        SDL_SIM_SetCursor(mouse->def_cursor);
    }

    for (prev = NULL, curr = mouse->cursors; curr;
         prev = curr, curr = curr->next) {
        if (curr == cursor) {
            if (prev) {
                prev->next = curr->next;
            } else {
                mouse->cursors = curr->next;
            }

            SDL_FreeSurface(curr->surface);
            SDL_free(curr);
            return;
        }
    }
}

int SDL_SIM_ShowCursor(int toggle)
{
    SDL_SIM_Mouse *mouse = SDL_SIM_GetMouse();
    SDL_bool shown;

    if (mouse == NULL) {
        return 0;
    }

    shown = mouse->cursor_shown;
    if (toggle >= 0) {
        if (toggle) {
            mouse->cursor_shown = SDL_TRUE;
        } else {
            mouse->cursor_shown = SDL_FALSE;
        }
        if (mouse->cursor_shown != shown) {
            SDL_SIM_SetCursor(NULL);
        }
    }
    return shown;
}

void SDL_SIM_BlitCursor(SDL_Surface *surface)
{
    SDL_SIM_Mouse *mouse = SDL_SIM_GetMouse();
    SDL_Cursor *cursor;
    SDL_Rect mouse_dest;
    int x;
    int y;

    if (!mouse)
        return;

    if (!mouse->cursor_shown)
        return;

    if (!mouse->cur_cursor)
        return;

    cursor = mouse->cur_cursor;

    SDL_GetMouseState(&x, &y);
    
    mouse_dest.x = x - cursor->hot_x;
    mouse_dest.y = y - cursor->hot_y;
    mouse_dest.w = cursor->surface->w;
    mouse_dest.h = cursor->surface->h;

    SDL_BlitSurface(cursor->surface, NULL, surface, &mouse_dest);
}
