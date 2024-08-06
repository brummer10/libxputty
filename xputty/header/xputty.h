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

/**
 * @file xputty.h
 * @brief This file contains definitions and structs used on all platforms.
 * Platform specific definitions are located in xwidget-platform.h
 * Xlib compatibility definitions for MSWindows are located in xputty-mswin.h.
 */

#pragma once

#ifndef XPUTTY1_H_
#define XPUTTY1_H_

#if defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#ifndef _OS_UNIX_
#define _OS_UNIX_ 1
#endif
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <limits.h>

#include <math.h>
#include <cairo.h>
#include "xputty-mswin.h" // no ifdef needed

#ifdef _OS_UNIX_
#include <cairo-xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h> 
#include <X11/Xresource.h>
#endif //_OS_UNIX_

#ifdef ENABLE_NLS
#include <libintl.h>
#include <locale.h>
#define _(S) gettext(S)
#else
#define _(S) S
#endif


#ifdef __cplusplus
extern "C" {
#endif

#ifdef _OS_UNIX_
#define DND_STATUS_ACCEPT(e) ((e)->xclient.data.l[1] & 0x1L)
#define DND_VERSION(e) ((e)->xclient.data.l[1] >> 24)
#define DND_SOURCE_WIN(e) ((e)->xclient.data.l[0])
#define DND_DROP_TIME(e) ((e)->xclient.data.l[2])
#endif //_OS_UNIX_


/*---------------------------------------------------------------------
-----------------------------------------------------------------------    
                    define debug print
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

#ifndef DEBUG
#define DEBUG 0
#ifndef NDEBUG
#define NDEBUG // switch of assertion checks
#endif
#endif
#ifndef LIBDEBUG
#define LIBDEBUG 0
#endif
/**
 * @brief debug_print         - print out state messages when compiled with
 * the -DLIBDEBUG flag
 */


#ifdef NODEF // _WIN32 //DebugPrint
#define debug_print(...) \
            { char xxdeb[1024]; snprintf(xxdeb, 1024, __VA_ARGS__); OutputDebugString(xxdeb); }
#else
#define debug_print(...) \
            ((void)((LIBDEBUG) ? fprintf(stderr, __VA_ARGS__) : 0))
#endif

/*---------------------------------------------------------------------
-----------------------------------------------------------------------    
                define min/max if not defined already
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

/**
 * @brief min         - set a maximal value (x) as return value
 */

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

/**
 * @brief max         - set a minimal value (x) as return value
 */

#ifndef max
#define max(x, y) ((x) < (y) ? (y) : (x))
#endif

/*---------------------------------------------------------------------
-----------------------------------------------------------------------    
                    define check if char holds UTF 8 string
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

/**
 * @brief IS_UTF8         - check if a char contain UTF 8 formated signs
 */

#define IS_UTF8(c) (((c)&0xc0)==0xc0)

/*---------------------------------------------------------------------
-----------------------------------------------------------------------    
                forward declareted structs
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

/**
 * @brief Childlist_t       - maintain a Widget_t list of childs for a Widget_t
 */

typedef struct Childlist_t Childlist_t;

/**
 * @brief Adjustment_t       - Adjustment_t for a Widget_t
 */

typedef struct Adjustment_t Adjustment_t ;

/**
 * @brief Widget_t           - the Widget_t base struct
 */

typedef struct Widget_t Widget_t;

/**
 * @brief XColor_t           - the Widget_t Color struct
 */

typedef struct XColor_t XColor_t;

/**
 * @brief SystrayColor_t     - the Systray Icon background Color
 */

typedef struct SystrayColor_t SystrayColor_t;

/**
 * @brief  Xputty          - the main struct.It should be declared
 * before any other call to a Xputty function.
 */

typedef struct Xputty Xputty;

/**
 * @brief *xfunc       - function pointer to connect XEvents from a Widget_t to a event handler
 * @param *widget      - void pointer to the widget
 * @param *user_data   - void pointer to attached user_data, maybe NULL
 */

typedef void (*xfunc)(void * widget, void* user_data);

#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------
-----------------------------------------------------------------------    
                xputty library headers
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

// library header
#include "xwidget.h"
#include "xadjustment.h"
#include "xchildlist.h"
#include "xcolor.h"
#include "xpngloader.h"
#include "xsvgloader.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Xputty             - the main struct. 
 * \n It should be declared before any other call to a Xputty function.
 * \n Xputty store a pointer in the childlist,
 *  to any Widget_t related to this instance of libxputty.
 * \n The first created Widget_t is the toplevel window.
 * \n When the toplevel window call destroy_widget(), Xputty call
 * destroy_widget() for all remaining Widget_t's in the main childlist.
 * \n So any allocated memory should be released before the
 * toplevel window finaly close.
 * @param *childlist         - pointer to the main childlist
 * @param *dpy               - pointer to the display in use
 * @param run                - bool to quit the main loop
 * @param *color_scheme      - theming scheme for all Widget_t
 * @param *hold_grab         - pointer to a modal Widget_t
 */

struct Xputty{
/** pointer to the main childlist */
    Childlist_t *childlist;
/** pointer to the display in use */
    Display *dpy;
/** theming scheme for all Widget_t */
    XColor_t *color_scheme;
/** background color for Sytray Icon */
    SystrayColor_t *systray_color;
/** pointer to a modal Widget_t */
    Widget_t *hold_grab;
/** pointer to a Widget_t snooping keys*/
    Widget_t *key_snooper;
/** pointer to a modal sub Widget_t */
    Widget_t *submenu;
/** pointer to the textbuffer for copy/paste */
    unsigned char *ctext;
/** small fontsize for all Widget_t*/
    int small_font;
/** normal fontsize  for all Widget_t*/
    int normal_font;
/** big fontsize  for all Widget_t*/
    int big_font;
/** size of the textbuffer for copy/paste */
    int csize;
/** Atoms for handling drag and drop */
    int dnd_version;
/** bool to quit the main loop */
    bool run;
/** bool to indicate if button is pressed on grab window */
    bool is_grab;
/** float to hold the current dpi setting of the Display */
    float hdpi;
/** padding struct to align clean **/
    char pad[2];
    Window dnd_source_window;
    Atom XdndAware;
    Atom XdndTypeList;
    Atom XdndSelection;
    Atom XdndStatus;
    Atom XdndEnter;
    Atom XdndPosition;
    Atom XdndLeave;
    Atom XdndDrop;
    Atom XdndActionCopy;
    Atom XdndFinished;

    Atom dnd_type_text;
    Atom dnd_type_uri;
    Atom dnd_type_utf8;
    Atom dnd_type;

    Atom selection;
    Atom targets_atom;
    Atom text_atom;
    Atom UTF8;
};

/**
 * @brief main_init         - open the Display and init the
 * main->childlist.
 * \n Set the bool run to true.
 * \n The bool run is used to terminate the main event loop.
 * \n main_init() should be called directly after the declaration of Xputty
 * before the first Widget_t get created.
 * \n Any Widget_t created afterwards will be added to the main childlist.
 * \n The main childlist is used to check if a Widget_t is valid to receive a Event.
 * \n Xputty check if a Widget_t is registerd in the main childlist, and only forward
 * events when it found the Widget_t in the list.
 * \n When a Widget_t call destroy_widget() any childs of this Widget_t receive
 * a call to destroy_widget() to release there memory, they get removed from the main childlist
 * and finaly the Widget_t itself will be removed from the main childlist as well.
 * On main_quit() any remaining Widget_t from the main childlist will be destroyed,
 * to ensure that we leave the memory clean.
 * @param *main             - pointer to the main Xputty struct
 */

void main_init(Xputty *main);

/**
 * @brief main_run          - start the main event loop.
 * \n It should be start after your Widget_t's been created.
 * \n You could create and destroy additional Widget_t's
 * at any time later during run.
 * @param *main             - pointer to the main Xputty struct
 */

void main_run(Xputty *main);

/**@example simple-example.c
 * Simple example of how to use this
 */

/**
 * @brief run_embedded      - the main event loop to run embedded UI's.
 * \n It should be start after your Widget_t's been created.
 * \n You could create and destroy additional Widget_t's
 * at any time later during run.
 * @param *main             - pointer to the main Xputty struct
 */

void run_embedded(Xputty *main);

/**
 * @brief main_quit         - destroy all remaining Widget_t's from the
 * main->childlist.
 * \n Free all resources which may be allocated between init
 * and quit.
 * \n It should be called after main_run()/run_embedded();
 * @param *main             - pointer to the main Xputty struct
 */

void main_quit(Xputty *main);

#ifdef __cplusplus
}
#endif

#endif //XPUTTY_H_

