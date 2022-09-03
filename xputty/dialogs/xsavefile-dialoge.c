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


static void file_released_callback(void *w_, void* user_data);
static void combo_response(void *w_, void* user_data);
static void set_selected_file(FileDialog *file_dialog, int reload);

static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    cairo_rectangle(w->crb,0,0,width,height);
    set_pattern(w,&w->color_scheme->selected,&w->color_scheme->normal,BACKGROUND_);
    cairo_fill (w->crb);

    use_fg_color_scheme(w, NORMAL_);
    cairo_set_font_size (w->crb, 12.0);
    cairo_move_to (w->crb, 20, 35);
    cairo_show_text(w->crb, _("Base Directory"));
    cairo_move_to (w->crb, 20, 85);
    cairo_show_text(w->crb, _("Places"));
    cairo_move_to (w->crb, 130, 85);
    cairo_show_text(w->crb, _("Entries"));
    cairo_move_to (w->crb, 20, 340-w->scale.scale_y);
    cairo_show_text(w->crb, _("Load: "));
    cairo_move_to (w->crb, 45, 380-w->scale.scale_y);
    cairo_show_text(w->crb, _("Show hidden files")); 
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
    listview_set_list(file_dialog->ft,file_dialog->fp->file_names , (int)file_dialog->fp->file_counter);
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

static void get_entry(Widget_t *w) {
    Widget_t *p = (Widget_t*)w->parent;
    FileDialog *file_dialog = (FileDialog *)p->parent_struct;
    if (strlen( file_dialog->text_entry->input_label))
        file_dialog->text_entry->input_label[strlen( file_dialog->text_entry->input_label)-1] = 0;
    file_dialog->text_entry->label = file_dialog->text_entry->input_label;
}

static void file_released_b_callback(void *w_, void *button, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    set_selected_file(file_dialog, 1);
    if(file_dialog->fp->selected_file) {
        file_dialog->w->label = file_dialog->fp->selected_file;
        expose_widget(file_dialog->w);
    }
}

static void dummy(void *w_, void* user_data) {
    
}

static void reload_from_dir(FileDialog *file_dialog) {
    file_dialog->ft->func.value_changed_callback = dummy;
    assert(file_dialog->fp->path != NULL);
    listview_remove_list(file_dialog->ft);
    combobox_delete_entrys(file_dialog->ct);
    int ds = fp_get_files(file_dialog->fp,file_dialog->fp->path, 1, 1);
    file_dialog->ft->func.button_release_callback = file_released_b_callback;
    int set_f = set_files(file_dialog);
    set_dirs(file_dialog);
    combobox_set_active_entry(file_dialog->ct, ds);
    if (set_f != -1) {
        listview_set_active_entry(file_dialog->ft, set_f);
    } else {
        listview_unset_active_entry(file_dialog->ft);
    }
    listview_unset_active_entry(file_dialog->xdg_dirs);
    expose_widget(file_dialog->ft);
    expose_widget(file_dialog->ct);
    expose_widget(file_dialog->xdg_dirs);
}

static void set_selected_file(FileDialog *file_dialog, int reload) {
    Widget_t* menu =  file_dialog->ct->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    if ((int)adj_get_value(file_dialog->ct->adj) < 0) return;
    free(file_dialog->fp->selected_file);
    file_dialog->fp->selected_file = NULL;
    get_entry(file_dialog->text_entry);
    if (strlen(file_dialog->text_entry->label)) {
        asprintf(&file_dialog->fp->selected_file, "%s/%s", comboboxlist->list_names[(int)adj_get_value(file_dialog->ct->adj)],
            file_dialog->text_entry->label);
    } else if(file_dialog->fp->file_counter ) {
        struct stat sb;
        if (stat(file_dialog->fp->file_names[(int)adj_get_value(file_dialog->ft->adj)], &sb) == 0 && S_ISDIR(sb.st_mode)) {
            asprintf(&file_dialog->fp->path, "%s",file_dialog->fp->file_names[(int)adj_get_value(file_dialog->ft->adj)]);
            if (reload) reload_from_dir(file_dialog);
            return;
        }
        asprintf(&file_dialog->fp->selected_file, "%s/%s", comboboxlist->list_names[(int)adj_get_value(file_dialog->ct->adj)],
            file_dialog->fp->file_names[(int)adj_get_value(file_dialog->ft->adj)]);
    }
}

