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


/*
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
*/

#include <dirent.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <stdarg.h> 
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#include <libgen.h>

#include "xfile-dialog.h"
#include "xmessage-dialog.h"

static void combo_response(void *w_, void* user_data);
static void set_selected_file(FileDialog *file_dialog);

static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    cairo_rectangle(w->crb,0,0,width,height);
    set_pattern(w,&w->app->color_scheme->selected,&w->app->color_scheme->normal,BACKGROUND_);
    cairo_fill (w->crb);

    //widget_set_scale(w);
    use_fg_color_scheme(w, NORMAL_);
    cairo_set_font_size (w->crb, 12.0);
    cairo_move_to (w->crb, 20, 35);
    cairo_show_text(w->crb, _("Base Directory"));
    cairo_move_to (w->crb, 20, 85);
    cairo_show_text(w->crb, _("Places"));
    cairo_move_to (w->crb, 130, 85);
    cairo_show_text(w->crb, _("Entries"));
    cairo_move_to (w->crb, 20, 330-w->scale.scale_y);
    cairo_show_text(w->crb, _("Load: "));
    cairo_move_to (w->crb, 45, 360-w->scale.scale_y);
    cairo_show_text(w->crb, _("Show hidden files")); 
    cairo_move_to (w->crb, 45, 390-w->scale.scale_y);
    cairo_show_text(w->crb, _("List view")); 
    cairo_move_to (w->crb, 60, 330-w->scale.scale_y);
    cairo_show_text(w->crb, w->label);
    //widget_reset_scale(w);
    if (w->image) {
        cairo_set_source_surface (w->crb, w->image, 180, 332-w->scale.scale_y);
        cairo_paint (w->crb);
    }
}

static void draw_fd_hslider(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;

    int width = w->width-2;
    int height = w->height-2;
    float center = (float)height/2;

    float sliderstate = adj_get_state(w->adj_x);

    use_text_color_scheme(w, get_color_state(w));
    cairo_move_to (w->crb, 0.0, center);
    cairo_line_to(w->crb,width,center);
    cairo_set_line_width(w->crb,center/10);
    cairo_stroke(w->crb);

    use_bg_color_scheme(w, get_color_state(w));
    cairo_rectangle(w->crb, (width-height)*sliderstate,0,height, height);
    cairo_fill(w->crb);
    cairo_new_path (w->crb);

    use_text_color_scheme(w, get_color_state(w));
    cairo_set_line_width(w->crb,3);
    cairo_move_to (w->crb,((width-height)*sliderstate)+center, 0.0);
    cairo_line_to(w->crb,((width-height)*sliderstate)+center,height);
    cairo_stroke(w->crb);
    cairo_new_path (w->crb);
}

static void button_quit_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
   
    if (w->flags & HAS_POINTER && !adj_get_value(w->adj)){
        file_dialog->parent->func.dialog_callback(file_dialog->parent,NULL);
        file_dialog->send_clear_func = false;
        destroy_widget(file_dialog->w,file_dialog->w->app);
    }
}

static inline int set_files(FileDialog *file_dialog) {
    if (file_dialog->list_view) {
        listview_set_list(file_dialog->ft,file_dialog->fp->file_names , (int)file_dialog->fp->file_counter);
    } else {
        multi_listview_set_list(file_dialog->ft,file_dialog->fp->file_names , (int)file_dialog->fp->file_counter);
    }
    int ret = -1;
    int i = 0;
    for (; i<(int)file_dialog->fp->file_counter; i++) {
        if(file_dialog->fp->selected_file && strcmp(file_dialog->fp->file_names[i],
          basename(file_dialog->fp->selected_file))==0 )  ret = i;
    }
    return ret;
}

static void set_dirs(FileDialog *file_dialog) {
    int i = 0;
    for (; i<(int)file_dialog->fp->dir_counter; i++) {
        combobox_add_entry(file_dialog->ct,file_dialog->fp->dir_names[i]);
    }
}

