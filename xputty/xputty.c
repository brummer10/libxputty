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

#include "xputty.h"


void main_init(Xputty *main) {
    main->dpy = XOpenDisplay(0);
    assert(main->dpy);
    main->childlist = (Childlist_t*)malloc(sizeof(Childlist_t));
    assert(main->childlist);
    childlist_init(main->childlist);
    main->color_scheme = (XColor_t*)malloc(sizeof(XColor_t));
    assert(main->color_scheme);
    set_dark_theme(main);
    main->systray_color = (SystrayColor_t*)malloc(sizeof(SystrayColor_t));
    assert(main->systray_color);
    set_systray_color(main, 0.3, 0.3, 0.3, 1.0);
    main->hold_grab = NULL;
    main->key_snooper = NULL;
    main->submenu = NULL;
    main->run = true;
    main->is_grab = false;
    main->small_font = 10;
    main->normal_font = 12;
    main->big_font = 16;
    main->ctext = NULL;
    main->csize = 0;

    main->dnd_source_window = 0;
    main->dnd_version = 5;
    main->XdndAware = XInternAtom (main->dpy, "XdndAware", False);
    main->XdndTypeList = XInternAtom (main->dpy, "XdndTypeList", False);
    main->XdndSelection = XInternAtom (main->dpy, "XdndSelection", False);
    main->XdndStatus = XInternAtom (main->dpy, "XdndStatus", False);
    main->XdndEnter = XInternAtom (main->dpy, "XdndEnter", False);
    main->XdndPosition = XInternAtom (main->dpy, "XdndPosition", False);
    main->XdndLeave = XInternAtom (main->dpy, "XdndLeave", False);
    main->XdndDrop = XInternAtom (main->dpy, "XdndDrop", False);
    main->XdndActionCopy = XInternAtom (main->dpy, "XdndActionCopy", False);
    main->XdndFinished = XInternAtom (main->dpy, "XdndFinished", False);
    main->dnd_type_uri = XInternAtom (main->dpy, "text/uri-list", False);
    main->dnd_type_text = XInternAtom (main->dpy, "text/plain", False);
    main->dnd_type_utf8 = XInternAtom (main->dpy, "UTF8_STRING", False);
    main->dnd_type = None;

    main->selection = XInternAtom(main->dpy, "CLIPBOARD", 0);
    main->targets_atom = XInternAtom(main->dpy, "TARGETS", 0);
    main->text_atom = XInternAtom(main->dpy, "TEXT", 0);
    main->UTF8 = XInternAtom(main->dpy, "UTF8_STRING", 1);
    if (main->UTF8 == None) main->UTF8 = XA_STRING;
}

