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


#include "xmenu_private.h"


int _menu_remove_low_dash(char *str) {

    char *src;
    char *dst;
    int i = 0;
    int r = 0;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != '_') {
            dst++;
        } else {
            r = i;
        }
        i++;
    }
    *dst = '\0';
    return r;
}

void _draw_menu_label(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int height = attrs.height;

    cairo_text_extents_t extents;
    use_text_color_scheme(w, get_color_state(w));
    cairo_set_font_size (w->crb, w->app->normal_font/w->scale.ascale);
    cairo_text_extents(w->crb, "A", &extents);
    double lh = extents.height;

    if (strstr(w->label, "_")) {
        cairo_text_extents(w->crb, "--", &extents);
        double underline = extents.width;
        strncpy(w->input_label,w->label, sizeof(w->input_label)-1);
        int pos = _menu_remove_low_dash(w->input_label);
        int len = strlen(w->input_label);
        cairo_text_extents(w->crb,w->input_label , &extents);
        int set_line = (extents.width/len) * pos;
        cairo_move_to (w->crb, 10, (height+lh)*0.5);
        cairo_show_text(w->crb, w->input_label);
        cairo_set_line_width(w->crb, 1.0);
        cairo_move_to (w->crb, 10 + set_line, (height+lh)*0.55);
        cairo_line_to(w->crb,10 + set_line + underline, (height+lh)*0.55);
        cairo_stroke(w->crb);
    } else {
        cairo_text_extents(w->crb,w->label , &extents);
        cairo_move_to (w->crb, 10, (height+lh)*0.5);
        cairo_show_text(w->crb, w->label);
    }

    cairo_new_path (w->crb);
}

void _check_menu_state(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *parent = w->parent;
    int i = parent->childlist->elem-1;
    for(;i>-1;i--) {
        Widget_t *wid = parent->childlist->childs[i];
        if (childlist_has_child(wid->childlist)) {
            if ((wid->childlist->childs[0] == w->app->hold_grab) && (wid !=w)) {
                XUngrabPointer(w->app->dpy,CurrentTime);
                widget_hide(w->app->hold_grab);
                w->app->hold_grab = NULL;
                if (w->app->submenu) {
                    widget_hide(w->app->submenu);
                    w->app->submenu = NULL;
                }
                pop_menu_show(w, w->childlist->childs[0], 6, true);
                break;
            }
        }
    }
    transparent_draw(w_, user_data);
}

void _menu_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XButtonEvent *xbutton = (XButtonEvent*)button_;
    if (w->flags & HAS_POINTER && xbutton->button == Button1) {
        pop_menu_show(w, w->childlist->childs[0], 6, true);
    }
}

void _menu_entry_released(void *w_, void* item_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t * bar = NULL;
    int i = w->app->childlist->elem-1;
    for(;i>-1;i--) {
        Widget_t *wid = w->app->childlist->childs[i];
        if (wid == w) {
            bar = w->app->childlist->childs[i-1];
            int old_value = (int)adj_get_value(bar->adj);
            adj_set_value(bar->adj, (float)*(int*)item_);
            if (old_value == *(int*)item_) {
                bar->func.value_changed_callback(bar, NULL);
            }
            break;
        }
    }    
}

void _draw_menu_slider(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    int v = (int)w->adj->max_value;
    if (!v) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;
    float sliderstate = adj_get_state(w->adj);
    use_bg_color_scheme(w, get_color_state(w));
    cairo_rectangle(w->crb, 0,0,width,height);
    cairo_fill_preserve(w->crb);
    use_shadow_color_scheme(w, NORMAL_);
    cairo_fill(w->crb);
    use_bg_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, 0,(height-10)*sliderstate,width,10);
    cairo_fill(w->crb);
}

void _draw_menu(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    use_bg_color_scheme(w, get_color_state(w));
    cairo_paint (w->crb);
}

