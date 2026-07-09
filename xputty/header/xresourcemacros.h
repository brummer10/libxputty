/*
 *                           0BSD
 *
 *                    BSD Zero Clause License
 *
 *  Copyright (c) 2019 Hermann Meyer
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

#ifndef XRESOURCEMACROS_H
#define XRESOURCEMACROS_H

#include "config.h"
/**  
        define some MACROS to read png data from binary stream 
        png's been converted to object files with
        ld -r -b binary name.png -o name.o
*/

#ifdef __cplusplus
extern "C" {
#endif

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

#else /* mingw and gnu/linux ld */

#define EXTLD(NAME) \
  extern const unsigned char _binary_ ## NAME ## _start[]; \
  extern const unsigned char _binary_ ## NAME ## _end[];
#define LDVAR(NAME) \
  _binary_ ## NAME ## _start
#define LDLEN(NAME) \
  ((_binary_ ## NAME ## _end) - (_binary_ ## NAME ## _start))

#endif

/**
        define some MACROS to read font data from a binary stream.
        TrueType/OpenType data is not self-delimiting, 
        so FT_New_Memory_Face() needs the exact buffer size.
        fonts get converted to object files the same way as the png's,
        either with xxd -i, or with ld -r -b binary name.ttf -o name.o
*/

#define EXTLD_FONT(NAME)  EXTLD(NAME)
#define LDVAR_FONT(NAME)  LDVAR(NAME)
#define LDLEN_FONT(NAME)  ((unsigned int)LDLEN(NAME))

#else /* xxd -i NAME */

#define EXTLD(NAME) \
    extern const unsigned char NAME;
#define LDVAR(NAME) \
    (const unsigned char*)&NAME

#define EXTLD_FONT(NAME) \
    extern const unsigned char NAME; \
    extern const unsigned int NAME ## _len;
#define LDVAR_FONT(NAME) \
    (const unsigned char*)&NAME
#define LDLEN_FONT(NAME) \
    NAME ## _len

#endif

#ifdef __cplusplus
}
#endif

#endif //XRESOURCEMACROS_H
