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


#include "xmenu.h"
#include "xmenu_private.h"


void pop_menu_show(Widget_t *parent, Widget_t *menu, int elem, bool above) {
    if (!childlist_has_child(menu->childlist)) return;
    Widget_t* view_port =  menu->childlist->childs[0];
    if (!view_port->childlist->elem) return;
    _configure_menu(parent, menu, elem, above);
    pop_widget_show_all(menu);
    int err = XGrabPointer(menu->app->dpy, DefaultRootWindow(parent->app->dpy), True,
                 ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
                 GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
    menu->app->hold_grab = menu;

    if (err) debug_print("Error grap pointer\n");
}

Widget_t* create_viewport(Widget_t *parent, int width, int height) {
    Widget_t *wid = create_widget(parent->app, parent, 0, 0, width, height);
    wid->scale.gravity = NONE;
    wid->flags &= ~USE_TRANSPARENCY;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, -1.0,1.0, CL_VIEWPORT);
    wid->adj = wid->adj_y;
    wid->func.adj_callback = _set_viewpoint;
    wid->func.expose_callback = _draw_viewslider;
    return wid;
}

Widget_t* create_menu(Widget_t *parent, int height) {

    int x1, y1;
    Window child;
    XTranslateCoordinates( parent->app->dpy, parent->widget, DefaultRootWindow(parent->app->dpy), 0, 0, &x1, &y1, &child );
    Widget_t *wid = create_window(parent->app, DefaultRootWindow(parent->app->dpy), x1, y1, 10, height, true);
    create_viewport(wid, 10, 5*height);
    Atom window_type = XInternAtom(wid->app->dpy, "_NET_WM_WINDOW_TYPE", False);
    Atom window_type_popup = XInternAtom(wid->app->dpy, "_NET_WM_WINDOW_TYPE_DROPDOWN_MENU", False);
    XChangeProperty(wid->app->dpy, wid->widget, window_type,
        XA_ATOM, 32, PropModeReplace, (unsigned char *) &window_type_popup, 1);
    Atom window_state = XInternAtom(wid->app->dpy, "_NET_WM_STATE", False);
    Atom window_state_modal = XInternAtom(wid->app->dpy, "_NET_WM_STATE_MODAL", False);
    XChangeProperty(wid->app->dpy, wid->widget, window_state,
        XA_ATOM, 32, PropModeReplace, (unsigned char *) &window_state_modal, 1);
    XSetTransientForHint(parent->app->dpy, wid->widget, parent->widget);
    wid->func.expose_callback = _draw_menu;
    wid->flags |= IS_POPUP;
    wid->scale.gravity = NONE;
    childlist_add_child(parent->childlist, wid);
    return wid;
}

Widget_t* menu_add_item(Widget_t *menu,const char * label) {
    Widget_t* view_port =  menu->childlist->childs[0];
    XWindowAttributes attrs;
    XGetWindowAttributes(menu->app->dpy, (Window)menu->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    int si = childlist_has_child(view_port->childlist);
    Widget_t *wid = create_widget(menu->app, view_port, 0, height*si, width, height);
    float max_value = view_port->adj->max_value+1.0;
    set_adjustment(view_port->adj,0.0, 0.0, 0.0, max_value,1.0, CL_VIEWPORT);
    wid->scale.gravity = MENUITEM;
    wid->flags &= ~USE_TRANSPARENCY;
    wid->label = label;
    wid->func.expose_callback = _draw_item;
    wid->func.enter_callback = transparent_draw;
    wid->func.leave_callback = transparent_draw;
    return wid;
}

Widget_t* menu_add_check_item(Widget_t *menu, const char * label) {
    Widget_t *wid = menu_add_item(menu, label);
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, 1.0,1.0, CL_TOGGLE);
    wid->adj = wid->adj_y;
    wid->func.expose_callback = _draw_check_item;
    wid->func.button_press_callback = _check_item_button_pressed;
    return wid;
}

void radio_item_set_active(Widget_t *w) {
    Widget_t * p = (Widget_t*) w->parent;
    int i = p->childlist->elem-1;
    for(;i>-1;i--) {
        Widget_t *wid = p->childlist->childs[i];
        if (wid->adj && wid->flags & IS_RADIO) {
            if (wid == w) adj_set_value(wid->adj_y, 1.0);
            else adj_set_value(wid->adj_y, 0.0);
        }
    }
}

Widget_t* menu_add_radio_item(Widget_t *menu, const char * label) {
    Widget_t *wid = menu_add_check_item(menu, label);
    wid->flags |= IS_RADIO;
    wid->func.expose_callback = _draw_check_item;
    wid->func.button_press_callback = _radio_item_button_pressed;
    radio_item_set_active(wid);
    return wid;
}


void menu_add_numeric_items(Widget_t *menu, int *imin, int *imax) {
    Widget_t* view_port =  menu->childlist->childs[0];
    XWindowAttributes attrs;
    XGetWindowAttributes(menu->app->dpy, (Window)menu->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    int si = childlist_has_child(view_port->childlist);
    float max_value = view_port->adj->max_value;
    Widget_t *wid;
    int i = (*imin);
    int o = (*imax)+1;
    for (;i<o;i++) {
        si = childlist_has_child(view_port->childlist);
        wid = create_widget(menu->app, view_port, 0, height*si, width, height);
        max_value += 1.0;
        set_adjustment(view_port->adj,0.0, 0.0, 0.0, max_value,1.0, CL_VIEWPORT);
        wid->scale.gravity = MENUITEM;
        wid->flags &= ~USE_TRANSPARENCY;
        snprintf(wid->input_label, 31, "%i",i);
        wid->label = wid->input_label;
        wid->func.expose_callback = _draw_item;
        wid->func.enter_callback = transparent_draw;
        wid->func.leave_callback = transparent_draw;
    }
}
