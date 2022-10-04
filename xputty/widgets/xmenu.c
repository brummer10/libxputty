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
#include "xslider.h"


void pop_menu_show(Widget_t *parent, Widget_t *menu, int elem, bool above) {
    if (!childlist_has_child(menu->childlist)) return;
    Widget_t* view_port =  menu->childlist->childs[0];
    if (!view_port->childlist->elem) return;
    _configure_menu(parent, menu, elem, above);
    pop_widget_show_all(menu);
    if (view_port->childlist->elem <= elem)
        widget_hide(menu->childlist->childs[1]);
    int err = XGrabPointer(menu->app->dpy, DefaultRootWindow(parent->app->dpy), True,
                 ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
                 GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
    menu->app->hold_grab = menu;

    if (err) debug_print("Error grap pointer\n");
}

void pop_submenu_show(Widget_t *parent, Widget_t *menu, int elem, bool above) {
    if (!childlist_has_child(menu->childlist)) return;
    Widget_t* view_port =  menu->childlist->childs[0];
    if (!view_port->childlist->elem) return;
    _configure_menu(parent, menu, elem, above);
    submenu_widget_show_all(menu);
    menu->app->submenu = menu;
}

Widget_t* create_viewport(Widget_t *parent, int width, int height) {
    Widget_t *wid = create_widget(parent->app, parent, 0, 0, width, height);
    wid->scale.gravity = NONE;
    wid->flags &= ~USE_TRANSPARENCY;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, -1.0,1.0, CL_VIEWPORT);
    wid->adj = wid->adj_y;
    wid->func.adj_callback = _set_viewpoint;
    return wid;
}

void set_active_radio_entry(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    int v = (int)adj_get_value(w->adj);
    Widget_t *menu = w->childlist->childs[0];
    Widget_t *view_port =  menu->childlist->childs[0];
    if (!childlist_has_child(view_port->childlist)) return;
    int i = view_port->childlist->elem-1;
    if (v > i || v < 0) return;
    Widget_t *wi = view_port->childlist->childs[v];
    for(;i>-1;i--) {
        Widget_t *wid = view_port->childlist->childs[i];
        if (wid->adj && wid->flags & IS_RADIO) {
            if (wid == wi) adj_set_value(wid->adj_y, 1.0);
            else adj_set_value(wid->adj_y, 0.0);
        }
    }
}

void set_active_radio_entry_num(Widget_t *w, int active) {
    Widget_t *menu = w->childlist->childs[0];
    Widget_t *view_port =  menu->childlist->childs[0];
    Widget_t *wid = view_port->childlist->childs[active];
    if (wid->adj && wid->flags & IS_RADIO)
        radio_item_set_active(wid);
}

Widget_t *add_menu(Widget_t *parent, const char * label,
                        int x, int y, int width, int height) {
    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
    wid->label = label;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, -1.0,1.0, CL_NONE);
    wid->adj = wid->adj_y;
    wid->scale.gravity = NONE;
    wid->state = 0;
    wid->func.adj_callback = set_active_radio_entry;
    wid->func.expose_callback = _draw_menu_label;
    wid->func.enter_callback = _check_menu_state;
    wid->func.leave_callback = transparent_draw;
    wid->func.button_release_callback = _menu_released;

    Widget_t *menu = create_menu(wid, 25);
    menu->func.button_release_callback = _menu_entry_released;

    return wid;
}

Widget_t *add_menubar(Widget_t *parent, const char * label,
                        int x, int y, int width, int height) {
    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
    wid->scale.gravity = NORTHEAST;
    wid->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    return wid;
}

Widget_t *menubar_add_menu(Widget_t *parent, const char * label) {
    cairo_text_extents_t extents;
    cairo_text_extents(parent->crb,label , &extents);
    int width = (int)extents.width+20;
    XWindowAttributes attrs;
    XGetWindowAttributes(parent->app->dpy, (Window)parent->widget, &attrs);
    int height = attrs.height;
    int x = 0;
    int i = parent->childlist->elem-1;
    for(;i>-1;i--) {
        Widget_t *w = parent->childlist->childs[i];
        XGetWindowAttributes(parent->app->dpy, (Window)w->widget, &attrs);
        x += attrs.width;
    }
    Widget_t *wid = add_menu(parent, label, x, 0, width, height);
    return wid;
}

