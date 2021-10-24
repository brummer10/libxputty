/*
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

#ifndef XDRAWING_AREA_H_
#define XDRAWING_AREA_H_

#include "xputty.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief add_image           - add a image to a Widget_t
 * @param *parent             - pointer to the Widget_t request the image
 * @param *label              - Label to show on the image
 * @param x,y,width,height    - the position/geometry to create the image
 * @return Widget_t*          - pointer to the Widget_t image struct
 */

Widget_t* add_image(Widget_t *parent, const char * label,
                int x, int y, int width, int height);

#ifdef __cplusplus
}
#endif

#endif //XDRAWING_AREA_H_
