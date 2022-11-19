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

#include <libgen.h>

#include "xfile-dialog.h"
#include "xmessage-dialog.h"

static void file_released_callback(void *w_, void *button, void* user_data);
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
    set_pattern(w,&w->color_scheme->selected,&w->color_scheme->normal,BACKGROUND_);
    cairo_fill (w->crb);

    //widget_set_scale(w);
    use_fg_color_scheme(w, NORMAL_);
    cairo_set_font_size (w->crb, 12.0);
    cairo_move_to (w->crb, 20, 35);
    cairo_show_text(w->crb, _("Directory"));
    cairo_move_to (w->crb, 20, 85);
    cairo_show_text(w->crb, _("Directories"));
    cairo_move_to (w->crb, 20, 340-w->scale.scale_y);
    cairo_show_text(w->crb, _("Select: "));
    cairo_move_to (w->crb, 45, 380-w->scale.scale_y);
    cairo_show_text(w->crb, _("Show hidden Directories")); 
    cairo_move_to (w->crb, 70, 340-w->scale.scale_y);
    cairo_show_text(w->crb, w->label);
    //widget_reset_scale(w);
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

static void forward_key_press(void *w_, void *key_, void *user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    if (file_dialog->text_entry)
        file_dialog->text_entry->func.key_press_callback(file_dialog->text_entry, key_, user_data);
}

static void forward_listview_key_press(void *w_, void *key_, void *user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *parent = (Widget_t*)w->parent;
    FileDialog *file_dialog = (FileDialog *)parent->parent_struct;
    if (file_dialog->text_entry)
        file_dialog->text_entry->func.key_press_callback(file_dialog->text_entry, key_, user_data);
}

static inline int set_files(FileDialog *file_dialog) {
    listview_set_list(file_dialog->ft,file_dialog->fp->file_names , (int)file_dialog->fp->file_counter);
    int ret = 0;
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

static void file_released_b_callback(void *w_, void *button, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    set_selected_file(file_dialog);
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
    int ds = fp_get_files(file_dialog->fp,file_dialog->fp->path, 1, 0);
    file_dialog->ft->func.button_release_callback = file_released_b_callback;
    int set_f = set_files(file_dialog);
    set_dirs(file_dialog);
    combobox_set_active_entry(file_dialog->ct, ds);
    listview_set_active_entry(file_dialog->ft, set_f);
    expose_widget(file_dialog->ft);
    expose_widget(file_dialog->ct);
}

static void set_selected_file(FileDialog *file_dialog) {
    if(adj_get_value(file_dialog->ft->adj)<0 ||
        adj_get_value(file_dialog->ft->adj) > file_dialog->fp->file_counter) return;

    struct stat sb;
    if (stat(file_dialog->fp->file_names[(int)adj_get_value(file_dialog->ft->adj)], &sb) == 0 && S_ISDIR(sb.st_mode)) {
        asprintf(&file_dialog->fp->path, "%s",file_dialog->fp->file_names[(int)adj_get_value(file_dialog->ft->adj)]);
        reload_from_dir(file_dialog);
       // return;
    }

    Widget_t* menu =  file_dialog->ct->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    if ((int)adj_get_value(file_dialog->ct->adj) < 0) return;
    free(file_dialog->fp->selected_file);
    file_dialog->fp->selected_file = NULL;
    asprintf(&file_dialog->fp->selected_file, "%s/",comboboxlist->list_names[(int)adj_get_value(file_dialog->ct->adj)]);
    assert(file_dialog->fp->selected_file != NULL);
}

static void file_released_callback(void *w_, void *button, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    set_selected_file(file_dialog);
    if(file_dialog->fp->selected_file) {
        file_dialog->w->label = file_dialog->fp->selected_file;
        expose_widget(file_dialog->w);
    }
}

static void set_selected_dir(FileDialog *file_dialog) {
    Widget_t* menu =  file_dialog->ct->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    if ((int)adj_get_value(file_dialog->ct->adj) < 0) return;
    free(file_dialog->fp->selected_file);
    file_dialog->fp->selected_file = NULL;
    asprintf(&file_dialog->fp->selected_file, "%s/",comboboxlist->list_names[(int)adj_get_value(file_dialog->ct->adj)]);
    assert(file_dialog->fp->selected_file != NULL);
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
    file_dialog->w->label = file_dialog->fp->path;
    expose_widget(file_dialog->w);
}

static void button_ok_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        if(!file_dialog->fp->selected_file) {
            set_selected_dir(file_dialog);
        }
        if(file_dialog->fp->selected_file) {
            file_dialog->parent->func.dialog_callback(file_dialog->parent,&file_dialog->fp->selected_file);
            file_dialog->send_clear_func = false;
        } else {
            open_message_dialog(w, INFO_BOX, _("INFO"),
                    _("Please select a file"),NULL);
            return;
        }
        destroy_widget(file_dialog->w,file_dialog->w->app);
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
    int ds = fp_get_files(file_dialog->fp,file_dialog->fp->path, 1, 0);
    file_dialog->ft->func.button_release_callback = file_released_callback;
    int set_f = set_files(file_dialog);
    set_dirs(file_dialog);
    combobox_set_active_entry(file_dialog->ct, ds);
    listview_set_active_entry(file_dialog->ft, set_f);
    expose_widget(file_dialog->ft);
}

