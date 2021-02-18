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

#ifndef XMENU_H_
#define XMENU_H_

#include "xputty.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief pop_menu_show       - pop up a menu to a Widget_t
 * @param *parent             - pointer to the Widget_t the menu should pop over
 * @param *menu               - the menu to show
 * @return void
 */

void pop_menu_show(Widget_t *parent, Widget_t *menu, int elem, bool above);

/**
 * @brief pop_submenu_show    - pop up a submenu to a Widget_t
 * @param *parent             - pointer to the Widget_t the menu should pop over
 * @param *menu               - the menu to show
 * @return void
 */

void pop_submenu_show(Widget_t *parent, Widget_t *menu, int elem, bool above);

/**
 * @brief create_viewport     - create a viewport on a menu to a Widget_t
 * @param *parent             - pointer to the Widget_t the menu should pop over
 * @param width               - define the width of the viewport
 * @param height              - define the height of the viewport
 * @return Widget_t*          - pointer to the Widget_t viewport
 */

Widget_t* create_viewport(Widget_t *parent, int width, int height);

/**
 * @brief add_menu            - add a menu to a Widget_t
 * @param *parent             - pointer to the Widget_t the menu should pop over
 * @param *label              - the label of the menu
 * @param x,y,width,height    - the position/geometry to create the menu
 * @return Widget_t*          - pointer to the Widget_t menu struct
 */

Widget_t* add_menu(Widget_t *parent, const char * label,
                        int x, int y, int width, int height);

/**
 * @brief add_menubar         - add a menubar to a Widget_t
 * @param *parent             - pointer to the Widget_t hold the menubar
 * @param *label              - unused
 * @param x,y,width,height    - the position/geometry to create the menubar
 * @return Widget_t*          - pointer to the Widget_t menu struct
 */

Widget_t *add_menubar(Widget_t *parent, const char * label,
                        int x, int y, int width, int height);

/**
 * @brief menubar_add_menu    - add a menu to a menubar
 * @param *wid                - pointer to the Widget_t menu
 * @param *label              - Label to show on the menu
 * @return Widget_t*          - pointer to the Widget_t menu_item struct
 */

Widget_t *menubar_add_menu(Widget_t *parent, const char * label);

/**
 * @brief menu_add_entry      - add a entry to menu
 * @param *wid                - pointer to the Widget_t menu
 * @param *label              - Label to show on the menu
 * @return Widget_t*          - pointer to the Widget_t menu_item struct
 */

Widget_t *menu_add_entry(Widget_t *wid, const char  * label);

/**
 * @brief menu_add_radio_entry- add a radio entry to menu
 * @param *wid                - pointer to the Widget_t menu
 * @param *label              - Label to show on the menu
 * @return Widget_t*          - pointer to the Widget_t menu_item struct
 */

Widget_t *menu_add_radio_entry(Widget_t *wid, const char  * label);

/**
 * @brief menu_add_check_entry- add a check entry to menu
 * @param *wid                - pointer to the Widget_t menu
 * @param *label              - Label to show on the menu
 * @return Widget_t*          - pointer to the Widget_t menu_item struct
 */

Widget_t *menu_add_check_entry(Widget_t *wid, const char  * label);

/**
 * @brief menu_add_accel_check_entry- add a check entry with accel to menu
 * @param *wid                - pointer to the Widget_t menu
 * @param *label              - Label to show on the menu
 * @return Widget_t*          - pointer to the Widget_t menu_item struct
 */

Widget_t *menu_add_accel_check_entry(Widget_t *wid, const char  * label);
/**
 * @brief menu_add_submenu    - add a submenu to a Widget_t
 * @param *parent             - pointer to the Widget_t the menu should pop over
 * @param *label              - the label of the menu
 * @return Widget_t*          - pointer to the Widget_t submenu struct
 */

Widget_t *menu_add_submenu(Widget_t *wid, const char  * label);

/**
 * @brief cmenu_add_submenu   - add a submenu to a create_menu
 * @param *parent             - pointer to the Widget_t the menu should pop over
 * @param *label              - the label of the menu
 * @return Widget_t*          - pointer to the Widget_t submenu struct
 */

Widget_t *cmenu_add_submenu(Widget_t *wid, const char  * label);

/**
 * @brief create_menu         - create a menu to a Widget_t
 * @param *parent             - pointer to the Widget_t the menu should pop over
 * @param height              - define the height of a single menu item
 * @return Widget_t*          - pointer to the Widget_t button struct
 */

Widget_t* create_menu(Widget_t *parent, int height);

/**
 * @brief menu_add_item       - add a item to menu
 * @param *menu               - pointer to the Widget_t menu
 * @param *label              - Label to show on the menu
 * @return Widget_t*          - pointer to the Widget_t menu_item struct
 */

Widget_t* menu_add_item(Widget_t *menu, const char * label);

/**
 * @brief menu_remove_item    - remove a item from menu
 * @param *menu               - pointer to the Widget_t menu
 * @return Widget_t*          - pointer to the Widget_t menu_item to be removed
 * @return void
 */

void menu_remove_item(Widget_t *menu, Widget_t *item);

/**
 * @brief menu_add_accel_item - add a accel item to menu
 * @param *menu               - pointer to the Widget_t menu
 * @param *label              - Label to show on the menu
 * @return Widget_t*          - pointer to the Widget_t menu_item struct
 */

Widget_t* menu_add_accel_item(Widget_t *menu, const char * label);

/**
 * @brief menu_add_check_item - add a item to menu
 * @param *menu               - pointer to the Widget_t menu
 * @param *label              - Label to show on the menu
 * @return Widget_t*          - pointer to the Widget_t menu_item struct
 */

Widget_t* menu_add_check_item(Widget_t *menu, const char * label);

/**
 * @brief radio_item_set_active       - activate selected radio item
 * @param *w                          - the Widget_t to activate
 * @return void
 */

void radio_item_set_active(Widget_t *w);

/**
 * @brief set_active_radio_item       - activate selected radio item by number
 * @param *w                          - the Widget_t hold the menu
 * @return void
 */

void set_active_radio_entry_num(Widget_t *w, int active);

/**
 * @brief menu_add_radio_item - add a radio item to menu
 * @param *menu               - pointer to the Widget_t menu
 * @param *label              - Label to show on the menu
 * @return Widget_t*          - pointer to the Widget_t menu_item struct
 */

Widget_t* menu_add_radio_item(Widget_t *menu, const char * label);

/**
 * @brief menu_add_numeric_items - add numeric items from imin to imax to menu
 * @param *menu                  - pointer to the Widget_t menu
 * @param *imin                  - the low number of the numeric items
 * @param *imax                  - the high number of the numeric items
 * @return void
 */

void menu_add_numeric_items(Widget_t *menu, int *imin, int *imax);

#ifdef __cplusplus
}
#endif

#endif //XMENU_H_
