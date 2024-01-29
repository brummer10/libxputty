/* vim:ts=4:sw=4:et:
 *                           0BSD
 *
 *                    BSD Zero Clause License
 *
 *  Copyright (c) 2019 Hermann Meyer
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.

 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

/**
 * @file xwidget-platform.h
 * @brief Platform wrapper functions (Linux, MSWindows):
 * This file contains forward definitions and structs for platform support.
 * Platform specific implementations are located in xwidget-linux.c
 * and xwidget-mswin.c.
 * Xlib compatibility definitions are located in xputty-mswin.h.
 */


#pragma once

#ifndef XWIDGET_PLATFORM_H
#define XWIDGET_PLATFORM_H

#include "xputty.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------
----------------------------------------------------------------------- 
                    common structs and definitions
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

/**
 *
 * @brief Metrics_t            - struct to receive window size, position & visibility
 * \n Pass this struct to os_get_window_metrics()
 * @param width                - current width
 * @param height               - current height
 * @param x                    - current x position on Parent
 * @param y                    - current y position on Parent
 * @param visible              - window is not hidden or closed
 */

typedef struct {
/** current width */
    int width;
/** current height */
    int height;
/** current x position on Parent */
    int x;
/** current y position on Parent */
    int y;
/** window is not hidden or closed */
    bool visible;
} Metrics_t;

/*---------------------------------------------------------------------
----------------------------------------------------------------------- 
                    wrapper functions
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

/**
 * @brief os_open_display          - Connect to the display to use
 * @param *display_name            - Display name could be 0 for default
 * @return Display                 - return pointer to the Display struct
 */

Display *os_open_display(char *display_name);

/**
 * @brief os_close_display         - Close connection to the used display
 * @param *dpy                     - Display to close
 */

void os_close_display(Display *dpy);

/**
 * @brief os_get_root_window       - get a pointer to the root window (desktop)
 * @param *main                    - pointer to the Xputty struct
 * @param flag                     - Could be IS_WIDGET for NO decorated windows (popup etc.)
 *                                 - or IS_WINDOW for decorated TopLevel Windows (Dialogs, MainWindow, etc)
 * @return WINDOW                  - return pointer to the parent Window to use
 */

Window os_get_root_window(Xputty *main, int flag);

/**
 * @brief os_destroy_window        - destroy a widget (close and remove from processing)
 * @param *w                       - widget to destroy
 */

void os_destroy_window(Widget_t *w);

/**
 * @brief os_translate_coords      - get x,y related to destination Window
 * @param *w                       - Widget_t pointer
 * @param from_window              - source Window
 * @param to_window                - destination Window
 * @param from_x, from_y           - point from the source Window
 * @param *to_x, *to_y             - (Pointer) point in the destination Window
 */

void os_translate_coords(Widget_t *w, Window from_window, Window to_window,
                          int from_x, int from_y, int *to_x, int *to_y);

/**
 * @brief os_get_window_metrics    - Get the Merics_t struct related to a Widget_t
 * @param *w                       - Pointer to the widget 
 * @param *metrics                 - Pointer to the Metrics_t struct to hold the info for widget 
 */

void os_get_window_metrics(Widget_t *w, Metrics_t *metrics);

/**
 * @brief os_set_window_min_size   - Set the min and base size of a Widget_t
 * @param *w                       - Pointer to the widget 
 * @param min_width, min_height    - the min size to set
 * @param base_width, base_height  - the base size to set
 */

void os_set_window_min_size(Widget_t *w, int min_width, int min_height,
                            int base_width, int base_height);

/**
 * @brief os_move_window           - Move a  Widget_t
 * @param *dpy                     - Pointer to the Display the widget belongs to
 * @param *w                       - Pointer to the widget 
 * @param x, y                     - the point to move to
 */

void os_move_window(Display *dpy, Widget_t *w, int x, int y);

/**
 * @brief os_resize_window         - Resize a  Widget_t
 * @param *dpy                     - Pointer to the Display the widget belongs to
 * @param *w                       - Pointer to the Widget_t
 * @param x, y                     - the new size
 */

void os_resize_window(Display *dpy, Widget_t *w, int x, int y);

/**
 * @brief os_get_surface_size      - get the size of the cairo surface
 * @param *surface                 - the cairo surface 
 * @param *width, *height          - (Pointer) to get the size
 */

void os_get_surface_size(cairo_surface_t *surface, int *width, int *height);

/**
 * @brief os_set_widget_surface_size  - set the size of a Widget_t cairo surface
 * @param *w                          - Pointer to the Widget_t the surface belongs to
 * @param *width, *height             - the size to set
 */

