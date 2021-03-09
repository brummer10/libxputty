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


#include "xwaveview_private.h"


void _rounded_view(cairo_t *cr,float x, float y, float w, float h, float lsize) {
    cairo_new_path (cr);
    float r = 20.0;
    cairo_move_to(cr, x+lsize+r,y);
    cairo_line_to(cr, x+w-r,y);
    cairo_curve_to(cr, x+w,y,x+w,y,x+w,y+r);
    cairo_line_to(cr, x+w,y+h-r);
    cairo_curve_to(cr, x+w,y+h,x+w,y+h,x+w-r,y+h);
    cairo_line_to(cr, x+r,y+h);
    cairo_curve_to(cr, x,y+h,x,y+h,x,y+h-r);
    cairo_line_to(cr, x,y+r);
    cairo_curve_to(cr, x,y,x,y,x+r,y);
}

void _draw_waveview(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    WaveView_t *wave_view = (WaveView_t*)w->private_struct;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width_t = attrs.width;
    int height_t = attrs.height;
    int half_height_t = height_t/2;

    cairo_text_extents_t extents;
    cairo_set_font_size (w->crb, w->app->normal_font/w->scale.ascale);
    cairo_text_extents(w->crb,w->label , &extents);

    cairo_set_line_width(w->crb,2);
    use_bg_color_scheme(w, NORMAL_);
    _rounded_view(w->crb, 2, 5, width_t-4, height_t-7, extents.width+15);
    cairo_fill_preserve(w->crb);
    use_frame_color_scheme(w, NORMAL_);
    cairo_stroke(w->crb);
    cairo_move_to(w->crb,2,half_height_t);
    cairo_line_to(w->crb, width_t-4, half_height_t);
    cairo_stroke(w->crb);

    use_text_color_scheme(w, get_color_state(w));
    cairo_move_to (w->crb, 30, extents.height);
    cairo_show_text(w->crb, w->label);
    cairo_new_path (w->crb);

    if (wave_view->size<1) return;
    float step = (float)(width_t-10)/(float)wave_view->size+1;
    float lstep = (float)(half_height_t-10.0);
    cairo_set_line_width(w->cr,2);
    use_fg_color_scheme(w, NORMAL_);
    int i = 0;
    for (;i<wave_view->size;i++) {
        cairo_line_to(w->crb, (float)(i+0.5)*step,(float)(half_height_t)+ -wave_view->wave[i]*lstep);
    }
    cairo_line_to(w->crb, width_t, half_height_t);
    cairo_line_to(w->crb, 2, half_height_t);
    cairo_close_path(w->crb);
    use_light_color_scheme(w, NORMAL_);
    cairo_fill_preserve(w->crb);
    use_fg_color_scheme(w, NORMAL_);
    cairo_stroke(w->crb);
    i = 0;
    for (;i<wave_view->size;i++) {
        cairo_line_to(w->crb, (float)(i+0.5)*step,(float)(half_height_t)+ wave_view->wave[i]*lstep);
    }
    cairo_line_to(w->crb, width_t, half_height_t);
    cairo_line_to(w->crb, 2, half_height_t);
    cairo_close_path(w->crb);
    use_light_color_scheme(w, NORMAL_);
    cairo_fill_preserve(w->crb);
    use_fg_color_scheme(w, NORMAL_);
    cairo_stroke(w->crb);
}
