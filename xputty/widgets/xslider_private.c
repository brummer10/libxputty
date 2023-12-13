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


#include "xslider_private.h"


void _pattern_vslider(Widget_t *w, Color_state st, int width) {
    Colors *c = get_color_scheme(w,st);
    if (!c) return;
    cairo_pattern_t *pat = cairo_pattern_create_linear (2, 2, width, 2);
    cairo_pattern_add_color_stop_rgba(pat, 0.0, c->base[0],  c->base[1], c->base[2],  c->base[3]);
    cairo_pattern_add_color_stop_rgba(pat, 0.5, 0.0, 0.0, 0.0, 0.0);
    cairo_pattern_add_color_stop_rgba(pat, 1.0, c->base[0],  c->base[1], c->base[2],  c->base[3]);
    cairo_set_source(w->crb, pat);
    cairo_pattern_destroy (pat);
}

void _pattern_hslider(Widget_t *w, Color_state st, int height) {
    Colors *c = get_color_scheme(w,st);
    if (!c) return;
    cairo_pattern_t *pat = cairo_pattern_create_linear (2, 2, 2, height);
    cairo_pattern_add_color_stop_rgba(pat, 0.0, c->base[0],  c->base[1], c->base[2],  c->base[3]);
    cairo_pattern_add_color_stop_rgba(pat, 0.5, 0.0, 0.0, 0.0, 0.0);
    cairo_pattern_add_color_stop_rgba(pat, 1.0, c->base[0],  c->base[1], c->base[2],  c->base[3]);
    cairo_set_source(w->crb, pat);
    cairo_pattern_destroy (pat);
}

void _draw_image_slider(Widget_t *w, int width_t, int height_t) {
    Slider_t *slider = (Slider_t*)w->private_struct;
    int width, height;
    os_get_surface_size(w->image, &width, &height);
    int size = width/slider->frames;
    double x = (double)width_t/(double)size;
    double y = (double)height_t/(double)height;
    double x1 = (double)size/(double)width_t;
    double y1 = (double)height/(double)height_t;
    double s = x>y ? y : x;
    double s1 = x1>y1 ? y1 : x1;
    int posx = (width_t/2 -(size*s)/2);
    int posy = (height_t/2 - ((height*s)/2));
    double sliderstate = adj_get_state(w->adj);
    int findex = (int)(((slider->frames)-1) * sliderstate);
    cairo_save(w->crb);
    cairo_scale(w->crb, s,s);
    cairo_translate(w->crb, posx * ((1-s)/s), posy * ((1-s)/s));
    cairo_set_source_surface (w->crb, w->image, -size*findex + posx, posy);
    cairo_rectangle(w->crb, posx, posy, size, height);
    cairo_fill(w->crb);
    cairo_scale(w->crb, s1,s1);
    cairo_restore(w->crb);
}

void _draw_vslider(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Metrics_t metrics;
    os_get_window_metrics(w, &metrics);
    int width = metrics.width-2;
    int height = metrics.height-2;
    if (!metrics.visible) return;
    float center = (float)width/2;
    float upcenter = (float)width;

    if (w->image) {
        _draw_image_slider(w, width, height-10);
    } else {
        float sliderstate = adj_get_state(w->adj_y);
        _pattern_vslider(w, get_color_state(w), width);
        cairo_move_to (w->crb, center, center);
        cairo_line_to(w->crb,center,height-center-10);
        cairo_set_line_cap (w->crb,CAIRO_LINE_CAP_ROUND);
        cairo_set_line_width(w->crb,center);
        cairo_stroke(w->crb);

        use_shadow_color_scheme(w, get_color_state(w));
        cairo_move_to (w->crb, center, center);
        cairo_line_to(w->crb,center,height-center-10);
        cairo_set_line_width(w->crb,center/10);
        cairo_stroke(w->crb);

        use_shadow_color_scheme(w, get_color_state(w));
        cairo_arc(w->crb,center, (height-center-10) -
            ((height-10-upcenter) * sliderstate), center/2, 0, 2 * M_PI );
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb,1);
        cairo_stroke(w->crb);


        use_bg_color_scheme(w, get_color_state(w));
        cairo_arc(w->crb,center, (height-center-10) -
            ((height-10-upcenter) * sliderstate), center/3, 0, 2 * M_PI );
        cairo_fill_preserve(w->crb);
        use_fg_color_scheme(w, NORMAL_);
        cairo_set_line_width(w->crb,center/15);
        cairo_stroke(w->crb);
        cairo_new_path (w->crb);
    }

    cairo_text_extents_t extents;
    use_text_color_scheme(w, get_color_state(w));
    cairo_set_font_size (w->crb, min(w->app->normal_font/w->scale.ascale, center/1.8));
    cairo_text_extents(w->crb,w->label , &extents);

    cairo_move_to (w->crb, center-extents.width/2, height);
    cairo_show_text(w->crb, w->label);
    cairo_new_path (w->crb);

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
    cairo_set_font_size (w->crb, min(w->app->small_font/w->scale.ascale, center/1.8));
    cairo_text_extents(w->crb,s , &extents);
    cairo_move_to (w->crb, center-extents.width/2, extents.height );
    cairo_show_text(w->crb, s);
    cairo_new_path (w->crb);
}

void _draw_hslider(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Metrics_t metrics;
    os_get_window_metrics(w, &metrics);
    int width = metrics.width-2;
    int height = metrics.height-2;
    if (!metrics.visible) return;
    float center = (float)height/2;
    float upcenter = (float)height;
    
    if (w->image) {
        _draw_image_slider(w, width, height);
    } else {
        float sliderstate = adj_get_state(w->adj_x);
        _pattern_hslider(w, get_color_state(w), height);
        cairo_move_to (w->crb, center, center);
        cairo_line_to(w->crb,width-center-10,center);
        cairo_set_line_cap (w->crb,CAIRO_LINE_CAP_ROUND);
        cairo_set_line_width(w->crb,center);
        cairo_stroke(w->crb);

        use_shadow_color_scheme(w, get_color_state(w));
        cairo_move_to (w->crb, center, center);
        cairo_line_to(w->crb,width-center-10,center);
        cairo_set_line_width(w->crb,center/10);
        cairo_stroke(w->crb);

        use_shadow_color_scheme(w, get_color_state(w));
        cairo_arc(w->crb, (center) +
            ((width-10-upcenter) * sliderstate),center, center/2, 0, 2 * M_PI );
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb,1);
        cairo_stroke(w->crb);

        use_bg_color_scheme(w, get_color_state(w));
        cairo_arc(w->crb, (center) +
            ((width-10-upcenter) * sliderstate),center, center/3, 0, 2 * M_PI );
        cairo_fill_preserve(w->crb);
        use_fg_color_scheme(w, NORMAL_);
        cairo_set_line_width(w->crb,center/15);
        cairo_stroke(w->crb);
        cairo_new_path (w->crb);
    }
    cairo_text_extents_t extents;
    use_text_color_scheme(w, get_color_state(w));
    cairo_set_font_size (w->crb, w->app->normal_font/w->scale.ascale);
    cairo_text_extents(w->crb,w->label , &extents);

    cairo_move_to (w->crb, width/2-extents.width/2, height );
    cairo_show_text(w->crb, w->label);
    cairo_new_path (w->crb);

    cairo_set_font_size (w->crb, w->app->small_font/w->scale.ascale);
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
    cairo_text_extents(w->crb,s , &extents);
    cairo_move_to (w->crb, width/2-extents.width/2, extents.height );
    cairo_show_text(w->crb, s);
    cairo_new_path (w->crb);
}

void _slider_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    expose_widget(w);
}
