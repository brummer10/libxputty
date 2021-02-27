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


#include "xtabbox_private.h"


void _rounded_box(cairo_t *cr,float x, float y, float w, float h, float lsize) {
    cairo_new_path (cr);
    float r = 10.0;
    cairo_move_to(cr, x+lsize,y);
    cairo_line_to(cr, x+w,y);
    cairo_curve_to(cr, x+w,y,x+w,y,x+w,y);
    cairo_line_to(cr, x+w,y+h-r);
    cairo_curve_to(cr, x+w,y+h,x+w,y+h,x+w-r,y+h);
    cairo_line_to(cr, x+r,y+h);
    cairo_curve_to(cr, x,y+h,x,y+h,x,y+h-r);
    cairo_line_to(cr, x,y+r);
    cairo_curve_to(cr, x,y,x,y,x,y);
}

void _draw_tabbox(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width_t = attrs.width;
    int height_t = attrs.height;

    int tabsize = 1;
    int elem = w->childlist->elem;
    if (elem) tabsize = width_t/elem;
    int v = (int)adj_get_value(w->adj);

    cairo_new_path (w->crb);
    cairo_set_line_width(w->crb,1);
    use_frame_color_scheme(w, NORMAL_);
    _rounded_box(w->crb, 1, 21, width_t-2, height_t-22, (v+1)*tabsize);
    cairo_stroke(w->crb);

    cairo_text_extents_t extents;
    use_text_color_scheme(w, get_color_state(w));
    cairo_set_font_size (w->crb, w->app->normal_font/w->scale.ascale);
    int i = 0;
    int t = 0;
    for(;i<elem;i++) {
        Widget_t *wi = w->childlist->childs[i];
        if(v == i) {
            cairo_move_to (w->crb, t+1, 21);
            cairo_line_to(w->crb, t+1, 1);
            cairo_line_to(w->crb, t+tabsize-1, 1);
            cairo_line_to(w->crb, t+tabsize-1, 21);
            use_frame_color_scheme(w, NORMAL_);
            cairo_stroke(w->crb);
            use_text_color_scheme(w, ACTIVE_);
            widget_show_all(wi);
        } else {
            use_bg_color_scheme(w, ACTIVE_);
            cairo_rectangle(w->crb, t+2, 1, tabsize-4, 20);
            cairo_fill_preserve(w->crb);
            use_frame_color_scheme(w, NORMAL_);
            cairo_stroke(w->crb);
            use_text_color_scheme(w, INSENSITIVE_);
            widget_hide(wi);
        }

        cairo_text_extents(w->crb,"Äy" , &extents);
        cairo_move_to (w->crb, 5+t, 2+extents.height);
        cairo_show_text(w->crb, wi->label);
        cairo_new_path (w->crb);
        t += tabsize;
    }
}

void _draw_tab(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = (Widget_t*)w->parent;
    _draw_tabbox(p, NULL);
}

void _tab_button_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width_t = attrs.width;
    if (w->flags & HAS_POINTER) {
        XButtonEvent *xbutton = (XButtonEvent*)button_;
        if (xbutton->y < 20) {
            int tabsize = width_t;
            int elem = w->childlist->elem;
            if (elem) tabsize = width_t/elem;
            adj_set_value(w->adj,(float)(xbutton->x/tabsize));
            expose_widget(w);
        }
     }
   
}
