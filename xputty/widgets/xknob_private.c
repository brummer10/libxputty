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


#include "xknob_private.h"

static void _show_label(Widget_t *w, int width, int height) {
    use_text_color_scheme(w, get_color_state(w));
    cairo_text_extents_t extents;
    /** show label below the knob**/
    cairo_set_font_size (w->crb, w->app->normal_font/w->scale.ascale);
    cairo_text_extents(w->crb,w->label , &extents);
    cairo_move_to (w->crb, (width*0.5)-(extents.width/2), height-(extents.height/4));
    cairo_show_text(w->crb, w->label);
    cairo_new_path (w->crb);
}

void _draw_image_knob(Widget_t *w, int width_t, int height_t) {
    int width, height;
    os_get_surface_size(w->image, &width, &height);
    double x = (double)width_t/(double)height;
    double y = (double)height/(double)width_t;
    double knobstate = adj_get_state(w->adj_y);
    int findex = (int)(((width/height)-1) * knobstate);
    int posx = 0;
    int posy = (height_t/2 - ((height*x)/2));
    if (width_t > height_t) {
        x = (double)height_t/(double)height;
        y = (double)height/(double)height_t;
        posx = (width_t/2 -((height*x)/2));
        posy = 0;
    }
    cairo_save(w->crb);
    cairo_scale(w->crb, x,x);
    cairo_translate(w->crb, posx * ((1-x)/x), posy * ((1-x)/x));
    cairo_set_source_surface (w->crb, w->image, -height*findex + posx, posy);
    cairo_rectangle(w->crb, posx, posy, height, height);
    cairo_fill(w->crb);
    cairo_scale(w->crb, y,y);
    cairo_restore(w->crb);
}

void _draw_knob_image(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    _draw_image_knob(w, w->width, w->height);
    _show_label(w, w->width-2, w->height-2);
}

void _draw_knob(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Metrics_t metrics;
    os_get_window_metrics(w, &metrics);
    int width = metrics.width-2;
    int height = metrics.height - (w->app->small_font + 7);
    if (!metrics.visible) return;

    const double scale_zero = 20 * (M_PI/180); // defines "dead zone" for knobs
    int arc_offset = 0;
    int knob_x = 0;
    int knob_y = 0;

    int grow = (width > height) ? height:width;
    knob_x = grow-1;
    knob_y = grow-1;
    /** get values for the knob **/

    const int knobx1 = width* 0.5;

    const int knoby1 = height * 0.5;
    if (w->image) {
        _draw_image_knob(w, width, height);
    } else {

        const double knobstate = adj_get_state(w->adj_y);
        const double angle = scale_zero + knobstate * 2 * (M_PI - scale_zero);

        const double pointer_off =knob_x/6;
        const double radius = min(knob_x-pointer_off, knob_y-pointer_off) / 2;

        const double add_angle = 90 * (M_PI / 180.);

        // base
        use_base_color_scheme(w, INSENSITIVE_);
        cairo_set_line_width(w->crb,  w->app->small_font/w->scale.ascale);
        cairo_arc (w->crb, knobx1+arc_offset, knoby1+arc_offset, radius,
              add_angle + scale_zero, add_angle + scale_zero + 320 * (M_PI/180));
        cairo_stroke(w->crb);

        // indicator
        cairo_new_sub_path(w->crb);
        use_fg_color_scheme(w, NORMAL_);
        cairo_arc (w->crb,knobx1+arc_offset, knoby1+arc_offset, radius,
              add_angle + scale_zero, add_angle + angle);
        cairo_stroke(w->crb);

    }
    use_text_color_scheme(w, get_color_state(w));
    cairo_text_extents_t extents;
    /** show value on the kob**/
    if (w->state) {
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
        cairo_set_font_size (w->crb, w->app->small_font/w->scale.ascale);
        cairo_text_extents(w->crb, s, &extents);
        cairo_move_to (w->crb, knobx1-extents.width/2, knoby1+extents.height/2);
        cairo_show_text(w->crb, s);
        cairo_new_path (w->crb);
    }

    _show_label(w, width, height + (w->app->small_font + 7));
}

void _knob_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER) w->state= 1;
    expose_widget(w);
}
