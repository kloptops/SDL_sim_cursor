# SDL_sim_cursor

This small library emulates the SDL_*Cursor family of functions, useful if your driver doesnt support cursors (such as small embedded linux handhelds).

This uses the [GNOME Aidwaita 24x24 cursors](https://github.com/manu-mannattil/adwaita-cursors) which have been converted to 4bit by MADCAT.


Most of the code is from SDL2, from the file `src/events/SDL_mouse.c`.


