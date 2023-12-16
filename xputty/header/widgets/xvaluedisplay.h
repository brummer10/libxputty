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

#ifndef XVALUEDISPLAY_H_
#define XVALUEDISPLAY_H_

#include "xputty.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief add_valuedisplay    - add a valuedisplay widget to a Widget_t
 * @param *parent             - pointer to the Widget_t request the valuedisplay
 * @param *label              - Label to show on the valuedisplay
 * @param x,y,width,height    - the position/geometry to create the valuedisplay
 * @return Widget_t*          - pointer to the Widget_t valuedisplay struct
 */

Widget_t* add_valuedisplay(Widget_t *parent, const char * label,
                int x, int y, int width, int height);

/**
 * @brief add_popup_spinbox   - add a popup spinbox to a valuedisplay
 * this is implemented in the valuedisplay via double click
 * @param *parent             - pointer to the Widget_t  valuedisplay
 * @param *label              - Label to show on the valuedisplay stub
 * @param x,y,width,height    - the position/geometry to create the spinbox
 * the spinbox will popup above the valuedisplay anyway
 * @return Widget_t*          - pointer to the Widget_t valuedisplay struct
 */

Widget_t* add_popup_spinbox(Widget_t *parent, const char * label,
                int x, int y, int width, int height);

#ifdef __cplusplus
}
#endif

#endif //XVALUEDISPLAY_H_
