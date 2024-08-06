/* vim:ts=4:sw=4:et:
 *
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

#if defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#ifndef _OS_UNIX_
#define _OS_UNIX_ 1
#endif
#endif

#ifdef _OS_UNIX_

#include <sys/stat.h>

#include "xwidget.h"
#include "xwidget_private.h"
#include "xasprintf.h"

#ifdef __cplusplus
extern "C" {
#endif

int (*default_error_handler) (Display *dpy, XErrorEvent *e);

int intern_error_handler(Display *dpy, XErrorEvent *e) {
    return 0;
}

// convert active codepage to utf8 (not needed on linux)
char *utf8_from_locale(char *localestr) {
    char *s_to = NULL;
    asprintf(&s_to, "%s",localestr);
    return s_to;
}

// convert utf8 to active codepage (not needed on linux)
char *locale_from_utf8(char *utf8str) {
    char *s_to = NULL;
    asprintf(&s_to, "%s",utf8str);
    return s_to;
}

Display *os_open_display(char *display_name) {
    return XOpenDisplay(display_name);
}

void os_close_display(Display *dpy) {
    XCloseDisplay(dpy);
}

void os_destroy_window(Widget_t *w) {
    if (w->xic) XDestroyIC(w->xic);
    if (w->xim) XCloseIM(w->xim);
    XUnmapWindow(w->app->dpy, w->widget);
    XDestroyWindow(w->app->dpy, w->widget);
}

Window os_get_root_window(Xputty *main, int flag) {
    return DefaultRootWindow(main->dpy);
}

void os_translate_coords(Widget_t *w, Window from_window, Window to_window,
                          int from_x, int from_y, int *to_x, int *to_y) {
    Window child;
    XTranslateCoordinates(w->app->dpy, from_window, to_window,
                          from_x, from_y, to_x, to_y, &child);
}

void os_get_window_metrics(Widget_t *w_, Metrics_t *metrics) {
    Widget_t *wid = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(wid->app->dpy, (Window)wid->widget, &attrs);
    metrics->x = attrs.x;
    metrics->y = attrs.y;
    metrics->width = attrs.width;
    metrics->height = attrs.height;
    metrics->visible = (attrs.map_state == IsViewable);
}

void os_set_window_min_size(Widget_t *w, int min_width, int min_height,
                            int base_width, int base_height) {
    XSizeHints* win_size_hints;
    win_size_hints = XAllocSizeHints();
    win_size_hints->flags =  PMinSize|PBaseSize|PWinGravity;
    win_size_hints->min_width = min_width;
    win_size_hints->min_height = min_height;
    win_size_hints->base_width = base_width;
    win_size_hints->base_height = base_height;
    win_size_hints->win_gravity = CenterGravity;
    XSetWMNormalHints(w->app->dpy, w->widget, win_size_hints);
    XFree(win_size_hints);
}

void os_move_window(Display *dpy, Widget_t *w, int x, int y) {
    XMoveWindow(dpy,w->widget, x, y);
}

void os_resize_window(Display *dpy, Widget_t *w, int x, int y) {
    XResizeWindow(dpy, w->widget, x, y);
}

void os_get_surface_size(cairo_surface_t *surface, int *width, int *height) {
    *width = cairo_xlib_surface_get_width(surface);
    *height = cairo_xlib_surface_get_height(surface);
}

void os_set_widget_surface_size(Widget_t *w, int width, int height) {
    cairo_xlib_surface_set_size(w->surface, width, height);
}

void os_create_main_window_and_surface(Widget_t *w, Xputty *app, Window win,
                          int x, int y, int width, int height) {
    XSetWindowAttributes attributes;
    attributes.save_under = True;
    attributes.override_redirect = 0;

    long event_mask = StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask
                    |ButtonPressMask|Button1MotionMask;



    w->widget = XCreateWindow(app->dpy, win , x, y, width, height, 0,
                            CopyFromParent, InputOutput, CopyFromParent,
                            CopyFromParent, &attributes);
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

    os_set_window_min_size(w, width/2, height/2, width, height);

    w->surface =  cairo_xlib_surface_create (app->dpy, w->widget,  
                  DefaultVisual(app->dpy, DefaultScreen(app->dpy)), width, height);
}

void os_create_widget_window_and_surface(Widget_t *w, Xputty *app, Widget_t *parent,
                          int x, int y, int width, int height) {
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
}

void os_set_title(Widget_t *w, const char *title) {
    XStoreName(w->app->dpy, w->widget, title);
    XChangeProperty(w->app->dpy,  w->widget,
    XInternAtom(w->app->dpy, "_NET_WM_NAME", False),
        XInternAtom(w->app->dpy, "UTF8_STRING", False),
        8, PropModeReplace, (unsigned char *) title,
        strlen(title));
}

void os_widget_show(Widget_t *w) {
    XMapWindow(w->app->dpy, w->widget);
}

void os_widget_hide(Widget_t *w) {
    XUnmapWindow(w->app->dpy, w->widget);
}

void os_show_tooltip(Widget_t *wid, Widget_t *w) {
    unsigned int mask;
    int x, y, rx, ry;
    Window child, root;
    XQueryPointer(wid->app->dpy, wid->widget, &root, &child, &rx, &ry, &x, &y, &mask);
    int x1, y1;
    os_translate_coords(wid, wid->widget, os_get_root_window(wid->app, IS_WIDGET), x, y, &x1, &y1);
    XMoveWindow(w->app->dpy,w->widget,x1+10, y1-10);
    XMapWindow(w->app->dpy, w->widget);
}

void os_expose_widget(Widget_t *w) {
    XEvent exp;
    memset(&exp, 0, sizeof(exp));
    exp.type = Expose;
    exp.xexpose.window = w->widget;
    XSendEvent(w->app->dpy, w->widget, False, ExposureMask, (XEvent *)&exp);
}

void os_widget_event_loop(void *w_, void* event, Xputty *main, void* user_data) {
    Widget_t *wid = (Widget_t*)w_;
    XEvent *xev = (XEvent*)event;
    
    switch(xev->type) {
        case ConfigureNotify:
            wid->func.configure_callback(w_, user_data);
            //transparent_draw(w_, user_data);
            debug_print("Widget_t ConfigureNotify \n");
        break;

        case VisibilityNotify:
            wid->func.visibiliy_change_callback(w_, user_data);
            debug_print("Widget_t VisibilityNotify \n");
        break;

        case Expose:
            if (xev->xexpose.count == 0) {
                if ((wid->flags & FAST_REDRAW) == 0) {
                    XEvent ev;
                    while (XCheckTypedWindowEvent(main->dpy, wid->widget, Expose, &ev)) {}
                }
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
        {
            XButtonEvent *xbutton = &xev->xbutton;
            _check_grab(wid, xbutton, main);
            _check_submenu(wid, xbutton, main);
            if (wid->state == 4) break;
            if (xbutton->button == Button1) {
                if ((wid->flags & IS_POPUP) == 0) {
                    if (xbutton->time < wid->double_click+300) {
                        wid->func.double_click_callback(wid, xbutton, user_data);
                        break;
                    }
                    wid->double_click = xbutton->time;
                }
            }
            _has_pointer(wid, &xev->xbutton);
            if(wid->flags & HAS_POINTER) wid->state = 1;
            else wid->state = 0;
            _check_enum(wid, xbutton);
            wid->func.button_release_callback(w_, xbutton, user_data);
            debug_print("Widget_t  ButtonRelease %i\n", xev->xbutton.button);
        }
        break;

        case KeyPress:
            if (wid->state == 4) break;
            _check_keymap(wid, xev->xkey);
            wid->func.key_press_callback(w_, &xev->xkey, user_data);
            debug_print("Widget_t KeyPress %u\n", xev->xkey.keycode);
        break;

        case KeyRelease:
        {
            if (wid->state == 4) break;
            unsigned short is_retriggered = 0;
            if(wid->flags & NO_AUTOREPEAT) {
                char keys[32];
                XQueryKeymap(main->dpy, keys);

                if((keys[xev->xkey.keycode>>3] & (0x1 << (xev->xkey.keycode % 8))) && 
                        (xev->xkey.keycode > 119 || xev->xkey.keycode < 110)) {
                    is_retriggered = 1;
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
            if(!(xev->xcrossing.state & Button1Mask) &&
               !(xev->xcrossing.state & Button2Mask) &&
               !(xev->xcrossing.state & Button3Mask)) {
                wid->state = 0;
                wid->func.leave_callback(w_, user_data);
            }
            if (wid->flags & HAS_TOOLTIP) hide_tooltip(wid);
            debug_print("Widget_t LeaveNotify \n");
        break;

        case EnterNotify:
            wid->flags |= HAS_FOCUS;
            if (wid->state == 4) break;
            if(!(xev->xcrossing.state & Button1Mask) &&
               !(xev->xcrossing.state & Button2Mask) &&
               !(xev->xcrossing.state & Button3Mask)) {
                wid->state = 1;
                wid->func.enter_callback(w_, user_data);
                if (wid->flags & HAS_TOOLTIP) show_tooltip(wid);
                else _hide_all_tooltips(wid);
            }
            debug_print("Widget_t EnterNotify \n");
        break;

        case MotionNotify:
            if (wid->state == 4) break;
            if (xev->xmotion.state) {
                adj_set_motion_state(wid, xev->xmotion.x, xev->xmotion.y);
            }
            wid->func.motion_callback(w_,&xev->xmotion, user_data);
            debug_print("Widget_t MotionNotify x = %i Y = %i \n",xev->xmotion.x,xev->xmotion.y );
        break;

        case SelectionRequest:
            if (xev->xselectionrequest.selection != main->selection) break;
            send_to_clipboard(wid, xev);
            break;
        case SelectionClear:
            break;
        case SelectionNotify:
            if (xev->xselection.property == None) {
                wid->xpaste_callback(wid, NULL);
                break;
            }
            if (xev->xselection.selection == main->selection) {
                receive_paste_from_clipboard(wid, xev);
                break;
            }
            debug_print("Widget_t SelectionNotify\n");
            handle_drag_data(wid, xev);
            break;

        case ClientMessage:
            if (xev->xclient.message_type == main->XdndPosition) {
                debug_print( "XdndPosition\n");
                send_dnd_status_event(wid, xev);
            } else if (xev->xclient.message_type == main->XdndEnter) {
                debug_print( "XdndEnter\n");
                handle_dnd_enter(main, xev);
            } else if (xev->xclient.message_type == main->XdndLeave) {
                debug_print( "XdndLeave\n");
                main->dnd_type   = None;
                main->dnd_source_window = 0;
                main->dnd_version = 0;
            } else if (xev->xclient.message_type == main->XdndDrop) {
                if ((DND_SOURCE_WIN(xev) != main->dnd_source_window) ||
                    main->dnd_type == None || main->dnd_source_window == 0) {
                    break;
                }
                XConvertSelection(main->dpy, main->XdndSelection,
                                   main->dnd_type, main->XdndSelection, wid->widget, CurrentTime);

                send_dnd_finished_event(wid, xev);
            } else if (xev->xclient.message_type == XInternAtom(wid->app->dpy, "WIDGET_DESTROY", 1)) {
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
            break;
        default:
        break;
    }
}

void widget_set_dnd_aware(Widget_t *w) {
    Atom dnd_version = 5;
    XChangeProperty (w->app->dpy, w->widget, w->app->XdndAware, XA_ATOM,
                    32, PropModeReplace, (unsigned char*)&dnd_version, 1);
}

void widget_set_dnd_unaware(Widget_t *w) {
    XDeleteProperty(w->app->dpy, w->widget, w->app->XdndAware);
}

void strremove(char *str, const char *sub) {
    char *p, *q, *r;
    if ((q = r = strstr(str, sub)) != NULL) {
        size_t len = strlen(sub);
        while ((r = strstr(p = r + len, sub)) != NULL) {
            while (p < r)
                *q++ = *p++;
        }
        while ((*q++ = *p++) != '\0')
            continue;
    }
}

void strdecode(char *target, const char *needle, const char *replacement) {
    char buffer[1024] = { 0 };
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(needle);
    size_t repl_len = strlen(replacement);

    while (1) {
        const char *p = strstr(tmp, needle);
        if (p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;
        tmp = p + needle_len;
    }
    strcpy(target, buffer);
}


void handle_drag_data(Widget_t *w, XEvent* event) {
    if (event->xselection.property != w->app->XdndSelection) return;

    Atom type;
    int format;
    unsigned long  count = 0, remaining;
    unsigned char* data = 0;

    XGetWindowProperty(w->app->dpy,w->widget, event->xselection.property,
                        0, 65536, True, w->app->dnd_type, &type, &format,
                        &count, &remaining, &data);

    send_dnd_finished_event (w, event);

    if (!data || count == 0) {
        return;
    }
    char* dndfile = (char*)data;
    strdecode(dndfile, "%20", " ");
    strremove(dndfile, "file://");
    w->func.dnd_notify_callback(w, (void*)&dndfile);
    w->app->dnd_type = None;
    w->app->dnd_source_window = 0;
    free(data);
}

void handle_dnd_enter(Xputty *main, XEvent* event) {
    main->dnd_source_window = DND_SOURCE_WIN(event);
    main->dnd_version = 0;
    if (DND_STATUS_ACCEPT(event)) {
        main->dnd_version = DND_VERSION(event);
        if (main->dnd_version > 5) return;
        Atom type = 0;
        int format;
        unsigned long count, remaining;
        unsigned char *data = 0;

        XGetWindowProperty (main->dpy, main->dnd_source_window, main->XdndTypeList,
            0, 0x8000000L, False, XA_ATOM, &type, &format, &count, &remaining, &data);
        if (!data || type != XA_ATOM || format != 32) {
            if (data) {
                XFree (data);
            }
            return;
        }
        Atom* types = (Atom*)data;
        for (unsigned long l = 1; l < count; l++) {
            if ((types[l] == main->dnd_type_uri) || 
                (types[l] == main->dnd_type_text) ||
                (types[l] == main->dnd_type_utf8)) {
                main->dnd_type = types[l];
                break;
            }
        }
        if (data) {
            XFree (data);
        }
    } else {
        for (int i = 2; i < 5; ++i) {
            if ((event->xclient.data.l[i] == main->dnd_type_uri) || 
                (event->xclient.data.l[i] == main->dnd_type_text) || 
                (event->xclient.data.l[i] == main->dnd_type_utf8)) {
                main->dnd_type = event->xclient.data.l[i];
                break;
            }
        }
    }
}

void send_dnd_status_event(Widget_t *w, XEvent* event) {
    XEvent xev;
    memset (&xev, 0, sizeof (XEvent));
    xev.xany.type            = ClientMessage;
    xev.xany.display         = w->app->dpy;
    xev.xclient.window       = w->app->dnd_source_window;
    xev.xclient.message_type = w->app->XdndStatus;
    xev.xclient.format       = 32;
    xev.xclient.data.l[0]    = event->xany.window;
    xev.xclient.data.l[1]    = (w->app->dnd_type != None) ? 1 : 0;
    xev.xclient.data.l[2]    = DND_DROP_TIME(event);
    xev.xclient.data.l[3]    = 0;
    xev.xclient.data.l[4]    = w->app->XdndActionCopy;
    XSendEvent (w->app->dpy, w->app->dnd_source_window, False, NoEventMask, &xev);
}

void send_dnd_finished_event(Widget_t *w, XEvent* event) {
    if (w->app->dnd_version < 2) {
        return;
    }
    XEvent xev;
    memset (&xev, 0, sizeof (XEvent));
    xev.xany.type            = ClientMessage;
    xev.xany.display         = w->app->dpy;
    xev.xclient.window       = w->app->dnd_source_window;
    xev.xclient.message_type = w->app->XdndFinished;
    xev.xclient.format       = 32;
    xev.xclient.data.l[0]    = event->xany.window;
    xev.xclient.data.l[1]    = 1;
    xev.xclient.data.l[2]    = w->app->XdndActionCopy;
    XSendEvent (w->app->dpy, w->app->dnd_source_window, False, NoEventMask, &xev);
}

int have_paste(Widget_t *w) {
    return XGetSelectionOwner(w->app->dpy,w->app->selection);
}

void request_paste_from_clipboard(Widget_t *w) {
    Atom XSEL_DATA = XInternAtom(w->app->dpy, "XSEL_DATA", 0);
    XConvertSelection(w->app->dpy, w->app->selection, w->app->UTF8, XSEL_DATA, w->widget, CurrentTime);
}

void receive_paste_from_clipboard(Widget_t *w, XEvent* event) {
    if(event->xselection.property) {
        Atom target;
        char * data = NULL;
        int format;
        unsigned long N, size;
        XGetWindowProperty(event->xselection.display, event->xselection.requestor,
            event->xselection.property, 0L,(~0L), 0, AnyPropertyType, &target,
            &format, &size, &N,(unsigned char**)&data);
        if(target == w->app->UTF8 || target == XA_STRING) {
            free(w->app->ctext);
            w->app->ctext = NULL;
            w->app->ctext = (unsigned char*)strndup(data, size);
            XFree(data);
        }
        XDeleteProperty(event->xselection.display, event->xselection.requestor, event->xselection.property);
        w->xpaste_callback(w, (void*)&w->app->ctext);
    }
}

void copy_to_clipboard(Widget_t *w, char* text, int size) {
    XSetSelectionOwner (w->app->dpy, w->app->selection, w->widget, 0);
    if (XGetSelectionOwner (w->app->dpy, w->app->selection) != w->widget) return;
    free(w->app->ctext);
    w->app->ctext = NULL;
    w->app->ctext = (unsigned char*)strndup(text, size);
    w->app->csize = size;
}

void send_to_clipboard(Widget_t *w, XEvent* event) {
    XSelectionRequestEvent * xsr = &event->xselectionrequest;
    XSelectionEvent xev;
    memset (&xev, 0, sizeof (XSelectionEvent));
    int R = 0;
    xev.type = SelectionNotify;
    xev.display = xsr->display;
    xev.requestor = xsr->requestor;
    xev.selection = xsr->selection;
    xev.time = xsr->time;
    xev.target = xsr->target;
    xev.property = xsr->property;
    if (xev.target == w->app->targets_atom) {
        R = XChangeProperty (xev.display, xev.requestor, xev.property, XA_ATOM, 32,
            PropModeReplace, (unsigned char*)&w->app->UTF8, 1);
    } else if (xev.target == XA_STRING || xev.target == w->app->text_atom) {
        R = XChangeProperty(xev.display, xev.requestor, xev.property, XA_STRING, 8,
            PropModeReplace, w->app->ctext, w->app->csize);
    } else if (xev.target == w->app->UTF8) {
        R = XChangeProperty(xev.display, xev.requestor, xev.property, w->app->UTF8, 8,
            PropModeReplace, w->app->ctext, w->app->csize);
    } else {
        xev.property = None;
    }
    if ((R & 2) == 0) XSendEvent (w->app->dpy, xev.requestor, 0, 0, (XEvent *)&xev);
    debug_print("send to clipboard %s\n", w->app->ctext);
}

void os_send_configure_event(Widget_t *w,int x, int y, int width, int height) {
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

void os_send_button_press_event(Widget_t *w) {
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

void os_send_button_release_event(Widget_t *w) {
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

void os_send_systray_message(Widget_t *w) {
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

    if ( tray != None) {
        XSelectInput (w->app->dpy,tray,StructureNotifyMask);
        /* get tray color and set as background */
        XColor c;
        XWindowAttributes attrs;
        XGetWindowAttributes(w->app->dpy, DefaultRootWindow(w->app->dpy), &attrs);
        
        XImage *image = NULL;
        default_error_handler = XSetErrorHandler(intern_error_handler);
        image = XGetImage(w->app->dpy, DefaultRootWindow(w->app->dpy),
                        attrs.width -2, attrs.height -2, 1, 1, AllPlanes, XYPixmap);
        XSetErrorHandler(default_error_handler);
        if (image) {
            c.pixel = XGetPixel(image, 0, 0);
            XQueryColor (w->app->dpy, DefaultColormap(w->app->dpy, DefaultScreen (w->app->dpy)), &c);
            double r = (double)c.red/65535.0;
            double g = (double)c.green/65535.0;
            double b = (double)c.blue/65535.0;
            set_systray_color(w->app, r, g, b, 1.0);
            XDestroyImage(image);
        }
    }

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

