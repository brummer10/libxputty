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

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define MAX_CACHED_FONTS 8

typedef struct {
    const unsigned char *data;
    stbtt_fontinfo       info;
    int                  units_per_em;
    cairo_font_face_t   *cairo_face;
} cached_font;

static cached_font font_cache[MAX_CACHED_FONTS];
static int font_cache_count = 0;

static cairo_user_data_key_t font_data_key;

/* Cairo user-font callbacks */

static cairo_status_t stb_font_init(cairo_scaled_font_t  *scaled_font,
                                     cairo_t              *cr,
                                     cairo_font_extents_t *extents) {
    (void)cr;
    cairo_font_face_t *face = cairo_scaled_font_get_font_face(scaled_font);
    cached_font *cf = (cached_font*)cairo_font_face_get_user_data(face, &font_data_key);
    if (!cf) return CAIRO_STATUS_USER_FONT_ERROR;

    float scale = 1.0f / (float)cf->units_per_em;

    int ascent, descent, line_gap;
    stbtt_GetFontVMetrics(&cf->info, &ascent, &descent, &line_gap);

    extents->ascent  = ascent  * scale;
    extents->descent = -descent * scale; /* stb: descent is negative, cairo wants positive */
    extents->height  = (ascent - descent + line_gap) * scale;
    extents->max_x_advance = extents->height;
    extents->max_y_advance = 0.0;

    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t stb_unicode_to_glyph(cairo_scaled_font_t *scaled_font,
                                            unsigned long         unicode,
                                            unsigned long        *glyph) {
    cairo_font_face_t *face = cairo_scaled_font_get_font_face(scaled_font);
    cached_font *cf = (cached_font*)cairo_font_face_get_user_data(face, &font_data_key);
    if (!cf) return CAIRO_STATUS_USER_FONT_ERROR;

    int gi = stbtt_FindGlyphIndex(&cf->info, (int)unicode);
    *glyph = (unsigned long)gi;
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t stb_render_glyph(cairo_scaled_font_t  *scaled_font,
                                        unsigned long          glyph,
                                        cairo_t               *cr,
                                        cairo_text_extents_t  *extents) {
    cairo_font_face_t *face = cairo_scaled_font_get_font_face(scaled_font);
    cached_font *cf = (cached_font*)cairo_font_face_get_user_data(face, &font_data_key);
    if (!cf) return CAIRO_STATUS_USER_FONT_ERROR;

    float scale = 1.0f / (float)cf->units_per_em;

    stbtt_vertex *verts = NULL;
    int nverts = stbtt_GetGlyphShape(&cf->info, (int)glyph, &verts);

    /* font design space (y-up) -> cairo glyph space (y-down) */
    #define FX(v) ( (v) * scale)
    #define FY(v) (-(v) * scale)

    if (nverts > 0) {
        int i;
        double cx = 0.0, cy = 0.0; /* current point, for quad->cubic conversion */
        for (i = 0; i < nverts; i++) {
            stbtt_vertex *v = &verts[i];
            switch (v->type) {
                case STBTT_vmove:
                    cairo_close_path(cr);
                    cairo_move_to(cr, FX(v->x), FY(v->y));
                    cx = FX(v->x); cy = FY(v->y);
                    break;
                case STBTT_vline:
                    cairo_line_to(cr, FX(v->x), FY(v->y));
                    cx = FX(v->x); cy = FY(v->y);
                    break;
                case STBTT_vcurve: {
                    /* quadratic bezier -> cubic bezier */
                    double qx = FX(v->cx), qy = FY(v->cy);
                    double ex = FX(v->x),  ey = FY(v->y);
                    double c1x = cx + 2.0/3.0 * (qx - cx);
                    double c1y = cy + 2.0/3.0 * (qy - cy);
                    double c2x = ex + 2.0/3.0 * (qx - ex);
                    double c2y = ey + 2.0/3.0 * (qy - ey);
                    cairo_curve_to(cr, c1x, c1y, c2x, c2y, ex, ey);
                    cx = ex; cy = ey;
                    break;
                }
                case STBTT_vcubic: {
                    double c1x = FX(v->cx),  c1y = FY(v->cy);
                    double c2x = FX(v->cx1), c2y = FY(v->cy1);
                    double ex  = FX(v->x),   ey  = FY(v->y);
                    cairo_curve_to(cr, c1x, c1y, c2x, c2y, ex, ey);
                    cx = ex; cy = ey;
                    break;
                }
                default:
                    break;
            }
        }
        cairo_close_path(cr);
        cairo_fill(cr);
    }

    if (verts) stbtt_FreeShape(&cf->info, verts);

    #undef FX
    #undef FY

    int advance, lsb;
    stbtt_GetGlyphHMetrics(&cf->info, (int)glyph, &advance, &lsb);
    if (extents) {
        extents->x_advance = advance * scale;
        extents->y_advance = 0.0;
    }

    return CAIRO_STATUS_SUCCESS;
}

static cairo_font_face_t *_get_or_load(const unsigned char *data, unsigned int len) {
    int i;
    for (i = 0; i < font_cache_count; i++) {
        if (font_cache[i].data == data) {
            return font_cache[i].cairo_face;
        }
    }

    if (font_cache_count >= MAX_CACHED_FONTS) {
        debug_print("xftloader: font cache full, increase MAX_CACHED_FONTS\n");
        return NULL;
    }

    cached_font *cf = &font_cache[font_cache_count];
    cf->data = data;

    if (!stbtt_InitFont(&cf->info, data, stbtt_GetFontOffsetForIndex(data, 0))) {
        debug_print("xftloader: stbtt_InitFont failed\n");
        return NULL;
    }
    cf->units_per_em = 0;
    /* unitsPerEm isn't exposed directly by stb_truetype < recent versions;
       derive it from the head table ourselves. */
    {
        int loc, this_units_per_em = 1000; /* sane fallback */
        stbtt_uint8 *data8 = (stbtt_uint8*)data;
        loc = stbtt__find_table(data8, cf->info.fontstart, "head");
        if (loc) {
            this_units_per_em = ttUSHORT(data8 + loc + 18);
        }
        cf->units_per_em = this_units_per_em;
    }

    cairo_font_face_t *face = cairo_user_font_face_create();
    cairo_user_font_face_set_init_func(face, stb_font_init);
    cairo_user_font_face_set_render_glyph_func(face, stb_render_glyph);
    cairo_user_font_face_set_unicode_to_glyph_func(face, stb_unicode_to_glyph);

    if (cairo_font_face_status(face) != CAIRO_STATUS_SUCCESS) {
        debug_print("xftloader: cairo_user_font_face_create failed\n");
        cairo_font_face_destroy(face);
        return NULL;
    }

    cairo_font_face_set_user_data(face, &font_data_key, cf, NULL);

    cf->cairo_face = face;
    font_cache_count++;

    return face;
}

/* public API */

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
    }
    font_cache_count = 0;
}
