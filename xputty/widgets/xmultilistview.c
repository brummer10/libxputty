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


#include "xmultilistview.h"
#include "xmultilistview_private.h"
#include "xtooltip.h"
#include "xslider.h"


void multi_listview_set_active_entry(Widget_t *w, int active) {
    if(active<0) return;
    Widget_t* view_port =  w->childlist->childs[0];
    ViewMultiList_t *filelist = (ViewMultiList_t*)view_port->parent_struct;
    float value = (float)active;
    if (value>w->adj->max_value) value = w->adj->max_value;
    else if (value<w->adj->min_value) value = w->adj->min_value;
    filelist->active_item = (int)value;
    adj_set_value(w->adj,filelist->active_item);
}

void multi_listview_unset_active_entry(Widget_t *w) {
    Widget_t* view_port =  w->childlist->childs[0];
    ViewMultiList_t *filelist = (ViewMultiList_t*)view_port->parent_struct;
    filelist->active_item = -1;
    filelist->prelight_item = -1;
}

Widget_t* create_multi_listview_viewport(Widget_t *parent, int elem, int width, int height) {
    Widget_t *wid = create_widget(parent->app, parent, 0, 0, width, height);
    XSelectInput(wid->app->dpy, wid->widget,StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask
                    |ButtonPressMask|Button1MotionMask|PointerMotionMask);
    wid->scale.gravity = NORTHWEST;
    ViewMultiList_t *filelist;
    filelist = (ViewMultiList_t*)malloc(sizeof(ViewMultiList_t));
    filelist->show_items = elem;
    filelist->check_dir = 0;
    wid->flags |= HAS_MEM;
    wid->parent_struct = filelist;
    float max_value = -elem;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, max_value,1.0, CL_VIEWPORT);
    wid->adj = wid->adj_y;
    wid->func.adj_callback = _set_multi_listview_viewpoint;
    wid->func.motion_callback = _multi_list_motion;
    wid->func.leave_callback = _leave_multi_list;
    wid->func.button_release_callback = _multi_list_entry_released;
    wid->func.double_click_callback = _multi_list_entry_double_clicked;
    wid->func.key_press_callback = _multi_list_key_pressed;
    wid->func.expose_callback = _draw_multi_list;
    wid->func.configure_notify_callback = _reconfigure_multi_listview_viewport;
    wid->func.map_notify_callback = _reconfigure_multi_listview_viewport;
    wid->func.mem_free_callback = multi_listview_mem_free;
    return wid;
}

Widget_t* add_multi_listview(Widget_t *parent, const char * label,
                int x, int y, int width, int height) {

    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
    wid->label = label;
    wid->scale.gravity = CENTER;
    wid->flags &= ~USE_TRANSPARENCY;
    wid->func.expose_callback = _draw_multi_listview;
    int elem = height/75;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, -1.0,1.0, CL_NONE);
    wid->adj = wid->adj_y;
    Widget_t *viewport = create_multi_listview_viewport(wid, elem, width-10, height);

    ViewMultiList_t *filelist = (ViewMultiList_t*)viewport->parent_struct;
    filelist->folder = surface_get_png(wid, filelist->folder, LDVAR(directory_png));
    filelist->folder_select = surface_get_png(wid, filelist->folder_select, LDVAR(directory_select_png));
    filelist->file = surface_get_png(wid, filelist->folder, LDVAR(file_png));
    filelist->folder_scaled = NULL;
    filelist->folder_select_scaled = NULL;
    filelist->file_scaled = NULL;
    filelist->scale_down = 0.2;
    filelist->scale_up = 1.0/0.2;
    filelist->item_height = 375*filelist->scale_down;
    filelist->item_width = 500*filelist->scale_down;
    filelist->column = max(1,width/filelist->item_width);
    filelist->icon_pos = (filelist->item_width/2) - 120/filelist->scale_up;
    filelist->slider = add_vslider(wid, "", width-10, 0, 10, height);
    filelist->slider->func.expose_callback = _draw_multi_listviewslider;
    filelist->slider->adj_y = add_adjustment(filelist->slider,0.0, 0.0, 0.0, 1.0,1.0, CL_VIEWPORTSLIDER);
    filelist->slider->adj = filelist->slider->adj_y;
    filelist->slider->func.value_changed_callback = _set_multi_listviewport;
    filelist->slider->scale.gravity = WESTSOUTH;
    filelist->slider->flags &= ~USE_TRANSPARENCY;
    filelist->slider->flags |= NO_AUTOREPEAT | NO_PROPAGATE;
    filelist->slider->parent_struct = viewport;

    return wid;
}