void os_set_widget_surface_size(Widget_t *w, int width, int height);

/**
 * @brief os_create_main_window_and_surface  - create a Widget_t with a acairo surface
 * this function is used mainly internal on create_window()
 * @param *w                                 - the Widget_t to create 
 * @param *app                               - the Xputty struct the Widget_t belongs to
 * @param *win                               - the Parent Window were the Widget_t belongs to
 * @param x, y                               - the position related to the Parent Window
 * @param *width, *height                    - the Widget_t size 
 */

void os_create_main_window_and_surface(Widget_t *w, Xputty *app, Window win,
                          int x, int y, int width, int height);

/**
 * @brief os_create_main_widget_and_surface  - create a Widget_t with a acairo surface
 * this function is used mainly internal on create_widget()
 * @param *w                                 - the Widget_t to create 
 * @param *app                               - the Xputty struct the Widget_t belongs to
 * @param *parent                            - the Parent Widget_t were the Widget_t belongs to
 * @param x, y                               - the position related to the Parent Widget_t
 * @param *width, *height                    - the Widget_t size 
 */

void os_create_widget_window_and_surface(Widget_t *w, Xputty *app, Widget_t *parent,
                          int x, int y, int width, int height);

/**
 * @brief os_set_title             - Set the title of a Widget_t
 * @param *w                       - Pointer to the widget 
 * @param title                    - the title to set
 */

void os_set_title(Widget_t *w, const char *title);

/**
 * @brief os_widget_show           - Show a Widget_t
 * @param *w                       - Pointer to the widget to show
 */

void os_widget_show(Widget_t *w);

/**
 * @brief os_widget_hide           - Hide a Widget_t
 * @param *w                       - Pointer to the widget to hide
 */

void os_widget_hide(Widget_t *w);

/**
 * @brief os_show_tooltip          - Show a tooltip of a Widget_t
 * @param *wid                     - Pointer to the widget the tooltip belongs to
 * @param *w                       - the tooltip Widget_t
 */

void os_show_tooltip(Widget_t *wid, Widget_t *w);

/**
 * @brief os_transparent_draw      - Draw the Widget_t to the back buffer
 * @param *w_                      - Pointer to the widget 
 * @param *user_data               - additional user data or NULL
 */

void os_transparent_draw(void *w_, void *user_data);

/**
 * @brief os_expose_widget         - Draw the the back buffer to the Widget_t surface
 * @param *w                       - Pointer to the widget 
 */

void os_expose_widget(Widget_t *w);

/**
 * @brief os_send_configure_event  - Send a configure event to a Widget_t
 * @param *w                       - Pointer to the widget 
 * @param x, y                     - the position to set
 * @param width, height            - the size to set
 */

void os_send_configure_event(Widget_t *w,int x, int y, int width, int height);

/**
 * @brief os_send_button_press_event   - Send a button press event to a Widget_t
 * see XButtonEvent
 * @param *w                           - Pointer to the widget 
 */

void os_send_button_press_event(Widget_t *w);

/**
 * @brief os_send_button_release_event  - Send a button release event to a Widget_t
 * see XButtonEvent
 * @param *w                            - Pointer to the widget 
 */

void os_send_button_release_event(Widget_t *w);

/**
 * @brief os_send_send_systray_message  - Send a systray event to a Widget_t
 * only working on Linux for now 
 * @param *w                            - Pointer to the widget 
 */

void os_send_systray_message(Widget_t *w);

/**
 * @brief os_adjustment_callback        - called when a adjustment value have changed
 * used internal for redraw the Widget_t
 * @param *w_                           - Pointer to the widget 
 * @param *user_data                    - additional user data or NULL
 */

void os_adjustment_callback(void *w_, void *user_data);

/**
 * @brief os_ get_keyboard_input   - Get the Keyboard input and convert it to utf8
 * @param *w                       - Pointer to the widget 
 * @param *key                     - the XKeyEvent
 * @param *buf                     - the buffer to write the key in utf8 format
 * @param bufsize                  - the max buffer size to write to
 * @return bool                    - retrun true when the char is written to the buffer false on error
 */

bool os_get_keyboard_input(Widget_t *w, XKeyEvent *key, char *buf, size_t bufsize);

/**
 * @brief os_grab_pointer          - grab the mouse pointer. Works only on linux
 * @param *w                       - Pointer to the widget which will grab the mouse
 * @return int                     - Return 0 on error
 */

int os_grab_pointer(Widget_t *w);