void _draw_submenu(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    use_base_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, 0, 0, width , height);
    if(w->state==1) {
        use_base_color_scheme(w, PRELIGHT_);
    } else if(w->state==2) {
        use_base_color_scheme(w, SELECTED_);
    } else if(w->state==3) {
        use_base_color_scheme(w, ACTIVE_);
    }
    cairo_fill_preserve(w->crb);
    cairo_set_line_width(w->crb, 1.0);
    use_frame_color_scheme(w, PRELIGHT_);
    cairo_stroke(w->crb); 

    cairo_text_extents_t extents;
    /** show label **/
    use_text_color_scheme(w, get_color_state(w));
    cairo_set_font_size (w->crb, w->app->normal_font/w->scale.ascale);

    if (strstr(w->label, "_")) {
        cairo_text_extents(w->crb, "--", &extents);
        double underline = extents.width;
        strncpy(w->input_label,w->label, sizeof(w->input_label)-1);
        int pos = _menu_remove_low_dash(w->input_label);
        int len = strlen(w->input_label);
        cairo_text_extents(w->crb,w->input_label , &extents);
        int set_line = (extents.width/len) * pos;
        cairo_move_to (w->crb, 20, (height+extents.height)*0.5);
        cairo_show_text(w->crb, w->input_label);
        cairo_set_line_width(w->crb, 1.0);
        cairo_move_to (w->crb, 20 + set_line, (height+extents.height)*0.55);
        cairo_line_to(w->crb,20 + set_line + underline, (height+extents.height)*0.55);
        cairo_stroke(w->crb);
    } else {
        cairo_text_extents(w->crb,w->label , &extents);
        cairo_move_to (w->crb, 20, (height+extents.height)*0.5);
        cairo_show_text(w->crb, w->label);
    }
    cairo_move_to (w->crb, (width-15), (height+extents.height)*0.5);
    cairo_show_text(w->crb, ">");
    cairo_new_path (w->crb);
}

void _enter_submenu(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    if (childlist_has_child(w->childlist)) {
        if (w->app->submenu) {
            if (w->app->submenu != w->childlist->childs[0]) {
                widget_hide(w->app->submenu);
                w->app->submenu = NULL;
            }
        }
        pop_submenu_show(w, w->childlist->childs[0], 6, false);
    }
    transparent_draw(w_, user_data);
}

void _leave_submenu(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;

    if (!w->data) {
        XCrossingEvent notify;
        memset(&notify, 0, sizeof(notify));
        notify.type = LeaveNotify;
        notify.display = w->app->dpy;
        notify.send_event = True;
        notify.subwindow = w->widget;
        notify.window = w->widget;
        notify.x = 1;
        notify.y = 1;
        notify.same_screen = True;
        notify.focus = False;
        XSendEvent(w->app->dpy, w->widget,True,LeaveWindowMask, (XEvent*)&notify);

        w->data = 1;
        return;
    }
    w->data = 0;
    if (childlist_has_child(w->childlist)) {
        Widget_t *parent = (Widget_t*) w->parent;
        int i=0;
        for(;i<parent->childlist->elem;i++) {
            if (parent->childlist->childs[i]->flags & HAS_FOCUS) {
                widget_hide(w->childlist->childs[0]);
                if (w->app->submenu == w->childlist->childs[0])
                    w->app->submenu = NULL;
                break;
            }
        }
        
    }
    transparent_draw(w_, user_data);
}

void _draw_item(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    use_base_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, 0, 0, width , height);
    if(w->state==1) {
        use_base_color_scheme(w, PRELIGHT_);
    } else if(w->state==2) {
        use_base_color_scheme(w, SELECTED_);
    } else if(w->state==3) {
        use_base_color_scheme(w, ACTIVE_);
    }
    cairo_fill_preserve(w->crb);
    cairo_set_line_width(w->crb, 1.0);
    use_frame_color_scheme(w, PRELIGHT_);
    cairo_stroke(w->crb);

    cairo_text_extents_t extents;
    /** show label **/
    use_text_color_scheme(w, get_color_state(w));
    cairo_set_font_size (w->crb, w->app->normal_font/w->scale.ascale);
    cairo_text_extents(w->crb,w->label , &extents);
    cairo_move_to (w->crb, 20, (height+extents.height)*0.5);
    cairo_show_text(w->crb, w->label);
    cairo_new_path (w->crb);
}