void main_run(Xputty *main) {
    Widget_t * wid = main->childlist->childs[0]; 
    Atom WM_DELETE_WINDOW;
    WM_DELETE_WINDOW = XInternAtom(main->dpy, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(main->dpy, wid->widget, &WM_DELETE_WINDOW, 1);

    XEvent xev;
    int ew;
    Widget_t * w = NULL;

    while (main->run && (XNextEvent(main->dpy, &xev)>=0)) {
        if (XFilterEvent(&xev, None)) continue;
        ew = childlist_find_widget(main->childlist, xev.xany.window);
        if(ew  >= 0) {
            w = main->childlist->childs[ew];
            w->event_callback(w, &xev, main, NULL);
        }

        switch (xev.type) {
            case ButtonPress:
            {
                bool is_item = False;
                if(main->submenu != NULL) {
                    if (childlist_has_child(main->submenu->childlist)) {
                        Widget_t *slider = main->submenu->childlist->childs[1];
                        if (xev.xbutton.window == slider->widget) {
                            break;
                        }
                        Widget_t *view_port = main->submenu->childlist->childs[0];
                        int i = view_port->childlist->elem-1;
                        for(;i>-1;i--) {
                            Widget_t *w = view_port->childlist->childs[i];
                            if (xev.xbutton.window == w->widget) {
                                is_item = True;
                                break;
                            }
                        }
                    }
                }
                if(main->hold_grab != NULL && main->hold_grab->flags & IS_POPUP) {
                    if (childlist_has_child(main->hold_grab->childlist)) {
                        Widget_t *slider = main->hold_grab->childlist->childs[1];
                        if (xev.xbutton.window == slider->widget) {
                            break;
                        }
                        Widget_t *view_port = main->hold_grab->childlist->childs[0];
                        int i = view_port->childlist->elem-1;
                        for(;i>-1;i--) {
                            Widget_t *w = view_port->childlist->childs[i];
                            if (xev.xbutton.window == w->widget) {
                                is_item = True;
                                break;
                            }
                        }
                        if (xev.xbutton.window == view_port->widget) is_item = True;
                    }
                    if (!is_item) {
                        XUngrabPointer(main->dpy,CurrentTime);
                        widget_hide(main->hold_grab);
                        main->hold_grab = NULL;
                    }
                } else if(main->hold_grab != NULL && ((main->hold_grab->flags & IS_POPUP) == 0)) {
                    main->hold_grab->func.button_press_callback(main->hold_grab, &xev.xbutton, NULL);
                }
            }
            break;
            case ButtonRelease:
            {
                if(main->hold_grab != NULL && ((main->hold_grab->flags & IS_POPUP) == 0)) {
                    main->hold_grab->func.button_release_callback(main->hold_grab, &xev.xbutton, NULL);
                }
            }
            break;
            case KeyPress:
            {
                if (main->key_snooper != NULL && xev.xkey.window != main->key_snooper->widget) {
                    main->key_snooper->func.key_press_callback(main->key_snooper, &xev.xkey, NULL);
                }
            }
            break;
            case KeyRelease:
            {
                if (main->key_snooper != NULL && xev.xkey.window != main->key_snooper->widget) {
                    main->key_snooper->func.key_release_callback(main->key_snooper, &xev.xkey, NULL);
                }
            }
            break;
            case ClientMessage:
            {
                if (xev.xclient.data.l[0] == (long int)WM_DELETE_WINDOW &&
                        xev.xclient.window == wid->widget) {
                    main->run = false;
                } else if (xev.xclient.data.l[0] == (long int)WM_DELETE_WINDOW) {
                    int i = childlist_find_widget(main->childlist, xev.xclient.window);
                    if(i<1) return;
                    Widget_t *w = main->childlist->childs[i];
                    if(w->flags & HIDE_ON_DELETE) widget_hide(w);
                    else destroy_widget(main->childlist->childs[i],main);
                }
                break;
            }
        }
    }
}

void run_embedded(Xputty *main) {

    XEvent xev;
    int ew = -1;

    while (XPending(main->dpy) > 0) {
        XNextEvent(main->dpy, &xev);
        if (xev.type == ClientMessage || xev.type == SelectionNotify) {
            Widget_t * w = main->childlist->childs[0];
            w->event_callback(w, &xev, main, NULL);
        }
        ew = childlist_find_widget(main->childlist, xev.xany.window);
        if(ew  >= 0) {
            Widget_t * w = main->childlist->childs[ew];
            w->event_callback(w, &xev, main, NULL);
        }
        switch (xev.type) {
        case ButtonPress:
        {
            bool is_item = False;
            if(main->submenu != NULL) {
                if (childlist_has_child(main->submenu->childlist)) {
                    Widget_t *slider = main->submenu->childlist->childs[1];
                    if (xev.xbutton.window == slider->widget) {
                        break;
                    }
                    Widget_t *view_port = main->submenu->childlist->childs[0];
                    int i = view_port->childlist->elem-1;
                    for(;i>-1;i--) {
                        Widget_t *w = view_port->childlist->childs[i];
                        if (xev.xbutton.window == w->widget) {
                            is_item = True;
                            break;
                        }
                    }
                }
            }
            if(main->hold_grab != NULL && main->hold_grab->flags & IS_POPUP) {
                if (childlist_has_child(main->hold_grab->childlist)) {
                    Widget_t *slider = main->hold_grab->childlist->childs[1];
                    if (xev.xbutton.window == slider->widget) {
                        break;
                    }
                    Widget_t *view_port = main->hold_grab->childlist->childs[0];
                    int i = view_port->childlist->elem-1;
                    for(;i>-1;i--) {
                        Widget_t *w = view_port->childlist->childs[i];
                        if (xev.xbutton.window == w->widget) {
                            is_item = True;
                            break;
                        }
                    }
                    if (xev.xbutton.window == view_port->widget) is_item = True;
                }
                if (!is_item) {
                    XUngrabPointer(main->dpy,CurrentTime);
                    widget_hide(main->hold_grab);
                    main->hold_grab = NULL;
                }
            } else if(main->hold_grab != NULL && ((main->hold_grab->flags & IS_POPUP) == 0)) {
                main->hold_grab->func.button_press_callback(main->hold_grab, &xev.xbutton, NULL);
            }
        }
        break;
        case ButtonRelease:
        {
            if(main->hold_grab != NULL && ((main->hold_grab->flags & IS_POPUP) == 0)) {
                main->hold_grab->func.button_release_callback(main->hold_grab, &xev.xbutton, NULL);
            }
        }
        break;
        case KeyPress:
        {
            if (main->key_snooper != NULL && xev.xkey.window != main->key_snooper->widget) {
                main->key_snooper->func.key_press_callback(main->key_snooper, &xev.xkey, NULL);
            }
        }
        break;
        case KeyRelease:
        {
            if (main->key_snooper != NULL && xev.xkey.window != main->key_snooper->widget) {
                main->key_snooper->func.key_release_callback(main->key_snooper, &xev.xkey, NULL);
            }
        }
        break;
        case ClientMessage:
            if (xev.xclient.data.l[0] == (long int)XInternAtom(main->dpy, "WM_DELETE_WINDOW", True) ) {
                int i = childlist_find_widget(main->childlist, xev.xclient.window);
                if(i<1) return;
                Widget_t *w = main->childlist->childs[i];
                if(w->flags & HIDE_ON_DELETE) widget_hide(w);
                else destroy_widget(w, main);
            }
        break;
        }
    }
}

void main_quit(Xputty *main) {
    int i = main->childlist->elem-1;
    for(;i>-1;i--) {
        Widget_t *w = main->childlist->childs[i];
        destroy_widget(w, main);
    }
    childlist_destroy(main->childlist);
    free(main->childlist);
    free(main->color_scheme);
    free(main->systray_color);
    XCloseDisplay(main->dpy);
    free(main->ctext);
    debug_print("quit\n");
}
