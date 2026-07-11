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

#include "xftloader.h"

#define MAX_CACHED_FONTS 8

typedef struct {
    const unsigned char *data;
    FT_Face ft_face;
    cairo_font_face_t *cairo_face;
} cached_font;

static FT_Library ft_library = NULL;
static cached_font font_cache[MAX_CACHED_FONTS];
static int font_cache_count = 0;

static cairo_font_face_t *_get_or_load(const unsigned char *data, unsigned int len) {
    int i;
    for (i = 0; i < font_cache_count; i++) {
        if (font_cache[i].data == data) {
            return font_cache[i].cairo_face;
        }
    }

    if (!ft_library) {
        if (FT_Init_FreeType(&ft_library) != 0) {
            debug_print("xftloader: FT_Init_FreeType failed\n");
            return NULL;
        }
    }

    if (font_cache_count >= MAX_CACHED_FONTS) {
        debug_print("xftloader: font cache full, increase MAX_CACHED_FONTS\n");
        return NULL;
    }

    FT_Face face;
    if (FT_New_Memory_Face(ft_library, data, (FT_Long)len, 0, &face) != 0) {
        debug_print("xftloader: FT_New_Memory_Face failed\n");
        return NULL;
    }

    cairo_font_face_t *cface = cairo_ft_font_face_create_for_ft_face(face, 0);
    if (cairo_font_face_status(cface) != CAIRO_STATUS_SUCCESS) {
        debug_print("xftloader: cairo_ft_font_face_create_for_ft_face failed\n");
        cairo_font_face_destroy(cface);
        FT_Done_Face(face);
        return NULL;
    }

    font_cache[font_cache_count].data = data;
    font_cache[font_cache_count].ft_face = face;
    font_cache[font_cache_count].cairo_face = cface;
    font_cache_count++;

    return cface;
}

cairo_font_face_t *get_font_face_from_ttf(const unsigned char* data, unsigned int len) {
    return _get_or_load(data, len);
}

void widget_set_font_from_ttf(Widget_t *w, const unsigned char* data, unsigned int len) {
    cairo_font_face_t *face = _get_or_load(data, len);
    if (face) {
        cairo_set_font_face(w->cr, face);
        cairo_set_font_face(w->crb, face);
    }
}

void surface_set_font_from_ttf(cairo_t *cr, const unsigned char* data, unsigned int len){
    cairo_font_face_t *face = _get_or_load(data, len);
    if (face) {
        cairo_set_font_face(cr, face);
    }
}

void xftloader_cleanup(void) {
    int i;
    for (i = 0; i < font_cache_count; i++) {
        cairo_font_face_destroy(font_cache[i].cairo_face);
        FT_Done_Face(font_cache[i].ft_face);
    }
    font_cache_count = 0;

    if (ft_library) {
        FT_Done_FreeType(ft_library);
        ft_library = NULL;
    }
}