static void file_released_b_callback(void *w_, void *button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    XButtonEvent *xbutton = (XButtonEvent*)button_;
    if(xbutton->button == Button1) {
        set_selected_file(file_dialog);
        if(file_dialog->fp->selected_file) {
            file_dialog->w->label = file_dialog->fp->selected_file;
            expose_widget(file_dialog->w);
        }
    }
}

static void reload_from_dir(FileDialog *file_dialog) {
    assert(file_dialog->fp->path != NULL);
    if (file_dialog->list_view) {
        listview_remove_list(file_dialog->ft);
    } else {
        multi_listview_remove_list(file_dialog->ft);
    }
    combobox_delete_entrys(file_dialog->ct);
    int ds = fp_get_files(file_dialog->fp,file_dialog->fp->path, 1, 1);
    int set_f = set_files(file_dialog);
    set_dirs(file_dialog);
    combobox_set_active_entry(file_dialog->ct, ds);
    if (set_f != -1) {
        if (file_dialog->list_view) {
            listview_set_active_entry(file_dialog->ft, set_f);
        } else {
            multi_listview_set_active_entry(file_dialog->ft, set_f);
        }
    } else {
        if (file_dialog->list_view) {
            listview_unset_active_entry(file_dialog->ft);
        } else {
            multi_listview_unset_active_entry(file_dialog->ft);
        }
    }
    listview_unset_active_entry(file_dialog->xdg_dirs);
    expose_widget(file_dialog->ft);
    expose_widget(file_dialog->ct);
    expose_widget(file_dialog->xdg_dirs);
}

static void show_preview(FileDialog *file_dialog, const char* file_name) {
    Widget_t* w = file_dialog->w;
    cairo_surface_t *getpng = cairo_image_surface_create_from_png (file_name);
    int width = cairo_image_surface_get_width(getpng);
    int height = cairo_image_surface_get_height(getpng);
    int width_t = 80;
    int height_t = 80;
    double x = (double)width_t/(double)width;
    double y = (double)height_t/(double)height;
    if (width > 20*height)
        x = y = (double)width_t/(double)height;
    cairo_surface_destroy(w->image);
    w->image = NULL;

    w->image = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width_t, height_t);
    cairo_t *cri = cairo_create (w->image);
    cairo_scale(cri, x,y);    
    cairo_set_source_surface (cri, getpng,0,0);
    cairo_paint (cri);
    cairo_surface_destroy(getpng);
    cairo_destroy(cri);
    expose_widget(w);
}

static void show_svg_preview(FileDialog *file_dialog, const char* file_name) {
    Widget_t* w = file_dialog->w;
    widget_get_scaled_svg_from_file(w, file_name, 80, 80);
    expose_widget(w);
}

static void set_selected_file(FileDialog *file_dialog) {
    if(adj_get_value(file_dialog->ft->adj)<0 ||
        adj_get_value(file_dialog->ft->adj) > file_dialog->fp->file_counter) return;

    struct stat sb;
    if (stat(file_dialog->fp->file_names[(int)adj_get_value(file_dialog->ft->adj)], &sb) == 0 && S_ISDIR(sb.st_mode)) {
        asprintf(&file_dialog->fp->path, "%s",file_dialog->fp->file_names[(int)adj_get_value(file_dialog->ft->adj)]);
        reload_from_dir(file_dialog);
        return;
    }

    Widget_t* menu =  file_dialog->ct->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    if ((int)adj_get_value(file_dialog->ct->adj) < 0) return;
    free(file_dialog->fp->selected_file);
    file_dialog->fp->selected_file = NULL;
    if (strlen(comboboxlist->list_names[(int)adj_get_value(file_dialog->ct->adj)]) > 1) {
        asprintf(&file_dialog->fp->selected_file, "%s/%s",
            comboboxlist->list_names[(int)adj_get_value(file_dialog->ct->adj)],
            file_dialog->fp->file_names[(int)adj_get_value(file_dialog->ft->adj)]);
    } else {
        asprintf(&file_dialog->fp->selected_file, "/%s",
            file_dialog->fp->file_names[(int)adj_get_value(file_dialog->ft->adj)]);
    }
    assert(file_dialog->fp->selected_file != NULL);
    if (strstr(file_dialog->fp->selected_file, ".png")) {
        show_preview(file_dialog, file_dialog->fp->selected_file);
    } else if (strstr(file_dialog->fp->selected_file, ".svg")) {
        show_svg_preview(file_dialog, file_dialog->fp->selected_file);
    } else if (file_dialog->w->image) {
        cairo_surface_destroy(file_dialog->w->image);
        file_dialog->w->image = NULL;
        expose_widget(file_dialog->w);
    }
}

