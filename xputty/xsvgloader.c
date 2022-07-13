#include <assert.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "cairo/cairo.h"

#define NANOSVG_ALL_COLOR_KEYWORDS
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#include "xsvgloader.h"
#include "b64_encode.h"

#ifndef min
#define min(x, y) (((x) < (y)) ? (x) : (y))
#endif


static inline void to_rgba(double*a, unsigned int const c) {
    a[0] = (c & 0xff) / 255.;
    a[1] = ((c >> 8) & 0xff) / 255.;
    a[2] = ((c >> 16) & 0xff) / 255.;
    a[3] = ((c >> 24) & 0xff) / 255.;
}

static inline void inverse(double*a, float const* const t) {
    double const invdet = (1. / ((double)(t[0]) * t[3] - (double)(t[2]) * t[1]));

    a[0] = t[3] * invdet;
    a[1] = -t[1] * invdet;
    a[2] = -t[2] * invdet;
    a[3] = t[0] * invdet;
    a[4] = ((double)(t[2]) * t[5] - (double)(t[3]) * t[4]) * invdet;
    a[5] = ((double)(t[1]) * t[4] - (double)(t[0]) * t[5]) * invdet;
}

static inline void draw_stroke(struct NSVGshape* const shape, cairo_t* const cr) {
    switch (shape->stroke.type) {
        case NSVG_PAINT_NONE:
            break;

        case NSVG_PAINT_COLOR:
        {
            double c[4]= {0};
            to_rgba(c, shape->stroke.color);
            cairo_set_source_rgba(cr, c[0], c[1], c[2], shape->opacity * c[3]);

            int const count = shape->strokeDashCount;
            if (count) {
                double dashes[sizeof(shape->strokeDashArray) / sizeof(*shape->strokeDashArray)];
                memcpy(dashes, shape->strokeDashArray, sizeof(*shape->strokeDashArray + count));
                cairo_set_dash(cr, dashes, shape->strokeDashCount, shape->strokeDashOffset);
            }
        }

        switch (shape->strokeLineCap) {
            case NSVG_CAP_BUTT:
                cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
                break;

            case NSVG_CAP_ROUND:
                cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
                break;

            case NSVG_CAP_SQUARE:
                cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
                break;

            default:
                cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
                break;
                //assert(0);
        }

        switch (shape->strokeLineJoin) {
            case NSVG_JOIN_MITER:
                cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
                cairo_set_miter_limit(cr, shape->miterLimit);
                break;

            case NSVG_JOIN_ROUND:
                cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
                break;

            case NSVG_JOIN_BEVEL:
                cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);
                break;

            default:
                cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
                cairo_set_miter_limit(cr, shape->miterLimit);
                break;
                //assert(0);
        }
        cairo_set_line_width(cr, shape->strokeWidth);
        cairo_stroke_preserve(cr);
        break;

        default:
            break;
            //assert(0);
    }
}

static inline void draw_svg_shape(cairo_t* const cr, struct NSVGshape* const shape) {
    cairo_new_path(cr);

    NSVGpath* path = shape->paths;
    for (; path; path = path->next) {
        float* p = path->pts;
        //cairo_new_sub_path(cr);
        cairo_move_to(cr, p[0], p[1]);
        float* const end = (p + 2 * path->npts);
        for (p += 2; end != p; p += 6) {
            cairo_curve_to(cr, p[0], p[1], p[2], p[3], p[4], p[5]);
        }
        if (path->closed) {
            cairo_close_path(cr);
        }
    }

    // fill
    int const type = shape->fill.type;
    switch (type) {
        case NSVG_PAINT_NONE:
            break;

        case NSVG_PAINT_COLOR:
        case NSVG_PAINT_LINEAR_GRADIENT:
        case NSVG_PAINT_RADIAL_GRADIENT:
        {
            switch (shape->fillRule) {
                case NSVG_FILLRULE_NONZERO:
                    cairo_set_fill_rule(cr, CAIRO_FILL_RULE_WINDING);
                    break;

                case NSVG_FILLRULE_EVENODD:
                    cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
                    break;

                default:
                    assert(0);
            }
            cairo_pattern_t* pat = {0};

            switch (type) {
                case NSVG_PAINT_COLOR:
                {
                    double c[4] = {0};
                    to_rgba(c, shape->fill.color);
                    cairo_set_source_rgba(cr, c[0], c[1], c[2], shape->opacity * c[3]);
                    cairo_fill_preserve(cr);
                    draw_stroke(shape, cr);
                    return;
                }

                case NSVG_PAINT_LINEAR_GRADIENT:
                {
                    double t[6] = {0};
                    inverse(t, shape->fill.gradient->xform);
                    pat = cairo_pattern_create_linear(t[4], t[5], t[4] + t[2], t[5] + t[3]);
                    break;
                }

                case NSVG_PAINT_RADIAL_GRADIENT:
                {
                    NSVGgradient* g = shape->fill.gradient;
                    double t[6] = {0};
                    inverse(t, g->xform);
                    double const r = t[0];
                    pat = cairo_pattern_create_radial(g->fx * -r, g->fy * -r, 0., t[4], t[5], r);
                    break;
                }

                default:
                    assert(0);
            }

            assert(pat);
            NSVGgradient* g = shape->fill.gradient;

            switch (g->spread) {
            case NSVG_SPREAD_PAD:
                cairo_pattern_set_extend(pat, CAIRO_EXTEND_PAD);
                break;

            case NSVG_SPREAD_REFLECT:
                cairo_pattern_set_extend(pat, CAIRO_EXTEND_REFLECT);
                break;

            case NSVG_SPREAD_REPEAT:
                cairo_pattern_set_extend(pat, CAIRO_EXTEND_REPEAT);
                break;

            default:
                assert(0);
            }

            int const ns = g->nstops;
            for (int i = 0; ns != i; ++i) {
                NSVGgradientStop stop = g->stops[i];
                double c[4] = {0};
                to_rgba(c, stop.color);
                cairo_pattern_add_color_stop_rgba(pat, stop.offset,
                            c[0], c[1], c[2], shape->opacity * c[3]);
            }
            cairo_set_source(cr, pat);
            cairo_fill_preserve(cr);
            cairo_pattern_destroy(pat);
            break;
        }

        default:
            assert(0);
    }
    draw_stroke(shape, cr);
}