static void button_hidden_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    if (w->flags & HAS_POINTER) {
        file_dialog->fp->show_hidden = adj_get_value(w->adj) ? true : false;
        reload_all(file_dialog);
    }
}

static void get_entry(Widget_t *w) {
    Widget_t *p = (Widget_t*)w->parent;
    FileDialog *file_dialog = (FileDialog *)p->parent_struct;
    if (strlen( file_dialog->text_entry->input_label))
        file_dialog->text_entry->input_label[strlen( file_dialog->text_entry->input_label)-1] = 0;
    file_dialog->text_entry->label = file_dialog->text_entry->input_label;
}

static void save_on_enter(void *w_) {
    Widget_t *w = (Widget_t*)w_;
    FileDialog *file_dialog = (FileDialog *)w->parent_struct;
    Widget_t* menu =  file_dialog->ct->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    if (strlen( file_dialog->text_entry->input_label)) {
        asprintf(&file_dialog->fp->selected_file, "%s/%s",comboboxlist->list_names[(int)adj_get_value(file_dialog->ct->adj)],
            file_dialog->text_entry->label);
        asprintf(&file_dialog->fp->path, "%s",file_dialog->fp->selected_file);

        struct stat st = {0};
        int result = mkdir(file_dialog->fp->selected_file, 0755);
        if (result == 0 || (stat(file_dialog->fp->selected_file, &st) != -1))
            reload_from_dir(file_dialog);
    }
    destroy_widget(file_dialog->text_entry, file_dialog->w->app);
    file_dialog->w->label = file_dialog->fp->path;
    expose_widget(file_dialog->w);
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
            get_entry(file_dialog->text_entry);
            save_on_enter(file_dialog->w_okay);
            return;
        }
        if(status == XLookupChars || status == XLookupBoth){
            entry_add_text(w, buf);
        }
    }
}