void multi_listview_mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    ViewMultiList_t *filelist = (ViewMultiList_t*)w->parent_struct;
    cairo_surface_destroy(filelist->folder);
    cairo_surface_destroy(filelist->folder_select);
    cairo_surface_destroy(filelist->file);
    cairo_surface_destroy(filelist->folder_scaled);
    cairo_surface_destroy(filelist->folder_select_scaled);
    cairo_surface_destroy(filelist->file_scaled);
    free(filelist);
}

void multi_listview_remove_list(Widget_t *listview) {
    Widget_t* view_port =  listview->childlist->childs[0];
    ViewMultiList_t *filelist = (ViewMultiList_t*)view_port->parent_struct;
    filelist->list_names = NULL;
    filelist->list_size = 0;
    XWindowAttributes attrs;
    XGetWindowAttributes(listview->app->dpy, (Window)listview->widget, &attrs);
    int height = attrs.height;
    float elem = height/filelist->item_height;
    set_adjustment(listview->adj,0.0, 0.0, 0.0, -1.0,1.0, CL_NONE);
    set_adjustment(view_port->adj,0.0, 0.0, 0.0, -elem,1.0, CL_VIEWPORT);
    adj_set_value(filelist->slider->adj,0.0);
}

void multi_listview_set_list(Widget_t *listview, char **list, int list_size) {
    Widget_t* view_port =  listview->childlist->childs[0];
    ViewMultiList_t *filelist = (ViewMultiList_t*)view_port->parent_struct;
    filelist->list_names = list;
    filelist->list_size = list_size;
    set_adjustment(listview->adj,0.0, 0.0, 0.0, (float)(list_size-1.0),1.0, CL_NONE);
    float max_value = view_port->adj->max_value+ (float)list_size;
    set_adjustment(view_port->adj,0.0, 0.0, 0.0, max_value,1.0, CL_VIEWPORT);
    multi_listview_set_item_size(listview, filelist->scale_down);
}

void multi_listview_set_check_dir(Widget_t *listview, int set) {
    Widget_t* view_port =  listview->childlist->childs[0];
    ViewMultiList_t *filelist = (ViewMultiList_t*)view_port->parent_struct;
    filelist->check_dir = set;
}

cairo_surface_t * scale_image(Widget_t *listview, ViewMultiList_t *filelist,
                cairo_surface_t *orig, cairo_surface_t *scaled) {

    cairo_surface_destroy(scaled);
    scaled = cairo_surface_create_similar (listview->surface, 
        CAIRO_CONTENT_COLOR_ALPHA, filelist->item_width,
                                    filelist->item_height);
    assert(cairo_surface_status(scaled) == CAIRO_STATUS_SUCCESS); 

    cairo_t *cri = cairo_create (scaled);
    cairo_scale(cri,filelist->scale_down, filelist->scale_down);
    cairo_set_source_surface (cri, orig,0,0);
    cairo_rectangle(cri, 0, 0, filelist->item_width* filelist->scale_up,
                                filelist->item_height* filelist->scale_up);
    cairo_fill (cri);
    cairo_destroy(cri);
    return scaled;
}

void multi_listview_set_item_size(Widget_t *listview, float set) {
    Widget_t* view_port =  listview->childlist->childs[0];
    ViewMultiList_t *filelist = (ViewMultiList_t*)view_port->parent_struct;
    filelist->scale_down = set;
    filelist->scale_up = 1.0/set;
    filelist->item_height = 375*filelist->scale_down;
    filelist->item_width = 500*filelist->scale_down;
    _reconfigure_multi_listview_viewport(view_port, NULL);
    filelist->folder_scaled = scale_image(listview,filelist,filelist->folder, filelist->folder_scaled);
    filelist->folder_select_scaled = scale_image(listview,filelist,filelist->folder_select, filelist->folder_select_scaled);
    filelist->file_scaled = scale_image(listview,filelist,filelist->file, filelist->file_scaled);
    expose_widget(view_port);
}
