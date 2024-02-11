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

#ifndef XLISTVIEW_H_
#define XLISTVIEW_H_

#include "xputty.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ViewList_t         - struct to hold information for the listview
 * @param prelight_item      - the list item which has the focus
 * @param active_item        - the list item which is selected(active)
 * @param show_items         - the viewable amount of list items
 * @param list_size          - total amount of list items in list
 * @param check_dir          - check if list item is a directory
 * @param **list_names       - the list entrys
 */

typedef struct {
    char **list_names;
    char *tooltip_text;
    Widget_t *slider;
    cairo_surface_t *folder;
    cairo_surface_t *file;
    int prelight_item;
    int active_item;
    int prev_prelight_item;
    int prev_active_item;
    int show_items;
    int item_height;
    int list_size;
    int check_dir;
    float scale;
    char pad[4];
} ViewList_t;

/**
 * @brief listview_set_active_entry   - set the active listview entry
 * @param *w                         - void pointer to the Widget_t listview
 * @param active                     - the active entry (int)
 */

void listview_set_active_entry(Widget_t *w, int active);

/**
 * @brief listview_unset_active_entry - unset the active listview entry
 * @param *w                          - void pointer to the Widget_t listview
 */

void listview_unset_active_entry(Widget_t *w);

/**
 * @brief create_listview_viewport     - create a viewport on a listview to a Widget_t
 * @param *parent             - pointer to the Widget_t the listview should pop over
 * @param elem                - how many entries to show (max value)
 * @param width               - define the width of the viewport
 * @param height              - define the height of the viewport
 * @return Widget_t*          - pointer to the Widget_t viewport
 */

Widget_t* create_listview_viewport(Widget_t *parent, int elem, int width, int height);

/**
 * @brief add_label           - add a listview to a Widget_t
 * @param *parent             - pointer to the Widget_t request the label
 * @param *label              - Label to show on the button
 * @param x,y,width,height    - the position/geometry to create the label
 * @return Widget_t*          - pointer to the Widget_t label struct
 */

Widget_t* add_listview(Widget_t *parent, const char * label,
                int x, int y, int width, int height);

/**
 * @brief listview_mem_free        - release additional used memory when destroy the Widget_t
 * @param *w_                      - void pointer to the Widget_t
 * @param *user_data               - void pointer to attached user_data
 */

void listview_mem_free(void *w_, void* user_data);

/**
 * @brief listview_remove_list - remove the list from a listview  Widget_t
 * @param *listview            - pointer to the Widget_t listview which should show the list
 */

void listview_remove_list(Widget_t *listview);

/**
 * @brief listview_set_list   - set a list to a listview  Widget_t
 * @param *listview           - pointer to the Widget_t listview which should show the list
 * @param **list              - pointer to the list
 * @param list_size           - how many items contain the list
 */

void listview_set_list(Widget_t *listview, char **list, int list_size);

/**
 * @brief listview_set_check_dir - set optional check if list item is a directory
 * @param *listview              - pointer to the Widget_t listview which should show the list
 * @param set                    - 0 = no check (default); 1 = check
 */

void listview_set_check_dir(Widget_t *listview, int set);

/**
 * @brief listview_set_scale_factor - set optional scale factor for list item
 * @param *listview                 - pointer to the Widget_t listview which should show the list
 * @param set                       - 0.2 = no scale (default); 0.1 = scale down 0.4 = scale up
 */

void listview_set_scale_factor(Widget_t *listview, float set);

#ifdef __cplusplus
}
#endif

#endif //XLISTVIEW_H_