static void file_released_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    set_selected_file(file_dialog, 1);
    if(file_dialog->fp->selected_file) {
        file_dialog->w->label = file_dialog->fp->selected_file;
        expose_widget(file_dialog->w);
    }
}

static void forward_key_press(void *w_, void *key_, void *user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    file_dialog->text_entry->func.key_press_callback(file_dialog->text_entry, key_, user_data);
}

static void reload_file_entrys(FileDialog *file_dialog) {
    listview_remove_list(file_dialog->ft);
    fp_get_files(file_dialog->fp,file_dialog->fp->path, 0, 1);
    int set_f = set_files(file_dialog);
    if (set_f != -1) {
        listview_set_active_entry(file_dialog->ft, set_f);
    } else {
        listview_unset_active_entry(file_dialog->ft);
    }
    file_dialog->ft->func.value_changed_callback = file_released_callback;
    expose_widget(file_dialog->ft);
}

static void combo_response(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    Widget_t* menu =  w->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    if ((int)adj_get_value(w->adj) < 0) return;
    free(file_dialog->fp->path);
    file_dialog->fp->path = NULL;
    asprintf(&file_dialog->fp->path, "%s",comboboxlist->list_names[(int)adj_get_value(w->adj)]);
    assert(file_dialog->fp->path != NULL);
    reload_from_dir(file_dialog);
}

static void save_and_exit(void *w_) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    if(file_dialog->fp->selected_file) {
        file_dialog->parent->func.dialog_callback(file_dialog->parent,&file_dialog->fp->selected_file);
        file_dialog->send_clear_func = false;
        destroy_widget(file_dialog->w,file_dialog->w->app);
    } else {
        open_message_dialog(w, INFO_BOX, _("INFO"), 
                _("Please enter a file name"),NULL);
    }
    
}

static void question_response(void *w_, void* user_data) {
    if(user_data !=NULL) {
        int response = *(int*)user_data;
        if(response == 0) {
            save_and_exit(w_);
        }
    }
}

static void button_ok_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        set_selected_file(file_dialog, 0);
        if( access(file_dialog->fp->selected_file, F_OK ) != -1 ) {
            open_message_dialog(w, QUESTION_BOX, file_dialog->fp->selected_file, 
                _("File already exists, would you overwrite it?"),NULL);
            w->func.dialog_callback = question_response;
        } else {
            save_and_exit(w_);
        }
   }
}

static void save_on_enter(void *w_) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    set_selected_file(file_dialog, 0);
    if( access(file_dialog->fp->selected_file, F_OK ) != -1 ) {
        open_message_dialog(w, QUESTION_BOX, file_dialog->fp->selected_file, 
           _("File already exists, would you overwrite it?"),NULL);
        w->func.dialog_callback = question_response;
    } else {
        save_and_exit(w_);
    }

}

static void reload_all(FileDialog *file_dialog) {
    file_dialog->ft->func.value_changed_callback = dummy;
    Widget_t* menu =  file_dialog->ct->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    if ((int)adj_get_value(file_dialog->ct->adj) < 0) return;
    free(file_dialog->fp->path);
    file_dialog->fp->path = NULL;
    asprintf(&file_dialog->fp->path, "%s",comboboxlist->list_names[(int)adj_get_value(file_dialog->ct->adj)]);
    assert(file_dialog->fp->path != NULL);
    listview_remove_list(file_dialog->ft);
    combobox_delete_entrys(file_dialog->ct);
    int ds = fp_get_files(file_dialog->fp,file_dialog->fp->path, 1, 1);
    int set_f = set_files(file_dialog);
    set_dirs(file_dialog);
    combobox_set_active_entry(file_dialog->ct, ds);
    if (set_f != -1) {
        listview_set_active_entry(file_dialog->ft, set_f);
    } else {
        listview_unset_active_entry(file_dialog->ft);
    }
    file_dialog->ft->func.value_changed_callback = file_released_callback;
    expose_widget(file_dialog->ft);
}

