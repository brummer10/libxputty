/*
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

#include "xputty.h"


void main_init(Xputty *main) {
    main->dpy = os_open_display(0);
    assert(main->dpy);
    main->hdpi = 1.0;
    os_get_dpi(main);
    main->childlist = (Childlist_t*)malloc(sizeof(Childlist_t));
    assert(main->childlist);
    childlist_init(main->childlist);
    main->color_scheme = (XColor_t*)malloc(sizeof(XColor_t));
    assert(main->color_scheme);
    set_dark_theme(main);
    main->systray_color = (SystrayColor_t*)malloc(sizeof(SystrayColor_t));
    assert(main->systray_color);
    set_systray_color(main, 0.3, 0.3, 0.3, 1.0);
    main->hold_grab = NULL;
    main->key_snooper = NULL;
    main->submenu = NULL;
    main->run = true;
    main->is_grab = false;
    main->small_font = 10 * main->hdpi;
    main->normal_font = 12 * main->hdpi;
    main->big_font = 16 * main->hdpi;
    main->ctext = NULL;
    main->csize = 0;
    os_init_dnd(main);
}

void main_run(Xputty *main) {
    os_main_run(main);
}

void run_embedded(Xputty *main) {
    os_run_embedded(main);
}

void main_quit(Xputty *main) {
    int i = main->childlist->elem-1;
    for(;i>-1;i--) {
        Widget_t *w = main->childlist->childs[i];
        destroy_widget(w, main);
    }
    childlist_destroy(main->childlist);
    free(main->childlist);
    free(main->color_scheme);
    free(main->systray_color);
    os_close_display(main->dpy);
    free(main->ctext);
    debug_print("quit\n");
}
