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


#include "xcombobox_private.h"
#include "xtooltip.h"


void _draw_combobox_button(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width-2;
    int height = attrs.height-4;
    if (attrs.map_state != IsViewable) return;
    if (!w->state && (int)w->adj_y->value)
        w->state = 3;

    cairo_rectangle(w->crb,2.0, 4.0, width, height);

    if(w->state==0) {
        cairo_set_line_width(w->crb, 1.0);
        _pattern_out(w, NORMAL_, height);
        cairo_fill_preserve(w->crb);
        use_frame_color_scheme(w, PRELIGHT_);
    } else if(w->state==1) {
        _pattern_out(w, PRELIGHT_, height);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.5);
        use_frame_color_scheme(w, PRELIGHT_);
    } else if(w->state==2) {
        _pattern_in(w, SELECTED_, height);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.0);
        use_frame_color_scheme(w, PRELIGHT_);
    } else if(w->state==3) {
        _pattern_in(w, ACTIVE_, height);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.0);
        use_frame_color_scheme(w, PRELIGHT_);
    }
    cairo_stroke(w->crb); 

    if(w->state==2) {
        cairo_rectangle(w->crb,4.0, 6.0, width, height);
        cairo_stroke(w->crb);
        cairo_rectangle(w->crb,3.0, 4.0, width, height);
        cairo_stroke(w->crb);
    } else if (w->state==3) {
        cairo_rectangle(w->crb,3.0, 4.0, width, height);
        cairo_stroke(w->crb);
    }

    float offset = 0.0;
    if(w->state==0) {
        use_fg_color_scheme(w, NORMAL_);
    } else if(w->state==1) {
        use_fg_color_scheme(w, PRELIGHT_);
        offset = 1.0;
    } else if(w->state==2) {
        use_fg_color_scheme(w, SELECTED_);
        offset = 2.0;
    } else if(w->state==3) {
        use_fg_color_scheme(w, ACTIVE_);
        offset = 1.0;
    }
    use_text_color_scheme(w, get_color_state(w));
    int wa = width/1.1;
    int h = height/2.2;
    int wa1 = width/1.55;
    int h1 = height/1.3;
    int wa2 = width/2.8;
   
    cairo_move_to(w->crb, wa+offset, h+offset);
    cairo_line_to(w->crb, wa1+offset, h1+offset);
    cairo_line_to(w->crb, wa2+offset, h+offset);
    cairo_line_to(w->crb, wa+offset, h+offset);
    cairo_fill(w->crb);
   
}

void _draw_combobox(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width = attrs.width-2;
    int height = attrs.height-2;
    if (attrs.map_state != IsViewable) return;
    int v = (int)adj_get_value(w->adj);
    int vl = v - (int) w->adj->min_value;
   // if (v<0) return;
    Widget_t * menu = w->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;

    cairo_rectangle(w->crb,2.0, 2.0, width, height);

    if(w->state==0) {
        cairo_set_line_width(w->crb, 1.0);
        use_shadow_color_scheme(w, NORMAL_);
        cairo_fill_preserve(w->crb);
        use_frame_color_scheme(w, NORMAL_);
    } else if(w->state==1) {
        use_shadow_color_scheme(w, PRELIGHT_);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.5);
        use_frame_color_scheme(w, NORMAL_);
    } else if(w->state==2) {
        use_shadow_color_scheme(w, SELECTED_);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.0);
        use_frame_color_scheme(w, SELECTED_);
    } else if(w->state==3) {
        use_shadow_color_scheme(w, ACTIVE_);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.0);
        use_frame_color_scheme(w, ACTIVE_);
    } else if(w->state==4) {
        use_shadow_color_scheme(w, INSENSITIVE_);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.0);
        use_frame_color_scheme(w, INSENSITIVE_);
    }
    cairo_stroke(w->crb);

    cairo_rectangle(w->crb,4.0, 4.0, width, height);
    cairo_stroke(w->crb);
    cairo_rectangle(w->crb,3.0, 3.0, width, height);
    cairo_stroke(w->crb);
    if (comboboxlist->list_size<1) return;
    if (vl<0) return;

    cairo_text_extents_t extents;

    use_text_color_scheme(w, get_color_state(w));
    float font_size = w->app->normal_font/w->scale.ascale;
    cairo_set_font_size (w->crb, font_size);
    cairo_text_extents(w->crb,"Ay", &extents);
    double h = extents.height;

    cairo_move_to (w->crb, 15, (height+h)*0.55);
    cairo_show_text(w->crb, comboboxlist->list_names[vl]);
    cairo_new_path (w->crb);
    if (extents.width > (float)width-20) {
        tooltip_set_text(w,comboboxlist->list_names[vl]);
        w->flags |= HAS_TOOLTIP;
    } else {
        w->flags &= ~HAS_TOOLTIP;
    }
}

