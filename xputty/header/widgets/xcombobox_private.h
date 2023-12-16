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

#ifndef XCOMBOBOX_PRIVATE_H_
#define XCOMBOBOX_PRIVATE_H_

#include "xcombobox.h"
#include "xbutton_private.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief _draw_combobox_button  - internal draw the combobox button to the buffer
 * @param *w_                    - void pointer to the Widget_t combobox button
 * @param *user_data             - void pointer to attached user_data
 */

void _draw_combobox_button(void *w_, void* user_data);

/**
 * @brief _draw_combobox         - internal draw the combobox to the buffer
 * @param *w_                    - void pointer to the Widget_t combobox
 * @param *user_data             - void pointer to attached user_data
 */

void _draw_combobox(void *w_, void* user_data);

/**
 * @brief _draw_combobox_menu    - internal draw the combobox menu to the buffer
 * @param *w_                    - void pointer to the Widget_t combobox
 * @param *user_data             - void pointer to attached user_data
 */

void _draw_combobox_menu(void *w_, void* user_data);

/**
 * @brief _draw_combobox_entrys  - internal draw the combobox entrys to the buffer
 * @param *w_                    - void pointer to the Widget_t combobox
 * @param *user_data             - void pointer to attached user_data
 */

void _draw_combobox_entrys(void *w_, void* user_data);

/**
 * @brief _combobox_motion           - mouse pointer move in viewport
 * @param *w_                        - void pointer to the Widget_t viewport
 * @param *xmotion_                  - void pointer to XEvent.xmotion struct
 * @param *user_data                 - void pointer to attached user_data
 */

void _combobox_motion(void *w_, void* xmotion_, void* user_data);

/**
 * @brief _combobox_key_released     - viewport entry released key
 * @param *w_                        - void pointer to the Widget_t viewport
 * @param *xkey_                     - void pointer to XEvent.xkey struct
 * @param *user_data                 - void pointer to attached user_data
 */

void _combobox_key_pressed(void *w_, void* xkey_, void* user_data);

/**
 * @brief _combobox_entry_released   - viewport entry released mouse button
 * @param *w_                        - void pointer to the Widget_t viewport
 * @param *button_                   - void pointer to XEvent.xbutton struct
 * @param *user_data                 - void pointer to attached user_data
 */

void _combobox_entry_released(void *w_, void* button_, void* user_data);

/**
 * @brief _leave_combobox         - mouse pointer leave the combobox
 * @param *w_                     - void pointer to the Widget_t combobox
 * @param *user_data              - void pointer to attached user_data
 */

void _leave_combobox(void *w_, void* user_data);

/**
 * @brief _reconfigure_combobox_viewport - set slider scale and step to match viewport entrys
 * @param *w_                            - void pointer to the Widget_t combobox
 * @param *user_data                     - void pointer to attached user_data
 */

void _reconfigure_combobox_viewport(void *w_, void* user_data);

/**
 * @brief _set_combobox_viewpoint     - set the slider state from viewport position
 * @param *w_                         - void pointer to the Widget_t combobox
 * @param *user_data                  - void pointer to attached user_data
 */

void _set_combobox_viewpoint(void *w_, void* user_data);

/**
 * @brief _draw_combobox_menu_slider  - internal draw the combobox menu slider
 * @param *w_                         - void pointer to the Widget_t combobox
 * @param *user_data                  - void pointer to attached user_data
 */

void _draw_combobox_menu_slider(void *w_, void* user_data);

/**
 * @brief _set_combobox_menu_viewport - set the viewport position from slider state
 * @param *w_                         - void pointer to the Widget_t combobox
 * @param *user_data                  - void pointer to attached user_data
 */

void _set_combobox_menu_viewport(void *w_, void* user_data);

/**
 * @brief _combobox_button_released  - popup menu on right click
 * @param *w_                        - void pointer to the Widget_t button
 * @param *button_                   - void pointer to XEvent.xbutton struct
 * @param *user_data                 - void pointer to attached user_data
 */

void _combobox_button_released(void *w_, void* button_, void* user_data);

/**
 * @brief _button_combobox_released  - popup the combobox menu
 * @param *w_                        - void pointer to the Widget_t button
 * @param *button_                   - void pointer to XEvent.xbutton struct
 * @param *user_data                 - void pointer to attached user_data
 */

void _button_combobox_released(void *w_, void* button_, void* user_data);

/**
 * @brief _entry_released            - the combobox menu release func
 * @param *w_                        - void pointer to the Widget_t menu
 * @param *item_                     - void pointer to the selected item *(int*)
 * @param *user_data                 - void pointer to the item label *(const char**)
 */

void _entry_released(void *w_, void* item_, void* user_data);

/**
 * @brief _set_entry                 - set the active combobox entry on adjustment change
 * @param *w_                        - void pointer to the Widget_t combobox
 * @param *user_data                 - void pointer to the item label *(const char**)
 */

void _set_entry(void *w_, void* user_data);

/**
 * @brief _configure_combobox_menu   - set final size and position of menu to a Widget_t
 * @param *parent                    - pointer to the Widget_t the menu should pop over
 * @param *menu                      - the menu to show
 * @param elem                       - how many entrys to show
 * @param above                      - show menu below (0) or beside (1) the combobox
 */

void _configure_combobox_menu(Widget_t *parent, Widget_t *menu, int elem, bool above);


#ifdef __cplusplus
}
#endif

#endif //XCOMBOBOX_PRIVATE_H_
