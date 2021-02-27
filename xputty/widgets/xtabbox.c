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


#include "xtabbox.h"
#include "xtabbox_private.h"


Widget_t* tabbox_add_tab(Widget_t *tabbox, const char * label) {
    XWindowAttributes attrs;
    XGetWindowAttributes(tabbox->app->dpy, (Window)tabbox->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    Widget_t *wid = create_widget(tabbox->app,tabbox, 4, 20,  width-8, height-24);
    wid->scale.gravity = NORTHWEST;
    wid->label = label;
    wid->func.expose_callback = _draw_tab;
    //wid->func.map_notify_callback = _tab_mapped_callback;
    float max_value = tabbox->adj->max_value+1.0;
    set_adjustment(tabbox->adj,0.0, max_value, 0.0, max_value,1.0, CL_NONE);
    adj_set_value(tabbox->adj,0.0);
    return wid;
}

Widget_t* add_tabbox(Widget_t *parent, const char * label,
                int x, int y, int width, int height) {

    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
    wid->label = label;
    wid->scale.gravity = CENTER;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, -1.0,1.0, CL_NONE);
    wid->adj = wid->adj_y;
    wid->func.expose_callback = _draw_tabbox;
    //wid->func.button_press_callback = _tab_button_pressed;
    wid->func.button_release_callback = _tab_button_released;
    //wid->func.map_notify_callback = _tabbox_mapped_callback;
    return wid;
}

void tabbox_remove_tab(Widget_t *tabbox, int tab) {
    int elem = tabbox->childlist->elem;
    if (tab > elem) return;
    Widget_t *wid = tabbox->childlist->childs[tab];
    destroy_widget(wid, tabbox->app);
    float max_value = tabbox->adj->max_value-1.0;
    set_adjustment(tabbox->adj,0.0, max_value, 0.0, max_value,1.0, CL_NONE);
}
