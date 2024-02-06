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

#include "xcombobox.h"
#include "xcombobox_private.h"
#include "xtooltip.h"
#include "xslider.h"


void combobox_set_active_entry(Widget_t *w, int active) {
    float value = (float)active;
    if (value>w->adj->max_value) value = w->adj->max_value;
    else if (value<w->adj->min_value) value = w->adj->min_value;
    adj_set_value(w->adj, value);
}

void combobox_rename_entry(Widget_t *w, int active, const char* label) {
    float value = (float)active;
    if ((value>w->adj->max_value) || (value<w->adj->min_value)) return;
    Widget_t * menu = w->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    //fprintf(stderr,"%s %s\n", comboboxlist->list_names[active], label);
    free(comboboxlist->list_names[active]);
    comboboxlist->list_names[active] = NULL;
    asprintf(&comboboxlist->list_names[active],"%s",label);
    assert(comboboxlist->list_names != NULL);
}

void combobox_mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    ComboBox_t *comboboxlist = (ComboBox_t*)w->parent_struct;
    unsigned int j = 0;
    for(; j<comboboxlist->list_size;j++) {
        free(comboboxlist->list_names[j]);
        comboboxlist->list_names[j] = NULL;
    }
    free(comboboxlist->list_names);
    free(comboboxlist);
}

void combobox_set_menu_size(Widget_t *combobox, int v) {
    Widget_t * menu = combobox->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    comboboxlist->show_items = v;
    _configure_combobox_menu(combobox, menu, comboboxlist->show_items, true);
}

void combobox_set_pop_position(Widget_t *combobox, int v) {
    Widget_t * menu = combobox->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    comboboxlist->pop_pos = v;
}

void combobox_delete_entrys(Widget_t *combobox) {
    Widget_t * menu = combobox->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    unsigned int j = 0;
    for(; j<comboboxlist->list_size;j++) {
        free(comboboxlist->list_names[j]);
        comboboxlist->list_names[j] = NULL;
    }
    comboboxlist->list_size = 0;
    set_adjustment(combobox->adj,0.0, 0.0, 0.0, -1.0,1.0, CL_ENUM);
    set_adjustment(view_port->adj,0.0, 0.0, 0.0, -6.0,1.0, CL_ENUM);
    set_adjustment(comboboxlist->slider->adj,0.0, 0.0, 0.0, 1.0,0.0085, CL_VIEWPORTSLIDER);
}

void pop_combobox_menu_show(Widget_t *parent, Widget_t *menu, bool above) {
    if (!childlist_has_child(menu->childlist)) return;
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    if (!comboboxlist->list_size) return;
    _configure_combobox_menu(parent, menu, comboboxlist->show_items, above);
    pop_widget_show_all(menu);
    int err = os_grab_pointer(menu);
    menu->app->hold_grab = menu;

    if (err) debug_print("Error grap pointer\n");
}


Widget_t* create_combobox_viewport(Widget_t *parent, int elem, int width, int height) {
    Widget_t *wid = create_widget(parent->app, parent, 0, 0, width, height);
    os_set_input_mask(wid);
    wid->scale.gravity = CENTER;
    ComboBox_t *comboboxlist;
    comboboxlist = (ComboBox_t*)malloc(sizeof(ComboBox_t));
    comboboxlist->show_items = elem;
    comboboxlist->prelight_item = 0;
    comboboxlist->active_item = 0;
    comboboxlist->list_names = NULL;
    comboboxlist->list_size = 0;
    comboboxlist->item_height = 25 * parent->app->hdpi;
    comboboxlist->sc = 1.0;
    comboboxlist->pop_pos = 0;
    wid->flags |= HAS_MEM;
    wid->parent_struct = comboboxlist;
    float max_value = -elem;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, max_value,1.0, CL_VIEWPORT);
    wid->adj = wid->adj_y;
    wid->func.adj_callback = _set_combobox_viewpoint;
    wid->func.motion_callback = _combobox_motion;
    wid->func.leave_callback = _leave_combobox;
    wid->func.button_release_callback = _combobox_entry_released;
    wid->func.key_press_callback = _combobox_key_pressed;
    wid->func.expose_callback = _draw_combobox_entrys;
    wid->func.configure_notify_callback = _reconfigure_combobox_viewport;
    wid->func.mem_free_callback = combobox_mem_free;
    return wid;
}

Widget_t* create_combobox_menu(Widget_t *parent, int height) {

    int x1, y1;
    os_translate_coords( parent, parent->widget, os_get_root_window(parent->app, IS_WIDGET), 0, 0, &x1, &y1);
    Widget_t *wid = create_window(parent->app, os_get_root_window(parent->app, IS_WIDGET), x1, y1, 10, height);
    Widget_t *viewport = create_combobox_viewport(wid, 6, 10, 5*height);
    ComboBox_t *comboboxlist = (ComboBox_t*)viewport->parent_struct;
    comboboxlist->combobox = parent;

    os_set_window_attrb(wid);
    os_set_transient_for_hint(parent, wid);
    wid->func.expose_callback = _draw_combobox_menu;
    wid->flags |= IS_POPUP;
    wid->scale.gravity = NONE;
    childlist_add_child(parent->childlist, wid);

    comboboxlist->slider = add_vslider(wid, "", 0, 0, 10, height);
    comboboxlist->slider->func.expose_callback = _draw_combobox_menu_slider;
    set_adjustment(comboboxlist->slider->adj_y,0.0, 0.0, 0.0, 1.0,0.0085, CL_VIEWPORTSLIDER);
    comboboxlist->slider->adj = comboboxlist->slider->adj_y;
    comboboxlist->slider->func.value_changed_callback = _set_combobox_menu_viewport;
    comboboxlist->slider->scale.gravity = NORTHWEST;
    comboboxlist->slider->flags &= ~USE_TRANSPARENCY;
    comboboxlist->slider->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    comboboxlist->slider->parent_struct = viewport;

    return wid;
}

Widget_t* add_combobox(Widget_t *parent, const char  * label, int x, int y, int width, int height) {

    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
    wid->label = label;
    wid->scale.gravity = CENTER;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, -1.0,1.0, CL_ENUM);
    wid->adj = wid->adj_y;
    wid->func.adj_callback = _set_entry;
    wid->func.expose_callback = _draw_combobox;
    wid->func.enter_callback = os_transparent_draw;
    wid->func.leave_callback = os_transparent_draw;
    wid->func.button_release_callback = _combobox_button_released;

    Widget_t* button = add_button(wid, "", width-20, 0, 20, height);
    button->func.expose_callback = _draw_combobox_button;  
    button->func.button_release_callback = _button_combobox_released;

    Widget_t* menu = create_combobox_menu(wid, 25);
    menu->func.button_release_callback = _entry_released;

    return wid;
}

void combobox_add_entry(Widget_t *wid, const char  * label) {
    Widget_t *menu = wid->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    comboboxlist->list_size++;
    comboboxlist->list_names = (char **)realloc(comboboxlist->list_names,
      comboboxlist->list_size * sizeof(char *));
    asprintf(&comboboxlist->list_names[comboboxlist->list_size-1],"%s",label);
    assert(comboboxlist->list_names != NULL);
    float max_value = wid->adj->max_value+1.0;
    set_adjustment(wid->adj,0.0, max_value, 0.0, max_value,1.0, CL_ENUM);

}

void combobox_add_numeric_entrys(Widget_t *wid, int imin, int imax) {
    int i = imin;
    int o = imax+1;
    for (;i<o;i++) {
        char s[32];
        snprintf(s, 31, "%i",i);
        combobox_add_entry(wid, s);
    }
}
