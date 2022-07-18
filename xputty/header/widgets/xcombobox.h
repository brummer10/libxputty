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

#ifndef XCOMBOBOX_H_
#define XCOMBOBOX_H_

#include "xputty.h"
#include "xmenu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* comment out xasprintf and define _GNU_SOURCE on top of this file will remove this dependency.*/
#include "xasprintf.h"

/**
 * @brief ComboBox_t         - struct to hold information for the combobox
 * @param prelight_item      - the list item which has the focus
 * @param active_item        - the list item which is selected(active)
 * @param show_items         - the viewable amount of list items
 * @param list_size          - total amount of list items in list
 * @param **list_names       - the list entrys
 */

typedef struct {
    Widget_t *slider;
    Widget_t *combobox;
    int prelight_item;
    int active_item;
    int show_items;
    int list_size;
    char **list_names;
} ComboBox_t;


void pop_combobox_menu_show(Widget_t *parent, Widget_t *menu, bool above);

/**
 * @brief add_combobox        - add a combobox
 * @param *parent             - pointer to the Widget_t request the button
 * @param x,y,width,height    - the position/geometry to create the button
 * @return Widget_t*          - pointer to the Widget_t button struct
 */

Widget_t* add_combobox(Widget_t *parent, const char  * label, int x, int y, int width, int height);

/**
 * @brief combobox_add_entry    - add a entry to the combobox
 * @param *wid                  - pointer to the Widget_t combobox
 * @param *label                - Label to show on the menu
 * @return Widget_t*            - pointer to the Widget_t menu_item struct
 */

void combobox_add_entry(Widget_t *wid, const char  * label);

/**
 * @brief combobox_add_numeric_items - add numeric items from imin to imax to wid
 * @param *wid                       - pointer to the Widget_t combobox
 * @param *imin                      - the low number of the numeric items
 * @param *imax                      - the high number of the numeric items
 * @return void
 */

void combobox_add_numeric_entrys(Widget_t *wid, int imin, int imax);

/**
 * @brief combobox_set_active_entry  - set the active combobox entry
 * @param *w_                        - void pointer to the Widget_t combobox
 * @param active                     - the active entry (int)
 * @return void
 */

void combobox_set_active_entry(Widget_t *w, int active);

/**
 * @brief combobox_mem_free        - release additional used memory when destroy the Widget_t
 * @param *w_                      - void pointer to the Widget_t
 * @param *user_data               - void pointer to attached user_data
 * @return void
 */

void combobox_mem_free(void *w_, void* user_data);

/**
 * @brief combobox_delete_entrys   - free the list hold the combobox entrys
 * @param *combobox                - void pointer to the Widget_t combobox
 * @return void
 */

void combobox_delete_entrys(Widget_t *combobox);

/**
 * @brief combobox_set_menu_size     - set the number of entrys shown in the pop menu
 * @param *combobox                  - pointer to the Widget_t combobox
 * @param v                          - the max entry count (int)
 * @return void
 */

void combobox_set_menu_size(Widget_t *combobox, int v);

#ifdef __cplusplus
}
#endif

#endif //XCOMBOBOX_H_
