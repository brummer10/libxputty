/*
 *                           0BSD
 *
 *                    BSD Zero Clause License
 *
 *  Copyright (c) 2026 Hermann Meyer
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
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

#ifndef XFTLOADER_H_
#define XFTLOADER_H_

#include "xputty.h"
#include <cairo-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief widget_set_font_from_ttf - load embedded font data as a
 * cairo_font_face_t and set it as the active font on the Widget_t's
 * cairo context. The underlying FT_Library/FT_Face are owned and cached
 * internally, so repeated calls with the same data pointer are cheap.
 * @param *w      - pointer to the Widget_t which should use the font
 * @param *data   - pointer to the binary font data, LDVAR_FONT(name)
 * @param len     - length of the binary font data, LDLEN_FONT(name)
 */

void widget_set_font_from_ttf(Widget_t *w, const unsigned char* data, unsigned int len);

/**
 * @brief sureface_set_font_from_ttf - load embedded font data as a
 * cairo_font_face_t and set it as the active font on the cairo context. 
 * The underlying FT_Library/FT_Face are owned and cached
 * internally, so repeated calls with the same data pointer are cheap.
 * @param *cr     - pointer to the cairo context which should use the font
 * @param *data   - pointer to the binary font data, LDVAR_FONT(name)
 * @param len     - length of the binary font data, LDLEN_FONT(name)
 */

void surface_set_font_from_ttf(cairo_t *cr, const unsigned char* data, unsigned int len);

/**
 * @brief get_font_face_from_ttf   - load embedded font data as a
 * cairo_font_face_t without binding it to a Widget_t. Useful when you
 * want to keep a reference (e.g. in Xputty->font_face_regular) and
 * apply it to many widgets yourself via cairo_set_font_face().
 * @param *data   - pointer to the binary font data, LDVAR_FONT(name)
 * @param len     - length of the binary font data, LDLEN_FONT(name)
 * @return cairo_font_face_t*      - the cairo font face, or NULL on failure.
 *                                   Owned by xftloader, do not destroy yourself.
 */

cairo_font_face_t *get_font_face_from_ttf(const unsigned char* data, unsigned int len);

/**
 * @brief xftloader_cleanup        - release all cached FT_Faces,
 * cairo_font_face_t's and the FT_Library. Call once on shutdown,
 * e.g. from main_quit().
 */

void xftloader_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif //XFTLOADER_H_
