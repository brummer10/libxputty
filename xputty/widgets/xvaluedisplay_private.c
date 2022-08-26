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


#include "xvaluedisplay_private.h"


void _draw_valuedisplay(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width-2;
    int height = attrs.height-2;
    if (attrs.map_state != IsViewable) return;

    cairo_rectangle(w->crb,2.0, 2.0, width, height);

    if(w->state==0) {
        cairo_set_line_width(w->crb, 1.0);
        use_shadow_color_scheme(w, NORMAL_);
        cairo_fill_preserve(w->crb);
        use_frame_color_scheme(w, NORMAL_);
    } else if(w->state==1) {
        use_shadow_color_scheme(w, PRELIGHT_);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.5);
        use_frame_color_scheme(w, NORMAL_);
    } else if(w->state==2) {
        use_shadow_color_scheme(w, SELECTED_);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.0);
        use_frame_color_scheme(w, SELECTED_);
    } else if(w->state==3) {
        use_shadow_color_scheme(w, ACTIVE_);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.0);
        use_frame_color_scheme(w, ACTIVE_);
    } else if(w->state==4) {
        use_shadow_color_scheme(w, INSENSITIVE_);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.0);
        use_frame_color_scheme(w, INSENSITIVE_);
    }
    cairo_stroke(w->crb); 

    cairo_rectangle(w->crb,4.0, 4.0, width, height);
    cairo_stroke(w->crb);
    cairo_rectangle(w->crb,3.0, 3.0, width, height);
    cairo_stroke(w->crb);

    cairo_text_extents_t extents;

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

    
    use_text_color_scheme(w, get_color_state(w));
    float font_size = w->app->normal_font/w->scale.ascale;
    cairo_set_font_size (w->crb, font_size);
    cairo_text_extents(w->crb,s , &extents);
    cairo_move_to (w->crb, (width-extents.width)*0.5, (height+extents.height)*0.55);
    cairo_show_text(w->crb, s);
    cairo_new_path (w->crb);

}

void _draw_spinbox(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = (Widget_t*)w->parent;
    Widget_t *parent = (Widget_t*)p->parent;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width-2;
    int height = attrs.height-2;
    if (attrs.map_state != IsViewable) return;

    cairo_rectangle(w->crb,2.0, 2.0, width, height);
    use_bg_color_scheme(w, NORMAL_);
    cairo_fill_preserve(w->crb);
    use_text_color_scheme(w, NORMAL_);
    cairo_stroke(w->crb);

    cairo_text_extents_t extents;

    char s[64];
    const char* format[] = {"%.1f", "%.2f", "%.3f"};
    float value = adj_get_value(parent->adj);
    if (fabs(parent->adj->step)>0.99) {
        snprintf(s, 63,"%d",  (int) value);
    } else if (fabs(parent->adj->step)>0.09) {
        snprintf(s, 63, format[1-1], value);
    } else {
        snprintf(s, 63, format[2-1], value);
    }

    
    use_text_color_scheme(w, get_color_state(w));
    float font_size = w->app->normal_font/w->scale.ascale;
    cairo_set_font_size (w->crb, font_size);
    cairo_text_extents(w->crb,s , &extents);
    cairo_move_to (w->crb, (width-extents.width)*0.5, (height+extents.height)*0.55);
    cairo_show_text(w->crb, s);
    cairo_new_path (w->crb);
}

void _draw_buttons(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    cairo_rectangle(w->crb,0.0, 2.0, width, height-2);
    use_bg_color_scheme(w, NORMAL_);
    cairo_fill_preserve(w->crb);
    use_text_color_scheme(w, NORMAL_);
    cairo_stroke(w->crb);
    cairo_rectangle(w->crb,0.0, height/2, width, height-2);
    cairo_stroke(w->crb);
    use_text_color_scheme(w, get_color_state(w));
    float font_size = w->app->big_font/w->scale.ascale;
    cairo_set_font_size (w->crb, font_size);
    cairo_move_to (w->crb, 5,18);
    cairo_show_text(w->crb, "+");
    cairo_move_to (w->crb, 7,38);
    cairo_show_text(w->crb, "-"); 
}

void _buttons_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = (Widget_t*)w->parent;
    Widget_t *parent = (Widget_t*)p->parent;
    XButtonEvent *xbutton = (XButtonEvent*)button_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;
    if (xbutton->button == Button1) {
        if (xbutton->y > height/2) 
            adj_set_value(parent->adj, adj_get_value(parent->adj)-parent->adj->step);
        else
           adj_set_value(parent->adj, adj_get_value(parent->adj)+parent->adj->step);
        expose_widget(p);
    } else if (xbutton->button == Button4) {
        adj_set_value(parent->adj, adj_get_value(parent->adj)+parent->adj->step);
        expose_widget(p);
    } else if (xbutton->button == Button5) {
        adj_set_value(parent->adj, adj_get_value(parent->adj)-parent->adj->step);
        expose_widget(p);
    }
}

void _popup_spinbox(void *w_, void* button, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *spin_box = (Widget_t*)w->childlist->childs[0];
    int x1, y1;
    Window child;
    XTranslateCoordinates( w->app->dpy, w->widget, DefaultRootWindow(w->app->dpy), 0, 0, &x1, &y1, &child );
    XMoveWindow(spin_box->app->dpy,spin_box->widget,x1-10, y1-10);
    pop_widget_show_all(spin_box);
    int err = XGrabPointer(w->app->dpy, DefaultRootWindow(w->app->dpy), True,
                 ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
                 GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
    w->app->hold_grab = spin_box;
    if (err) debug_print("Error grap pointer\n");
}