void os_transparent_draw(void *w_, void *user_data) {
   Widget_t *w = (Widget_t *)w_;
   transparent_draw(w, user_data);   
}

void os_adjustment_callback(void *w_, void *user_data) {
   Widget_t *w = (Widget_t *)w_;
   transparent_draw(w, user_data);
}

int os_grab_pointer(Widget_t *w) {
    return XGrabPointer(w->app->dpy, DefaultRootWindow(w->app->dpy), True,
             ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
             GrabModeAsync, GrabModeAsync, None, None, CurrentTime);

}

void os_set_input_mask(Widget_t *w) {
    XSelectInput(w->app->dpy, w->widget,StructureNotifyMask|ExposureMask|KeyPressMask
                |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask|KeyReleaseMask
                |ButtonPressMask|Button1MotionMask|PointerMotionMask);    
}

void os_set_window_attrb(Widget_t *w) {
    XSetWindowAttributes attributes;
    attributes.override_redirect = True;
    XChangeWindowAttributes(w->app->dpy, w->widget, CWOverrideRedirect, &attributes);

    Atom window_type = XInternAtom(w->app->dpy, "_NET_WM_WINDOW_TYPE", False);
    Atom window_type_popup = XInternAtom(w->app->dpy, "_NET_WM_WINDOW_TYPE_DROPDOWN_MENU", False);
    XChangeProperty(w->app->dpy, w->widget, window_type,
        XA_ATOM, 32, PropModeReplace, (unsigned char *) &window_type_popup,1 );

    Atom window_state = XInternAtom(w->app->dpy, "_NET_WM_STATE", False);
    Atom window_state_modal = XInternAtom(w->app->dpy, "_NET_WM_STATE_MODAL", False);
    XChangeProperty(w->app->dpy, w->widget, window_state,
        XA_ATOM, 32, PropModeReplace, (unsigned char *) &window_state_modal, 1);
}