void _draw_combobox_menu(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    set_pattern(w,&w->app->color_scheme->normal,&w->app->color_scheme->active,BACKGROUND_);
    cairo_paint (w->cr);
}

void _draw_combobox_entrys(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;
    ComboBox_t *comboboxlist = (ComboBox_t*)w->parent_struct;

    use_base_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, 0, 0, width, height);
    cairo_fill (w->crb);

    int i = (int)max(0,adj_get_value(w->adj));
    int a = 0;
    int j = comboboxlist->list_size<comboboxlist->show_items+i+1 ? 
      comboboxlist->list_size : comboboxlist->show_items+i+1;
    for(;i<j;i++) {
        if(i == comboboxlist->prelight_item && i == comboboxlist->active_item)
            use_base_color_scheme(w, ACTIVE_);
        else if(i == comboboxlist->prelight_item)
            use_base_color_scheme(w, PRELIGHT_);
        else if (i == comboboxlist->active_item)
            use_base_color_scheme(w, SELECTED_);
        else
            use_base_color_scheme(w,NORMAL_ );
        cairo_rectangle(w->crb, 0, a*25, width, 25);
        cairo_fill_preserve(w->crb);
        cairo_set_line_width(w->crb, 1.0);
        use_frame_color_scheme(w, PRELIGHT_);
        cairo_stroke(w->crb); 
        cairo_text_extents_t extents;
        /** show label **/
        if(i == comboboxlist->prelight_item && i == comboboxlist->active_item)
            use_text_color_scheme(w, ACTIVE_);
        else if(i == comboboxlist->prelight_item)
            use_text_color_scheme(w, PRELIGHT_);
        else if (i == comboboxlist->active_item)
            use_text_color_scheme(w, SELECTED_);
        else
            use_text_color_scheme(w,NORMAL_ );

        cairo_set_font_size (w->crb, 12);
        cairo_text_extents(w->crb,"Ay", &extents);
        double h = extents.height;
        cairo_text_extents(w->crb,comboboxlist->list_names[i] , &extents);

        cairo_move_to (w->crb, 15, (25*(a+1)) - h +2);
        cairo_show_text(w->crb, comboboxlist->list_names[i]);
        cairo_new_path (w->crb);
        if (i == comboboxlist->prelight_item && extents.width > (float)width-20) {
            tooltip_set_text(w,comboboxlist->list_names[i]);
            w->flags |= HAS_TOOLTIP;
            show_tooltip(w);
        } else if (i == comboboxlist->prelight_item && extents.width < (float)width-20) {
            w->flags &= ~HAS_TOOLTIP;
            hide_tooltip(w);
        }
        a++;
    }
}

void _combobox_motion(void *w_, void* xmotion_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    ComboBox_t *comboboxlist = (ComboBox_t*)w->parent_struct;
    XMotionEvent *xmotion = (XMotionEvent*)xmotion_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int height = attrs.height;
    int _items = height/(height/25);
    int prelight_item = (xmotion->y/_items)  + (int)max(0,adj_get_value(w->adj));
    if(prelight_item != comboboxlist->prelight_item) {
        comboboxlist->prelight_item = prelight_item;
    }
    expose_widget(w);
}

