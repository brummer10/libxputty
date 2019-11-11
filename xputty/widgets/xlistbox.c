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


#include "xlistbox.h"
#include "xlistbox_private.h"


/**
 * @brief listbox_set_active_entry   - set the active listbox entry
 * @param *w_                        - void pointer to the Widget_t listbox
 * @param active                     - the active entry (int)
 * @return void
 */

void listbox_set_active_entry(Widget_t *w, int active) {
    float value = (float)active;
    if (value>w->adj->max_value) value = w->adj->max_value;
    else if (value<w->adj->min_value) value = w->adj->min_value;
    adj_set_value(w->adj, value);
}

/**
 * @brief create_listbox_viewport     - create a viewport on a listbox to a Widget_t
 * @param *parent                     - pointer to the Widget_t request the viewport
 * @param width                       - define the width of the viewport
 * @param height                      - define the height of the viewport
 * @return Widget_t*                  - pointer to the Widget_t viewport
 */

Widget_t* create_listbox_viewport(Widget_t *parent, int elem, int width, int height) {
    Widget_t *wid = create_widget(parent->app, parent, 0, 0, width, height);
    wid->scale.gravity = NORTHEAST;
    float max_value = -elem ;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, max_value,1.0, CL_VIEWPORT);
    wid->adj = wid->adj_y;
    wid->func.adj_callback = _set_listbox_viewpoint;
    wid->func.expose_callback = _draw_listbox_viewslider;
    wid->func.configure_notify_callback = _reconfigure_listbox_viewport;
    wid->func.map_notify_callback = _configure_listbox;
    return wid;
}

/**
 * @brief add_label           - add a listbox to a Widget_t
 * @param *parent             - pointer to the Widget_t request the listbox
 * @param *label              - Label to show on the button
 * @param x,y,width,height    - the position/geometry to create the listbox
 * @return Widget_t*          - pointer to the Widget_t label struct
 */

Widget_t* add_listbox(Widget_t *parent, const char * label,
                int x, int y, int width, int height) {

    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
    wid->label = label;
    wid->scale.gravity = CENTER;
    wid->func.expose_callback = _draw_listbox;
    int elem = height/25;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, -1.0,1.0, CL_NONE);
    wid->adj = wid->adj_y;
    create_listbox_viewport(wid, elem, width, height);
    return wid;
}

/**
 * @brief listbox_add_entry   - add a item to listbox
 * @param *listbox            - pointer to the Widget_t listbox
 * @param *label              - Label to show on the listbox
 * @return Widget_t*          - pointer to the Widget_t listbox_item struct
 */

Widget_t* listbox_add_entry(Widget_t *listbox, const char * label) {
    Widget_t* view_port =  listbox->childlist->childs[0];
    int width = listbox->scale.init_width;
    int si = childlist_has_child(view_port->childlist);
    Widget_t *wid = create_widget(listbox->app, view_port, 0, 25*si, width, 25);
    float max_value = view_port->adj->max_value+1.0;
    set_adjustment(view_port->adj,0.0, 0.0, 0.0, max_value,1.0, CL_VIEWPORT);
    max_value = listbox->adj->max_value+1.0;
    set_adjustment(listbox->adj,0.0, 0.0, 0.0, max_value,1.0, CL_NONE);
    wid->flags &= ~USE_TRANSPARENCY | ~HAS_TOOLTIP;
    wid->scale.gravity = MENUITEM;
    wid->label = label;
    wid->func.expose_callback = _draw_listbox_item;
    wid->func.enter_callback = transparent_draw;
    wid->func.leave_callback = transparent_draw;
    wid->func.button_release_callback = _listbox_entry_released;
    return wid;
}
