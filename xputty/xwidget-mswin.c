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

#ifdef _WIN32 //PlatformWrappers

#include <windowsx.h>

#include "xwidget.h"
#include "xwidget_private.h"
#include "xasprintf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UINT (WINAPI *PGDFS)();
TCHAR szMainUIClassName[]   = TEXT("xputtyMainUI____0123456789ABCDEF");
TCHAR szWidgetUIClassName[] = TEXT("xputtyWidgetUI__0123456789ABCDEF");

// forward declarations
void SetClientSize(HWND hwnd, int clientWidth, int clientHeight);
BOOL SetMouseTracking(HWND hwnd, BOOL enable);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT onPaint( HWND hwnd, WPARAM wParam, LPARAM lParam );

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
            internal helper functions
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

void debug_lasterror(const char *prefix, const char *prefix2) {
    LPSTR msg = NULL;
    DWORD err = GetLastError();
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER
                |FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&msg, 0, NULL);
    if (size) {
        debug_print("%s%s:ERR=%8.8lx (%ld): %s\n",prefix?prefix:"",prefix2?prefix2:"",err,err,msg);
        LocalFree(msg);
    } else {
        DWORD fmt_err = GetLastError();
        debug_print("%s%s:ERROR:FormatMessage for ERR=%8.8lx (%ld) returned %8.8lx (%ld)\n",
                (prefix ? prefix : ""), (prefix2 ? prefix2 : ""), err, err, fmt_err, fmt_err);
    }
}

/*---------------------------------------------------------------------
            codepage conversion
----------------------------------------------------------------------*/