Widget_t *menu_add_entry(Widget_t *wid, const char  * label) {
    Widget_t *menu = wid->childlist->childs[0];
    Widget_t *item = menu_add_accel_item(menu,label);
    float max_value = wid->adj->max_value+1.0;
    set_adjustment(wid->adj,0.0, max_value, 0.0, max_value,1.0, CL_NONE);
    
    return item;
}

Widget_t *menu_add_radio_entry(Widget_t *wid, const char  * label) {
    Widget_t *menu = wid->childlist->childs[0];
    Widget_t *item = menu_add_radio_item(menu,label);
    float max_value = wid->adj->max_value+1.0;
    set_adjustment(wid->adj,0.0, max_value, 0.0, max_value,1.0, CL_NONE);
    
    return item;
}

Widget_t *menu_add_check_entry(Widget_t *wid, const char  * label) {
    Widget_t *menu = wid->childlist->childs[0];
    Widget_t *item = menu_add_check_item(menu,label);
    float max_value = wid->adj->max_value+1.0;
    set_adjustment(wid->adj,0.0, max_value, 0.0, max_value,1.0, CL_NONE);
    
    return item;
}

Widget_t *menu_add_accel_check_entry(Widget_t *wid, const char  * label) {
    Widget_t *menu = wid->childlist->childs[0];
    Widget_t *item = menu_add_check_item(menu,label);
    item->func.expose_callback = _draw_accel_check_item;
    float max_value = wid->adj->max_value+1.0;
    set_adjustment(wid->adj,0.0, max_value, 0.0, max_value,1.0, CL_NONE);
    
    return item;
}

Widget_t* menu_add_value_entry(Widget_t *wid,const char * label) {
    Widget_t *menu = wid->childlist->childs[0];
    Widget_t* view_port =  menu->childlist->childs[0];
    XWindowAttributes attrs;
    XGetWindowAttributes(menu->app->dpy, (Window)menu->widget, &attrs);
    int width = attrs.width;
    int height = 40;
    int si = childlist_has_child(view_port->childlist);
    Widget_t *item = create_widget(menu->app, view_port, 0, height*si, width, height);
    float max_value = view_port->adj->max_value+1.0;
    set_adjustment(view_port->adj,0.0, 0.0, 0.0, max_value,1.0, CL_VIEWPORT);
    item->scale.gravity = MENUITEM;
    item->flags &= ~USE_TRANSPARENCY;
    item->flags |= FAST_REDRAW;
    item->label = label;
    item->func.expose_callback = _draw_item;
    item->func.enter_callback = transparent_draw;
    item->func.leave_callback = transparent_draw;
    item->adj_y = add_adjustment(item,0.0, 0.0, 0.0, 1.0,0.01, CL_CONTINUOS);
    item->adj = item->adj_y;
    item->func.expose_callback = _draw_value_item;
    item->func.button_release_callback = _value_item_released;
    return item;
}

