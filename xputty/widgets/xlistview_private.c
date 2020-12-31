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


#include "xlistview_private.h"
#include "xtooltip.h"
#include <sys/stat.h>


void _draw_listview(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    set_pattern(w,&w->app->color_scheme->normal,&w->app->color_scheme->active,BACKGROUND_);
    cairo_paint (w->cr);
}

void _draw_list(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;
    ViewList_t *filelist = (ViewList_t*)w->parent_struct;

    use_base_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, 0, 0, width, height);
    cairo_fill (w->crb);
    cairo_set_font_size (w->crb, w->app->normal_font/w->scale.ascale);
    cairo_text_extents_t extents;
    cairo_text_extents(w->crb,"Ay", &extents);
    double h = extents.height;

    int i = (int)max(0,adj_get_value(w->adj));
    int a = 0;
    int j = filelist->list_size<filelist->show_items+i+1 ? 
      filelist->list_size : filelist->show_items+i+1;
    for(;i<j;i++) {
        if(i == filelist->prelight_item && i == filelist->active_item)
            use_base_color_scheme(w, ACTIVE_);
        else if(i == filelist->prelight_item)
            use_base_color_scheme(w, PRELIGHT_);
        else if (i == filelist->active_item)
            use_base_color_scheme(w, SELECTED_);
        else
            use_base_color_scheme(w,NORMAL_ );
        cairo_rectangle(w->crb, 0, a*25, width, 25);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.0);
        use_frame_color_scheme(w, PRELIGHT_);
        cairo_stroke(w->crb); 
        /** show label **/
        if(i == filelist->prelight_item && i == filelist->active_item)
            use_text_color_scheme(w, ACTIVE_);
        else if(i == filelist->prelight_item)
            use_text_color_scheme(w, PRELIGHT_);
        else if (i == filelist->active_item)
            use_text_color_scheme(w, SELECTED_);
        else
            use_text_color_scheme(w,NORMAL_ );

        struct stat sb;
        if (stat(filelist->list_names[i], &sb) == 0 && S_ISDIR(sb.st_mode)) {
            use_text_color_scheme(w, INSENSITIVE_);
        }
        cairo_text_extents(w->crb,filelist->list_names[i] , &extents);

        cairo_move_to (w->crb, 20, (25*(a+1)) - h +2 );
        cairo_show_text(w->crb, filelist->list_names[i]);
        cairo_new_path (w->crb);
        if (i == filelist->prelight_item && extents.width > (float)width-20) {
            tooltip_set_text(w,filelist->list_names[i]);
            w->flags |= HAS_TOOLTIP;
            show_tooltip(w);
        } else {
            w->flags &= ~HAS_TOOLTIP;
            hide_tooltip(w);
        }
        a++;
    }
}

void _list_motion(void *w_, void* xmotion_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    ViewList_t *filelist = (ViewList_t*)w->parent_struct;
    XMotionEvent *xmotion = (XMotionEvent*)xmotion_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int height = attrs.height;
    int _items = height/(height/25);
    int prelight_item = (xmotion->y/_items)  + (int)max(0,adj_get_value(w->adj));
    if(prelight_item != filelist->prelight_item) {
        filelist->prelight_item = prelight_item;
    }
    expose_widget(w);
}

void _list_key_pressed(void *w_, void* xkey_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XKeyEvent *xkey = (XKeyEvent*)xkey_;
    ViewList_t *filelist = (ViewList_t*)w->parent_struct;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int height = attrs.height;
    int _items = height/(height/25);
    filelist->prelight_item = xkey->y/_items  + (int)max(0,adj_get_value(w->adj));
    int nk = key_mapping(w->app->dpy, xkey);
    if (nk) {
        switch (nk) {
            case 3:
            case 4:
            case 5:
            case 6: filelist->prelight_item = xkey->y/_items  + (int)max(0,adj_get_value(w->adj));
            break;
            default:
            break;
        }
    }
}

void _list_entry_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER) {
        ViewList_t *filelist = (ViewList_t*)w->parent_struct;
        XButtonEvent *xbutton = (XButtonEvent*)button_;
        XWindowAttributes attrs;
        XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
        int height = attrs.height;
        int _items = height/(height/25);
        int prelight_item = xbutton->y/_items  + (int)max(0,adj_get_value(w->adj));
        if(xbutton->button == Button4) {
            if(prelight_item != filelist->prelight_item) {
                filelist->prelight_item = prelight_item;
            }
        } else if (xbutton->button == Button5) {
            if(prelight_item != filelist->prelight_item) {
                filelist->prelight_item = prelight_item;
            }
        } else if(xbutton->button == Button1) {
            Widget_t* listview = (Widget_t*) w->parent;
            filelist->active_item = filelist->prelight_item;
            adj_set_value(listview->adj,filelist->active_item);
            listview->func.button_release_callback(listview,NULL,NULL);
        }
    }
}

void _list_entry_double_clicked(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t* listview = (Widget_t*) w->parent;
    listview->func.double_click_callback(listview,button_,NULL);
}

void _leave_list(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    ViewList_t *filelist = (ViewList_t*)w->parent_struct;
    filelist->prelight_item = -1;
    expose_widget(w);
}

void _reconfigure_listview_viewport(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    float st = adj_get_state(w->adj);
    Widget_t* listview = (Widget_t*) w->parent;
    ViewList_t *filelist = (ViewList_t*)w->parent_struct;
    XWindowAttributes attrs;
    XGetWindowAttributes(listview->app->dpy, (Window)listview->widget, &attrs);
    int height = attrs.height;
    filelist->show_items = height/25;
    w->adj->max_value = filelist->list_size-filelist->show_items;
    adj_set_state(w->adj,st);
}

void _configure_listview(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t* listview = (Widget_t*) w->parent;
    ViewList_t *filelist = (ViewList_t*)w->parent_struct;
    XWindowAttributes attrs;
    XGetWindowAttributes(listview->app->dpy, (Window)listview->widget, &attrs);
    int width = attrs.width;
    int height = attrs.height;
    filelist->show_items = height/25;
    filelist->slider->adj->step = max(0.0,1.0/(filelist->list_size-filelist->show_items));
    adj_set_scale(filelist->slider->adj, ((float)filelist->list_size/(float)filelist->show_items)/25.0);
    XResizeWindow (w->app->dpy, w->widget, width, height);
}

void _set_listview_viewpoint(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    ViewList_t *filelist = (ViewList_t*)w->parent_struct;
    adj_set_state(filelist->slider->adj,adj_get_state(w->adj));
    expose_widget(w);
}

void _draw_listviewslider(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    int v = (int)w->adj->max_value;
    if (!v) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;
    float sliderstate = adj_get_state(w->adj);
    use_bg_color_scheme(w, get_color_state(w));
    cairo_rectangle(w->crb, 0,0,width,height);
    cairo_fill_preserve(w->crb);
    use_shadow_color_scheme(w, NORMAL_);
    cairo_fill(w->crb);
    use_bg_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, 0,(height-10)*sliderstate,width,10);
    cairo_fill(w->crb);
}

void _set_listviewport(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *viewport = (Widget_t*)w->parent_struct;
    adj_set_state(viewport->adj, adj_get_state(w->adj));
    expose_widget(w);
}