static void reload_file_entrys(FileDialog *file_dialog) {
    if (file_dialog->list_view) {
        listview_remove_list(file_dialog->ft);
    } else {
        multi_listview_remove_list(file_dialog->ft);
    }
    fp_get_files(file_dialog->fp,file_dialog->fp->path, 0, 1);
    if (!file_dialog->fp->file_counter)
        fp_get_files(file_dialog->fp,file_dialog->fp->path, 1, 1);
    int set_f = set_files(file_dialog);
    if (set_f != -1) {
        if (file_dialog->list_view) {
            listview_set_active_entry(file_dialog->ft, set_f);
        } else {
            multi_listview_set_active_entry(file_dialog->ft, set_f);
        }
    } else {
        if (file_dialog->list_view) {
            listview_unset_active_entry(file_dialog->ft);
        } else {
            multi_listview_unset_active_entry(file_dialog->ft);
        }
    }
    expose_widget(file_dialog->ft);
}

static void combo_response(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    Widget_t* menu =  w->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    if ((int)adj_get_value(file_dialog->ct->adj) < 0) return;
    free(file_dialog->fp->path);
    file_dialog->fp->path = NULL;
    asprintf(&file_dialog->fp->path, "%s",comboboxlist->list_names[(int)adj_get_value(w->adj)]);
    assert(file_dialog->fp->path != NULL);
    reload_from_dir(file_dialog);
}

static void button_ok_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        if(file_dialog->fp->selected_file) {
            file_dialog->parent->func.dialog_callback(file_dialog->parent,&file_dialog->fp->selected_file);
            file_dialog->send_clear_func = false;
        } else {
            Widget_t *dia = open_message_dialog(w, INFO_BOX, _("INFO"), _("Please select a file"),NULL);
            XSetTransientForHint(file_dialog->w->app->dpy, dia->widget, file_dialog->w->widget);
            return;
        }
        destroy_widget(file_dialog->w,file_dialog->w->app);
   }
}

static void file_double_click_callback(void *w_, void *button, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    if(!file_dialog->fp->selected_file) {
        set_selected_file(file_dialog);
    }
    if(file_dialog->fp->selected_file) {
        file_dialog->parent->func.dialog_callback(file_dialog->parent,&file_dialog->fp->selected_file);
        file_dialog->send_clear_func = false;
    } else {
        Widget_t *dia = open_message_dialog(w, INFO_BOX, _("INFO"), _("Please select a file"),NULL);
        XSetTransientForHint(file_dialog->w->app->dpy, dia->widget, file_dialog->w->widget);
        return;
    }
    destroy_widget(file_dialog->w,file_dialog->w->app);
}