Widget_t *menu_add_submenu(Widget_t *w, const char  * label) {
    float max_value2 = w->adj->max_value+1.0;
    set_adjustment(w->adj,0.0, max_value2, 0.0, max_value2,1.0, CL_NONE);
    Widget_t *menu = w->childlist->childs[0];
    Widget_t* view_port =  menu->childlist->childs[0];
    XWindowAttributes attrs;
    XGetWindowAttributes(menu->app->dpy, (Window)menu->widget, &attrs);
    int width = attrs.width;
    int height = menu->scale.init_height;
    int si = childlist_has_child(view_port->childlist);
    Widget_t *wid = create_widget(menu->app, view_port, 0, height*si, width, height);
    float max_value1 = view_port->adj->max_value+1.0;
    set_adjustment(view_port->adj,0.0, 0.0, 0.0, max_value1,1.0, CL_VIEWPORT);
    wid->scale.gravity = MENUITEM;
    wid->flags &= ~USE_TRANSPARENCY;
    wid->flags |= FAST_REDRAW;
    wid->label = label;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, -1.0,1.0, CL_NONE);
    wid->adj = wid->adj_y;
    float max_value = wid->adj->max_value+1.0;
    set_adjustment(wid->adj,0.0, max_value, 0.0, max_value,1.0, CL_NONE);
    wid->func.adj_callback = set_active_radio_entry;
    wid->func.expose_callback = _draw_submenu;
    wid->func.enter_callback = _enter_submenu;
    wid->func.leave_callback = _leave_submenu;
    Widget_t* submenu = create_menu(wid, 25);
    submenu->flags |= IS_SUBMENU;

    submenu->func.button_release_callback = _menu_entry_released;
    return wid;
}

Widget_t *cmenu_add_submenu(Widget_t *w, const char  * label) {
    Widget_t *menu = w;
    Widget_t* view_port =  menu->childlist->childs[0];
    XWindowAttributes attrs;
    XGetWindowAttributes(menu->app->dpy, (Window)menu->widget, &attrs);
    int width = attrs.width;
    int height = menu->scale.init_height;
    int si = childlist_has_child(view_port->childlist);
    Widget_t *wid = create_widget(menu->app, view_port, 0, height*si, width, height);
    float max_value1 = view_port->adj->max_value+1.0;
    set_adjustment(view_port->adj,0.0, 0.0, 0.0, max_value1,1.0, CL_VIEWPORT);
    wid->scale.gravity = MENUITEM;
    wid->flags &= ~USE_TRANSPARENCY;
    wid->flags |= FAST_REDRAW;
    wid->label = label;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, -1.0,1.0, CL_NONE);
    wid->adj = wid->adj_y;
    float max_value = wid->adj->max_value+1.0;
    set_adjustment(wid->adj,0.0, max_value, 0.0, max_value,1.0, CL_NONE);
    wid->func.adj_callback = set_active_radio_entry;
    wid->func.expose_callback = _draw_submenu;
    wid->func.enter_callback = _enter_submenu;
    wid->func.leave_callback = _leave_submenu;
    Widget_t* submenu = create_menu(wid, 25);
    submenu->flags |= IS_SUBMENU;

    submenu->func.button_release_callback = _menu_entry_released;
    return wid;
}

Widget_t* create_menu(Widget_t *parent, int height) {

    int x1, y1;
    Window child;
    XTranslateCoordinates( parent->app->dpy, parent->widget, DefaultRootWindow(parent->app->dpy), 0, 0, &x1, &y1, &child );
    Widget_t *wid = create_window(parent->app, DefaultRootWindow(parent->app->dpy), x1, y1, 10, height);
    memcpy(wid->color_scheme, parent->color_scheme, sizeof (struct XColor_t));
    create_viewport(wid, 10, 5*height);

    XSetWindowAttributes attributes;
    attributes.override_redirect = True;
    XChangeWindowAttributes(parent->app->dpy, wid->widget, CWOverrideRedirect, &attributes);

    Atom window_type = XInternAtom(wid->app->dpy, "_NET_WM_WINDOW_TYPE", False);
    Atom window_type_popup = XInternAtom(wid->app->dpy, "_NET_WM_WINDOW_TYPE_DROPDOWN_MENU", False);
    XChangeProperty(wid->app->dpy, wid->widget, window_type,
        XA_ATOM, 32, PropModeReplace, (unsigned char *) &window_type_popup,1 );

    Atom window_state = XInternAtom(wid->app->dpy, "_NET_WM_STATE", False);
    Atom window_state_modal = XInternAtom(wid->app->dpy, "_NET_WM_STATE_MODAL", False);
    XChangeProperty(wid->app->dpy, wid->widget, window_state,
        XA_ATOM, 32, PropModeReplace, (unsigned char *) &window_state_modal, 1);

    XSetTransientForHint(parent->app->dpy,wid->widget,parent->widget);
    wid->func.expose_callback = _draw_menu;
    wid->flags |= IS_POPUP;
    wid->scale.gravity = NONE;
    childlist_add_child(parent->childlist, wid);

    Widget_t *slider = add_vslider(wid, "", 0, 0, 10, height);
    slider->func.expose_callback = _draw_menu_slider;
    set_adjustment(slider->adj_y,0.0, 0.0, 0.0, 1.0,0.0085, CL_VIEWPORTSLIDER);
    slider->adj = slider->adj_y;
    slider->func.value_changed_callback = _set_menu_viewpoint;
    slider->scale.gravity = NORTHWEST;
    slider->flags &= ~USE_TRANSPARENCY;
    slider->flags |= NO_AUTOREPEAT | NO_PROPAGATE;

    return wid;
}

