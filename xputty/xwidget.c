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


#include "xwidget.h"
#include "xwidget_private.h"


int key_mapping(Display *dpy, XKeyEvent *xkey) {
    if (xkey->keycode == XKeysymToKeycode(dpy,XK_Tab))
        return (xkey->state == ShiftMask) ? 1 : 2;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Up))
        return 3;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Right))
        return 4;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Down))
        return 5;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Left))
        return 6;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Home))
        return 7;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Insert))
        return 8;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_End))
        return 9;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_Return))
        return 10;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_BackSpace))
        return 11;
    // keypad
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Subtract))
        return 1;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Add))
        return 2;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Up))
        return 3;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Right))
        return 4;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Down))
        return 5;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Left))
        return 6;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Home))
        return 7;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Insert))
        return 8;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_End))
        return 9;
    else if (xkey->keycode == XKeysymToKeycode(dpy,XK_KP_Enter))
        return 10;
    else return 0;
}

void destroy_widget(Widget_t * w, Xputty *main) {
    int count = childlist_find_child(main->childlist, w);
    if (count == 0 && main->run == true) {
        quit(w);
    } else if(childlist_find_child(main->childlist, w)>=0) {
        if(w->flags & REUSE_IMAGE) {
            w->image = NULL;
        }
        if(w->flags & HAS_MEM) {
            w->func.mem_free_callback(w, NULL);
        }
        childlist_remove_child(main->childlist, w);
        int ch = childlist_has_child(w->childlist);
        if (ch) {
            int i = ch;
            for(;i>0;i--) {
                destroy_widget(w->childlist->childs[i-1],main);
            }
            destroy_widget(w,main);
        }
        if(w->flags & IS_WIDGET) {
            Widget_t *p = (Widget_t *) w->parent;
            childlist_remove_child(p->childlist, w);
        }
        delete_adjustment(w->adj_x);
        delete_adjustment(w->adj_y);
        childlist_destroy(w->childlist);
        cairo_surface_destroy(w->image);
        cairo_destroy(w->crb);
        cairo_surface_destroy(w->buffer);
        cairo_destroy(w->cr);
        cairo_surface_destroy(w->surface);
        
        XDestroyIC(w->xic);
        XCloseIM(w->xim);
        XUnmapWindow(w->app->dpy, w->widget);
        XDestroyWindow(w->app->dpy, w->widget);
        free(w->childlist);
        free(w);
        w = NULL;
    }
}

void configure_event(void *w_, void* user_data) {
    Widget_t *wid = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(wid->app->dpy, (Window)wid->widget, &attrs);
    if (wid->width != attrs.width || wid->height != attrs.height) {
        wid->scale.scale_x    = (float)wid->scale.init_width - attrs.width;
        wid->scale.scale_y    = (float)wid->scale.init_height - attrs.height;
        wid->scale.cscale_x   = (float)((float)wid->scale.init_width/(float)attrs.width);
        wid->scale.cscale_y   = (float)((float)wid->scale.init_height/(float)attrs.height);
        wid->scale.rcscale_x   = (float)((float)attrs.width/(float)wid->scale.init_width);
        wid->scale.rcscale_y   = (float)((float)attrs.height/(float)wid->scale.init_height);
        wid->scale.ascale     = wid->scale.cscale_x < wid->scale.cscale_y ? 
                                wid->scale.cscale_y : wid->scale.cscale_x;

        _resize_surface(wid, attrs.width, attrs.height); 

        debug_print("Widget_t configure callback width %i height %i\n", attrs.width, attrs.height);

        _resize_childs(wid);
    }
    wid->func.configure_notify_callback(wid,NULL);
}

void widget_reset_scale(Widget_t *w) {
    cairo_scale(w->crb, w->scale.cscale_x,w->scale.cscale_y);
}

void widget_set_scale(Widget_t *w) {
    cairo_scale(w->crb, w->scale.rcscale_x,w->scale.rcscale_y);
}

