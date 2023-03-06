# SDL_sim_cursor

This small library emulates the SDL_*Cursor family of functions, useful if your driver doesnt support cursors (such as small embedded linux handhelds).

This uses the [GNOME Aidwaita 24x24 cursors](https://github.com/manu-mannattil/adwaita-cursors) which have been converted to 4bit by [medeirosT](https://github.com/medeirosT/adwaita-2bit-cusors).


Most of the code is from [SDL2](https://github.com/libsdl-org/SDL), from the file `src/events/SDL_mouse.c`.


## Usage

Place `SDL_sim_cursor.h` into your source tree.

```C

/* at the following include into a header that includes SDL.h */
#include "SDL_sim_cursor.h"

/* And in atleast 1 c/cpp file that includes the header you placed the above line in, add at the top of the file */
#define SDL_SIM_CURSOR_COMPILE 1

/* After SDL_Init() add the following: */
SDL_SIM_MouseInit();

/* And before SDL_Quit() */
SDL_SIM_MouseQuit();

```

If the program uses SDL_Surfaces / SDL_BlitSurface use the following:

```C
/* You need to find the call to the SDL_BlitSurface that draws to the main window surface, call this to the surface. */
SDL_SIM_BlitCursor(window_surface);

```

If the program uses SDL_Renderer you need to use the following:

```C
/* Assign the renderer to the SDL_sim_cursor library */
SDL_SIM_Set_Renderer(current_renderer);

/* Find SDL_RenderPresent and do */
SDL_SIM_RenderCursor(NULL);

```