Widget_t* menu_add_item(Widget_t *menu,const char * label) {
    Widget_t* view_port =  menu->childlist->childs[0];
    XWindowAttributes attrs;
    XGetWindowAttributes(menu->app->dpy, (Window)menu->widget, &attrs);
    int width = attrs.width;
    int height = menu->scale.init_height;
    int si = childlist_has_child(view_port->childlist);
    Widget_t *wid = create_widget(menu->app, view_port, 0, height*si, width, height);
    float max_value = view_port->adj->max_value+1.0;
    set_adjustment(view_port->adj,0.0, 0.0, 0.0, max_value,1.0, CL_VIEWPORT);
    wid->scale.gravity = MENUITEM;
    wid->flags &= ~USE_TRANSPARENCY;
    wid->flags |= FAST_REDRAW;
    wid->label = label;
    wid->func.expose_callback = _draw_item;
    wid->func.enter_callback = transparent_draw;
    wid->func.leave_callback = transparent_draw;
    return wid;
}

void menu_remove_item(Widget_t *menu, Widget_t *item) {
    Widget_t* view_port =  menu->childlist->childs[0];
    float max_value = view_port->adj->max_value-1.0;
    destroy_widget(item,menu->app);
    set_adjustment(view_port->adj,0.0, 0.0, 0.0, max_value,1.0, CL_VIEWPORT);
    XMoveWindow(menu->app->dpy,view_port->widget,0.0, 0.0);
}

Widget_t* menu_add_accel_item(Widget_t *menu,const char * label) {
    Widget_t *wid = menu_add_item(menu, label);
    wid->func.expose_callback = _draw_accel_item;
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
            if (wid == w) {
                adj_set_value(wid->adj_y, 1.0);
            } else {
                adj_set_value(wid->adj_y, 0.0);
            }
            wid->state=0;
        }
    }
}

Widget_t* menu_add_radio_item(Widget_t *menu, const char * label) {
    Widget_t *wid = menu_add_check_item(menu, label);
    wid->flags |= IS_RADIO;
    wid->func.expose_callback = _draw_check_item;
    wid->func.button_press_callback = _radio_item_button_pressed;
    //radio_item_set_active(wid);
    return wid;
}

Widget_t* menu_add_value_item(Widget_t *menu,const char * label) {
    Widget_t *wid = menu_add_item(menu, label);
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, 1.0,0.01, CL_CONTINUOS);
    wid->adj = wid->adj_y;
    wid->func.expose_callback = _draw_value_item;
    wid->func.button_release_callback = _value_item_released;
    return wid;
}

void menu_add_numeric_items(Widget_t *menu, int *imin, int *imax) {
    Widget_t* view_port =  menu->childlist->childs[0];
    XWindowAttributes attrs;
    XGetWindowAttributes(menu->app->dpy, (Window)menu->widget, &attrs);
    int width = attrs.width;
    int height = menu->scale.init_height;
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
        wid->flags |= FAST_REDRAW;
        snprintf(wid->input_label, 31, "%i",i);
        wid->label = wid->input_label;
        wid->func.expose_callback = _draw_item;
        wid->func.enter_callback = transparent_draw;
        wid->func.leave_callback = transparent_draw;
    }
}