static void reload_all(FileDialog *file_dialog) {
    Widget_t* menu =  file_dialog->ct->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    if ((int)adj_get_value(file_dialog->ct->adj) < 0) return;
    free(file_dialog->fp->path);
    file_dialog->fp->path = NULL;
    asprintf(&file_dialog->fp->path, "%s",comboboxlist->list_names[(int)adj_get_value(file_dialog->ct->adj)]);
    assert(file_dialog->fp->path != NULL);
    if (file_dialog->list_view) {
        listview_remove_list(file_dialog->ft);
    } else {
        multi_listview_remove_list(file_dialog->ft);
    }
    combobox_delete_entrys(file_dialog->ct);
    int ds = fp_get_files(file_dialog->fp,file_dialog->fp->path, 1, 1);
    int set_f = set_files(file_dialog);
    set_dirs(file_dialog);
    combobox_set_active_entry(file_dialog->ct, ds);
    if (set_f != -1) {
        if (file_dialog->list_view) {
            listview_set_active_entry(file_dialog->ft, set_f);
        } else {
            multi_listview_set_active_entry(file_dialog->ft, set_f);
        }
    } else {
        if (file_dialog->list_view) {
            listview_unset_active_entry(file_dialog->ft);
        } else {
            multi_listview_unset_active_entry(file_dialog->ft);
        }
    }
    expose_widget(file_dialog->ft);
}

static void set_scale_factor_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    float v = adj_get_value(w->adj);
    if (!file_dialog->list_view) {
        multi_listview_set_item_size(file_dialog->ft, v);
    }
}

static void open_dir_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        reload_from_dir(file_dialog);
    }
}

static void button_hidden_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    if (w->flags & HAS_POINTER) {
        file_dialog->fp->show_hidden = adj_get_value(w->adj) ? true : false;
        reload_all(file_dialog);
    }
}

static void button_view_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    if (w->flags & HAS_POINTER) {
        file_dialog->list_view = adj_get_value(w->adj) ? true : false;
    }
    if (file_dialog->list_view) {
        destroy_widget(file_dialog->ft, w->app);
        file_dialog->ft = add_listview(file_dialog->w, "", 130, 90, 510, 225);
        file_dialog->ft->parent_struct = file_dialog;
        file_dialog->ft->scale.gravity = NORTHWEST;
        file_dialog->ft->flags |= NO_PROPAGATE;
        listview_set_check_dir(file_dialog->ft, 1);
        file_dialog->ft->func.button_release_callback = file_released_b_callback;
        file_dialog->ft->func.double_click_callback = file_double_click_callback;
        int set_f = set_files(file_dialog); 
        if (set_f != -1) {
            listview_set_active_entry(file_dialog->ft, set_f);
        } else {
            listview_unset_active_entry(file_dialog->ft);
        }
        widget_show_all(file_dialog->ft);
    } else {
        destroy_widget(file_dialog->ft, w->app);
        file_dialog->ft = add_multi_listview(file_dialog->w, "", 130, 90, 510, 225);
        file_dialog->ft->parent_struct = file_dialog;
        file_dialog->ft->scale.gravity = NORTHWEST;
        file_dialog->ft->flags |= NO_PROPAGATE;
        multi_listview_set_check_dir(file_dialog->ft, 1);
        file_dialog->ft->func.button_release_callback = file_released_b_callback;
        file_dialog->ft->func.double_click_callback = file_double_click_callback;
        int set_f = set_files(file_dialog); 
        if (set_f != -1) {
            multi_listview_set_active_entry(file_dialog->ft, set_f);
        } else {
            multi_listview_unset_active_entry(file_dialog->ft);
        }
        multi_listview_set_item_size(file_dialog->ft,adj_get_value(file_dialog->scale_size->adj));
        widget_show_all(file_dialog->ft);
    }
    resize_childs(file_dialog->w);
}

static void set_filter_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    if (file_dialog->fp->use_filter != (int)adj_get_value(w->adj)) {
        file_dialog->fp->use_filter = (int)adj_get_value(w->adj);
        Widget_t* menu =  w->childlist->childs[1];
        Widget_t* view_port =  menu->childlist->childs[0];
        ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
        if ((int)adj_get_value(file_dialog->ct->adj) < 0) return;
        free(file_dialog->fp->filter);
        file_dialog->fp->filter = NULL;
        asprintf(&file_dialog->fp->filter, "%s",comboboxlist->list_names[(int)adj_get_value(w->adj)]);
        assert(file_dialog->fp->filter != NULL);

        reload_file_entrys(file_dialog);
    }
}

