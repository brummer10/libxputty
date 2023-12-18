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

#ifndef XPNGLOADER_H_
#define XPNGLOADER_H_

#include "xputty.h"
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif


/**  
        define some MACROS to read png data from binary stream 
        png's been converted to object files with
        ld -r -b binary name.png -o name.o
*/

#ifdef USE_LD

#ifdef __APPLE__

#ifdef __cplusplus
}
#endif

#include <mach-o/getsect.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EXTLD(NAME) \
  extern const unsigned char _section$__DATA__ ## NAME [];
#define LDVAR(NAME) _section$__DATA__ ## NAME
#define LDLEN(NAME) (getsectbyname("__DATA", "__" #NAME)->size)

#elif (defined __WIN32__)  /* mingw */


#define EXTLD(NAME) \
  extern const unsigned char _binary_ ## NAME ## _start[]; \
  extern const unsigned char _binary_ ## NAME ## _end[];
#define LDVAR(NAME) \
  _binary_ ## NAME ## _start
#define LDLEN(NAME) \
  ((_binary_ ## NAME ## _end) - (_binary_ ## NAME ## _start))

#else /* gnu/linux ld */

#define EXTLD(NAME) \
  extern const unsigned char _binary_ ## NAME ## _start[]; \
  extern const unsigned char _binary_ ## NAME ## _end[];
#define LDVAR(NAME) \
  _binary_ ## NAME ## _start
#define LDLEN(NAME) \
  ((_binary_ ## NAME ## _end) - (_binary_ ## NAME ## _start))
#endif

#else /* xxd -i NAME */

#define EXTLD(NAME) \
    extern const unsigned char NAME;
#define LDVAR(NAME) \
    (const unsigned char*)&NAME

#endif


/*---------------------------------------------------------------------
-----------------------------------------------------------------------    
                    define needed structs
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

/**
 * @brief binary_stream      - struct definition to read binary data 
 * into cairo surface
 * @param *data              - pointer to the binary image data
 * @param position           - pointer to the read position
 */

typedef struct  {
    const unsigned char * data;
    long int position;
} binary_stream;

#include "xresources.h"

/**
 * @brief cairo_image_surface_create_from_stream  - read binary data 
 * into cairo surface from stream
 * @param *name                      - pointer to the binary image data LDVAR(name)
 * @return cairo_surface_t           - the cairo_image_surface
 */

cairo_surface_t *cairo_image_surface_create_from_stream ( const unsigned char* name);

/**
 * @brief widget_get_png             - read png into Widget_t xlib surface
 * @param *w                         - pointer to the Widget_t which should use the png
 * @param *name                      - pointer to the binary image data LDVAR(name)
 */

void widget_get_png(Widget_t *w, const unsigned char* name);

/**
 * @brief widget_get_scaled_png      - read scaled png into Widget_t xlib surface
 * @param *w                         - pointer to the Widget_t which should use the png
 * @param *name                      - pointer to the binary image data LDVAR(name)
 */


void widget_get_scaled_png(Widget_t *w, const unsigned char* name);

/**
 * @brief widget_get_png_from_file   - read png from file into Widget_t xlib surface
 * @param *w                         - pointer to the Widget_t which should use the png
 * @param *filename                  - pointer to the image path/file
 */

void widget_get_png_from_file(Widget_t *w, const char* filename);

/**
 * @brief widget_get_scaled_png_from_file  - read scaled png into Widget_t xlib surface
 * @param *w                               - pointer to the Widget_t which should use the png
 * @param *filename                        - pointer to the image path/file
 */

void widget_get_scaled_png_from_file(Widget_t *w, const char* filename);

/**
 * @brief widget_get_surface_ptr     - set pointer to a 2. Widget_t xlib surface
 * @param *w                         - pointer to the Widget_t which should use the surface
 * @param *wid                       - pointer to the Widget_t which hold the surface
 */

void widget_get_surface_ptr(Widget_t *w, Widget_t *wid);

/**
 * @brief surface_get_png            - read png into Widget_t xlib surface
 * @param *w                         - pointer to the Widget_t which should use the png
 * @param *sf                        - pointer to the cairo_surface_t which should hold the png
 * @param *name                      - pointer to the binary image data LDVAR(name)
 * @return cairo_surface_t           - the cairo_xlib_surface, you need to free it by yourself
 */

cairo_surface_t * surface_get_png(Widget_t *w, cairo_surface_t *sf, const unsigned char* name);

/**
 * @brief widget_set_icon_from_surface - set icon image from cairo surface for Widget_t
 * those icon will be used in the Window decoration and the toolbar (depending on the Widnow manager)
 * @param *w                           - pointer to the Widget_t which should use the icon
 * @param *image                       - pointer to the cairo_surface_t to use for the icon
 */

void widget_set_icon_from_surface(Widget_t *w,  cairo_surface_t *image);

/**
 * @brief widget_set_icon_from_png   - set icon image from png binary to Widget_t 
 * those icon will be used in the Window decoration and the toolbar (depending on the Widnow manager)
 * @param *w                         - pointer to the Widget_t which should use the icon
 * @param *name                      - pointer to the binary image data LDVAR(name)
 */

void widget_set_icon_from_png(Widget_t *w, const unsigned char* name);

#ifdef __cplusplus
}
#endif

#endif //XPNGLOADER_H_
