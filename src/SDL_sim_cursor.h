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

#ifndef SDL_SIM_CURSOR
#define SDL_SIM_CURSOR

#include <SDL.h>

extern void SDL_SIM_MouseInit();
extern void SDL_SIM_MouseQuit();

extern void SDL_SIM_SetDefaultCursor(SDL_Cursor *cursor);
extern SDL_Cursor *SDL_SIM_CreateCursor(const Uint8 *data, const Uint8 *mask,
                 int w, int h, int hot_x, int hot_y);
extern SDL_Cursor *SDL_SIM_CreateColorCursor(SDL_Surface *surface, int hot_x, int hot_y);
extern SDL_Cursor *SDL_SIM_CreateSystemCursor(SDL_SystemCursor id);
extern void SDL_SIM_SetCursor(SDL_Cursor *cursor);
extern SDL_Cursor *SDL_SIM_GetCursor();
extern SDL_Cursor *SDL_SIM_GetDefaultCursor();
extern void SDL_SIM_FreeCursor(SDL_Cursor *cursor);
extern int SDL_SIM_ShowCursor(int toggle);

/* */
extern void SDL_SIM_BlitCursor(SDL_Surface *surface);

#ifdef SDL_SIM_ENABLE
#define SDL_BlitCursor SDL_SIM_BlitCursor
#define SDL_SetCursor SDL_SIM_SetCursor
#define SDL_CreateCursor SDL_SIM_CreateCursor
#define SDL_CreateSystemCursor SDL_SIM_CreateSystemCursor
#define SDL_ShowCursor SDL_SIM_ShowCursor
#endif

#endif