static void fd_mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    if(file_dialog->icon) {
        XFreePixmap(w->app->dpy, (*file_dialog->icon));
        file_dialog->icon = NULL;
    }
    if(file_dialog->send_clear_func)
        file_dialog->parent->func.dialog_callback(file_dialog->parent,NULL);
    fp_free(file_dialog->fp);
    free(file_dialog->fp);
    free(file_dialog);
}

static int starts_with(const char *restrict string, const char *restrict prefix) {
    while(*prefix)
    {
        if(*prefix++ != *string++)
            return 0;
    }

    return 1;
}

static int strremove(char *string, const char *restrict find){
    if(strstr(string, find) == NULL) return 0;
    char *temporaryString = malloc(strlen(strstr(string, find) + strlen(find)) + 1);
    strcpy(temporaryString, strstr(string, find) + strlen(find));
    char* rep = strstr(string, find);
    *rep = '\0';
    //strcat(string, replaceWith);
    strcat(string, temporaryString);
    free(temporaryString);
    return 1;
}

static void parse_xdg_dirs(FileDialog *file_dialog) {
    
    char xdg_dir[204];
    file_dialog->home_dir = getenv("HOME");
    if (file_dialog->home_dir == NULL) {
        file_dialog->home_dir = getpwuid(getuid())->pw_dir;
    }
    if (file_dialog->home_dir == NULL) return;
    sprintf(xdg_dir,"%s/.config/user-dirs.dirs", file_dialog->home_dir);
    FILE * fp = NULL;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    file_dialog->xdg_user_dirs = (char **)realloc(file_dialog->xdg_user_dirs,
      (file_dialog->xdg_dir_counter + 1) * sizeof(char *));
    assert(file_dialog->xdg_user_dirs != NULL);
    asprintf(&file_dialog->xdg_user_dirs[file_dialog->xdg_dir_counter++], "%s", _("Home"));
    assert(file_dialog->xdg_user_dirs[file_dialog->xdg_dir_counter-1] != NULL);
    

    fp = fopen(xdg_dir, "r");
    if (fp != NULL) {
        while ((read = getline(&line, &len, fp)) != -1) {
            if(starts_with(line,"XDG_")) {
                char* xdg = strstr(line, "$HOME/");
                if (!strremove(xdg, "$HOME/")) continue;
                char* rep = strstr(xdg, "\"");
                *rep = '\0';
                file_dialog->xdg_user_dirs = (char **)realloc(file_dialog->xdg_user_dirs,
                  (file_dialog->xdg_dir_counter + 1) * sizeof(char *));
                assert(file_dialog->xdg_user_dirs != NULL);
                asprintf(&file_dialog->xdg_user_dirs[file_dialog->xdg_dir_counter++], "%s", xdg);
                assert(file_dialog->xdg_user_dirs[file_dialog->xdg_dir_counter-1] != NULL);
            }
        }
        fclose(fp);
    }

    file_dialog->xdg_user_dirs = (char **)realloc(file_dialog->xdg_user_dirs,
      (file_dialog->xdg_dir_counter + 1) * sizeof(char *));
    assert(file_dialog->xdg_user_dirs != NULL);
    asprintf(&file_dialog->xdg_user_dirs[file_dialog->xdg_dir_counter++], "%s", _("Computer"));
    assert(file_dialog->xdg_user_dirs[file_dialog->xdg_dir_counter-1] != NULL);

    if (line)
        free(line);    
}