static void draw_svg_image(cairo_t* const cr, struct NSVGimage* const image,
                                    double const w, double const h) {
    double const sm = (min(w / image->width, h / image->height));
    cairo_translate(cr, .5 * (w - sm * image->width), .5 * (h - sm * image->height));
    cairo_scale(cr, sm, sm);
    // draw shapes
    NSVGshape* shape = image->shapes;
    for (; shape; shape = shape->next) {
        draw_svg_shape(cr, shape);
    }
}

void draw_svg_image_to_pos(cairo_t* const cr, struct NSVGimage* const image,
                double const x, double const y, double const w, double const h) {
    cairo_save(cr);
    double const sm = (min(w / image->width, h / image->height));
    cairo_translate(cr, x + .5 * (w - sm * image->width), y + .5 * (h - sm * image->height));
    cairo_scale(cr, sm, sm);
    // draw shapes
    NSVGshape* shape = image->shapes;
    for (; shape; shape = shape->next) {
        if (NSVG_FLAGS_VISIBLE & shape->flags) {
            draw_svg_shape(cr, shape);
        }
    }
    cairo_restore(cr);
}

void widget_get_svg(Widget_t *w, const char* name) {
    char* b64dst;
    b64dst = (char*)malloc((strlen(name)+1) * sizeof(char));
    b64_decode((char*)name, b64dst);
    struct NSVGimage* const image = nsvgParse(b64dst, "px", 96);

    if (!image) return;
    int width_t = image->width;
    int height_t = image->height;
    cairo_surface_destroy(w->image);
    w->image = NULL;
    
    w->image = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width_t, height_t);
    cairo_t *cri = cairo_create (w->image);
    draw_svg_image(cri, image, width_t, height_t);
    nsvgDelete(image);
    free(b64dst);
    cairo_destroy(cri);
}

void widget_get_scaled_svg(Widget_t *w, const char* name) {
    char* b64dst;
    b64dst = (char*)malloc((strlen(name)+1) * sizeof(char));
    b64_decode((char*)name, b64dst);
    struct NSVGimage* const image = nsvgParse(b64dst, "px", 96);

    if (!image) return;
    int width = image->width;
    int height = image->height;
    int width_t = w->scale.init_width;
    int height_t = w->scale.init_height;
    double x = (double)width_t/(double)width;
    double y = (double)height_t/(double)height;    

    cairo_surface_t *getsvg = cairo_image_surface_create ( CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *cris = cairo_create (getsvg);
    draw_svg_image(cris, image, width, height);

    cairo_surface_destroy(w->image);
    w->image = NULL;
    w->image = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width_t, height_t);
    cairo_t *cri = cairo_create (w->image);

    cairo_scale(cri, x,y);    
    cairo_set_source_surface (cri, getsvg,0,0);
    cairo_paint (cri);

    cairo_destroy(cris);
    cairo_surface_destroy(getsvg);
    nsvgDelete(image);
    free(b64dst);
    cairo_destroy(cri);
}

void widget_get_svg_from_file(Widget_t *w, const char* filename) {
    struct NSVGimage* const image = nsvgParseFromFile(filename, "px", 96);

    if (!image) return;
    int width_t = w->scale.init_width;
    int height_t = w->scale.init_height;
    cairo_surface_destroy(w->image);
    w->image = NULL;
    
    w->image = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width_t, height_t);
    cairo_t *cri = cairo_create (w->image);
    draw_svg_image(cri, image, width_t, height_t);
    nsvgDelete(image);
    cairo_destroy(cri);
}

void widget_get_scaled_svg_from_file(Widget_t *w, const char* filename, int width_t, int height_t) {
    struct NSVGimage* const image = nsvgParseFromFile(filename, "px", 96);

    if (!image) return;
    cairo_surface_destroy(w->image);
    w->image = NULL;
    
    w->image = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width_t, height_t);
    cairo_t *cri = cairo_create (w->image);
    draw_svg_image(cri, image, width_t, height_t);
    nsvgDelete(image);
    cairo_destroy(cri);
}

cairo_surface_t *cairo_image_surface_create_from_svg ( const char* filename) {
    struct NSVGimage* const image = nsvgParseFromFile(filename, "px", 96);

    if (!image) return NULL;
    int width_t = image->width;
    int height_t = image->height;
    
    cairo_surface_t *getsvg = cairo_image_surface_create ( CAIRO_FORMAT_ARGB32, width_t, height_t);
    cairo_t *cri = cairo_create (getsvg);
    draw_svg_image(cri, image, width_t, height_t);
    nsvgDelete(image);
    return getsvg;
}
