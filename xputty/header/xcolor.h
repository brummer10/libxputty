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

#ifdef __cplusplus
extern "C" {
#endif


/**
 * 
 * @brief Color_state         - select color mode to use on Widget_t
 * @param NORMAL_             - normal state 
 * @param PRELIGHT_           - prelight state (mouse is hover) 
 * @param SELECTED_           - selected state 
 * @param ACTIVE_             - active state (mouse is pressed) 
 * @param INSENSITIVE_        - Widget_t can't react on mouse/key input
 */

typedef enum {
    NORMAL_,
    PRELIGHT_,
    SELECTED_,
    ACTIVE_,
    INSENSITIVE_,
} Color_state;

/**
 * 
 * @brief Color_mod            - select color mod to use on draw
 * @param FORGROUND_           - use the foreground color set 
 * @param BACKGROUND_          - use the background color set 
 * @param BASE_                - use the base color set 
 * @param TEXT_                - use the text color set 
 * @param SHADOW_              - use the shadow color set 
 * @param FRAME_               - use the frame color set 
 * @param LIGHT_               - use the light color set 
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
 * @param base[4]               - base {red, green, blue, alpha}
 * @param text[4]               - text {red, green, blue, alpha}
 * @param shadow[4]             - shadow {red, green, blue, alpha}
 * @param frame[4]              - frame {red, green, blue, alpha}
 * @param light[4]              - light {red, green, blue, alpha}
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
 * XColor_t could be used for theming you Widget_t set
 * @param normal             - Colors for normal state 
 * @param prelight           - Colors for prelight state (mouse is hover) 
 * @param selected           - Colors for selected state 
 * @param active             - Colors for active state (mouse is pressed) 
 * @param insensitive        - Colors for Widget_t can't react on mouse/key input
 */

struct XColor_t {
    Colors normal;
    Colors prelight;
    Colors selected;
    Colors active;
    Colors insensitive;
};

/**
 * @brief SystrayColor         - the Systray Color struct
 * SystrayColor could be used for the backgroung color of the SystrayIcon
 * @param r             - red channel
 * @param g             - green channel
 * @param b             - blue channel
 * @param a             - alpha channel 
 */

struct SystrayColor_t{
    double r;
    double g;
    double b;
    double a;
};

/**
 * @brief set_dark_theme    - init the XColor_t struct to the default 
 * dark theme 
 * @param *main             - pointer to the main Xputty struct
 */

void set_dark_theme(Xputty *main);

/**
 * @brief set_light_theme   - init the XColor_t struct to the default 
 * light theme 
 * @param *main             - pointer to the main Xputty struct
 */

void set_light_theme(Xputty *main);

/**
 * @brief color_scheme_to_childs    - copy a colorscheme to all childs 
 * light theme 
 * @param *wid                      - pointer to the Widget_t take the colorscheme from
 */

void color_scheme_to_childs(Widget_t *wid);


/**
 * @brief set_widget_color  - set a individual Widget_t color
 * @param *w                - the Widget_t the color belong to
 * @param st                - the state the color belong to
 * @param *mod              - the Color mod to set
 * @param r                 - the red color
 * @param g                 - the green color
 * @param b                 - the blue color
 * @param a                 - the alpha channel
 */

void set_widget_color(Widget_t *w, Color_state st, Color_mod mod,
                            double r, double g, double b, double a);

/**
 * @brief widget_set_color  - set a individual Widget_t color
 * @param *c                - direct pointer to the color array
 * @param r                 - the red color
 * @param g                 - the green color
 * @param b                 - the blue color
 * @param a                 - the alpha channel
 */

void widget_set_color(double *c, double r, double g, double b, double a);

/**
 * @brief get_color_scheme  - get pointer to the Colors struct to use
 * in relation to the Color_state
 * @param *wid              - pointer to the Widget_t
 * @param *st               - the Color state to use
 * @return *Colors          - pointer to the Widget_t Colors struct 
 */

Colors *get_color_scheme(Widget_t *wid, Color_state st);

/**
 * @brief get_color_state   - get the Color_state to use in relation to the
 * Widget_t state
 * @param *wid              - pointer to the Widget_t
 * @return Color_state      - the Color_state related to the Widget_t state
 */

Color_state get_color_state(Widget_t *wid);

/**
 * @brief use_fg_color_scheme  - use forground Colors to paint on Widget_t
 * @param *w                   - the Widget_t to set the Colors
 * @param st                   - the Color_state to use
 */

void use_fg_color_scheme(Widget_t *w, Color_state st);

/**
 * @brief use_bg_color_scheme  - use background Colors to paint on Widget_t
 * @param w                    - the Widget_t to set the Colors
 * @param st                   - the Color_state to use
 */

void use_bg_color_scheme(Widget_t *w, Color_state st);

/**
 * @brief use_base_color_scheme  - use base Colors to paint on Widget_t
 * @param w                      - the Widget_t to set the Colors
 * @param st                     - the Color_state to use
 */

void use_base_color_scheme(Widget_t *w, Color_state st);

/**
 * @brief use_text_color_scheme  - use text Colors to paint on Widget_t
 * @param w                      - the Widget_t to set the Colors
 * @param st                     - the Color_state to use
 */

void use_text_color_scheme(Widget_t *w, Color_state st);

/**
 * @brief use_shadow_color_scheme  - use shadow Colors to paint on Widget_t
 * @param w                        - the Widget_t to set the Colors
 * @param st                       - the Color_state to use
 */

void use_shadow_color_scheme(Widget_t *w, Color_state st);

/**
 * @brief use_frame_color_scheme   - use frame Colors to paint on Widget_t
 * @param w                        - the Widget_t to set the Colors
 * @param st                       - the Color_state to use
 */

void use_frame_color_scheme(Widget_t *w, Color_state st);

/**
 * @brief use_light_color_scheme   - use light Colors to paint on Widget_t
 * @param w                        - the Widget_t to set the Colors
 * @param st                       - the Color_state to use
 */

void use_light_color_scheme(Widget_t *w, Color_state st);

/**
 * @brief use_systray_color        - use systray Colors to paint on Widget_t
 * @param w                        - the Widget_t to set the Colors
 */

void use_systray_color(Widget_t *w);

/**
 * @brief set_systray_color        - set the systray background color
 * @param main                     - pointer to the main Xputty struct
 * @param r                        - the red Color to use
 * @param g                        - the green Color to use
 * @param b                        - the blue Color to use
 * @param a                        - the alpha Cannel to use
 */

void set_systray_color(Xputty *main, double r, double g, double b, double a);

/**
 * @brief set_pattern       - set pattern for the selected Colors
 * @param *w                - pointer to the Widget_t to set the pattern
 * @param *from             - the Colors set to pattern from (0)
 * @param *to               - the Colors set to pattern to (1)
 * @param mod               - the Color_mod to use
 */

void set_pattern(Widget_t *w, Colors *from, Colors *to, Color_mod mod);

#ifdef __cplusplus
}
#endif

#endif //XCOLOR_H_