void _combobox_key_pressed(void *w_, void* xkey_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XKeyEvent *xkey = (XKeyEvent*)xkey_;
    ComboBox_t *comboboxlist = (ComboBox_t*)w->parent_struct;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int height = attrs.height;
    int _items = height/(height/25);
    comboboxlist->prelight_item = xkey->y/_items  + (int)max(0,adj_get_value(w->adj));
    int nk = key_mapping(w->app->dpy, xkey);
    if (nk) {
        switch (nk) {
            case 3:
            case 4:
            case 5:
            case 6: comboboxlist->prelight_item = xkey->y/_items  + (int)max(0,adj_get_value(w->adj));
            break;
            default:
            break;
        }
    }
}

void _combobox_entry_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER) {
        ComboBox_t *comboboxlist = (ComboBox_t*)w->parent_struct;
        XButtonEvent *xbutton = (XButtonEvent*)button_;
        XWindowAttributes attrs;
        XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
        int height = attrs.height;
        int _items = height/(height/25);
        int prelight_item = xbutton->y/_items  + (int)max(0,adj_get_value(w->adj));
        if(xbutton->button == Button4) {
            if(prelight_item != comboboxlist->prelight_item) {
                comboboxlist->prelight_item = prelight_item;
            }
        } else if (xbutton->button == Button5) {
            if(prelight_item != comboboxlist->prelight_item) {
                comboboxlist->prelight_item = prelight_item;
            }
        } else if(xbutton->button == Button1) {
            Widget_t* combobox = (Widget_t*) w->parent;
            comboboxlist->active_item = comboboxlist->prelight_item;
            adj_set_value(combobox->adj,comboboxlist->active_item);
            adj_set_value(comboboxlist->combobox->adj,comboboxlist->active_item +
                                        comboboxlist->combobox->adj->min_value);
            combobox->func.button_release_callback(combobox,NULL,NULL);
        }
    }
}

void _leave_combobox(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    ComboBox_t *comboboxlist = (ComboBox_t*)w->parent_struct;
    comboboxlist->prelight_item = -1;
    expose_widget(w);
}

void _reconfigure_combobox_viewport(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    float st = adj_get_state(w->adj);
    Widget_t* combobox = (Widget_t*) w->parent;
    ComboBox_t *comboboxlist = (ComboBox_t*)w->parent_struct;
    XWindowAttributes attrs;
    XGetWindowAttributes(combobox->app->dpy, (Window)combobox->widget, &attrs);
    int height = attrs.height;
    comboboxlist->show_items = height/25;
    set_adjustment(comboboxlist->slider->adj,0.0, 0.0, 0.0,
        (float)(comboboxlist->list_size-(comboboxlist->show_items-1)),1.0, CL_VIEWPORTSLIDER);
    adj_set_scale(comboboxlist->slider->adj,
        ((float)(comboboxlist->list_size)/(float)comboboxlist->show_items)/25.0);
    adj_set_state(w->adj,st);
}

void _set_combobox_viewpoint(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    ComboBox_t *comboboxlist = (ComboBox_t*)w->parent_struct;
    adj_set_state(comboboxlist->slider->adj,adj_get_state(w->adj));
    expose_widget(w);
}

void _draw_combobox_menu_slider(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t* view_port = (Widget_t*)w->parent_struct;
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    int v = (int)w->adj->max_value;
    if (!v) return;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;
    int show_items = height/25;
    float slidersize = 1.0;
    if (comboboxlist->list_size > show_items)
        slidersize = (float)((float)show_items/(float)comboboxlist->list_size);
    float sliderstate = adj_get_state(w->adj);
    use_bg_color_scheme(w, get_color_state(w));
    cairo_rectangle(w->crb, 0,0,width,height);
    cairo_fill_preserve(w->crb);
    use_shadow_color_scheme(w, NORMAL_);
    cairo_fill(w->crb);
    use_bg_color_scheme(w, NORMAL_);
    cairo_rectangle(w->crb, 0,((float)height-
        ((float)height*slidersize))*sliderstate,width,((float)height*slidersize));
    cairo_fill(w->crb);
}