static void xdg_dir_select_callback(void *w_, void *button, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    int v = (int)adj_get_value(w->adj);
    if (v == 0) {
        free(file_dialog->fp->path);
        file_dialog->fp->path = NULL;
        asprintf(&file_dialog->fp->path, "%s",file_dialog->home_dir);
        assert(file_dialog->fp->path != NULL);
    } else if (v == file_dialog->xdg_dir_counter) {
        free(file_dialog->fp->path);
        file_dialog->fp->path = NULL;
        asprintf(&file_dialog->fp->path, "%s",PATH_SEPARATOR);
        assert(file_dialog->fp->path != NULL);
    } else {
        free(file_dialog->fp->path);
        file_dialog->fp->path = NULL;
        asprintf(&file_dialog->fp->path, "%s/%s",file_dialog->home_dir,file_dialog->xdg_user_dirs[v]);
        assert(file_dialog->fp->path != NULL);
    }
    reload_from_dir(file_dialog);
}

static void add_xdg_dirs(FileDialog *file_dialog) {
    file_dialog->xdg_dirs = add_listview(file_dialog->w, "", 20, 90, 100, 225);
    file_dialog->xdg_dirs->parent_struct = file_dialog;
    file_dialog->xdg_dirs->scale.gravity = EASTNORTH;
    file_dialog->xdg_dirs->flags |= NO_PROPAGATE;
    listview_set_list(file_dialog->xdg_dirs, file_dialog->xdg_user_dirs, (int)file_dialog->xdg_dir_counter);
    file_dialog->xdg_dirs->func.button_release_callback = xdg_dir_select_callback;
    listview_unset_active_entry(file_dialog->xdg_dirs);
}

