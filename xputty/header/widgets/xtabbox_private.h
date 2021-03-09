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

#ifndef XTABBOX_PRIVATE_H_
#define XTABBOX_PRIVATE_H_

#include "xtabbox.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief _draw_tabbox         - draw the tabbox on expose call
 * @param *w_                  - the tabbox to draw
 * @param *user_data           - attached user_data
 * @return void
 */

void _draw_tabbox(void *w_, void* user_data);

/**
 * @brief _draw_tab            - draw a single tab on expose call
 * @param *w_                  - the tab to draw
 * @param *user_data           - attached user_data
 * @return void
 */

void _draw_tab(void *w_, void* user_data);

/**
 * @brief _tab_button_released   - select the tab to show (hide all other)
 * @param *button                - the xbutton which is pressed
 * @param *user_data             - attached user_data
 * @return void
 */

void _tab_button_released(void *w_, void* button_, void* user_data);

#ifdef __cplusplus
}
#endif

#endif //XTABBOX_PRIVATE_H_
