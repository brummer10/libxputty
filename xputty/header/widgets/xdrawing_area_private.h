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

#ifndef XDRAWING_AREA_PRIVATE_H_
#define XDRAWING_AREA_PRIVATE_H_

#include "xdrawing_area.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief _draw_image            - internal draw the image to the buffer
 * @param *w_                    - void pointer to the Widget_t image
 * @param *user_data             - void pointer to attached user_data
 */

void _draw_image(void *w_, void* user_data);

#ifdef __cplusplus
}
#endif

#endif //XDRAWING_AREA_PRIVATE_H_
