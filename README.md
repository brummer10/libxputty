# libxputty

A damn tiny abstraction Layer to create cross compatible window/widgets 
with cairo surfaces for MSWindows (HWND) and Linux (X11).


This repository contain only the library files to allow easily and lightweight 
embedding into other projects.

API Documentation is here: [Documentation](https://brummer10.github.io/libxputty-docs/docs/html/files.html)

A simple example is here: [EXAMPLE](https://brummer10.github.io/libxputty-docs/docs/html/simple-example_8c-example.html)

## Features

- easy creation of widgets and windows
- easy handling of multiple windows including multiple widgets
- easy memory management by the xputty main struct
- easy to use main struct to handle the lifetime of all widgets and windows
    - Xputty main;
    - main_init(&main);
    - create_windows();
    - main_run(&main);
    - main_quit(&main);
- easy embedding Windows into other native windows
- all events been handled by function pointers, thus allow
    - easy connection to event handlers by simply overwriting the defaults with you own handlers
    - implement you own events by add a function pointer and connect to it
    - block free connection between dialog boxes and the main window
- easy to use x/y adjustments to create your own controller widgets like sliders, knobs, buttons or trackballs
- double buffered cairo surfaces to enable transparent drawing on child widgets
- handle child widgets via childlist
- multiple implemented resizing options
- a couple of defined Widgets and Dialogs ready to use
- full documented API [Documentation](https://brummer10.github.io/libxputty-docs/docs/html/files.html)
- static linking to create position independent applications


         0BSD 
BSD Zero Clause License
