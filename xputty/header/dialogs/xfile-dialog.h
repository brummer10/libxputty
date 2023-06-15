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

#ifndef XFILE_DIALOG_H_
#define XFILE_DIALOG_H_

#include "xwidgets.h"
#include "xdgmime.h"
#include "xasprintf.h"
#include "xfilepicker.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    Widget_t *parent;
    Widget_t *w;
    Widget_t *ct;
    Widget_t *ft;
    Widget_t *w_quit;
    Widget_t *w_okay;
    Widget_t *w_hidden;
    Widget_t *sel_dir;
    Widget_t *set_filter;
    Widget_t *text_entry;
    Widget_t *xdg_dirs;
    Widget_t *view;
    Widget_t *scale_size;
    FilePicker *fp;
    char **xdg_user_dirs;
    char **xdg_user_dirs_path;
    const char *home_dir;
    unsigned int xdg_dir_counter;
    bool list_view;
    bool send_clear_func;
    char pad[2];
} FileDialog;

typedef struct {
    Widget_t *w;
    char *last_path;
    const char *path;
    const char *filter;
    bool is_active;
    char pad[7];
} FileButton;

Widget_t *open_directory_dialog(Widget_t *w, const char *path);

/**
 * @brief open_file_dialog  - open a non blocking dialog window,
 * to select a file. The path to open the file-selector could be NULL
 * It will open then in $HOME.
 * The filter to use could be NULL, then the file-selector will show all files.
 * @return Widget_t*           - pointer to the Widget_t struct
 */

Widget_t *open_file_dialog(Widget_t *w, const char *path, const char *filter);

/**
 * @brief save_file_dialog  - open a non blocking dialog window,
 * to select a file. The path to open the file-selector could be NULL
 * It will open then in $HOME.
 * The filter to use could be NULL, then the file-selector will show all files.
 * @return Widget_t*           - pointer to the Widget_t struct
 */

Widget_t *save_file_dialog(Widget_t *w, const char *path, const char *filter);

/**
 * @brief add_file_button      - add a pre-defined button which will pop-up a file dialog
 * when pressed. path and filter will e hand over to the file selector.
 * @return Widget_t*           - pointer to the Widget_t struct
 */

Widget_t *add_file_button(Widget_t *parent, int x, int y, int width, int height,
                           const char *path, const char *filter);

/**
 * @brief add_save_file_button - add a pre-defined button which will pop-up a file dialog
 * when pressed. path and filter will e hand over to the file selector.
 * @return Widget_t*           - pointer to the Widget_t struct
 */

Widget_t *add_save_file_button(Widget_t *parent, int x, int y, int width, int height,
                           const char *path, const char *filter);

#ifdef __cplusplus
}
#endif

#endif //XFILE_DIALOG_H_
