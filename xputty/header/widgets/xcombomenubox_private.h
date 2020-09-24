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

#ifndef XCOMBOMENUBOX_PRIVATE_H_
#define XCOMBOMENUBOX_PRIVATE_H_

#include "xcombomenubox.h"
#include "xbutton_private.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief _draw_combomenubox_button  - internal draw the combomenubox button to the buffer
 * @param *w_                    - void pointer to the Widget_t combomenubox button
 * @param *user_data             - void pointer to attached user_data
 * @return void
 */

void _draw_combomenubox_button(void *w_, void* user_data);

/**
 * @brief _draw_combomenubox         - internal draw the combomenubox to the buffer
 * @param *w_                    - void pointer to the Widget_t combomenubox
 * @param *user_data             - void pointer to attached user_data
 * @return void
 */

void _draw_combomenubox(void *w_, void* user_data);

/**
 * @brief _combomenubox_button_released  - popup menu on right click
 * @param *w_                        - void pointer to the Widget_t button
 * @param *button                    - void pointer to XEvent.xbutton struct
 * @param *user_data                 - void pointer to attached user_data
 * @return void
 */

void _combomenubox_button_released(void *w_, void* button_, void* user_data);

/**
 * @brief _button_combomenubox_released  - popup the combomenubox menu
 * @param *w_                        - void pointer to the Widget_t button
 * @param *button                    - void pointer to XEvent.xbutton struct
 * @param *user_data                 - void pointer to attached user_data
 * @return void
 */

void _button_combomenubox_released(void *w_, void* button_, void* user_data);

/**
 * @brief _entry_released            - the combomenubox menu release func
 * @param *w_                        - void pointer to the Widget_t menu
 * @param *item                      - void pointer to the selected item *(int*)
 * @param *user_data                 - void pointer to the item label *(const char**)
 * @return void
 */

void _combomenubox_entry_released(void *w_, void* item_, void* user_data);

/**
 * @brief _set_entry                 - set the active combomenubox entry on adjustment change
 * @param *w_                        - void pointer to the Widget_t combomenubox
 * @param *user_data                 - void pointer to the item label *(const char**)
 * @return void
 */

void _combomenubox_set_entry(void *w_, void* user_data);


#ifdef __cplusplus
}
#endif

#endif //XCOMBOMENUBOX_PRIVATE_H_
