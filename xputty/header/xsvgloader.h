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

#pragma once

#ifndef XSVGLOADER_H_
#define XSVGLOADER_H_

#include "xputty.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief widget_get_svg             - read svg into Widget_t xlib surface
 * @param *w                         - pointer to the Widget_t which should use the svg
 * @param *name                      - pointer to the base64 image data
 * @return void
 */

void widget_get_svg(Widget_t *w, const char* name);

/**
 * @brief widget_get_scaled_svg      - read svg scaled into Widget_t xlib surface
 * @param *w                         - pointer to the Widget_t which should use the svg
 * @param *name                      - pointer to the base64 image data
 * @return void
 */

void widget_get_scaled_svg(Widget_t *w, const char* name);

/**
 * @brief widget_get_svg_from_file   - read svg into Widget_t xlib surface
 * @param *w                         - pointer to the Widget_t which should use the svg
 * @param *filename                  - pointer to the image data file
 * @return void
 */

void widget_get_svg_from_file(Widget_t *w, const char* filename);

/**
 * @brief widget_get_scaled_svg_from_file    - read svg into Widget_t xlib surface
 * @param *w                                 - pointer to the Widget_t which should use the svg
 * @param *filename                          - pointer to the image data file
 * @param  width_t                           - the requested width
 * @param  height_t                          - the requested height
 * @return void
 */

void widget_get_scaled_svg_from_file(Widget_t *w, const char* filename, int width_t, int height_t);

cairo_surface_t *cairo_image_surface_create_from_svg ( const char* name);

#ifdef __cplusplus
}
#endif

#endif //XSVGLOADER_H_