static void open_dir_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        reload_all(file_dialog);
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

static void set_filter_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    if (file_dialog->fp->use_filter != (int)adj_get_value(w->adj)) {
        file_dialog->fp->use_filter = (int)adj_get_value(w->adj);
        Widget_t* menu =  w->childlist->childs[1];
        Widget_t* view_port =  menu->childlist->childs[0];
        ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
        if ((int)adj_get_value(w->adj) < 0) return;
        free(file_dialog->fp->filter);
        file_dialog->fp->filter = NULL;
        asprintf(&file_dialog->fp->filter, "%s",comboboxlist->list_names[(int)adj_get_value(w->adj)]);
        assert(file_dialog->fp->filter != NULL);

        reload_file_entrys(file_dialog);
    }
}

static void draw_entry(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    if (attrs.map_state != IsViewable) return;

    use_base_color_scheme(w, NORMAL_);
    cairo_rectangle(w->cr,0,0,width,height);
    cairo_fill_preserve (w->cr);
    use_text_color_scheme(w, NORMAL_);
    cairo_set_line_width(w->cr, 2.0);
    cairo_stroke(w->cr);

    cairo_set_font_size (w->cr, 9.0);

    cairo_move_to (w->cr, 2, 9);
    cairo_show_text(w->cr, " ");
}

static void entry_add_text(void  *w_, void *label_) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    char *label = (char*)label_;
    if (!label) {
        label = (char*)"";
    }
    draw_entry(w,NULL);
    cairo_text_extents_t extents;
    use_text_color_scheme(w, NORMAL_);
    cairo_set_font_size (w->cr, 11.0);
    if (strlen( w->input_label))
         w->input_label[strlen( w->input_label)-1] = 0;
    if (strlen( w->input_label)<30) {
        if (strlen(label))
        strcat( w->input_label, label);
    }
    w->label = w->input_label;
    strcat( w->input_label, "|");
    cairo_set_font_size (w->cr, 12.0);
    cairo_text_extents(w->cr, w->input_label , &extents);

    cairo_move_to (w->cr, 2, 12.0+extents.height);
    cairo_show_text(w->cr,  w->input_label);

}

static void entry_clip(Widget_t *w) {
    draw_entry(w,NULL);
    cairo_text_extents_t extents;
    use_text_color_scheme(w, NORMAL_);
    cairo_set_font_size (w->cr, 11.0);

    // check for UTF 8 char
    if (strlen( w->input_label)>=2) {
        int i = strlen( w->input_label)-1;
        int j = 0;
        int u = 0;
        for(;i>0;i--) {
            if(IS_UTF8(w->input_label[i])) {
                 u++;
            }
            j++;
            if (u == 1) break;
            if (j>2) break;
        }
        if (!u) j =2;

        memset(&w->input_label[strlen( w->input_label)-(sizeof(char)*(j))],0,sizeof(char)*(j));
        strcat( w->input_label, "|");
    }
    cairo_set_font_size (w->cr, 12.0);
    cairo_text_extents(w->cr, w->input_label , &extents);

    cairo_move_to (w->cr, 2, 12.0+extents.height);
    cairo_show_text(w->cr,  w->input_label);

}

static void entry_get_text(void *w_, void *key_, void *user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XKeyEvent *key = (XKeyEvent*)key_;
    if (!key) return;
    int nk = key_mapping(w->app->dpy, key);
    if (nk == 11) {
        entry_clip(w);
    } else {
        Status status;
        KeySym keysym;
        char buf[32];
        Xutf8LookupString(w->xic, key, buf, sizeof(buf) - 1, &keysym, &status);
        if (keysym == XK_Return) {
            FileDialog *file_dialog = (FileDialog *)w->parent_struct;
            save_on_enter(file_dialog->w_okay);
            return;
        }
        if(status == XLookupChars || status == XLookupBoth){
            entry_add_text(w, buf);
        }
    }
}