void _set_combobox_menu_viewport(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *viewport = (Widget_t*)w->parent_struct;
    adj_set_state(viewport->adj, adj_get_state(w->adj));
    expose_widget(w);
}

void _combobox_button_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (w->flags & HAS_POINTER){
        XButtonEvent *xbutton = (XButtonEvent*)button_;
        if (xbutton->button == Button3) {
            w->state=1;
            pop_combobox_menu_show(w, w->childlist->childs[1], true);
        }
    }
}

void _button_combobox_released(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XButtonEvent *xbutton = (XButtonEvent*)button_;
    if (w->flags & HAS_POINTER && xbutton->button == Button1) {
        Widget_t *parent = (Widget_t*)w->parent;
        w->state=1;
        pop_combobox_menu_show(parent, parent->childlist->childs[1], true);
    }
    adj_set_value(w->adj, 0.0);
}

void _entry_released(void *w_, void* item_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t* view_port =  w->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    _set_entry(comboboxlist->combobox, NULL);
}

void _set_entry(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    int v = (int)adj_get_value(w->adj)-w->adj->min_value;
    Widget_t * menu = w->childlist->childs[1];
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    if(v>=0 && comboboxlist->list_size) {
        w->label = comboboxlist->list_names[v];
        transparent_draw(w, user_data);
    }
}

void _configure_combobox_menu(Widget_t *parent, Widget_t *menu, int elem, bool above) {
    Widget_t* view_port =  menu->childlist->childs[0];
    ComboBox_t *comboboxlist = (ComboBox_t*)view_port->parent_struct;
    Widget_t *slider =  menu->childlist->childs[1];
    if (!comboboxlist->list_size) return;
    int height = 25;
    int x1, y1;
    int posy = (above) ? parent->height : 0;
    Window child;
    XTranslateCoordinates( parent->app->dpy, parent->widget, DefaultRootWindow(parent->app->dpy), 0, posy, &x1, &y1, &child );
    int item_width = 1.0;
    cairo_text_extents_t extents;
    int i = comboboxlist->list_size-1;
    float m = view_port->adj->min_value;
    set_adjustment(view_port->adj,0.0, view_port->adj->value, m, i-(elem-1)+m,1.0, CL_VIEWPORT);
    bool is_not_scrolable = false;
    if(comboboxlist->list_size <= elem) {
        elem = comboboxlist->list_size;
        is_not_scrolable = true;
    }
    cairo_set_font_size (view_port->crb, height/2);
    for(;i>-1;i--) {
        cairo_text_extents(view_port->crb,comboboxlist->list_names[i] , &extents);
        
        item_width = max(item_width, (int)extents.width+40);
        if(is_not_scrolable) view_port->scale.gravity = NORTHEAST;
    }
    if(above) {
        if(item_width<parent->width)item_width = parent->width;
    }
    int snum = DefaultScreen(parent->app->dpy);
    int screen_height = DisplayHeight(parent->app->dpy, snum);
    if (y1+(height*elem) > screen_height) y1 = y1-((height*elem)+parent->height);
    XResizeWindow (menu->app->dpy, menu->widget, item_width, height*elem);
    XResizeWindow (view_port->app->dpy, view_port->widget, item_width, height*elem);
    XMoveWindow(menu->app->dpy,slider->widget,item_width-15, 0);
    XResizeWindow(menu->app->dpy,slider->widget,15,height*elem);
    XMoveWindow(menu->app->dpy,menu->widget,x1, y1);   
}
