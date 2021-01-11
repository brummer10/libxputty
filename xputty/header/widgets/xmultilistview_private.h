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

#ifndef XMULTILISTVIEW_PRIVATE_H_
#define XMULTILISTVIEW_PRIVATE_H_

#include "xmultilistview.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief _draw_multi_listview           - draw listview on expose call
 * @param *w_                  - the listview to draw
 * @param *user_data           - attached user_data
 * @return void
 */

void _draw_multi_listview(void *w_, void* user_data);

/**
 * @brief _draw_multi_list           - draw list on expose call
 * @param *w_                  - the Widget_t to draw
 * @param *user_data           - attached user_data
 * @return void
 */

void _draw_multi_list(void *w_, void* user_data);

/**
 * @brief _multi_list_motion             - follow mouse pointer to set prelight item 
 * @param *w_                      - void pointer to the Widget_t listview
 * @param *xmotion_                - void pointer to the XMotionEvent
 * @param *user_data               - void pointer to attached user_data
 * @return void
 */

void _multi_list_motion(void *w_, void* xmotion_, void* user_data);

/**
 * @brief _multi_list_key_pressed        - move listview or set active entry 
 * @param *w_                      - void pointer to the Widget_t listview
 * @param *button_                 - void pointer to the XButtonEvent
 * @param *user_data               - void pointer to attached user_data
 * @return void
 */

void _multi_list_key_pressed(void *w_, void* xkey_, void* user_data);

/**
 * @brief _multi_list_entry_released     - move listview or set active entry 
 * @param *w_                      - void pointer to the Widget_t listview
 * @param *button_                 - void pointer to the XButtonEvent
 * @param *user_data               - void pointer to attached user_data
 * @return void
 */

void _multi_list_entry_released(void *w_, void* button_, void* user_data);

/**
 * @brief _multi_list_entry_double_clicke - forward double click from list entry to list view 
 * @param *w_                       - void pointer to the Widget_t listview
 * @param *button_                  - void pointer to the XButtonEvent
 * @param *user_data                - void pointer to attached user_data
 * @return void
 */

void _multi_list_entry_double_clicked(void *w_, void* button_, void* user_data);

/**
 * @brief _leave_multi_list          - draw list on pointer leave
 * @param *w_                  - the Widget_t to draw
 * @param *user_data           - attached user_data
 * @return void
 */

void _leave_multi_list(void *w_, void* user_data);

/**
 * @brief _reconfigure_multi_listview_viewport - reconfigure the viewport adjustment
 * on size changes
 * @param *w_                      - void pointer to view_port
 * @param *user_data               - attached user_data
 * @return void
 */

void _reconfigure_multi_listview_viewport(void *w_, void* user_data);

/**
 * @brief _draw_multi_listviewslider     - draw a slider on the viewport
 * to indicate the view point
 * @param *w_                      - void pointer to view_port
 * @param *user_data               - attached user_data
 * @return void
 */

void _draw_multi_listviewslider(void *w_, void* user_data);

/**
 * @brief _set_multi_listview_viewpoint  - move the view_port to position
 * @param *w_                      - void pointer to view_port
 * @param *user_data               - attached user_data
 * @return void
 */

void _set_multi_listview_viewpoint(void *w_, void* user_data);
/**
 * @brief _set_multi_listviewport        - move the view_port to position
 * by listviewslider state
 * @param *w_                      - void pointer to view_port
 * @param *user_data               - attached user_data
 * @return void
 */

void _set_multi_listviewport(void *w_, void* user_data);

#ifdef __cplusplus
}
#endif

#endif //XMULTILISTVIEW_PRIVATE_H_
