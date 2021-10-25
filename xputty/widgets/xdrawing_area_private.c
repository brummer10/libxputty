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


#include "xdrawing_area_private.h"


void _rounded_iframe(cairo_t *cr,float x, float y, float w, float h) {
    cairo_new_path (cr);
    float r = 20.0;
    cairo_move_to(cr, x+r,y);
    cairo_line_to(cr, x+w-r,y);
    cairo_curve_to(cr, x+w,y,x+w,y,x+w,y+r);
    cairo_line_to(cr, x+w,y+h-r);
    cairo_curve_to(cr, x+w,y+h,x+w,y+h,x+w-r,y+h);
    cairo_line_to(cr, x+r,y+h);
    cairo_curve_to(cr, x,y+h,x,y+h,x,y+h-r);
    cairo_line_to(cr, x,y+r);
    cairo_curve_to(cr, x,y,x,y,x+r,y);
}

void _draw_image(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width_t = attrs.width;
    int height_t = attrs.height;

    if (!w->image) {
        cairo_text_extents_t extents;
        use_base_color_scheme(w, get_color_state(w));
        cairo_set_font_size (w->crb, w->app->big_font/w->scale.ascale);
        cairo_text_extents(w->crb,"Missing Image" , &extents);
        cairo_move_to (w->crb, (w->width -extents.width)*0.5, (w->height - extents.height)*0.5);
        cairo_show_text(w->crb, "Missing Image");
        cairo_new_path (w->crb);

        cairo_set_line_width(w->crb,3);
        _rounded_iframe(w->crb, 5, 5, width_t-10, height_t-10);
        cairo_stroke(w->crb);
    }

    if (w->image) {
        int width = cairo_xlib_surface_get_width(w->image);
        int height = cairo_xlib_surface_get_height(w->image);
        double x = (double)width_t/(double)(width);
        double y = (double)height_t/(double)height;
        double x1 = (double)(width)/(double)width_t;
        double y1 = (double)height/(double)height_t;
        cairo_scale(w->crb, x,y);
        cairo_set_source_surface (w->crb, w->image, 0, 0);
        cairo_paint (w->crb);
        cairo_scale(w->crb, x1,y1);
    } 
}