void os_set_transient_for_hint(Widget_t *parent, Widget_t *w) {

    XSetTransientForHint(parent->app->dpy,w->widget,parent->widget);
}

int os_get_screen_height(Widget_t *w){
    int snum = DefaultScreen(w->app->dpy);
    return DisplayHeight(w->app->dpy, snum);
}

int os_get_screen_width(Widget_t *w){
    int snum = DefaultScreen(w->app->dpy);
    return DisplayWidth(w->app->dpy, snum);
}

void os_get_dpi(Xputty *main) {
    XrmDatabase	db;   
    XrmValue ret;
    char *resource_string;
    char *type;

    XrmInitialize();
    resource_string = XResourceManagerString(main->dpy);
    db = XrmGetStringDatabase(resource_string);
    if (resource_string) {
        if (XrmGetResource(db, "Xft.dpi", "String", &type, &ret) == True) {
            if (ret.addr) {
                main->hdpi = atof(ret.addr)/96;
            }
        }
    }
}

bool os_is_directory(const char *filename) {
    struct stat sb;
    return (stat(filename, &sb) == 0 && S_ISDIR(sb.st_mode));
}

char *os_get_home_dir() {
    char *pPath = NULL;
    asprintf(&pPath, "%s", getenv("HOME"));
    assert(pPath != NULL);
    return pPath;
}