Widget_t *create_window(Xputty *app, Window win,
                          int x, int y, int width, int height, bool temporary) {

    Widget_t *w = (Widget_t*)malloc(sizeof(Widget_t));
    assert(w != NULL);
    debug_print("assert(w)\n");
    XSetWindowAttributes attributes;
    attributes.save_under = True;
    attributes.override_redirect = temporary ? True : False;

    long event_mask = StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask
                    |ButtonPressMask|Button1MotionMask;



    w->widget = XCreateWindow(app->dpy, win , x, y, width, height, 0,
                            CopyFromParent, InputOutput, CopyFromParent,
                            CopyFromParent | CWOverrideRedirect, &attributes);
    debug_print("XCreateWindow\n");

    XSetLocaleModifiers("");
    w->xim = XOpenIM(app->dpy, 0, 0, 0);
    if(!w->xim){
        XSetLocaleModifiers("@im=none");
        w->xim = XOpenIM(app->dpy, 0, 0, 0);
    }

    w->xic = XCreateIC(w->xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                    XNClientWindow, w->widget, XNFocusWindow,  w->widget, NULL);

    XSetICFocus(w->xic);

    XSelectInput(app->dpy, w->widget, event_mask);

    XSizeHints* win_size_hints;
    win_size_hints = XAllocSizeHints();
    win_size_hints->flags =  PMinSize|PBaseSize|PWinGravity;
    win_size_hints->min_width = width/2;
    win_size_hints->min_height = height/2;
    win_size_hints->base_width = width;
    win_size_hints->base_height = height;
    win_size_hints->win_gravity = CenterGravity;
    XSetWMNormalHints(app->dpy, w->widget, win_size_hints);
    XFree(win_size_hints);

    w->surface =  cairo_xlib_surface_create (app->dpy, w->widget,  
                  DefaultVisual(app->dpy, DefaultScreen(app->dpy)), width, height);

    assert(cairo_surface_status(w->surface) == CAIRO_STATUS_SUCCESS);
    w->cr = cairo_create(w->surface);
    cairo_select_font_face (w->cr, "Roboto", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_NORMAL);

    w->buffer = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width, height);
    assert(cairo_surface_status(w->buffer) == CAIRO_STATUS_SUCCESS);
    w->crb = cairo_create (w->buffer);
    cairo_select_font_face (w->crb, "Roboto", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_NORMAL);

    w->image = NULL;

    w->flags = IS_WINDOW;
    w->flags &= ~NO_AUTOREPEAT;
    w->flags &= ~FAST_REDRAW;
    w->flags &= ~HIDE_ON_DELETE;
    w->flags &= ~REUSE_IMAGE;
    w->flags &= ~NO_PROPAGATE;
    w->app = app;
    w->parent = &win;
    w->parent_struct = NULL;
    w->label = NULL;
    memset(w->input_label, 0, 32 * (sizeof w->input_label[0]));
    w->state = 0;
    w->data = 0;
    w->x = x;
    w->y = y;
    w->width = width;
    w->height = height;
    w->scale.init_x = x;
    w->scale.init_y = y;
    w->scale.init_width = width;
    w->scale.init_height = height;
    w->scale.scale_x  = 0.0;
    w->scale.scale_y  = 0.0;
    w->scale.cscale_x = 1.0;
    w->scale.cscale_y = 1.0;
    w->scale.rcscale_x = 1.0;
    w->scale.rcscale_y = 1.0;
    w->scale.ascale   = 1.0;
    w->scale.gravity  = CENTER;
    w->adj_x = NULL;
    w->adj_y = NULL;
    w->adj   = NULL;
    w->childlist = (Childlist_t*)malloc(sizeof(Childlist_t));
    assert(w->childlist != NULL);
    childlist_init(w->childlist);
    w->event_callback = widget_event_loop;
    w->func.expose_callback = _dummy_callback;
    w->func.configure_callback = configure_event;
    w->func.button_press_callback = _dummy1_callback;
    w->func.button_release_callback = _dummy1_callback;
    w->func.motion_callback = _dummy1_callback;
    w->func.adj_callback = transparent_draw;
    w->func.value_changed_callback = _dummy_callback;
    w->func.key_press_callback = _dummy1_callback;
    w->func.key_release_callback = _dummy1_callback;
    w->func.enter_callback = _dummy_callback;
    w->func.leave_callback = _dummy_callback;
    w->func.user_callback = _dummy_callback;
    w->func.mem_free_callback = _dummy_callback;
    w->func.configure_notify_callback = _dummy_callback;
    w->func.map_notify_callback = _dummy_callback;
    w->func.unmap_notify_callback = _dummy_callback;
    w->func.dialog_callback = _dummy_callback;

    childlist_add_child(app->childlist,w);
    //XMapWindow(app->dpy, w->widget);
    debug_print("size of Func_t = %lu\n", sizeof(w->func)/sizeof(void*));
    return w;
}

