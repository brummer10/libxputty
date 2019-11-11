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

#ifndef XCOLOR_H_
#define XCOLOR_H_

#include "xputty.h"


/**
 * 
 * @brief Color_state         - select color mode to use on Widget_t
 */

typedef enum {
    NORMAL_,
    PRELIGHT_,
    SELECTED_,
    ACTIVE_,
} Color_state;

/**
 * 
 * @brief Color_set         - select color set to use on draw
 */

typedef enum {
    FORGROUND_,
    BACKGROUND_,
    BASE_,
    TEXT_,
    SHADOW_,
    FRAME_,
    LIGHT_,
} Color_mod;

/**
 * 
 * @brief Color_t               - struct used to set cairo color for Widget_t
 * @param fg[4]                 - forground {red, green, blue, alpha}
 * @param bg[4]                 - background {red, green, blue, alpha}
 * @param ba[4]                 - base {red, green, blue, alpha}
 */

typedef struct {
    double fg[4];
    double bg[4];
    double base[4];
    double text[4];
    double shadow[4];
    double frame[4];
    double light[4];
} Colors;

/**
 * @brief XColor_t           - the Widget_t Color struct
 */

struct XColor_t {
    Colors normal;
    Colors prelight;
    Colors selected;
    Colors active;
};

/**
 * @brief set_dark_theme  - init color shema to default values 
 * @param *main             - pointer to the main Xputty struct
 * @return void
 */

void set_dark_theme(Xputty *main);

/**
 * @brief set_light_theme  - init color shema to light theme 
 * @param *main             - pointer to the main Xputty struct
 * @return void
 */

void set_light_theme(Xputty *main);

/**
 * @brief get_color_scheme  - init color shema to default values 
 * @param *st               - the Color state to use
 * @return void
 */

Colors *get_color_scheme(Xputty *main, Color_state st);

/**
 * @brief get_color_state   - get the Color_state to use
 * @param *wid              - pointer to the Widget_t
 * @return Color_state      - the Color_state related to the Widget_t state
 */

Color_state get_color_state(Widget_t *wid);

/**
 * @brief use_fg_color_scheme  - set normal forground color for Widget_t
 * @param w                    - the Widget_t to send the event to
 * @param st                   - the Color_state to use
 * @return void
 */

void use_fg_color_scheme(Widget_t *w, Color_state st);

/**
 * @brief use_bg_color_scheme  - set normal background color for Widget_t
 * @param w                    - the Widget_t to send the event to
 * @param st                   - the Color_state to use
 * @return void
 */

void use_bg_color_scheme(Widget_t *w, Color_state st);

/**
 * @brief use_base_color_scheme  - set base color for Widget_t
 * @param w                      - the Widget_t to send the event to
 * @param st                     - the Color_state to use
 * @return void
 */

void use_base_color_scheme(Widget_t *w, Color_state st);

/**
 * @brief use_text_color_scheme  - set text color for Widget_t
 * @param w                      - the Widget_t to send the event to
 * @param st                     - the Color_state to use
 * @return void
 */

void use_text_color_scheme(Widget_t *w, Color_state st);

/**
 * @brief use_shadow_color_scheme  - set shadow color for Widget_t
 * @param w                        - the Widget_t to send the event to
 * @param st                       - the Color_state to use
 * @return void
 */

void use_shadow_color_scheme(Widget_t *w, Color_state st);

/**
 * @brief use_frame_color_scheme   - set frame color for Widget_t
 * @param w                        - the Widget_t to send the event to
 * @param st                       - the Color_state to use
 * @return void
 */

void use_frame_color_scheme(Widget_t *w, Color_state st);

/**
 * @brief use_light_color_scheme   - set light color for Widget_t
 * @param w                        - the Widget_t to send the event to
 * @param st                       - the Color_state to use
 * @return void
 */

void use_light_color_scheme(Widget_t *w, Color_state st);

/**
 * @brief set_pattern       - set pattern for the selected colors
 * @param *w_               - void pointer to the Widget_t button
 * @param *from             - the Colors set to use from
 * @param *to               - the Colors set to use to
 * @param mod               - the Color_mod to use
 * @param width             - the width of the base
 * @return void
 */

void set_pattern(Widget_t *w, Colors *from, Colors *to, Color_mod mod);

#endif //XCOLOR_H_
