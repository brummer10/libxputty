# libxputty

A damn tiny abstraction Layer to create X11 window/widgets with cairo surfaces 

This repository contain only the library files to allow easily and lightweight 
embedding into other projects.

For check out some examples go here:
[Examples](https://github.com/brummer10/Xputty)

## Features

- easy creation of widgets and windows within the xlib windows system
- easy handling of multiple windows including multiple widgets
- easy memory managment by the xputty main struct
- easy to use main struct to handle the lifetime of all widgets and windows
    - Xputty main;
    - main_init(&main);
    - create_windows();
    - main_run(&main);
    - main_quit(&main);
- easy embedding XWindows into other native windows
- all events been handled by function pointers, thus allow
    - easy connection to event handlers by simply overwriting the defaults with you own handlers
    - implement you own events by add a function pointer and connect to it
    - block free connection between dialog boxes and the main window
- easy to use x/y adjustments to create your own controller widgets like sliders, knobs, buttons or trackballs
- double buffered cairo surfaces to enable transparent drawing on child widgets
- handle child widgets via childlist
- multiple implemented resizing options
- a couple of defined Widgets and Dialogs ready to use
- full documented API [Documentation](https://brummer10.github.io/Xputty/html/index.html)
- static linking to create position independent applications


Here is the usual hello world:

![simple-example](https://github.com/brummer10/Xputty/raw/master/examples/simple-example.png)

produced  by this code:

```C

#include "xputty.h"

/** your own expose function */
static void draw_window(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    cairo_set_source_rgb (w->crb, 1, 1, 1);
    cairo_paint (w->crb);
}

int main (int argc, char ** argv)
{
    /** acces the main struct */
    Xputty app;
    /** init the main struct */
    main_init(&app);
    /** create a Window on default root window */
    Widget_t *w = create_window(&app, DefaultRootWindow(app.dpy), 0, 0, 300, 200);
    /** acces Xlib function */
    XStoreName(app.dpy, w->widget, "Hello world");
    /** overwrite event handler with your own */
    w->func.expose_callback = draw_window;
    /** map the Window to display */
    widget_show_all(w);
    /** run the event loop */
    main_run(&app);
    /** clean up after the event loop is finished */
    main_quit(&app);
    return 0;
}

```

Here are a bit more advanced examples:

![Theme-example](https://github.com/brummer10/Xputty/raw/master/examples/Theme-example.png)&nbsp;&nbsp;&nbsp;&nbsp;![xgain](https://github.com/brummer10/Xputty/raw/master/examples/xgain.png)


![midikeyboard](https://github.com/brummer10/Xputty/raw/master/examples/midikeyboard.png)


![File-selector](https://github.com/brummer10/Xputty/raw/master/examples/File-selector.png)


## License

         0BSD 
BSD Zero Clause License