Widget_t *create_widget(Xputty *app, Widget_t *parent,
                          int x, int y, int width, int height) {

    Widget_t *w = (Widget_t*)malloc(sizeof(Widget_t));
    assert(w != NULL);
    debug_print("assert(w)\n");
    XSetWindowAttributes attributes;
    attributes.save_under = True;
    attributes.override_redirect = True;

    long event_mask = StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask
                    |ButtonPressMask|Button1MotionMask;



    w->widget = XCreateWindow(app->dpy, parent->widget , x, y, width, height, 0,
                            CopyFromParent, InputOutput, CopyFromParent,
                            CopyFromParent|CWOverrideRedirect, &attributes);
    debug_print("XCreateWindow\n");

    XSetLocaleModifiers("");
    w->xim = XOpenIM(app->dpy, 0, 0, 0);
    if(!w->xim){
        XSetLocaleModifiers("@im=none");
        w->xim = XOpenIM(app->dpy, 0, 0, 0);
    }

    w->xic = XCreateIC(w->xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                    XNClientWindow, w->widget, XNFocusWindow,  w->widget, NULL);

    XSetICFocus(w->xic);

    XSelectInput(app->dpy, w->widget, event_mask);

    w->surface =  cairo_xlib_surface_create (app->dpy, w->widget,  
                  DefaultVisual(app->dpy, DefaultScreen(app->dpy)), width, height);
    assert(cairo_surface_status(w->surface) == CAIRO_STATUS_SUCCESS);
    w->cr = cairo_create(w->surface);
    cairo_select_font_face (w->cr, "Roboto", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_NORMAL);

    w->buffer = cairo_surface_create_similar (w->surface, 
                        CAIRO_CONTENT_COLOR_ALPHA, width, height);
    assert(cairo_surface_status(w->buffer) == CAIRO_STATUS_SUCCESS);
    w->crb = cairo_create (w->buffer);
    cairo_select_font_face (w->crb, "Roboto", CAIRO_FONT_SLANT_NORMAL,
                               CAIRO_FONT_WEIGHT_NORMAL);

    w->image = NULL;
    
    w->flags = IS_WIDGET | USE_TRANSPARENCY;
    w->flags &= ~NO_AUTOREPEAT;
    w->flags &= ~FAST_REDRAW;
    w->flags &= ~HIDE_ON_DELETE;
    w->flags &= ~REUSE_IMAGE;
    w->flags &= ~NO_PROPAGATE;
    w->app = app;
    w->parent = parent;
    w->parent_struct = NULL;
    w->label = NULL;
    memset(w->input_label, 0, 32 * (sizeof w->input_label[0]));
    w->state = 0;
    w->data = 0;
    w->x = x;
    w->y = y;
    w->width = width;
    w->height = height;
    w->scale.gravity = CENTER;
    w->scale.init_width = width;
    w->scale.init_height = height;
    w->scale.init_x   = x;
    w->scale.init_y   = y;
    w->scale.scale_x  = 0.0;
    w->scale.scale_y  = 0.0;
    w->scale.cscale_x = 1.0;
    w->scale.cscale_y = 1.0;
    w->scale.rcscale_x = 1.0;
    w->scale.rcscale_y = 1.0;
    w->scale.ascale   = 1.0;
    w->adj_x = NULL;
    w->adj_y = NULL;
    w->adj   = NULL;
    w->childlist = (Childlist_t*)malloc(sizeof(Childlist_t));
    assert(w->childlist != NULL);
    childlist_init(w->childlist);
    childlist_add_child(parent->childlist, w);
    w->event_callback = widget_event_loop;
    w->func.expose_callback = _dummy_callback;
    w->func.configure_callback = configure_event;
    w->func.button_press_callback = _dummy1_callback;
    w->func.button_release_callback = _dummy1_callback;
    w->func.motion_callback = _dummy1_callback;
    w->func.adj_callback = transparent_draw;
    w->func.value_changed_callback = _dummy_callback;
    w->func.key_press_callback = _dummy1_callback;
    w->func.key_release_callback = _dummy1_callback;
    w->func.enter_callback = _dummy_callback;
    w->func.leave_callback = _dummy_callback;
    w->func.user_callback = _dummy_callback;
    w->func.mem_free_callback = _dummy_callback;
    w->func.configure_notify_callback = _dummy_callback;
    w->func.map_notify_callback = _dummy_callback;
    w->func.unmap_notify_callback = _dummy_callback;
    w->func.dialog_callback = _dummy_callback;

    childlist_add_child(app->childlist,w);
    //XMapWindow(app->dpy, w->widget);
    debug_print("size of Widget_t = %ld\n", sizeof(struct Widget_t));
    return w;
}

