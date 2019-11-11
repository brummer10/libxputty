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

#ifndef XADJUSTMENT_H_
#define XADJUSTMENT_H_

#include "xputty.h"


/**
 * 
 * @brief CL_type        - enum type of controller adjustment
 * @param CL_NONE        - Widget_t didn't request a adjustment
 * @param CL_CONTINUOS   - Widget_t request a continuos adjustment
 * @param CL_TOGGLE      - Widget_t request a toggle adjustment
 * @param CL_BUTTON      - Widget_t request a button adjustment
 * @param CL_ENUM        - Widget_t request a enum adjustment
 */

typedef enum {
/** Widget_t didn't request a adjustment */
    CL_NONE         = 0x0001,
/** Widget_t request a continuos adjustment */
    CL_CONTINUOS    = 0x0002,
/** Widget_t request a toggle adjustment */
    CL_TOGGLE       = 0x0004,
/** Widget_t request a button adjustment */
    CL_BUTTON       = 0x0008,
/** Widget_t request a enum adjustment */
    CL_ENUM         = 0x0016,
/** Widget_t request a viewport adjustment */
    CL_VIEWPORT     = 0x0032,
/** Widget_t request a viewport adjustment */
    CL_METER        = 0x0064,
}CL_type;

/**
 * 
 * @brief Adjustment_t     - struct to hold a controller adjustment
 * @param *w               - pointer to the Widget_t contain the adjustment
 * @param std_value        - the standart value for the adjustment
 * @param value            - the current value of the adjustment
 * @param min_value        - the minimal value of the adjustment
 * @param max_value        - the maximal value of the adjustment
 * @param step             - the step to increase/decrease the adjustment
 * @param start_value      - the value of init the adjustment with
 * @param type             - should be on of the CL_type
 */

struct  Adjustment_t {
/** pointer to the Widget_t contain the adjustment */
    Widget_t *w;
/** the standart value for the adjustment */
    float std_value;
/** the current value of the adjustment */
    float value;
/** the minimal value of the adjustment */
    float min_value;
/** the maximal value of the adjustment */
    float max_value;
/** the step to increase/decrease the adjustment */
    float step;
/** the value of init the adjustment with */
    float start_value;
/** should be on of the CL_ type */
    CL_type type;
};

/**
 * @brief *add_adjustment    - adding a adjustment to the widget
 * @param *w                 - pointer to the Widget_t request a adjustment
 * @param std_value          - standard value of the adjustment
 * @param value              - current value of the adjustment
 * @param min_value          - minimum value of the adjustment
 * @param max_value          - maximal value of the adjustment
 * @param step               - step to increase/decrease the adjustment
 * @param type               - set CL_type of adjustment
 * @return *adj              - pointer to adjustment
 */

Adjustment_t *add_adjustment(Widget_t *w, float std_value, float value,
                     float min_value,float max_value, float step, CL_type type);


/**
 * @brief *set_adjustment    - set a new range to a existing Adjustment
 * @param *w                 - pointer to the Widget_t request a adjustment
 * @param std_value          - standard value of the adjustment
 * @param value              - current value of the adjustment
 * @param min_value          - minimum value of the adjustment
 * @param max_value          - maximal value of the adjustment
 * @param step               - step to increase/decrease the adjustment
 * @param type               - set CL_type of adjustment
 * @return *adj              - pointer to adjustment
 */

void set_adjustment(Adjustment_t *adj, float std_value, float value,
                float min_value,float max_value, float step, CL_type type);

/**
 * @brief delete_adjustment  - freeing the memory of the adjustment
 * @param *adj               - pointer to the Adjustment to free 
 * @return *void             - return NULL
 */

void *delete_adjustment(Adjustment_t *adj);

/**
 * @brief adj_get_state      - get the current state of the adjustment
 * @param *adj               - pointer to the Adjustment
 * @return float             - return the adjustment state (0<->1)
 */

float adj_get_state(Adjustment_t *adj);

/**
 * @brief adj_set_state      - set the current state of the adjustment
 * @param *adj               - pointer to the Adjustment
 * @return float             - return the adjustment state (0<->1)
 */

void adj_set_state(Adjustment_t *adj, float state);

/**
 * @brief adj_get_value      - get the current value of the adjustment
 * @param *adj               - pointer to the Adjustment
 * @return float             - return the adjustment value
 */

float adj_get_value(Adjustment_t *adj);

/**
 * @brief adj_set_value      - set the current value of the adjustment
 * @param *adj               - pointer to the Adjustment
 * @param v                  - value set the Adjustment to 
 * @return void
 */

void adj_set_value(Adjustment_t *adj, float v);

/**
 * @brief adj_set_start_value - set start value of the adjustment
 * @param *w                  - pointer to Widget_t containing the adjustment 
 * @return void
 */

void adj_set_start_value(void *w);

/**
 * @brief adj_set_motion_state      - set value/state of the adjustment
 * @param *adj               - pointer to Widget_t containing the adjustment
 * @param x                  - value for the xaxis
 * @param y                  - value for the yaxis
 * @return void
 */

void adj_set_motion_state(void *w, float x, float y);

/**
 * @brief check_value_changed   - check if value has changed and send
 * adj_callback if so
 * @param *adj                  - pointer to the Adjustment 
 * @param v                     - value to check 
 * @return void
 */

void check_value_changed(Adjustment_t *adj, float *value);

#endif //XADJUSTMENT_H_
