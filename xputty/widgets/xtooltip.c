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


#include "xtooltip.h"
#include "xtooltip_private.h"

void tooltip_set_text(Widget_t *w, const char* label) {
    Widget_t *wid = NULL;
    bool is_tooltip = false;
    int i = 0;
    for(;i<w->childlist->elem;i++) {
        wid = w->childlist->childs[i];
        if (wid->flags & IS_TOOLTIP) {
            wid->label = label;
            _get_width(wid);
            is_tooltip = true;
            break;
        }
    }
    if (!is_tooltip) add_tooltip(w, label);
}

void add_tooltip(Widget_t *w, const char* label) {
    Widget_t *wid = create_tooltip(w, 25, 25);
    wid->label = label;
    _get_width(wid);
}

Widget_t* create_tooltip(Widget_t *parent, int width, int height) {

    int x1, y1;
    Window child;
    XTranslateCoordinates( parent->app->dpy, parent->widget, DefaultRootWindow(parent->app->dpy), 0, 0, &x1, &y1, &child );
    Widget_t *wid = create_window(parent->app, DefaultRootWindow(parent->app->dpy), x1+10, y1+10, width, height);

    XSetWindowAttributes attributes;
    attributes.override_redirect = True;
    XChangeWindowAttributes(parent->app->dpy, wid->widget, CWOverrideRedirect, &attributes);

    Atom window_type = XInternAtom(wid->app->dpy, "_NET_WM_WINDOW_TYPE", False);
    Atom window_type_tooltip = XInternAtom(wid->app->dpy, "_NET_WM_WINDOW_TYPE_TOOLTIP", False);
    XChangeProperty(wid->app->dpy, wid->widget, window_type,
        XA_ATOM, 32, PropModeReplace, (unsigned char *) &window_type_tooltip,1 );

    Atom window_state = XInternAtom(wid->app->dpy, "_NET_WM_STATE", False);
    Atom window_state_modal = XInternAtom(wid->app->dpy, "_NET_WM_STATE_MODAL", False);
    XChangeProperty(wid->app->dpy, wid->widget, window_state,
        XA_ATOM, 32, PropModeReplace, (unsigned char *) &window_state_modal, 1);

    XSetTransientForHint(parent->app->dpy,wid->widget,parent->widget);
    wid->flags &= ~USE_TRANSPARENCY;
    wid->func.expose_callback = _draw_tooltip;
    wid->flags |= IS_TOOLTIP;
    parent->flags |= HAS_TOOLTIP;
    wid->scale.gravity = NONE;
    childlist_add_child(parent->childlist, wid);
    return wid;
}