static void fd_mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
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
    }

    file_dialog->xdg_user_dirs = (char **)realloc(file_dialog->xdg_user_dirs,
      (file_dialog->xdg_dir_counter + 1) * sizeof(char *));
    assert(file_dialog->xdg_user_dirs != NULL);
    asprintf(&file_dialog->xdg_user_dirs[file_dialog->xdg_dir_counter++], "%s", _("Computer"));
    assert(file_dialog->xdg_user_dirs[file_dialog->xdg_dir_counter-1] != NULL);

    fclose(fp);
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
    file_dialog->xdg_dirs->func.key_press_callback = forward_key_press;
    listview_set_list(file_dialog->xdg_dirs, file_dialog->xdg_user_dirs, (int)file_dialog->xdg_dir_counter);
    file_dialog->xdg_dirs->func.button_release_callback = xdg_dir_select_callback;
    listview_unset_active_entry(file_dialog->xdg_dirs);
}

Widget_t *save_file_dialog(Widget_t *w, const char *path, const char *filter) {
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

    file_dialog->w = create_window(w->app, DefaultRootWindow(w->app->dpy), 0, 0, 660, 420);
    file_dialog->w->flags |= HAS_MEM;
    file_dialog->w->parent_struct = file_dialog;
    widget_set_title(file_dialog->w, _("File Save"));
    file_dialog->w->func.expose_callback = draw_window;
    file_dialog->w->func.key_press_callback = forward_key_press;
    file_dialog->w->func.mem_free_callback = fd_mem_free;
    widget_set_icon_from_png(file_dialog->w,LDVAR(directory_png));

    XSizeHints* win_size_hints;
    win_size_hints = XAllocSizeHints();
    win_size_hints->flags =  PMinSize|PBaseSize|PMaxSize|PWinGravity;
    win_size_hints->min_width = 660;
    win_size_hints->min_height = 420;
    win_size_hints->base_width = 660;
    win_size_hints->base_height = 420;
    win_size_hints->max_width = 660;
    win_size_hints->max_height = 840;
    win_size_hints->win_gravity = CenterGravity;
    XSetWMNormalHints(file_dialog->w->app->dpy, file_dialog->w->widget, win_size_hints);
    XFree(win_size_hints);

    file_dialog->ct = add_combobox(file_dialog->w, "", 20, 40, 550, 30);
    file_dialog->ct->parent_struct = file_dialog;
    file_dialog->ct->scale.gravity = NORTHEAST;
    file_dialog->ct->func.value_changed_callback = combo_response;
    file_dialog->ct->func.key_press_callback = forward_key_press;

    file_dialog->sel_dir = add_button(file_dialog->w, _("Open"), 580, 40, 60, 30);
    file_dialog->sel_dir->parent_struct = file_dialog;
    file_dialog->sel_dir->scale.gravity = WESTNORTH;
    add_tooltip(file_dialog->sel_dir,_("Open sub-directory's"));
    file_dialog->sel_dir->func.value_changed_callback = open_dir_callback;
    file_dialog->sel_dir->func.key_press_callback = forward_key_press;

    file_dialog->ft = add_listview(file_dialog->w, "", 130, 90, 510, 225);
    file_dialog->ft->parent_struct = file_dialog;
    file_dialog->ft->scale.gravity = NORTHWEST;
    listview_set_check_dir(file_dialog->ft, 1);
    file_dialog->ft->func.key_press_callback = forward_key_press;

    int ds = fp_get_files(file_dialog->fp,file_dialog->fp->path, 1, 1);   
    int set_f = set_files(file_dialog); 
    set_dirs(file_dialog);
    combobox_set_active_entry(file_dialog->ct, ds);
    if (set_f != -1) {
        listview_set_active_entry(file_dialog->ft, set_f);
    } else {
        listview_unset_active_entry(file_dialog->ft);
    }
    file_dialog->ft->func.value_changed_callback = file_released_callback;

    add_xdg_dirs(file_dialog);

    file_dialog->text_entry = create_widget(file_dialog->w->app, file_dialog->w, 130, 320, 200, 30);
    memset(file_dialog->text_entry->input_label, 0, 32 * (sizeof file_dialog->text_entry->input_label[0]) );
    file_dialog->text_entry->func.expose_callback = entry_add_text;
    file_dialog->text_entry->func.key_press_callback = entry_get_text;
    file_dialog->text_entry->flags &= ~USE_TRANSPARENCY;
    file_dialog->text_entry->scale.gravity = EASTWEST;
    file_dialog->text_entry->parent_struct = file_dialog;
    Cursor c = XCreateFontCursor(file_dialog->w->app->dpy, XC_xterm);
    XDefineCursor (file_dialog->w->app->dpy, file_dialog->text_entry->widget, c);
    XFreeCursor(file_dialog->w->app->dpy, c);

    file_dialog->w_quit = add_button(file_dialog->w, _("Cancel"), 580, 350, 60, 60);
    file_dialog->w_quit->parent_struct = file_dialog;
    file_dialog->w_quit->scale.gravity = SOUTHWEST;
    add_tooltip(file_dialog->w_quit,_("Exit File Saver"));
    file_dialog->w_quit->func.value_changed_callback = button_quit_callback;
    file_dialog->w_quit->func.key_press_callback = forward_key_press;

    file_dialog->w_okay = add_button(file_dialog->w, _("Save"), 490, 350, 80, 60);
    file_dialog->w_okay->parent_struct = file_dialog;
    file_dialog->w_okay->scale.gravity = SOUTHWEST;
    add_tooltip(file_dialog->w_okay,_("Save as selected file"));
    file_dialog->w_okay->func.value_changed_callback = button_ok_callback;
    file_dialog->w_okay->func.key_press_callback = forward_key_press;

    file_dialog->set_filter = add_combobox(file_dialog->w, "", 340, 355, 120, 30);
    file_dialog->set_filter->parent_struct = file_dialog;
    file_dialog->set_filter->scale.gravity = SOUTHWEST;
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
    file_dialog->set_filter->func.key_press_callback = forward_key_press;
    file_dialog->set_filter->func.value_changed_callback = set_filter_callback;
    if(filter !=NULL && strlen(filter))
        combobox_set_active_entry(file_dialog->set_filter, 8);
    add_tooltip(file_dialog->set_filter->childlist->childs[0], _("File filter type"));

    file_dialog->w_hidden = add_check_button(file_dialog->w, "", 20, 365, 20, 20);
    file_dialog->w_hidden->parent_struct = file_dialog;
    file_dialog->w_hidden->scale.gravity = EASTWEST;
    add_tooltip(file_dialog->w_hidden,_("Show hidden files and folders"));
    file_dialog->w_hidden->func.key_press_callback = forward_key_press;
    file_dialog->w_hidden->func.value_changed_callback = button_hidden_callback;

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
    FileButton *filebutton = (FileButton *)w->parent_struct;
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
    FileButton *filebutton = (FileButton *)w->parent_struct;
    if (w->flags & HAS_POINTER && adj_get_value(w->adj)){
        filebutton->w = save_file_dialog(w,filebutton->path,filebutton->filter);
        filebutton->is_active = true;
    } else if (w->flags & HAS_POINTER && !adj_get_value(w->adj)){
        if(filebutton->is_active)
            destroy_widget(filebutton->w,w->app);
    }
}

static void fbutton_mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileButton *filebutton = (FileButton *)w->parent_struct;
    free(filebutton->last_path);
    filebutton->last_path = NULL;
    free(filebutton);
    filebutton = NULL;
}

Widget_t *add_save_file_button(Widget_t *parent, int x, int y, int width, int height,
                           const char *path, const char *filter) {
    FileButton *filebutton = (FileButton*)malloc(sizeof(FileButton));
    filebutton->path = path;
    filebutton->filter = filter;
    filebutton->last_path = NULL;
    filebutton->w = NULL;
    filebutton->is_active = false;
    Widget_t *fbutton = add_image_toggle_button(parent, "", x, y, width, height);
    fbutton->parent_struct = filebutton;
    fbutton->flags |= HAS_MEM;
    widget_get_png(fbutton, LDVAR(directory_open_png));
    fbutton->scale.gravity = CENTER;
    fbutton->func.mem_free_callback = fbutton_mem_free;
    fbutton->func.value_changed_callback = fbutton_callback;
    fbutton->func.dialog_callback = fdialog_response;
    return fbutton;
}