Widget_t *open_file_dialog(Widget_t *w, const char *path, const char *filter) {
    FileDialog *file_dialog = (FileDialog*)malloc(sizeof(FileDialog));

    file_dialog->xdg_user_dirs = NULL;
    file_dialog->xdg_dir_counter = 0;
    parse_xdg_dirs(file_dialog);
    file_dialog->fp = (FilePicker*)malloc(sizeof(FilePicker));

    struct stat sb;
    if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
        fp_init(file_dialog->fp, path);
    } else if (stat(file_dialog->home_dir, &sb) == 0 && S_ISDIR(sb.st_mode)) {
        fp_init(file_dialog->fp, file_dialog->home_dir);
    } else {
        fp_init(file_dialog->fp, "/");
    }

    file_dialog->parent = w;
    file_dialog->send_clear_func = true;
    file_dialog->icon = NULL;
    file_dialog->list_view = false;

    file_dialog->w = create_window(w->app, DefaultRootWindow(w->app->dpy), 0, 0, 660, 415);

    XSizeHints* win_size_hints;
    win_size_hints = XAllocSizeHints();
    win_size_hints->flags =  PMinSize|PBaseSize|PMaxSize|PWinGravity;
    win_size_hints->min_width = 660;
    win_size_hints->min_height = 415;
    win_size_hints->base_width = 660;
    win_size_hints->base_height = 415;
    win_size_hints->max_width = 960;
    win_size_hints->max_height = 865;
    win_size_hints->win_gravity = CenterGravity;
    XSetWMNormalHints(file_dialog->w->app->dpy, file_dialog->w->widget, win_size_hints);
    XFree(win_size_hints);

    file_dialog->w->flags |= HAS_MEM;
    file_dialog->w->parent_struct = file_dialog;
    file_dialog->w->flags |= NO_PROPAGATE;
    widget_set_title(file_dialog->w, _("File Selector"));
    file_dialog->w->func.expose_callback = draw_window;
    file_dialog->w->func.mem_free_callback = fd_mem_free;
    widget_set_icon_from_png(file_dialog->w,file_dialog->icon,LDVAR(directory_png));

    file_dialog->ct = add_combobox(file_dialog->w, "", 20, 40, 550, 30);
    file_dialog->ct->parent_struct = file_dialog;
    file_dialog->ct->scale.gravity = NORTHEAST;
    file_dialog->ct->flags |= NO_PROPAGATE;
    file_dialog->ct->func.value_changed_callback = combo_response;

    file_dialog->sel_dir = add_button(file_dialog->w, _("Open"), 580, 40, 60, 30);
    file_dialog->sel_dir->parent_struct = file_dialog;
    file_dialog->sel_dir->scale.gravity = WESTNORTH;
    file_dialog->sel_dir->flags |= NO_PROPAGATE;
    add_tooltip(file_dialog->sel_dir,_("Open sub-directory's"));
    file_dialog->sel_dir->func.value_changed_callback = open_dir_callback;

    file_dialog->scale_size = add_hslider(file_dialog->w, "", 580, 10, 60, 15);
    set_adjustment(file_dialog->scale_size->adj, 0.2, 0.2, 0.1, 0.4, 0.01, CL_CONTINUOS);
    file_dialog->scale_size->parent_struct = file_dialog;
    file_dialog->scale_size->scale.gravity = WESTNORTH;
    file_dialog->scale_size->flags |= NO_PROPAGATE;
    file_dialog->scale_size->func.expose_callback = draw_fd_hslider;
    add_tooltip(file_dialog->scale_size,_("Set Icon scale factor"));
    file_dialog->scale_size->func.value_changed_callback = set_scale_factor_callback;

    file_dialog->ft = add_multi_listview(file_dialog->w, "", 130, 90, 510, 225);
    file_dialog->ft->parent_struct = file_dialog;
    file_dialog->ft->scale.gravity = NORTHWEST;
    file_dialog->ft->flags |= NO_PROPAGATE;
    multi_listview_set_check_dir(file_dialog->ft, 1);
    file_dialog->ft->func.button_release_callback = file_released_b_callback;
    file_dialog->ft->func.double_click_callback = file_double_click_callback;

    int ds = fp_get_files(file_dialog->fp,file_dialog->fp->path, 1, 1);   
    int set_f = set_files(file_dialog); 
    set_dirs(file_dialog);
    combobox_set_active_entry(file_dialog->ct, ds);
    if (set_f != -1) {
        multi_listview_set_active_entry(file_dialog->ft, set_f);
    } else {
        multi_listview_unset_active_entry(file_dialog->ft);
    }

    add_xdg_dirs(file_dialog);

    file_dialog->w_quit = add_button(file_dialog->w, _("Cancel"), 580, 340, 60, 60);
    file_dialog->w_quit->parent_struct = file_dialog;
    file_dialog->w_quit->scale.gravity = SOUTHWEST;
    file_dialog->w_quit->flags |= NO_PROPAGATE;
    add_tooltip(file_dialog->w_quit,_("Exit file selector"));
    file_dialog->w_quit->func.value_changed_callback = button_quit_callback;

    file_dialog->w_okay = add_button(file_dialog->w, _("Load"), 510, 340, 60, 60);
    file_dialog->w_okay->parent_struct = file_dialog;
    file_dialog->w_okay->scale.gravity = SOUTHWEST;
    file_dialog->w_okay->flags |= NO_PROPAGATE;
    add_tooltip(file_dialog->w_okay,_("Load selected file"));
    file_dialog->w_okay->func.value_changed_callback = button_ok_callback;

    file_dialog->set_filter = add_combobox(file_dialog->w, "", 360, 345, 120, 30);
    file_dialog->set_filter->parent_struct = file_dialog;
    file_dialog->set_filter->scale.gravity = SOUTHWEST;
    file_dialog->set_filter->flags |= NO_PROPAGATE;
    combobox_add_entry(file_dialog->set_filter,_("all"));
    combobox_add_entry(file_dialog->set_filter,_("application"));
    combobox_add_entry(file_dialog->set_filter,_("audio"));
    combobox_add_entry(file_dialog->set_filter,_("font"));
    combobox_add_entry(file_dialog->set_filter,_("image"));
    combobox_add_entry(file_dialog->set_filter,_("text"));
    combobox_add_entry(file_dialog->set_filter,_("video"));
    combobox_add_entry(file_dialog->set_filter,_("x-content"));
    if(filter !=NULL && strlen(filter))
        combobox_add_entry(file_dialog->set_filter,filter);
    combobox_set_active_entry(file_dialog->set_filter, 0);
    file_dialog->set_filter->func.value_changed_callback = set_filter_callback;
    if(filter !=NULL && strlen(filter))
        combobox_set_active_entry(file_dialog->set_filter, 8);
    add_tooltip(file_dialog->set_filter->childlist->childs[0], _("File filter type"));

    file_dialog->w_hidden = add_check_button(file_dialog->w, "", 20, 345, 20, 20);
    file_dialog->w_hidden->parent_struct = file_dialog;
    file_dialog->w_hidden->scale.gravity = EASTWEST;
    file_dialog->w_hidden->flags |= NO_PROPAGATE;
    add_tooltip(file_dialog->w_hidden,_("Show hidden files and folders"));
    file_dialog->w_hidden->func.value_changed_callback = button_hidden_callback;

    file_dialog->view = add_check_button(file_dialog->w, "", 20, 375, 20, 20);
    file_dialog->view->parent_struct = file_dialog;
    file_dialog->view->scale.gravity = EASTWEST;
    file_dialog->view->flags |= NO_PROPAGATE;
    add_tooltip(file_dialog->view,_("Show entries in list view"));
    file_dialog->view->func.value_changed_callback = button_view_callback;

    widget_show_all(file_dialog->w);
    return file_dialog->w;
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------	
				add_file_button
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