bool os_get_keyboard_input(Widget_t *w, XKeyEvent *key, char *buf, size_t bufsize) {
    Status status;
    KeySym keysym;
    Xutf8LookupString(w->xic, key, buf, bufsize - 1, &keysym, &status);
    return (status == XLookupChars || status == XLookupBoth);
}

void os_free_pixmap(Widget_t *w, Pixmap pixmap) {
    XFreePixmap(w->app->dpy, pixmap);
}

void os_quit(Widget_t *w) {
    //Atom WM_DELETE_WINDOW = XInternAtom(w->app->dpy, "WM_DELETE_WINDOW", True);
    Atom WM_DELETE_WINDOW = os_register_wm_delete_window(w);
    XClientMessageEvent xevent;
    memset(&xevent, 0, sizeof (xevent));
    xevent.type = ClientMessage;
    xevent.message_type = WM_DELETE_WINDOW;
    xevent.display = w->app->dpy;
    xevent.window = get_toplevel_widget(w->app)->widget;
    xevent.format = 16;
    xevent.data.l[0] = WM_DELETE_WINDOW;
    XSendEvent(w->app->dpy, w->widget, 0, 0, (XEvent *)&xevent);
}

void os_quit_widget(Widget_t *w) {
    Atom QUIT_WIDGET = os_register_widget_destroy(w);
    XClientMessageEvent xevent;
    memset(&xevent, 0, sizeof (xevent));
    xevent.type = ClientMessage;
    xevent.message_type = QUIT_WIDGET;
    xevent.display = w->app->dpy;
    xevent.window = w->widget;
    xevent.format = 16;
    xevent.data.l[0] = 1;
    XSendEvent(w->app->dpy, w->widget, 0, 0, (XEvent *)&xevent);
}

Atom os_register_wm_delete_window(Widget_t * wid) {
    Atom WM_DELETE_WINDOW;
    WM_DELETE_WINDOW = XInternAtom(wid->app->dpy, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(wid->app->dpy, wid->widget, &WM_DELETE_WINDOW, 1);
    return WM_DELETE_WINDOW;
}

Atom os_register_widget_destroy(Widget_t * wid) {
    return XInternAtom(wid->app->dpy, "WIDGET_DESTROY", False);
}

void os_main_run(Xputty *main) {
    Widget_t * wid = main->childlist->childs[0]; 
    Atom WM_DELETE_WINDOW;
    WM_DELETE_WINDOW = XInternAtom(main->dpy, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(main->dpy, wid->widget, &WM_DELETE_WINDOW, 1);

    XEvent xev;
    int ew;
    Widget_t * w = NULL;

    while (main->run && (XNextEvent(main->dpy, &xev)>=0)) {
        // if (XFilterEvent(&xev, None)) continue;
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

void os_run_embedded(Xputty *main) {

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

void os_init_dnd(Xputty *main) {
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

#ifdef __cplusplus
}
#endif

#endif /* _OS_UNIX_ */