void connect_func(void (**event)(), void (*handler)()) {
    debug_print("address of a is: %p\n", (void*)event);
    debug_print("address of b is: %p\n", (void*)handler);
    *event = handler;
    debug_print("address of a is: %p\n", (void*)(*event));
}

void widget_set_title(Widget_t *w, const char *title) {
    XStoreName(w->app->dpy, w->widget, title);
}

void widget_show(Widget_t *w) {
    w->func.map_notify_callback(w, NULL);
    XMapWindow(w->app->dpy, w->widget);
}

void widget_hide(Widget_t *w) {
    int i=0;
    for(;i<w->childlist->elem;i++) {
        widget_hide(w->childlist->childs[i]);
    }
    w->func.unmap_notify_callback(w, NULL);
    XUnmapWindow(w->app->dpy, w->widget);
}

void widget_show_all(Widget_t *w) {
    if (w->flags & IS_POPUP || w->flags & IS_TOOLTIP) {
        return;
    } else {
        w->func.map_notify_callback(w, NULL);
        XMapWindow(w->app->dpy, w->widget);
        int i=0;
        for(;i<w->childlist->elem;i++) {
            widget_show_all(w->childlist->childs[i]);
        }
    }
}

void pop_widget_show_all(Widget_t *w) {
    w->func.map_notify_callback(w, NULL);
    XMapWindow(w->app->dpy, w->widget);
    int i=0;
    for(;i<w->childlist->elem;i++) {
        pop_widget_show_all(w->childlist->childs[i]);
    }
}

void show_tooltip(Widget_t *wid) {
    int i = 0;
    for(;i<wid->childlist->elem;i++) {
        Widget_t *w = wid->childlist->childs[i];
        if (w->flags & IS_TOOLTIP) {
            unsigned int mask;
            int x, y, rx, ry;
            Window child, root;
            XQueryPointer(wid->app->dpy, wid->widget, &root, &child, &rx, &ry, &x, &y, &mask);
            int x1, y1;
            XTranslateCoordinates( wid->app->dpy, wid->widget, DefaultRootWindow(wid->app->dpy),
                                                                       x, y, &x1, &y1, &child );
            XMoveWindow(w->app->dpy,w->widget,x1+10, y1-10);
            widget_show(w);
            break;
        }
    }
}

void hide_tooltip(Widget_t *wid) {
    int i = 0;
    for(;i<wid->childlist->elem;i++) {
        Widget_t *w = wid->childlist->childs[i];
        if (w->flags & IS_TOOLTIP) {
            widget_hide(w);
            break;
        }
    }
}

Widget_t *get_toplevel_widget(Xputty *main) {
    return  main->childlist->childs[0];
}

void expose_widget(Widget_t *w) {
    XEvent exp;
    memset(&exp, 0, sizeof(exp));
    exp.type = Expose;
    exp.xexpose.window = w->widget;
    XSendEvent(w->app->dpy, w->widget, False, ExposureMask, (XEvent *)&exp);
}

void transparent_draw(void * w_, void* user_data) {
    Widget_t *wid = (Widget_t*)w_;

    cairo_push_group (wid->cr);

    if (wid->flags & USE_TRANSPARENCY) {
        Widget_t *parent = (Widget_t*)wid->parent;
        XWindowAttributes attrs;
        XGetWindowAttributes(wid->app->dpy, wid->widget, &attrs);

        debug_print("Widget_t _transparency \n");
        cairo_set_source_surface (wid->crb, parent->buffer, -attrs.x, -attrs.y);
        cairo_paint (wid->crb);
    }

    cairo_push_group (wid->crb);
    wid->func.expose_callback(wid, user_data);
    cairo_pop_group_to_source (wid->crb);
    cairo_paint (wid->crb);

    cairo_set_source_surface (wid->cr, wid->buffer,0,0);
    cairo_paint (wid->cr);

    cairo_pop_group_to_source (wid->cr);
    cairo_paint (wid->cr);
    _propagate_child_expose(wid);
}

