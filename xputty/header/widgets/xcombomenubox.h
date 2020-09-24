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

#ifndef XCOMBOMENUBOX_H_
#define XCOMBOMENUBOX_H_

#include "xputty.h"
#include "xmenu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief add_combomenubox        - add a combomenubox
 * @param *parent             - pointer to the Widget_t request the button
 * @param x,y,width,height    - the position/geometry to create the button
 * @return Widget_t*          - pointer to the Widget_t button struct
 */

Widget_t* add_combomenubox(Widget_t *parent, const char  * label, int x, int y, int width, int height);

/**
 * @brief combomenubox_add_entry    - add a entry to the combomenubox
 * @param *wid                  - pointer to the Widget_t combomenubox
 * @param *label                - Label to show on the menu
 * @return Widget_t*            - pointer to the Widget_t menu_item struct
 */

Widget_t *combomenubox_add_entry(Widget_t *wid, const char  * label);

/**
 * @brief combomenubox_add_numeric_items - add numeric items from imin to imax to wid
 * @param *wid                       - pointer to the Widget_t combomenubox
 * @param *imin                      - the low number of the numeric items
 * @param *imax                      - the high number of the numeric items
 * @return void
 */

void combomenubox_add_numeric_entrys(Widget_t *wid, int imin, int imax);

/**
 * @brief combomenubox_set_active_entry  - set the active combomenubox entry
 * @param *w_                        - void pointer to the Widget_t combomenubox
 * @param active                     - the active entry (int)
 * @return void
 */

void combomenubox_set_active_entry(Widget_t *w, int active);

#ifdef __cplusplus
}
#endif

#endif //XCOMBOMENUBOX_H_
