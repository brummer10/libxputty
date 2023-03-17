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

#include "xpngloader.h"
#include <stdint.h>

/* 
 * @brief      load png data from binary blob into cairo surface
*/

cairo_status_t png_stream_reader (void *_stream, unsigned char *data, unsigned int length) {
    binary_stream * stream = (binary_stream *) _stream;
    memcpy(data, &stream->data[stream->position],length);
    stream->position += length;
    return CAIRO_STATUS_SUCCESS;
}

cairo_surface_t *cairo_image_surface_create_from_stream ( const unsigned char* name) {
    binary_stream png_stream;
    png_stream.data = name;
    png_stream.position = 0;
    return cairo_image_surface_create_from_png_stream(&png_stream_reader, (void *)&png_stream);
}

void _copy_surface_to_widget(Widget_t *w, cairo_surface_t *getpng) {
    int width = cairo_image_surface_get_width(getpng);
    int height = cairo_image_surface_get_height(getpng);
    cairo_surface_destroy(w->image);
    w->image = NULL;
    
    w->image = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width, height);
    cairo_t *cri = cairo_create (w->image);
    cairo_set_source_surface (cri, getpng,0,0);
    cairo_paint (cri);
    cairo_destroy(cri);
}

void _copy_scaled_surface_to_widget(Widget_t *w, cairo_surface_t *getpng) {
    int width = cairo_image_surface_get_width(getpng);
    int height = cairo_image_surface_get_height(getpng);
    int width_t = w->scale.init_width;
    int height_t = w->scale.init_height;
    double x = (double)width_t/(double)width;
    double y = (double)height_t/(double)height;
    cairo_surface_destroy(w->image);
    w->image = NULL;

    w->image = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width_t, height_t);
    cairo_t *cri = cairo_create (w->image);
    cairo_scale(cri, x,y);    
    cairo_set_source_surface (cri, getpng,0,0);
    cairo_paint (cri);
    cairo_destroy(cri);
}

void widget_get_png(Widget_t *w, const unsigned char* name) {
    cairo_surface_t *getpng = cairo_image_surface_create_from_stream (name);
    _copy_surface_to_widget(w, getpng);
    cairo_surface_destroy(getpng);
}

void widget_get_scaled_png(Widget_t *w, const unsigned char* name) {
    cairo_surface_t *getpng = cairo_image_surface_create_from_stream (name);
    _copy_scaled_surface_to_widget(w, getpng);
    cairo_surface_destroy(getpng);
}

void widget_get_png_from_file(Widget_t *w, const char* filename) {
    cairo_surface_t *getpng = cairo_image_surface_create_from_png (filename);
    _copy_surface_to_widget(w, getpng);
    cairo_surface_destroy(getpng);
}

void widget_get_scaled_png_from_file(Widget_t *w, const char* filename) {
    cairo_surface_t *getpng = cairo_image_surface_create_from_png (filename);
    _copy_scaled_surface_to_widget(w, getpng);
    cairo_surface_destroy(getpng);
}

void widget_get_surface_ptr(Widget_t *w, Widget_t *wid) {
    w->image = wid->image;
    w->flags |= REUSE_IMAGE;
}

cairo_surface_t * surface_get_png(Widget_t *w, cairo_surface_t *sf, const unsigned char* name) {
    cairo_surface_t *getpng = cairo_image_surface_create_from_stream (name);
    int width = cairo_image_surface_get_width(getpng);
    int height = cairo_image_surface_get_height(getpng);
    
    sf = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width, height);
    cairo_t *cri = cairo_create (sf);
    cairo_set_source_surface (cri, getpng,0,0);
    cairo_paint (cri);
    cairo_surface_destroy(getpng);
    cairo_destroy(cri);
    return sf;
}

void widget_set_icon_from_surface(Widget_t *w, cairo_surface_t *image) {
    int width_t = cairo_xlib_surface_get_width(image);
    int height_t = cairo_xlib_surface_get_height(image);
    cairo_surface_t *icon = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width_t, height_t);
    cairo_t *cri = cairo_create (icon);
    cairo_set_source_surface (cri, image,0,0);
    cairo_paint (cri);
    
    int stride = cairo_image_surface_get_stride (icon);
    unsigned long* icon_data = malloc(2+width_t*height_t * sizeof(unsigned long));
    memset(icon_data, 0, 2+width_t*height_t * sizeof(unsigned long));
    const unsigned char *data = cairo_image_surface_get_data(icon);
    icon_data[0] = width_t;
    icon_data[1] = height_t;
    int x = 0;
    int y = 0;
    unsigned long* output_pixel = icon_data;
    output_pixel += 2;
    for (y = 0; y < height_t; y++) {
        uint32_t *row = (uint32_t*) (data + y * stride);
        for (x = 0; x < width_t; x++) {
            output_pixel[0] |= row[x];
            output_pixel ++;
        }
    }
    
    Atom net_wm_icon = XInternAtom(w->app->dpy, "_NET_WM_ICON", False);
    Atom cardinal = XInternAtom(w->app->dpy, "CARDINAL", False);
    XChangeProperty(w->app->dpy, w->widget, net_wm_icon, cardinal, 32, PropModeReplace,
                                    (const unsigned char*) icon_data, 2+width_t*height_t);

    cairo_surface_destroy(icon);
    cairo_destroy(cri);
    free(icon_data);
}

void widget_set_icon_from_png(Widget_t *w, const unsigned char* name) {
    cairo_surface_t *image = cairo_image_surface_create_from_stream (name);
    int width_t = cairo_image_surface_get_width(image);
    int height_t = cairo_image_surface_get_height(image);
    int stride = cairo_image_surface_get_stride (image);
    unsigned long* icon_data = malloc(16+width_t*height_t * sizeof(unsigned long));
    memset(icon_data, 0, 16+width_t*height_t * sizeof(unsigned long));
    const unsigned char *data = cairo_image_surface_get_data(image);
    icon_data[0] = width_t;
    icon_data[1] = height_t;
    int x = 0;
    int y = 0;
    unsigned long* output_pixel = icon_data;
    output_pixel += 2;
    for (y = 0; y < height_t; y++) {
        uint32_t *row = (uint32_t*) (data + y * stride);
        for (x = 0; x < width_t; x++) {
            output_pixel[0] |= row[x];
            output_pixel ++;
        }
    }
    
    Atom net_wm_icon = XInternAtom(w->app->dpy, "_NET_WM_ICON", False);
    Atom cardinal = XInternAtom(w->app->dpy, "CARDINAL", False);
    XChangeProperty(w->app->dpy, w->widget, net_wm_icon, cardinal, 32, PropModeReplace,
                                    (const unsigned char*) icon_data, 2+width_t*height_t);

    cairo_surface_destroy(image);
    free(icon_data);
}

/*
cairo_surface_t* iamge = cairo_image_surface_create_from_stream( LDVAR(image_name_png));
*/