// general codepage conversion
char *convert_cp(DWORD cp_from, DWORD cp_to, char *s_from) {
    char *s_to = NULL;
    int flags = MB_PRECOMPOSED; // | MB_ERR_INVALID_CHARS;
    // prepare conversion to WideChar (using cp_from) - get required space
    size_t size = MultiByteToWideChar(cp_from, flags, s_from, -1, NULL, 0);
    if (size) {
        // convert Ansi to WideChar (pwc)
        wchar_t *pwc= (wchar_t*)malloc(size*2);
        size_t size_wc = MultiByteToWideChar(cp_from, flags, s_from, -1, pwc, size);
        if (size_wc) {
            // prepare conversion to cp_to - get required space
            flags = 0;
            size = WideCharToMultiByte(cp_to, flags, pwc, size_wc, NULL, 0, NULL, NULL);
            if (size) {
                // convert WideChar (pwc) to Ansi using cp_to
                s_to = (char*)malloc(size+1);
                memset(s_to, 0 , size+1);
                size = WideCharToMultiByte(cp_to, flags, pwc, size_wc, s_to, size, NULL, NULL);
            }
        }
        free(pwc);
    }
    // needs to be free()d by caller
    return s_to;
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
            common functions (required)
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

// convert active codepage to utf8
char *utf8_from_locale(char *localestr) {
    return (convert_cp(GetACP(), CP_UTF8, localestr));
}

// convert utf8 to active codepage
char *locale_from_utf8(char *utf8str) {
    return (convert_cp(CP_UTF8, GetACP(), utf8str));
}

Display *os_open_display(char *display_name) {
    // nothing to do on MSWin
    return (Display *)1;
}

void os_close_display(Display *dpy) {
    // nothing to do on MSWin
}

void os_destroy_window(Widget_t *w) {
    // mswin automatically sends WM_DESTROY to all child windows
    // floating windows need to be handled manually
    if (w && (IsWindow(w->widget))) {
        //debug_print("os_destroy_window:DestroyWindow:hwnd=%p\n",(w)?w->widget:NULL);
        DestroyWindow(w->widget);
    } else {
        debug_print("os_destroy_window:DestroyWindow:NOTFOUND:hwnd=%p\n",(w)?w->widget:NULL);
    }
}

Window os_get_root_window(Xputty *main, int flag) {
    if (flag & IS_WIDGET) return HWND_DESKTOP; // no decorated
    else return (HWND)-1; // decorated
}

void os_translate_coords(Widget_t *w, Window from_window, Window to_window,
                          int from_x, int from_y, int *to_x, int *to_y) {
    POINT Point = {from_x, from_y};
    MapWindowPoints(from_window, to_window, &Point, 1);
    *to_x = Point.x;
    *to_y = Point.y;
}

void os_get_window_metrics(Widget_t *w_, Metrics_t *metrics) {
    RECT WindowRect;
    RECT ClientRect;
    POINT Point;
    Widget_t *parent = (Widget_t *)w_->parent;

    if (GetWindowRect(w_->widget, &WindowRect) \
     && GetClientRect(w_->widget, &ClientRect)) {
        Point.x = WindowRect.left; // WindowRect has correct coords, but wrong width/height
        Point.y = WindowRect.top;  // ClientRect has correct width/height, but top/left == 0
        ScreenToClient(parent->widget, &Point); // "parent" is intentional (coords are relative to owner widget)
        metrics->x = Point.x;
        metrics->y = Point.y;
        metrics->width = ClientRect.right - ClientRect.left;
        metrics->height = ClientRect.bottom - ClientRect.top;
    }
    metrics->visible = IsWindowVisible(w_->widget);
}

// values are checked on WM_SIZE
void os_set_window_min_size(Widget_t *w, int min_width, int min_height,
                            int base_width, int base_height) {
    w->metrics_min.width = min_width;
    w->metrics_min.height = min_height;
}

void os_move_window(Display *dpy, Widget_t *w, int x, int y) {
    SetWindowPos(w->widget, NULL, //hWnd, hWndInsertAfter
      x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER); //X, Y, width, height, uFlags
}

void os_resize_window(Display *dpy, Widget_t *w, int x, int y) {
    SetWindowPos(w->widget, NULL, //hWnd, hWndInsertAfter
      0, 0, x, y, SWP_NOMOVE|SWP_NOZORDER); //X, Y, width, height, uFlags
}

void os_get_surface_size(cairo_surface_t *surface, int *width, int *height) {
    *width = cairo_image_surface_get_width(surface);
    *height = cairo_image_surface_get_height(surface);
}

void os_get_dpi(Xputty *main) {
    PGDFS pGDFS;
    pGDFS = (PGDFS) GetProcAddress(GetModuleHandle(TEXT("User32.dll")), "GetDpiForSystem");
    if(NULL != pGDFS) main->hdpi = (float)(pGDFS()/96);
    else main->hdpi = 1.0;    
}

void os_set_widget_surface_size(Widget_t *w, int width, int height) {
    // image_surface cant be resized (only xlib_surface can)
    cairo_destroy(w->cr);
    cairo_surface_destroy(w->surface);
    w->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    w->cr = cairo_create(w->surface);
}

void os_create_main_window_and_surface(Widget_t *w, Xputty *app, Window win,
                          int x, int y, int width, int height) {
    // Event callbacks already start during CreateWindow(),
    // so prepare childlist before that call on MSWin
    // (on Linux, adding to childlist starts message events)
    childlist_add_child(app->childlist,w);
    // prepare window class
    WNDCLASS wndclass = {0};
    HINSTANCE hInstance = NULL;

    snprintf(szMainUIClassName+16, 16, "%p", WndProc);
    snprintf(szWidgetUIClassName+16, 16, "%p", WndProc);

    // create a permanent surface for drawing (see onPaint() event)
    w->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

    wndclass.style         = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.hInstance     = hInstance;
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = NULL;
    wndclass.lpszClassName = szMainUIClassName;
    wndclass.cbWndExtra    = sizeof(w); // reserve space for SetWindowLongPtr
    RegisterClass(&wndclass);
    // TODO: pass window style (mainwindow,childwidget,popup,...) to create_window()
    // _WIN32 doesnt allow changing the style afterwards, as it is done in xmenu.cpp
    // (this also removes duplicate code for window/widget creation).
    // For the current situation it is sufficient to set popup style if parent is HWND_DESKTOP.
    DWORD dwStyle = 0;
    DWORD dwExStyle = 0;
    // dwExStyle:
    //   WS_EX_APPWINDOW - force(!) taskbar icon
    //   WS_EX_CONTROLPARENT - contains children, uses tab+cursor nav.
    //   WS_EX_NOPARENTNOTIFY - no PARENTNOTIFY on create/destroy
    //   WS_EX_TOOLWINDOW - no taskbar icon
    //   WS_EX_TOPMOST - above all others
    if (win == (HWND)-1) {
        // Dialogs with border
        dwStyle = WS_OVERLAPPEDWINDOW;
        dwExStyle = WS_EX_CONTROLPARENT | WS_EX_ACCEPTFILES;
        win = HWND_DESKTOP;
        // include border widths
        RECT Rect = {0};
        BOOL bMenu = false;
        Rect.right = width;
        Rect.bottom = height;
        if (AdjustWindowRectEx(&Rect, dwStyle, bMenu, dwExStyle)) {
            width = Rect.right - Rect.left;
            height = Rect.bottom - Rect.top;
        }
    } else {
        if (win == HWND_DESKTOP) {
            // Floating without border (popup, tooltip)
            dwStyle = WS_POPUP;
            dwExStyle = WS_EX_TOPMOST | WS_EX_CONTROLPARENT | WS_EX_TOOLWINDOW;
        } else {
            // Embedded widget
            dwStyle = WS_CHILD;
            dwExStyle = WS_EX_CONTROLPARENT | WS_EX_TOOLWINDOW;
        }
    }
    // create the window
    w->widget = CreateWindowEx(dwExStyle, szMainUIClassName,
                            TEXT("Draw Surface"), // lpWindowName
                            dwStyle, // dwStyle
                            CW_USEDEFAULT, CW_USEDEFAULT, // X, Y
                            width, height, // nWidth, nHeight
                            win, // hWndParent (no embeddeding takes place yet)
                            NULL, hInstance, (LPVOID)w); // hMenu, hInstance, lpParam
    SetParent(w->widget, win); // embed into parentWindow
    SetMouseTracking(w->widget, true); // for receiving WM_MOUSELEAVE

    os_set_window_min_size(w, width/2, height/2, width, height);
}

void os_create_widget_window_and_surface(Widget_t *w, Xputty *app, Widget_t *parent,
                          int x, int y, int width, int height) {
    // Event callbacks already start during CreateWindow(),
    // so prepare childlist before that call on MSWin
    // (on Linux, adding to childlist starts message events)
    childlist_add_child(app->childlist,w);
    // prepare window class
    WNDCLASS wndclass = {0};
    HINSTANCE hInstance = NULL;

    // create a permanent surface for drawing (see onPaint() event)
    w->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

    wndclass.style         = CS_DBLCLKS;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.hInstance     = hInstance;
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = NULL;
    wndclass.lpszClassName = szWidgetUIClassName;
    wndclass.cbWndExtra    = sizeof(w); // reserve space for SetWindowLongPtr
    RegisterClass(&wndclass);
    // create the window
    DWORD dwExStyle = WS_EX_CONTROLPARENT;
    w->widget = CreateWindowEx(dwExStyle, szWidgetUIClassName,
                            TEXT("Draw Surface"), // lpWindowName
                            WS_CHILD, // dwStyle
                            x, y, // X, Y
                            width, height, // nWidth, nHeight
                            parent->widget, // hWndParent (no embeddeding takes place yet)
                            NULL, hInstance, (LPVOID)w); // hMenu, hInstance, lpParam

    SetParent(w->widget, parent->widget); // embed into parentWindow
    SetMouseTracking(w->widget, true); // for receiving WM_MOUSELEAVE
}

void os_set_title(Widget_t *w, const char *title) {
    if (title)
        SetWindowText(w->widget, title);
}

void os_widget_show(Widget_t *w) {
    ShowWindow(w->widget, SW_SHOW);
}

void os_widget_hide(Widget_t *w) {
    ShowWindow(w->widget, SW_HIDE);
}

void os_show_tooltip(Widget_t *wid, Widget_t *w) {
    POINT pt;
    if (GetCursorPos(&pt)) {
        SetWindowPos(w->widget, NULL, //hWnd, hWndInsertAfter
          pt.x+10, pt.y-10, 0, 0,
          SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_SHOWWINDOW); //X, Y, width, height, uFlags
    }
}

void os_expose_widget(Widget_t *w) {
    RedrawWindow(w->widget, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
}

void os_widget_event_loop(void *w_, void* event, Xputty *main, void* user_data) {
    // nothing to do
}

void os_send_configure_event(Widget_t *w,int x, int y, int width, int height) {
    // STUB
}

void os_send_button_press_event(Widget_t *w) {
    // STUB
}

void os_send_button_release_event(Widget_t *w) {
    // STUB
}

void os_send_systray_message(Widget_t *w) {
    // STUB
}

int os_grab_pointer(Widget_t *w) {
    // STUB
    return 0;
}

void os_set_input_mask(Widget_t *w) {
    // STUB
}

void os_set_window_attrb(Widget_t *w) {
    // STUB
}

void os_set_transient_for_hint(Widget_t *parent, Widget_t *w) {
    POINT pt;
    if (GetCursorPos(&pt)) {
        SetWindowPos(w->widget, NULL, //hWnd, hWndInsertAfter
          pt.x+10, pt.y-10, 0, 0, SWP_NOSIZE|SWP_NOZORDER); //X, Y, width, height, uFlags
    }
}

int os_get_screen_height(Widget_t *w) {
    return GetSystemMetrics(SM_CYSCREEN);
}

int os_get_screen_width(Widget_t *w) {
    return GetSystemMetrics(SM_CXSCREEN);
}

char *os_get_home_dir() {
    char *pPath = NULL;
    asprintf(&pPath, "%s%s", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
    assert(pPath != NULL);
    fprintf(stderr, "%s\n", pPath);
    return pPath;
}

bool os_is_directory(const char *filename) {
    DWORD attribs = GetFileAttributes(filename);
    return ((attribs != INVALID_FILE_ATTRIBUTES) &&     // check if a valid file
    ((attribs & FILE_ATTRIBUTE_DIRECTORY) != 0) &&  // file is a directory
    ((attribs & FILE_ATTRIBUTE_HIDDEN) == 0) &&     // file is not hidden
    ((attribs & FILE_ATTRIBUTE_SYSTEM) == 0));     // file is not system
}

void os_transparent_draw(void *w_, void *user_data) {
    Widget_t *w = (Widget_t *)w_;
   // transparent_draw(w, user_data);
    RedrawWindow(w->widget, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}

void os_adjustment_callback(void *w_, void *user_data) {
    Widget_t *w = (Widget_t *)w_;
   // transparent_draw(w, user_data);
    RedrawWindow(w->widget, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
}

bool os_get_keyboard_input(Widget_t *w, XKeyEvent *key, char *buf, size_t bufsize) {
    char ansibuf[2];
    ansibuf[0] = (char)key->vk;
    ansibuf[1] = 0;
    char *utf8 = utf8_from_locale(ansibuf);
    int l=min(bufsize, strlen(utf8));
    strncpy(buf, utf8, l);
    buf[l] = 0;
    free(utf8);
    return key->vk_is_final_char; // for only feeding readily processed chars into input widgets
}

void os_free_pixmap(Widget_t *w, Pixmap pixmap) {
    // Pixmap stuff not implemented at all
}

void os_quit(Widget_t *w) {
    if (w) {
        WPARAM wParam = (WPARAM)get_toplevel_widget(w->app)->widget;
        DWORD msg = os_register_wm_delete_window(w);
        SendMessage(w->widget, msg, wParam, 0); // WM_DELETE_WINDOW
    }
    // UnregisterClass silently fails, if there are still more windows of this class
    if (UnregisterClass(szMainUIClassName, NULL)) {
        debug_print("UnregisterMainClass:%s:OK\n", szMainUIClassName);
    } else
        debug_lasterror("UnregisterMainClass:", szMainUIClassName);

    if (UnregisterClass(szWidgetUIClassName, NULL)) {
        debug_print("UnregisterWidgetClass:%s:OK\n", szWidgetUIClassName);
    } else
        debug_lasterror("UnregisterWidgetClass" ,szWidgetUIClassName);

}

void os_quit_widget(Widget_t *w) {
    // who invokes this?
    WPARAM wParam = (WPARAM)w->widget;
    DWORD msg = os_register_widget_destroy(w);
    SendMessage(w->widget, msg, wParam, 0); // WIDGET_DESTROY
}

Atom os_register_wm_delete_window(Widget_t * wid) {
    Atom msg = WM_USER + 01;
    //Atom msg = RegisterWindowMessage("XPUTTY_WM_DELETE_WINDOW");
    return msg;
}

Atom os_register_widget_destroy(Widget_t * wid) {
    Atom msg = WM_USER + 02 ;
    //Atom msg = RegisterWindowMessage("XPUTTY_WIDGET_DESTROY");
    return msg;
}

void os_main_run(Xputty *main) {
    MSG msg;
    BOOL bRet;
    while( main->run && (bRet = GetMessage(&msg, NULL, 0, 0)) > 0) {
        if (bRet == -1) {
          return; // error
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

void os_run_embedded(Xputty *main) {
    // messageloop already polled by plugin host
}

void os_init_dnd(Xputty *main) {
    // DND not implemented for win32
}

/*---------------------------------------------------------------------
----------------------------------------------------------------------- 
            private functions
-----------------------------------------------------------------------
----------------------------------------------------------------------*/

/*------------- the event loop ---------------*/

void build_xkey_event(XKeyEvent *ev, UINT msg, WPARAM wParam, LPARAM lParam) {
    UINT uVirtKey = (UINT)wParam;
    UINT uScanCode = (UINT)(HIWORD(lParam) & 0x1FF);
    BYTE lpKeyState[256];
    if (GetKeyboardState(lpKeyState)) {
        //https://stackoverflow.com/questions/42667205/maximum-number-of-characters-output-from-win32-tounicode-toascii
        // required size for the return buffer isn't exactly clear, maybe 255, so 1K should be a safe guess
        WCHAR lpChar[1024];
        UINT uFlags = 0x04; // 1=menu is active, 4=dont change anything
        if (msg == WM_CHAR) {
            ev->vk = uVirtKey;
            ev->vk_is_final_char = 1;
        } else {
            ToUnicode(uVirtKey, uScanCode, lpKeyState, lpChar, 2, uFlags);
            ev->vk = lpChar[0];
            ev->vk_is_final_char = 0;
        }
    }
    // handle special characters (only in KEYUP/DOWN?)
    switch (uScanCode) {
        case 0x0029: ev->keycode = XK_dead_circumflex;  break;
        case 0x000e: ev->keycode = XK_BackSpace;        break;
        case 0x000f: ev->keycode = XK_Tab;              break;
        case 0x001c: ev->keycode = XK_Return;           break;
        case 0x0147: ev->keycode = XK_Home;             break;
        case 0x014b: ev->keycode = XK_Left;             break;
        case 0x0148: ev->keycode = XK_Up;               break;
        case 0x014d: ev->keycode = XK_Right;            break;
        case 0x0150: ev->keycode = XK_Down;             break;
        case 0x014f: ev->keycode = XK_End;              break;
        case 0x0152: ev->keycode = XK_Insert;           break;
        case 0x011c: ev->keycode = XK_KP_Enter;         break;
        case 0x0047: ev->keycode = XK_KP_Home;          break;
        case 0x004b: ev->keycode = XK_KP_Left;          break;
        case 0x0048: ev->keycode = XK_KP_Up;            break;
        case 0x004d: ev->keycode = XK_KP_Right;         break;
        case 0x0050: ev->keycode = XK_KP_Down;          break;
        case 0x004f: ev->keycode = XK_KP_End;           break;
        case 0x0052: ev->keycode = XK_KP_Insert;        break;
        case 0x004e: ev->keycode = XK_KP_Add;           break;
        case 0x004a: ev->keycode = XK_KP_Subtract;      break;
        default:
            if (ev->vk == 0xfc) //'ü'
                ev->keycode = XK_udiaeresis;
            else if (ev->vk == 0xdc) //'Ü'
                ev->keycode = XK_dead_diaeresis;
            else
                ev->keycode = ev->vk;
    }
}

void HandleFiles(WPARAM wParam, Widget_t * wid)
{
    // DragQueryFile() takes a LPWSTR for the name so we need a TCHAR string
    TCHAR szName[MAX_PATH];

    // Here we cast the wParam as a HDROP handle to pass into the next functions
    HDROP hDrop = (HDROP)wParam;

    int count = DragQueryFile(hDrop, 0xFFFFFFFF, szName, MAX_PATH);

    // Here we go through all the files that were drag and dropped then display them
    for(int i = 0; i < count; i++) {
        DragQueryFile(hDrop, i, szName, MAX_PATH);

        // Bring up a message box that displays the current file being processed
        // MessageBox(GetForegroundWindow(), szName, "Current file received", MB_OK);
        char *dndfile = utf8_from_locale(szName);
        wid->func.dnd_notify_callback(wid, (void*)&dndfile);
        free(dndfile);
    }

    DragFinish(hDrop);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    XButtonEvent xbutton;
    XMotionEvent xmotion;
    XKeyEvent xkey;
    void *user_data = NULL;

    // be aware: "wid" can be NULL during window creation (esp. if there is a debugger attached)
    Widget_t *wid = (Widget_t *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    Xputty *main = wid ? wid-> app : NULL;

    xbutton.window = hwnd;
    xbutton.x = GET_X_LPARAM(lParam);
    xbutton.y = GET_Y_LPARAM(lParam);
    xmotion.window = hwnd;
    xmotion.x = GET_X_LPARAM(lParam);
    xmotion.y = GET_Y_LPARAM(lParam);

    switch (msg) {
        case WM_CREATE:
            debug_print("WM:WM_CREATE:hwnd=%p:wid=%p",hwnd,wid);
            {
                CREATESTRUCT *pCreate = (CREATESTRUCT *)lParam;
                wid = (Widget_t *)pCreate->lpCreateParams;
                // CreateWindowEx() hasnt returned yet, so wid->widget is not set
                wid->widget = hwnd;
                // make "wid" available in messageloop events via GetWindowLongPtr()
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)wid);
            }
            return 0;

        // MSWin only: React to close requests
        case WM_CLOSE:
            // standalone
            if (hwnd == main->childlist->childs[0]->widget) {
                // is main window: end application
                PostQuitMessage(0); // end messageloop (continuing to main_quit())
            } else // is sub window (menu, dialog, ...): close
                DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
            return 0;

        // X11:ConfigureNotify
        case WM_SIZE:
            if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
            else {
                if(wid->app->hold_grab && wid == get_toplevel_widget(wid->app)) {
                    widget_hide(wid->app->hold_grab);
                    wid->app->hold_grab = NULL;
                    if (wid->app->submenu) {
                        widget_hide(wid->app->submenu);
                        wid->app->submenu = NULL;
                    }
                }
                // Limit window size:
                // The plugin doesnt receive WM_MINMAXINFO or WM_SIZING.
                // SWP_NOSIZE in WM_WINDOWPOSCHANGING cant handle width/height separately.
                // Setting the client size afterwards turned out to be the best working option so far.
                // Plugin: Limits the "zoom" (as the hosts window can always become smaller)
                // Standalone: Limits the window size
                int width = LOWORD(lParam);
                int height = HIWORD(lParam);
                if ((width < wid->metrics_min.width)
                    || (height < wid->metrics_min.height)) {
                    SetClientSize(hwnd, max(width, wid->metrics_min.width),
                            max(height, wid->metrics_min.height));
                    return 0;
                }
                // Resize handler
                if (!wid->func.configure_callback) return 0;
                wid->func.configure_callback(wid, user_data);
                // widgets recive the call to os_transparent_draw via the configure_notify_callback
                // windows have the CS_HREDRAW | CS_VREDRAW flags set so that they be redrawn by the system
                return 0;
            }

        case WM_MOVE:
            if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
            if(wid->app->hold_grab && wid == get_toplevel_widget(wid->app)) {
                widget_hide(wid->app->hold_grab);
                wid->app->hold_grab = NULL;
                if (wid->app->submenu) {
                    widget_hide(wid->app->submenu);
                    wid->app->submenu = NULL;
                }
            }
        // X11:Expose
        case WM_PAINT:
            if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
            if (!(wid->crb)) {
                debug_print("WM_PAINT:BAILOUT:wid->crb==NULL\n");
                return 0;
            }
            return onPaint(hwnd, wParam, lParam); // not possible on mswin: (only fetch the last expose event)

        // MSWin only: Allow keyboard input
        case WM_ACTIVATE:
            SetFocus(hwnd);
            return 0;
        case WM_MOUSEACTIVATE:
            SetFocus(hwnd);
            return MA_ACTIVATE;

        // X11:ButtonPress
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
            SetCapture(hwnd); // also receive WM_MOUSEMOVE from outside this window
            
            if (msg == WM_LBUTTONDOWN)
                xbutton.button = Button1;
            else
                xbutton.button = Button3;
            if (wid->state == 4) break;
            if (wid->flags & HAS_TOOLTIP) hide_tooltip(wid);
            _button_press(wid, &xbutton, user_data);
            debug_print("Widget_t  ButtonPress %i\n", xbutton.button);
            return 0;
        case WM_MOUSEWHEEL:
            if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
            // opposed to X11, WM_MOUSEWHEEL doesnt contain mouse coordinates
            {
                DWORD pos = GetMessagePos();
                pt.x = GET_X_LPARAM(pos);
                pt.y = GET_Y_LPARAM(pos);
                if (ScreenToClient(hwnd, &pt)) {
                    wid->pos_x = pt.x;
                    wid->pos_y = pt.y;
                }
            }
            if (GET_WHEEL_DELTA_WPARAM(wParam) <= 0) {
                xbutton.button = Button5;
                _button_press(wid, &xbutton, user_data);
            } else {
                xbutton.button = Button4;
                _button_press(wid, &xbutton, user_data);
            }
            // forward WM_MOUSEWHEEL from menuitem to viewport (with slider)
            // (viewport lies below menuitem, so doesnt receive WM_MOUSEWHEEL)
            if(wid->app->hold_grab) {
                Widget_t *view_port = wid->app->hold_grab->childlist->childs[0];
                if (hwnd != view_port->widget)
                    SendMessage(view_port->widget, msg, wParam, lParam);
                RedrawWindow(view_port->widget, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
            }
            return 0;
        // X11:ButtonRelease
        case WM_LBUTTONDBLCLK:
            if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
            xbutton.button = Button1;
            wid->func.double_click_callback(wid, &xbutton, user_data);
            return 0;
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
            ReleaseCapture();
            if (msg == WM_LBUTTONUP)
                xbutton.button = Button1;
            else if (msg == WM_MBUTTONUP)
                xbutton.button = Button2;
            else
                xbutton.button = Button3;
            _check_grab(wid, &xbutton, wid->app);
            _check_submenu(wid, &xbutton, main);
            if (wid->state == 4) break;
            _has_pointer(wid, &xbutton);
            if(wid->flags & HAS_POINTER) wid->state = 1;
            else wid->state = 0;
            _check_enum(wid, &xbutton);
            wid->func.button_release_callback((void*)wid, &xbutton, user_data);
            debug_print("Widget_t  ButtonRelease %i\n", xbutton.button);
            return 0;

        // X11:KeyPress and X11:KeyRelease
        // The resulting character (e.g. from dead-key combinations) cannot be
        // determined from WM_KEYUP or WM_KEYDOWN: WM_CHAR has to be used instead.
        // To workaround that, WM_CHAR fires key_press- and key_release_event()
        // after another, with the flag "->vk_is_final_char" set, so the client
        // code can differentiate between real KEYUP/DOWN and fake CHAR events.
        case WM_CHAR:
        case WM_KEYDOWN:
        case WM_KEYUP:
            build_xkey_event(&xkey, msg, wParam, lParam);
            // X11:KeyPress
            if (msg != WM_KEYUP) { // WM_KEYDOWN and WM_CHAR: key_press_callback()
                if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
                if (wid->state == 4) return 0;
                // on Linux, retrigger check happens in KeyRelease (WM_KEYUP)
                unsigned short is_retriggered = 0;
                if(wid->flags & NO_AUTOREPEAT) {
                    if ((HIWORD(lParam) & KF_REPEAT) == KF_REPEAT)
                        is_retriggered = 1;
                }
                if (!is_retriggered) {
                    _check_keymap(wid, xkey);
                    wid->func.key_press_callback((void *)wid, &xkey, user_data);
                    debug_print("Widget_t KeyPress %x\n", xkey.keycode);
                }
            }
            //X11:KeyRelease
            if (msg != WM_KEYDOWN) { // WM_KEYUP and WM_CHAR: key_release_callback()
                if (wid->state == 4) return 0;
                // On MSWin, the REPEAT flag is always set for WM_KEYUP,
                // so the retrigger check has to take place in WM_KEYDOWN instead
                wid->func.key_release_callback((void *)wid, &xkey, user_data);
                debug_print("Widget_t KeyRelease %x\n", xkey.keycode);
            }
            return 0;

        case WM_CAPTURECHANGED:
            if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
            if (wid->flags & MOUSE_CAPTURE) {
                xbutton.button = Button1;
                wid->func.button_release_callback((void*)wid, &xbutton, user_data);
                wid->mouse_inside = false;
                wid->flags &= ~HAS_FOCUS;
                wid->func.leave_callback((void*)wid, user_data);
                debug_print("WM_CAPTURECHANGED\n");
            }
            return 0;

        // X11:LeaveNotify (X11:EnterNotify: see WM_MOUSEMOVE)
        case WM_MOUSELEAVE:
            if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
            // xputty -> xwidget: handled by "ButtonPress" event on Linux

                bool is_item = false;
                GetCursorPos(&pt);
                Window win_cur = WindowFromPoint(pt);
                if(wid->app->submenu != NULL) {
                     if (childlist_has_child(wid->app->submenu->childlist)) {
                        Widget_t *slider = wid->app->submenu->childlist->childs[1];
                        if (win_cur == slider->widget) {
                            return 0;
                        }
                        Widget_t *view_port = wid->app->submenu->childlist->childs[0];
                        int i = view_port->childlist->elem-1;
                        for(;i>-1;i--) {
                            Widget_t *w = view_port->childlist->childs[i];
                            if (win_cur == w->widget) {
                                is_item = True;
                                break;
                            }
                        }
                    }
                }
                if(wid->app->hold_grab != NULL && wid->app->hold_grab->flags & IS_POPUP) {
                    if (childlist_has_child(wid->app->hold_grab->childlist)) {
                        Widget_t *slider = wid->app->hold_grab->childlist->childs[1];
                        if (win_cur == slider->widget) {
                            return 0;
                        }
                        Widget_t *view_port = wid->app->hold_grab->childlist->childs[0];
                        int i = view_port->childlist->elem-1;
                        for(;i>-1;i--) {
                            Widget_t *w = view_port->childlist->childs[i];
                            if (win_cur == w->widget) {
                                is_item = True;
                                break;
                            }
                        }
                        if (win_cur == view_port->widget) is_item = True;
                    }
                    if (!is_item && (wParam & MK_LBUTTON || wParam & MK_RBUTTON)) {
                        ReleaseCapture();
                        widget_hide(wid->app->hold_grab);
                        wid->app->hold_grab = NULL;
                    }
                } else if(wid->app->hold_grab != NULL && ((wid->app->hold_grab->flags & IS_POPUP) == 0)) {
                    wid->app->hold_grab->func.button_press_callback(wid->app->hold_grab, &xbutton, NULL);
                }
 
/*
            // close popup menu if cursor moves out of widget
            if(wid->app->hold_grab != NULL) {
                GetCursorPos(&pt);
                Window win_cur = WindowFromPoint(pt);
                bool is_item = false;
                // still inside viewport? (finds menu entries in popup window)
                Widget_t *view_port = wid->app->hold_grab->childlist->childs[0];
                if (view_port) { // should never be NULL, but who knows :)
                    int i = view_port->childlist->elem-1;
                    for(;i>-1;i--) {
                        Widget_t *w = view_port->childlist->childs[i];
                        if (win_cur == w->widget) {
                            is_item = true;
                            break;
                        }
                    }
                    if (view_port && win_cur == view_port->widget) is_item = true; // inside slider area?
                }
                // still inside combobox? (finds combobox-button)
                Widget_t *menu = NULL;
                menu = (Widget_t *)wid->app->hold_grab->parent_struct;
                if (menu) { // can be NULL if not contained in combobox
                    int i = menu->childlist->elem-1;
                    for(;i>-1;i--) {
                        Widget_t *w = menu->childlist->childs[i];
                        if (win_cur == w->widget) {
                            is_item = true;
                            break;
                        }
                    }
                    if (menu && win_cur == menu->widget) is_item = true; // inside combobox textarea?
                }
                if (!is_item) {
                    ReleaseCapture();
                    widget_hide(wid->app->hold_grab);
                    wid->app->hold_grab = NULL;
                }
            }
*/
            // for emulating X11:EnterNotify
            wid->mouse_inside = false;

            if (wid->state == 4) break;
            //if(!(xev->xcrossing.state & Button1Mask)) {
            if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
                wid->state = 0;
                wid->flags &= ~HAS_FOCUS;
                wid->func.leave_callback((void*)wid, user_data);
                if (!(wid->flags & IS_WINDOW))
                    RedrawWindow(hwnd, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
            }
            if (wid->flags & HAS_TOOLTIP) hide_tooltip(wid);
            debug_print("Widget_t LeaveNotify:hwnd=%p",hwnd);

            return 0;

        // X11:MotionNotify
        case WM_MOUSEMOVE:
            if (!wid) return DefWindowProc(hwnd, msg, wParam, lParam);
            if (!wid->mouse_inside) {
                // emulate X11:EnterNotify
                int ch = childlist_has_child(main->childlist);
                if (ch) {
                    int i = ch;
                    for(;i>0;i--) {
                        if (main->childlist->childs[i-1] != wid) {
                            if (main->childlist->childs[i-1]->mouse_inside) {
                                main->childlist->childs[i-1]->mouse_inside = false;
                                if (main->childlist->childs[i-1]->state != 4) {
                                    main->childlist->childs[i-1]->state = 0;
                                    //RedrawWindow(main->childlist->childs[i-1]->widget, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
                                }
                            }
                        }
                    }
                }
                wid->mouse_inside = true;
                wid->flags |= HAS_FOCUS;
                if (wid->state == 4) break;

                //if(!(xev->xcrossing.state & Button1Mask)) {
                if (!(wParam & MK_LBUTTON)) {
                    wid->state = 1;
                    wid->func.enter_callback((void*)wid, user_data);
                   // if (!(wid->flags & IS_WINDOW))
                   //     RedrawWindow(hwnd, NULL, NULL, RDW_NOERASE | RDW_INVALIDATE | RDW_UPDATENOW);
                    if (wid->flags & HAS_TOOLTIP) show_tooltip(wid);
                    else _hide_all_tooltips(wid);
                }
                debug_print("Widget_t EnterNotify:hwnd=%p",hwnd);

                SetMouseTracking(hwnd, true); // for receiving (next) WM_MOUSELEAVE
            }
            // hovering, etc.
            if (wid->state == 4) return 0;
            if (wParam & MK_LBUTTON) {// TODO: why is this if() required here, but not on linux?
                adj_set_motion_state(wid, xmotion.x, xmotion.y);
                xmotion.state |= Button1Mask;
            }
            wid->func.motion_callback((void*)wid, &xmotion, user_data);
            debug_print("Widget_t MotionNotify x = %li Y = %li hwnd=%p\n",pt.x,pt.y,hwnd );
            return 0;

        case WM_DROPFILES:
            HandleFiles(wParam, wid);
            break;

        // X11:ClientMessage: not implemented (could be done with WM_USER / RegisterWindowMessage())
        case WM_USER + 01: // WM_DELETE_WINDOW
            // xwidget -> xputty (main_run())
            if (wid) {
                if (hwnd == main->childlist->childs[0]->widget) { // main window (this is not invoked for any other window?)
                    main->run = false;
                    destroy_widget(wid, main);
                } else {
                    int i = childlist_find_widget(main->childlist, (Window)wParam);
                    if(i<1) return 0;
                    Widget_t *w = main->childlist->childs[i];
                    if(w->flags & HIDE_ON_DELETE) widget_hide(w);
                    else { destroy_widget(main->childlist->childs[i],main);
                    }
                }
            }
            return 1;
        // X11:ClientMessage:WIDGET_DESTROY
        case WM_USER + 02: // WIDGET_DESTROY
            //os_widget_event_loop()
            if (wid) {
                int ch = childlist_has_child(wid->childlist);
                if (ch) {
                    int i = ch;
                    for(;i>0;i--) {
                        quit_widget(wid->childlist->childs[i-1]);
                    }
                    quit_widget(wid);
                } else {
                    destroy_widget(wid,wid->app);
                }
                return 0;
            }
            return 2;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

LRESULT onPaint( HWND hwnd, WPARAM wParam, LPARAM lParam ) {
    PAINTSTRUCT ps ;
    Widget_t *w = (Widget_t *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    // The cairo_win32_surface should only exist between BeginPaint()/EndPaint(),
    // otherwise it becomes unusable once the HDC of the owner window changes
    // (what can happen anytime, e.g. on resize).
    // Therefore, w->surface is created as a simple cairo_image_surface,
    // that can exist throughout the plugins lifetime (exception: see resize_event())
    // and is copied to a win32_surface in the onPaint() event (see WM_PAINT).

    // draw onto the image surface first
    transparent_draw(w, NULL);

    // prepare to update window
    HDC hdc = BeginPaint(hwnd, &ps );

    // create the cairo surface and context
    cairo_surface_t *surface = cairo_win32_surface_create (hdc);
    cairo_t *cr = cairo_create (surface);
    // copy contents of the (permanent) image_surface to the win32_surface
    cairo_set_source_surface(cr, w->surface, 0.0, 0.0);
    cairo_paint(cr);

    // cleanup
    cairo_destroy (cr);
    cairo_surface_destroy (surface);

    EndPaint( hwnd, &ps );
    return 0 ;
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/ 

void SetClientSize(HWND hwnd, int clientWidth, int clientHeight) {
    if (IsWindow(hwnd)) {
        DWORD dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE) ;
        DWORD dwExStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE) ;
        HMENU menu = GetMenu(hwnd) ;
        RECT rc = {0, 0, clientWidth, clientHeight} ;
        AdjustWindowRectEx(&rc, dwStyle, menu ? TRUE : FALSE, dwExStyle);
        SetWindowPos(hwnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
                     SWP_NOZORDER | SWP_NOMOVE) ;
    }
}

// WM_MOUSELEAVE is only reported ONCE after calling TrackMouseEvent(TME_LEAVE)
BOOL SetMouseTracking(HWND hwnd, BOOL enable) {
    TRACKMOUSEEVENT tme;

    tme.cbSize = sizeof(tme);
    tme.dwFlags = TME_LEAVE;
    if (!enable)
        tme.dwFlags |= TME_CANCEL;
    tme.hwndTrack = hwnd;
    tme.dwHoverTime = HOVER_DEFAULT;
    return TrackMouseEvent(&tme);
}

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/ 


#ifdef __cplusplus
}
#endif

#endif /* _WIN32 */