void _draw_value_item(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    use_base_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, 0, 0, width , height);
    if(w->state==1) {
        use_base_color_scheme(w, PRELIGHT_);
    } else if(w->state==2) {
        use_base_color_scheme(w, SELECTED_);
    } else if(w->state==3) {
        use_base_color_scheme(w, ACTIVE_);
    }
    cairo_fill_preserve(w->crb);
    cairo_set_line_width(w->crb, 1.0);
    use_frame_color_scheme(w, PRELIGHT_);
    cairo_stroke(w->crb);
    use_text_color_scheme(w, get_color_state(w));
    cairo_text_extents_t extents;
    /** show value on the item**/
    char s[64];
    const char* format[] = {"%.1f", "%.2f", "%.3f"};
    float value = adj_get_value(w->adj);
    if (fabs(w->adj->step)>0.99) {
        snprintf(s, 63,"%d",  (int) value);
    } else if (fabs(w->adj->step)>0.09) {
        snprintf(s, 63, format[1-1], value);
    } else {
        snprintf(s, 63, format[2-1], value);
    }
    cairo_set_font_size (w->crb, w->app->normal_font/w->scale.ascale);
    cairo_text_extents(w->crb, s, &extents);
    cairo_move_to (w->crb, 20, (height+extents.height)*0.5);
    cairo_show_text(w->crb, s);
    cairo_new_path (w->crb);
}

void _draw_accel_item(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    use_base_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, 0, 0, width , height);
    if(w->state==1) {
        use_base_color_scheme(w, PRELIGHT_);
    } else if(w->state==2) {
        use_base_color_scheme(w, SELECTED_);
    } else if(w->state==3) {
        use_base_color_scheme(w, ACTIVE_);
    }
    cairo_fill_preserve(w->crb);
    cairo_set_line_width(w->crb, 1.0);
    use_frame_color_scheme(w, PRELIGHT_);
    cairo_stroke(w->crb); 

    cairo_text_extents_t extents;
    /** show label **/
    use_text_color_scheme(w, get_color_state(w));
    cairo_set_font_size (w->crb, w->app->normal_font/w->scale.ascale);

    if (strstr(w->label, "_")) {
        cairo_text_extents(w->crb, "--", &extents);
        double underline = extents.width;
        strncpy(w->input_label,w->label, sizeof(w->input_label)-1);
        int pos = _menu_remove_low_dash(w->input_label);
        int len = strlen(w->input_label);
        cairo_text_extents(w->crb,w->input_label , &extents);
        int set_line = (extents.width/len) * pos;
        cairo_move_to (w->crb, 20, (height+extents.height)*0.5);
        cairo_show_text(w->crb, w->input_label);
        cairo_set_line_width(w->crb, 1.0);
        cairo_move_to (w->crb, 20 + set_line, (height+extents.height)*0.55);
        cairo_line_to(w->crb,20 + set_line + underline, (height+extents.height)*0.55);
        cairo_stroke(w->crb);
    } else {
        cairo_text_extents(w->crb,w->label , &extents);
        cairo_move_to (w->crb, 20, (height+extents.height)*0.5);
        cairo_show_text(w->crb, w->label);
    }
    cairo_new_path (w->crb);
}

void _draw_accel_check_item(void *w_, void* user_data) {
    _draw_accel_item(w_, user_data);
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int height = attrs.height;
    if (w->flags & IS_RADIO) {
        cairo_arc(w->crb, height/3, height/2, height/6, 0, 2 * M_PI );
    } else {
        cairo_rectangle(w->crb, height/6, height/3, height/3 , height/3);
    }
    use_shadow_color_scheme(w, get_color_state(w));
    cairo_fill(w->crb);
    if ((int) w->adj_y->value) {
        if (w->flags & IS_RADIO) {
            cairo_arc(w->crb, height/3, height/2, height/6-2, 0, 2 * M_PI );
        } else {
            cairo_rectangle(w->crb, height/6+1, height/3+1, height/3-2 , height/3-2);
        }
        use_fg_color_scheme(w, ACTIVE_);
        cairo_fill(w->crb);
    }
}

