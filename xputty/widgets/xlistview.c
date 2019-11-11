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


#include "xlistview.h"
#include "xlistview_private.h"
#include "xtooltip.h"


/**
 * @brief listview_set_active_entry   - set the active listview entry
 * @param *w_                        - void pointer to the Widget_t listview
 * @param active                     - the active entry (int)
 * @return void
 */

void listview_set_active_entry(Widget_t *w, int active) {
    if(active<0) return;
    Widget_t* view_port =  w->childlist->childs[0];
    ViewList_t *filelist = (ViewList_t*)view_port->parent_struct;
    float value = (float)active;
    if (value>w->adj->max_value) value = w->adj->max_value;
    else if (value<w->adj->min_value) value = w->adj->min_value;
    filelist->active_item = (int)value;
    adj_set_value(w->adj,filelist->active_item);
}

/**
 * @brief create_listview_viewport     - create a viewport on a listview to a Widget_t
 * @param *parent                     - pointer to the Widget_t request the viewport
 * @param width                       - define the width of the viewport
 * @param height                      - define the height of the viewport
 * @return Widget_t*                  - pointer to the Widget_t viewport
 */

Widget_t* create_listview_viewport(Widget_t *parent, int elem, int width, int height) {
    Widget_t *wid = create_widget(parent->app, parent, 0, 0, width, height);
    XSelectInput(wid->app->dpy, wid->widget,StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask
                    |ButtonPressMask|Button1MotionMask|PointerMotionMask);
    wid->scale.gravity = NORTHEAST;
    ViewList_t *filelist;
    filelist = (ViewList_t*)malloc(sizeof(ViewList_t));
    filelist->show_items = elem;
    wid->flags |= HAS_MEM;
    wid->parent_struct = filelist;
    float max_value = -elem;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, max_value,1.0, CL_VIEWPORT);
    wid->adj = wid->adj_y;
    wid->func.adj_callback = _set_listview_viewpoint;
    wid->func.motion_callback = _list_motion;
    wid->func.leave_callback = _leave_list;
    wid->func.button_release_callback = _list_entry_released;
    wid->func.key_press_callback = _list_key_pressed;
    wid->func.expose_callback = _draw_list;
    wid->func.configure_notify_callback = _reconfigure_listview_viewport;
    wid->func.map_notify_callback = _configure_listview;
    wid->func.mem_free_callback = listview_mem_free;
    return wid;
}

/**
 * @brief add_listview        - add a listview to a Widget_t
 * @param *parent             - pointer to the Widget_t request the listview
 * @param *label              - Label to show on the listview (not used)
 * @param x,y,width,height    - the position/geometry to create the listview
 * @return Widget_t*          - pointer to the Widget_t label struct
 */

Widget_t* add_listview(Widget_t *parent, const char * label,
                int x, int y, int width, int height) {

    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
    wid->label = label;
    wid->scale.gravity = CENTER;
    wid->flags &= ~USE_TRANSPARENCY;
    wid->func.expose_callback = _draw_listview;
    int elem = height/25;
    wid->adj_y = add_adjustment(wid,0.0, 0.0, 0.0, -1.0,1.0, CL_NONE);
    wid->adj = wid->adj_y;
    create_listview_viewport(wid, elem, width, height);
    return wid;
}

/**
 * @brief listview_mem_free        - release additional used memory when destroy the Widget_t
 * @param *w_                      - void pointer to the Widget_t
 * @param *user_data               - void pointer to attached user_data
 * @return void
 */

void listview_mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    ViewList_t *filelist = (ViewList_t*)w->parent_struct;
    free(filelist);
}

/**
 * @brief listview_set_list   - set a list to a listview  Widget_t
 * @param *listview           - pointer to the Widget_t listview which should show the list
 * @param **list              - pointer to the list
 * @param list_size           - how many items contain the list
 * @return void
 */

void listview_set_list(Widget_t *listview, char **list, int list_size) {
    Widget_t* view_port =  listview->childlist->childs[0];
    ViewList_t *filelist = (ViewList_t*)view_port->parent_struct;
    filelist->list_names = list;
    filelist->list_size = list_size;
    set_adjustment(listview->adj,0.0, 0.0, 0.0, (float)(list_size-1.0),1.0, CL_NONE);
    float max_value = view_port->adj->max_value+ (float)list_size;
    set_adjustment(view_port->adj,0.0, 0.0, 0.0, max_value,1.0, CL_VIEWPORT);
    _configure_listview(view_port, NULL);
}