static void add_dir_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER && !*(int*)user_data){
        FileDialog *file_dialog = (FileDialog *)w->parent_struct;
        file_dialog->text_entry = create_widget(file_dialog->w->app, file_dialog->w, 230, 360, 200, 30);
        memset(file_dialog->text_entry->input_label, 0, 32 * (sizeof file_dialog->text_entry->input_label[0]) );
        file_dialog->text_entry->func.expose_callback = entry_add_text;
        file_dialog->text_entry->func.key_press_callback = entry_get_text;
        file_dialog->text_entry->flags &= ~USE_TRANSPARENCY;
        file_dialog->text_entry->scale.gravity = CENTER;
        file_dialog->text_entry->parent_struct = file_dialog;
        widget_show_all(file_dialog->w);
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

Widget_t *open_directory_dialog(Widget_t *w, const char *path) {
    FileDialog *file_dialog = (FileDialog*)malloc(sizeof(FileDialog));
    
    file_dialog->fp = (FilePicker*)malloc(sizeof(FilePicker));
    fp_init(file_dialog->fp, (path) ? path : "/");
    file_dialog->parent = w;
    file_dialog->send_clear_func = true;

    file_dialog->w = create_window(w->app, DefaultRootWindow(w->app->dpy), 0, 0, 660, 420);

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

    file_dialog->text_entry = NULL;

    file_dialog->w->flags |= HAS_MEM;
    file_dialog->w->parent_struct = file_dialog;
    widget_set_title(file_dialog->w, _("Directory Selector"));
    file_dialog->w->func.expose_callback = draw_window;
    file_dialog->w->func.key_press_callback = forward_key_press;
    file_dialog->w->func.mem_free_callback = fd_mem_free;
    widget_set_icon_from_png(file_dialog->w, LDVAR(directory_png));

    file_dialog->ct = add_combobox(file_dialog->w, "", 20, 40, 550, 30);
    file_dialog->ct->parent_struct = file_dialog;
    file_dialog->ct->scale.gravity = NORTHEAST;
    file_dialog->ct->func.key_press_callback = forward_key_press;
    file_dialog->ct->func.value_changed_callback = combo_response;

    file_dialog->sel_dir = add_button(file_dialog->w, _("Add"), 580, 40, 60, 30);
    file_dialog->sel_dir->parent_struct = file_dialog;
    file_dialog->sel_dir->scale.gravity = WESTNORTH;
    add_tooltip(file_dialog->sel_dir,_("Add new Directory"));
    file_dialog->sel_dir->func.key_press_callback = forward_key_press;
    file_dialog->sel_dir->func.value_changed_callback = add_dir_callback;

    file_dialog->ft = add_listview(file_dialog->w, "", 20, 90, 620, 225);
    file_dialog->ft->parent_struct = file_dialog;
    file_dialog->ft->scale.gravity = NORTHWEST;
    //file_dialog->ft->func.value_changed_callback = file_released_callback;
    file_dialog->ft->func.key_press_callback = forward_key_press;
    file_dialog->ft->func.button_release_callback = file_released_callback;
    Widget_t* view_port = file_dialog->ft->childlist->childs[0];
    view_port->func.key_press_callback = forward_listview_key_press;

    int ds = fp_get_files(file_dialog->fp,file_dialog->fp->path, 1, 0);   
    int set_f = set_files(file_dialog); 
    set_dirs(file_dialog);
    combobox_set_active_entry(file_dialog->ct, ds);
    listview_set_active_entry(file_dialog->ft, set_f);

    file_dialog->w_quit = add_button(file_dialog->w, _("Cancel"), 580, 350, 60, 60);
    file_dialog->w_quit->parent_struct = file_dialog;
    file_dialog->w_quit->scale.gravity = SOUTHWEST;
    add_tooltip(file_dialog->w_quit,_("Exit Directory selector"));
    file_dialog->w_quit->func.key_press_callback = forward_key_press;
    file_dialog->w_quit->func.value_changed_callback = button_quit_callback;

    file_dialog->w_okay = add_button(file_dialog->w, _("Select"), 510, 350, 60, 60);
    file_dialog->w_okay->parent_struct = file_dialog;
    file_dialog->w_okay->scale.gravity = SOUTHWEST;
    add_tooltip(file_dialog->w_okay,_("Selected Directory"));
    file_dialog->w_okay->func.key_press_callback = forward_key_press;
    file_dialog->w_okay->func.value_changed_callback = button_ok_callback;

    file_dialog->w_hidden = add_check_button(file_dialog->w, "", 20, 365, 20, 20);
    file_dialog->w_hidden->parent_struct = file_dialog;
    file_dialog->w_hidden->scale.gravity = EASTWEST;
    add_tooltip(file_dialog->w_hidden,_("Show hidden Directories"));
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

static void ddialog_response(void *w_, void* user_data) {
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

static void dbutton_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileButton *filebutton = (FileButton *)w->parent_struct;
    if (w->flags & HAS_POINTER && adj_get_value(w->adj)){
        filebutton->w = open_directory_dialog(w,filebutton->path);
        filebutton->is_active = true;
    } else if (w->flags & HAS_POINTER && !adj_get_value(w->adj)){
        if(filebutton->is_active)
            destroy_widget(filebutton->w,w->app);
    }
}

static void dbutton_mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    FileButton *filebutton = (FileButton *)w->parent_struct;
    free(filebutton->last_path);
    filebutton->last_path = NULL;
    free(filebutton);
    filebutton = NULL;
}

Widget_t *add_directory_button(Widget_t *parent, int x, int y, int width, int height,
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
    fbutton->func.mem_free_callback = dbutton_mem_free;
    fbutton->func.value_changed_callback = dbutton_callback;
    fbutton->func.dialog_callback = ddialog_response;
    return fbutton;
}