void widget_event_loop(void *w_, void* event, Xputty *main, void* user_data) {
    Widget_t *wid = (Widget_t*)w_;
    XEvent *xev = (XEvent*)event;
    if (XFilterEvent(xev, wid->widget))
        return;
    
    switch(xev->type) {
        case ConfigureNotify:
            wid->func.configure_callback(w_, user_data);
            //transparent_draw(w_, user_data);
            debug_print("Widget_t ConfigureNotify \n");
        break;

        case Expose:
            if (xev->xexpose.count == 0) {
                transparent_draw(w_, user_data);
                debug_print("Widget_t Expose \n");
            }
        break;

        case ButtonPress:
            if (wid->state == 4) break;
            if (wid->flags & HAS_TOOLTIP) hide_tooltip(wid);
            _button_press(wid, &xev->xbutton, user_data);
            debug_print("Widget_t  ButtonPress %i\n", xev->xbutton.button);
        break;

        case ButtonRelease:
            _check_grab(wid, &xev->xbutton, main);
            if (wid->state == 4) break;
            _has_pointer(wid, &xev->xbutton);
            if(wid->flags & HAS_POINTER) wid->state = 1;
            else wid->state = 0;
            _check_enum(wid, &xev->xbutton);
            wid->func.button_release_callback(w_, &xev->xbutton, user_data);
            debug_print("Widget_t  ButtonRelease %i\n", xev->xbutton.button);
        break;

        case KeyPress:
            if (wid->state == 4) break;
            _check_keymap(wid, xev->xkey);
            wid->func.key_press_callback(w_, &xev->xkey, user_data);
            debug_print("Widget_t KeyPress %u\n", xev->xkey.keycode);
        break;

        case KeyRelease: 
            if (wid->state == 4) break;
            {
            unsigned short is_retriggered = 0;
            if(wid->flags & NO_AUTOREPEAT) {
                if (XEventsQueued(main->dpy, QueuedAlready)) {
                    XEvent nev;
                    XPeekEvent(main->dpy, &nev);
                    if (nev.type == KeyPress && nev.xkey.time == xev->xkey.time &&
                        nev.xkey.keycode == xev->xkey.keycode && 
                        (nev.xkey.keycode > 119 || nev.xkey.keycode < 110)) {
                        XNextEvent (main->dpy, xev);
                        is_retriggered = 1;
                    }
                }
            }
            if (!is_retriggered) {
                wid->func.key_release_callback(w_, &xev->xkey, user_data);
                debug_print("Widget_t KeyRelease %u\n", xev->xkey.keycode);
            }
        }
        break;

        case LeaveNotify:
            wid->flags &= ~HAS_FOCUS;
            if (wid->state == 4) break;
            if(!(xev->xcrossing.state & Button1Mask)) {
                wid->state = 0;
                wid->func.leave_callback(w_, user_data);
            }
            if (wid->flags & HAS_TOOLTIP) hide_tooltip(wid);
            debug_print("Widget_t LeaveNotify \n");
        break;

        case EnterNotify:
            wid->flags |= HAS_FOCUS;
            if (wid->state == 4) break;
            if(!(xev->xcrossing.state & Button1Mask)) {
                wid->state = 1;
                wid->func.enter_callback(w_, user_data);
                if (wid->flags & HAS_TOOLTIP) show_tooltip(wid);
                else _hide_all_tooltips(wid);
            }
            debug_print("Widget_t EnterNotify \n");
        break;

        case MotionNotify:
            if (wid->state == 4) break;
            adj_set_motion_state(wid, xev->xmotion.x, xev->xmotion.y);
            wid->func.motion_callback(w_,&xev->xmotion, user_data);
            debug_print("Widget_t MotionNotify x = %i Y = %i \n",xev->xmotion.x,xev->xmotion.y );
        break;

        case ClientMessage:
            if (xev->xclient.message_type == XInternAtom(wid->app->dpy, "WIDGET_DESTROY", 1)) {
                int ch = childlist_has_child(wid->childlist);
                if (ch) {
                    int i = ch;
                    for(;i>0;i--) {
                        quit_widget(wid->childlist->childs[i-1]);
                    }
                    quit_widget(wid);
                } else {
                    destroy_widget(wid,main);
                }
            }

        default:
        break;
    }
}

void send_configure_event(Widget_t *w,int x, int y, int width, int height) {
    XConfigureEvent notify;
    memset(&notify, 0, sizeof(notify));
    notify.type = ConfigureNotify;
    notify.display = w->app->dpy;
    notify.send_event = True;
    notify.event = w->widget;
    notify.window = w->widget;
    notify.x = x;
    notify.y = y;
    notify.width = width;
    notify.height = height;
    notify.border_width = 0;
    notify.above = None;
    notify.override_redirect = 1;
    XSendEvent( w->app->dpy, w->widget, true, StructureNotifyMask, (XEvent*)&notify );    
}