void _draw_check_item(void *w_, void* user_data) {
    _draw_item(w_, user_data);
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int height = attrs.height;
    if (w->flags & IS_RADIO) {
        cairo_arc(w->crb, height/3, height/2, height/6, 0, 2 * M_PI );
    } else {
        cairo_rectangle(w->crb, height/6, height/3, height/3 , height/3);
    }
    use_shadow_color_scheme(w, get_color_state(w));
    cairo_fill(w->crb);
    if ((int) w->adj_y->value) {
        if (w->flags & IS_RADIO) {
            cairo_arc(w->crb, height/3, height/2, height/6-2, 0, 2 * M_PI );
        } else {
            cairo_rectangle(w->crb, height/6+1, height/3+1, height/3-2 , height/3-2);
        }
        use_fg_color_scheme(w, ACTIVE_);
        cairo_fill(w->crb);
    }
}

void _set_viewpoint(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *menu = (Widget_t*)w->parent;
    Widget_t* slider =  menu->childlist->childs[1];
    int v = (int)max(0,adj_get_value(w->adj));
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->childlist->childs[0]->widget, &attrs);
    int height = attrs.height;
    XMoveWindow(w->app->dpy,w->widget,0, -height*v);
    adj_set_state(slider->adj,adj_get_state(w->adj));
}

void _set_menu_viewpoint(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *menu = (Widget_t*)w->parent;
    Widget_t*view_port =  menu->childlist->childs[0];
    adj_set_state(view_port->adj,adj_get_state(w->adj));
}

void _check_item_button_pressed(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_FOCUS) {
        float value = w->adj_y->value ? 0.0 : 1.0;
        adj_set_value(w->adj_y, value);
    }
}

void _radio_item_button_pressed(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_FOCUS) {
        radio_item_set_active(w);
    }
}

void _value_item_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    expose_widget(w);
}

void _configure_menu(Widget_t *parent, Widget_t *menu, int elem, bool above) {
    Widget_t* view_port =  menu->childlist->childs[0];
    Widget_t *slider =  menu->childlist->childs[1];
    if (!view_port->childlist->elem) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(menu->app->dpy, (Window)view_port->childlist->childs[0]->widget, &attrs);
    int height = attrs.height;
    int x1, y1;
    int posy = (above) ? parent->height : 0;
    int posx = (above) ? 0 : parent->width ;
    Window child;
    XTranslateCoordinates( parent->app->dpy, parent->widget, DefaultRootWindow(parent->app->dpy), posx, posy, &x1, &y1, &child );
    int item_width = 1.0;
    cairo_text_extents_t extents;
    int i = view_port->childlist->elem-1;
    set_adjustment(view_port->adj,0.0, view_port->adj->value, 0.0, i-(elem-1),1.0, CL_VIEWPORT);
    bool is_not_scrolable = false;
    if(view_port->childlist->elem <= elem) {
        elem = view_port->childlist->elem;
        is_not_scrolable = true;
    }
    for(;i>-1;i--) {
        Widget_t *w = view_port->childlist->childs[i];
        cairo_set_font_size (w->crb, w->app->normal_font/w->scale.ascale);
        cairo_text_extents(w->crb,w->label , &extents);
        
        item_width = max(item_width, (int)extents.width+40);
        if(is_not_scrolable) w->scale.gravity = NORTHEAST;
    }
    if(above) {
        if(item_width<parent->width)item_width = parent->width;
    }
    slider->adj->step = max(0.0,1.0/(view_port->childlist->elem-elem));
    adj_set_scale(slider->adj, ((float)view_port->childlist->elem/(float)elem)/25.0);
    int snum = DefaultScreen(parent->app->dpy);
    int screen_height = DisplayHeight(parent->app->dpy, snum);
    if (y1+(height*elem) > screen_height) y1 = y1-((height*elem)+parent->height);
    XResizeWindow (menu->app->dpy, menu->widget, item_width, height*elem);
    XResizeWindow (view_port->app->dpy, view_port->widget, item_width, height*view_port->childlist->elem);
    XMoveWindow(menu->app->dpy,slider->widget,item_width-10, 0);
    XResizeWindow(menu->app->dpy,slider->widget,10,height*elem);
    XMoveWindow(menu->app->dpy,menu->widget,x1, y1);
}
