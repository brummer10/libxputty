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

#ifndef XTABBOX_H_
#define XTABBOX_H_

#include "xputty.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief tabbox_add_tab      - add a tab to a tabbox Widget_t
 * @param *tabbox             - pointer to the tabbox Widget_t the tab should added to
 * @param *label              - the label of the tab
 * @return Widget_t*          - pointer to the Widget_t tab struct
 */

Widget_t* tabbox_add_tab(Widget_t *tabbox, const char * label);

/**
 * @brief add_tabbox          - add a tabbox to a Widget_t
 * @param *parent             - pointer to the Widget_t request the tabbox 
 * @param *label              - the label of the tabbox
 * @param x,y,width,height    - the position/geometry to create the tabbox
 * @return Widget_t*          - pointer to the Widget_t tabbox struct
 */

Widget_t* add_tabbox(Widget_t *parent, const char * label,
                int x, int y, int width, int height);

/**
 * @brief tabbox_remove_tab   - remove a tab from a tabbox Widget_t
 * @param *tabbox             - pointer to the tabbox Widget_t the tab should removed from
 * @param tab                 - the number of the tab
 * @return void
 */

void tabbox_remove_tab(Widget_t *tabbox, int tab);

#ifdef __cplusplus
}
#endif

#endif //XTABBOX_H_