void send_button_press_event(Widget_t *w) {
    XEvent event;
    memset(&event, 0, sizeof(XEvent));
    XWindowAttributes attr;
    XGetWindowAttributes(w->app->dpy, w->widget, &attr);
    event.type = ButtonPress;
    event.xbutton.same_screen = true;
    event.xbutton.root = None;
    event.xbutton.window = w->widget;
    event.xbutton.subwindow = None;
    event.xbutton.x = 1;
    event.xbutton.y = 1;
    event.xbutton.x_root = attr.x;
    event.xbutton.y_root = attr.y;
    event.xbutton.state = 0;
    event.xbutton.button = Button1;
    XSendEvent(w->app->dpy, PointerWindow, True, ButtonPressMask, &event);
}

void send_button_release_event(Widget_t *w) {
    XEvent event;
    memset(&event, 0, sizeof(XEvent));
    XWindowAttributes attr;
    XGetWindowAttributes(w->app->dpy, w->widget, &attr);
    event.type = ButtonRelease;
    event.xbutton.same_screen = true;
    event.xbutton.root = None;
    event.xbutton.window = w->widget;
    event.xbutton.subwindow = None;
    event.xbutton.x = 1;
    event.xbutton.y = 1;
    event.xbutton.x_root = attr.x;
    event.xbutton.y_root = attr.y;
    event.xbutton.state = 0;
    event.xbutton.button = Button1;
    XSendEvent(w->app->dpy, PointerWindow, True, ButtonReleaseMask, &event);
}

void send_systray_message(Widget_t *w) {
    XEvent event;
    Screen *xscreen;
    char buf[256];
    buf[0]=0;
    
    xscreen=DefaultScreenOfDisplay(w->app->dpy);
    sprintf(buf,"_NET_SYSTEM_TRAY_S%d",XScreenNumberOfScreen (xscreen));
    Atom selection_atom = XInternAtom (w->app->dpy,buf,0);

    Window tray = XGetSelectionOwner (w->app->dpy,selection_atom);
    Atom visualatom = XInternAtom(w->app->dpy, "_NET_SYSTEM_TRAY_VISUAL", False);
    VisualID value = XVisualIDFromVisual(DefaultVisual(w->app->dpy, DefaultScreen(w->app->dpy)));
    XChangeProperty(w->app->dpy, w->widget, visualatom, XA_VISUALID, 32,
            PropModeReplace, (unsigned char*)&value, 1);

    if ( tray != None)
        XSelectInput (w->app->dpy,tray,StructureNotifyMask);

    memset(&event, 0, sizeof(event));
    event.xclient.type = ClientMessage;
    event.xclient.window = tray;
    event.xclient.message_type = XInternAtom (w->app->dpy, "_NET_SYSTEM_TRAY_OPCODE", False );
    event.xclient.format = 32;
    event.xclient.data.l[0] = CurrentTime;
    event.xclient.data.l[1] = SYSTEM_TRAY_REQUEST_DOCK;
    event.xclient.data.l[2] = w->widget;
    event.xclient.data.l[3] = 0;
    event.xclient.data.l[4] = 0;

    XSendEvent(w->app->dpy, tray, False, NoEventMask, &event);
}

void quit(Widget_t *w) {
    Atom WM_DELETE_WINDOW = XInternAtom(w->app->dpy, "WM_DELETE_WINDOW", True);
    XClientMessageEvent xevent;
    xevent.type = ClientMessage;
    xevent.message_type = WM_DELETE_WINDOW;
    xevent.display = w->app->dpy;
    xevent.window = get_toplevel_widget(w->app)->widget;
    xevent.format = 16;
    xevent.data.l[0] = WM_DELETE_WINDOW;
    XSendEvent(w->app->dpy, w->widget, 0, 0, (XEvent *)&xevent);
}

void quit_widget(Widget_t *w) {
    Atom QUIT_WIDGET = XInternAtom(w->app->dpy, "WIDGET_DESTROY", False);
    XClientMessageEvent xevent;
    xevent.type = ClientMessage;
    xevent.message_type = QUIT_WIDGET;
    xevent.display = w->app->dpy;
    xevent.window = w->widget;
    xevent.format = 16;
    xevent.data.l[0] = 1;
    XSendEvent(w->app->dpy, w->widget, 0, 0, (XEvent *)&xevent);
}

