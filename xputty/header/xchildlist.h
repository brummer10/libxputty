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

#ifndef XCHILDLIST_H_
#define XCHILDLIST_H_

#include "xputty.h"


/**
 * 
 * @brief Childlist_t       - struct to hold a Widget_t child list
 * @param **childs          - dynamic array to hold pointers to the childs
 * @param size              - current size of array
 * @param cap               - current capacity of the array
 * @param elem              - current elements in the array 
 */

struct Childlist_t {
/** dynamic array to hold pointers to the childs */
    Widget_t **childs;
/** current size of array */
    size_t size;
/** current capacity of the array */
    int cap;
/** current elements in the array  */
    int elem;
};

/**
 * @brief childlist_init      - allocate the array to min size
 * @param *childlist          - pointer to the Childlist_t
 * @return void 
 */

void childlist_init(Childlist_t *childlist);

/**
 * @brief childlist_destroy   - free the childlist
 * @param *childlist          - pointer to the Childlist_t
 * @return void 
 */

void childlist_destroy(Childlist_t *childlist);

/**
 * @brief childlist_add_child - add a child to the childlist
 * @param *childlist          - pointer to the Childlist_t
 * @param *child              - pointer to the child to add
 * @return void 
 */

void childlist_add_child(Childlist_t *childlist, Widget_t *child);

/**
 * @brief childlist_add_child - remove a child from the childlist
 * @param *childlist          - pointer to the Childlist_t
 * @param *child              - pointer to the child to remove
 * @return void 
 */

void childlist_remove_child(Childlist_t *childlist, Widget_t *child);

/**
 * @brief childlist_find_child - find a child in a the childlist
 * @param *childlist           - pointer to the Childlist_t
 * @param *child               - pointer to the child to find
 * @return int                 - return position in childlist or -1 
 */

int childlist_find_child(Childlist_t *childlist, Widget_t *child);

/**
 * @brief childlist_find_widget - find a child Widget_t in a the childlist
 * @param *childlist            - pointer to the Childlist_t
 * @param child_window          - the window to find the Widget_t for
 * @return Widget_t*            - return pointer to WiDget or NULL
 */

int childlist_find_widget(Childlist_t *childlist, Window child_window);

/**
 * @brief childlist_has_child  - check if childlist contain a child
 * @param *childlist           - pointer to the Childlist_t
 * @return int                 - return element counter value
 */

int childlist_has_child(Childlist_t *childlist);

#endif //XCHILDLISTT_H_