static void fdialog_response(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileButton *filebutton = (FileButton *)w->private_struct;
    if(user_data !=NULL) {
        char *tmp = strdup(*(const char**)user_data);
        free(filebutton->last_path);
        filebutton->last_path = NULL;
        filebutton->last_path = strdup(dirname(tmp));
        filebutton->path = filebutton->last_path;
        free(tmp);
    }
    w->func.user_callback(w,user_data);
    filebutton->is_active = false;
    adj_set_value(w->adj,0.0);
}

static void fbutton_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileButton *filebutton = (FileButton *)w->private_struct;
    if (w->flags & HAS_POINTER && adj_get_value(w->adj)){
        filebutton->w = open_file_dialog(w,filebutton->path,filebutton->filter);
        Atom wmStateAbove = XInternAtom(w->app->dpy, "_NET_WM_STATE_ABOVE", 1 );
        Atom wmNetWmState = XInternAtom(w->app->dpy, "_NET_WM_STATE", 1 );
        XChangeProperty(w->app->dpy, filebutton->w->widget, wmNetWmState, XA_ATOM, 32, 
            PropModeReplace, (unsigned char *) &wmStateAbove, 1); 
        filebutton->is_active = true;
    } else if (w->flags & HAS_POINTER && !adj_get_value(w->adj)){
        if(filebutton->is_active)
            destroy_widget(filebutton->w,w->app);
    }
}

static void fbutton_mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileButton *filebutton = (FileButton *)w->private_struct;
    free(filebutton->last_path);
    filebutton->last_path = NULL;
    free(filebutton);
    filebutton = NULL;
}

Widget_t *add_file_button(Widget_t *parent, int x, int y, int width, int height,
                           const char *path, const char *filter) {
    FileButton *filebutton = (FileButton*)malloc(sizeof(FileButton));
    filebutton->path = path;
    filebutton->filter = filter;
    filebutton->last_path = NULL;
    filebutton->w = NULL;
    filebutton->is_active = false;
    Widget_t *fbutton = add_image_toggle_button(parent, "", x, y, width, height);
    fbutton->private_struct = filebutton;
    fbutton->flags |= HAS_MEM;
    widget_get_png(fbutton, LDVAR(directory_open_png));
    fbutton->scale.gravity = CENTER;
    fbutton->func.mem_free_callback = fbutton_mem_free;
    fbutton->func.value_changed_callback = fbutton_callback;
    fbutton->func.dialog_callback = fdialog_response;
    return fbutton;
}