/**
 * @brief os_set_input_mask        - set the Event mask to a Widget_t 
 * only work on linux, stub on Windows
 * @param *w                       - Pointer to the widget which will grab the mouse
 */

void os_set_input_mask(Widget_t *w);

/**
 * @brief os_set_window_attrb        - set the attribute mask to a Widget_t 
 * only work on linux, stub on Windows
 * @param *w                       - Pointer to the widget which will grab the mouse
 */

void os_set_window_attrb(Widget_t *w);

/**
 * @brief os_set_ transient_for_hint - set a Widget_t transient to a other Widget_t
 * only work on linux, stub on Windows
 * @param *parent                   - Pointer to the parent widget
 * @param *w                        - Pointer to the widget 
 */

void os_set_transient_for_hint(Widget_t *parent, Widget_t *w);

/**
 * @brief os_get_screen_height     - Get the height of the used Display
 * @param *w                       - Pointer to the widget 
 * @return int                     - the height of the Display
 */

int os_get_screen_height(Widget_t *w);

/**
 * @brief os_get_screen_width      - Get the width of the used Display
 * @param *w                       - Pointer to the widget 
 * @return int                     - the width of the Display
 */

int os_get_screen_width(Widget_t *w);

/**
 * @brief os_get_dpi               - Get the dpi of the used Display
 * @param *main                    - Pointer to the Xputty main struct
 */

void os_get_dpi(Xputty *main);

/**
 * @brief os_is directory          - check if a given path is a directory or a file
 * @param *filename                - the Path to check
 * @return bool                    - return true if path is directory, otrherwise false
 */

bool os_is_directory(const char *filename);

/**
 * @brief os_get_home_dir          - get the users home directory
 * @return *char                   - return the users home directory
 * must be freed by the caller
 */

char *os_get_home_dir();

/**
 * @brief os_free_pixmap           - Free the memory used by a Pixmap
 * only Linux, stub otherwise
 * @param *w                       - Pointer to the widget 
 * @param pixmap                   - the pixmap to be freed
 */

void os_free_pixmap(Widget_t *w, Pixmap pixmap);


/**
 * @brief utf8_from_locale         - convert active codepage to utf8
 * must be freed by the caller
 * @param *localestr               - Pointer to char to convert 
 * @return *char                   - the converted char in utf8 format
 */

char *utf8_from_locale(char *localestr);

/**
 * @brief locale_from_utf8         - convert utf8 to active codepage
 * must be freed by the caller
 * @param *utf8str                 - Pointer to char to convert 
 * @return *char                   - the converted char in locale format
 */

char *locale_from_utf8(char *utf8str);

/**
 * @brief os_quit                  - quit the main loop and free all used memory
 * @param *w                       - Pointer to the widget request the quit call
 */

void os_quit(Widget_t *w);

/**
 * @brief os_quit_widget           - remove a Widget_t from the main loop and free it's used memory
 * @param *w                       - Pointer to the widget request the quit call
 */

void os_quit_widget(Widget_t *w);

/**
 * @brief os_register_widget_destroy - Get the needed Atom to send a widget destroy message
 * @param *wid                       - Pointer to the widget request the quit call
 * @return ATOM                      - ATOM on linux, DWORD on windows
 */

Atom os_register_widget_destroy(Widget_t * wid);

/**
 * @brief os_register_wm_delete_window - Get the needed Atom to send a widget delete message
 * @param *wid                         - Pointer to the widget request the quit call
 * @return ATOM                        - ATOM on linux, DWORD on windows
 */

Atom os_register_wm_delete_window(Widget_t * wid);


/**
 * @brief os_widget_event_loop     - the Widget_t event loop
 * on windows all messges goes into WndProc, so stub here
 * @param *w_                      - Pointer to the widget 
 * @param *event                   - the event to handle
 * @param *main                    - pointer to the Xputty struct
 * @param *user_data               - additional user data could be NULL 
 */

void os_widget_event_loop(void *w_, void* event, Xputty *main, void* user_data);

/**
 * @brief os_run_embedded          - the event handling when run embedded
 * on windows all messges goes into WndProc, so stub here
 * @param *main                    - pointer to the Xputty struct
 */

void os_run_embedded(Xputty *main);

/**
 * @brief  os_main_run             - start the event loop
 * @param *main                    - pointer to the Xputty struct
 */

void os_main_run(Xputty *main);

/**
 * @brief  os_init_dnd             - register a Widget_t for handling  drag and drop events
 * only implemented on linux
 * @param *main                    - pointer to the Xputty struct
 */

void os_init_dnd(Xputty *main);



#ifdef __cplusplus
}
#endif

#endif //XWIDGET_PLATFORM_H
