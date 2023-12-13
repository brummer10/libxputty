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


#include "xvaluedisplay.h"
#include "xvaluedisplay_private.h"


Widget_t* add_popup_spinbox(Widget_t *parent, const char * label,
                int x, int y, int width, int height) {
    int x1, y1;
    os_translate_coords(parent, parent->widget, os_get_root_window(parent->app, IS_WIDGET), 0, 0, &x1, &y1); 
    Widget_t *wid = create_window(parent->app, os_get_root_window(parent->app, IS_WIDGET), x1, y1, width +40, height+20);
    os_set_window_attrb(wid);
    os_set_transient_for_hint(parent, wid);
    //wid->func.expose_callback = _draw_spinbox;
    wid->flags |= IS_POPUP;
    wid->scale.gravity = NONE;
    wid->parent = parent;
    childlist_add_child(parent->childlist, wid);
    Widget_t *view_port = create_widget(wid->app, wid, 0 , 0, width+20, height+20);
    view_port->func.expose_callback = _draw_spinbox;
    Widget_t *buttons = create_widget(wid->app, wid, width+20 , 0, 20, height+20);
    buttons->func.expose_callback = _draw_buttons;
    buttons->scale.gravity = NORTHWEST;
    buttons->flags &= ~USE_TRANSPARENCY;
    buttons->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    buttons->func.button_release_callback = _buttons_released;
    
    return wid;
}

Widget_t* add_valuedisplay(Widget_t *parent, const char * label,
                int x, int y, int width, int height) {

    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
    add_popup_spinbox(wid, label, x, y, width, height);
    wid->label = label;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, 1.0, 0.01, CL_CONTINUOS);
    wid->adj = wid->adj_y;
    wid->scale.gravity = CENTER;
    wid->func.enter_callback = os_transparent_draw;
    wid->func.leave_callback = os_transparent_draw;
    wid->func.double_click_callback = _popup_spinbox;
    wid->func.expose_callback = _draw_valuedisplay;
    return wid;
}
