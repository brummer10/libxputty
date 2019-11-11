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

#include "xmidi_keyboard.h"


void keysym_azerty_to_midi_key(long inkey, float *midi_key) {
    switch(inkey) {
        case(XK_w) : (*midi_key) = 12.0; /* w = C0 */
        break;
        case(XK_s) : (*midi_key) = 13.0; /* s */
        break;
        case(XK_x) : (*midi_key) = 14.0; /* x */
        break;
        case(XK_d) : (*midi_key) = 15.0; /* d */
        break;
        case(XK_c) : (*midi_key) = 16.0; /* c */
        break;
        case(XK_v) : (*midi_key) = 17.0; /* v */
        break;
        case(XK_g) : (*midi_key) = 18.0; /* g */
        break;
        case(XK_b) : (*midi_key) = 19.0; /* b */
        break;
        case(XK_h) : (*midi_key) = 20.0; /* h */
        break;
        case(XK_n) : (*midi_key) = 21.0; /* n */
        break;
        case(XK_j) : (*midi_key) = 22.0; /* j */
        break;
        case(XK_comma) : (*midi_key) = 23.0; /* , */
        break;
        case(XK_a) : (*midi_key) = 24.0; /* a */
        break;
        case(XK_eacute) : (*midi_key) = 25.0; /* eacute */
        break;
        case(XK_z) : (*midi_key) = 26.0; /* z */
        break;
        case(XK_quotedbl) : (*midi_key) = 27.0; /* quotedbl */
        break;
        case(XK_e) : (*midi_key) = 28.0; /* e */
        break;
        case(XK_r) : (*midi_key) = 29.0; /* r */
        break;
        case(XK_parenleft) : (*midi_key) = 30.0; /* parenleft */
        break;
        case(XK_t) : (*midi_key) = 31.0; /* t */
        break;
        case(XK_minus) : (*midi_key) = 32.0; /* minus */
        break;
        case(XK_y) : (*midi_key) = 33.0; /* y */
        break;
        case(XK_egrave) : (*midi_key) = 34.0; /* egrave */
        break;
        case(XK_u) : (*midi_key) = 35.0; /* u */
        break;
        case(XK_i) : (*midi_key) = 36.0; /* i */
        break;
        case(XK_ccedilla) : (*midi_key) = 37.0; /* ccedilla */
        break;
        case(XK_o) : (*midi_key) = 38.0; /* o */
        break;
        case(XK_agrave) : (*midi_key) = 39.0; /* agrave */
        break;
        case(XK_p) : (*midi_key) = 40.0; /* p */
        break;
        case(XK_dead_diaeresis) :
        case(XK_dead_circumflex) : (*midi_key) = 41.0; /* dead circumflex */
        break;
    }
}

void keysym_qwertz_to_midi_key(long inkey, float *midi_key) {
    switch(inkey) {
        case(XK_y) : (*midi_key) = 12.0; /* y = C0 */
        break;
        case(XK_s) : (*midi_key) = 13.0; /* s */
        break;
        case(XK_x) : (*midi_key) = 14.0; /* x */
        break;
        case(XK_d) : (*midi_key) = 15.0; /* d */
        break;
        case(XK_c) : (*midi_key) = 16.0; /* c */
        break;
        case(XK_v) : (*midi_key) = 17.0; /* v */
        break;
        case(XK_g) : (*midi_key) = 18.0; /* g */
        break;
        case(XK_b) : (*midi_key) = 19.0; /* b */
        break;
        case(XK_h) : (*midi_key) = 20.0; /* h */
        break;
        case(XK_n) : (*midi_key) = 21.0; /* n */
        break;
        case(XK_j) : (*midi_key) = 22.0; /* j */
        break;
        case(XK_m) : (*midi_key) = 23.0; /* m */
        break;
        case(XK_q) : (*midi_key) = 24.0; /* q */
        break;
        case(XK_2) : (*midi_key) = 25.0; /* 2 */
        break;
        case(XK_w) : (*midi_key) = 26.0; /* w */
        break;
        case(XK_3) : (*midi_key) = 27.0; /* 3 */
        break;
        case(XK_e) : (*midi_key) = 28.0; /* e */
        break;
        case(XK_r) : (*midi_key) = 29.0; /* r */
        break;
        case(XK_5) : (*midi_key) = 30.0; /* 5 */
        break;
        case(XK_t) : (*midi_key) = 31.0; /* t */
        break;
        case(XK_6) : (*midi_key) = 32.0; /* 6 */
        break;
        case(XK_z) : (*midi_key) = 33.0; /* z */
        break;
        case(XK_7) : (*midi_key) = 34.0; /* 7 */
        break;
        case(XK_u) : (*midi_key) = 35.0; /* u */
        break;
        case(XK_i) : (*midi_key) = 36.0; /* i */
        break;
        case(XK_9) : (*midi_key) = 37.0; /* 9 */
        break;
        case(XK_o) : (*midi_key) = 38.0; /* o */
        break;
        case(XK_0) : (*midi_key) = 39.0; /* 0 */
        break;
        case(XK_p) : (*midi_key) = 40.0; /* p */
        break;
        case(XK_udiaeresis) : (*midi_key) = 41.0; /* ü */
        break;
        case(XK_plus) : (*midi_key) = 42.0; /* + */
        break;
    }
}

void keysym_qwerty_to_midi_key(unsigned int inkey, float *midi_key) {
    keysym_qwertz_to_midi_key(inkey,midi_key);
    if ((*midi_key) == 12) (*midi_key) = 33;
    else if ((*midi_key) == 33) (*midi_key) = 12;
}

static void set_key_in_matrix(unsigned long *key_matrix, int key, bool set) {
    unsigned long *use_matrix = &key_matrix[0];
    
    if(key>94) {
        use_matrix = &key_matrix[3];
        key -=94;
    } else if(key>62) {
        use_matrix = &key_matrix[2];
        key -=62;
    } else if(key>31) {
        use_matrix = &key_matrix[1];
        key -= 31;
    }
    if (set) {
        (*use_matrix) |= (1 << key);
    }else {
        (*use_matrix) &= (~(1 << key));
    }
}

bool is_key_in_matrix(unsigned long *key_matrix, int key) {
    unsigned long *use_matrix = &key_matrix[0];
    
    
    if(key>94) {
        use_matrix = &key_matrix[3];
        key -=94;
    } else if(key>62) {
        use_matrix = &key_matrix[2];
        key -=62;
    } else if(key>31) {
        use_matrix = &key_matrix[1];
        key -= 31;
    }
    bool ret = false;
    if((*use_matrix) & (1<<key)) {
        ret = true;
    }
    return ret;
}

bool have_key_in_matrix(unsigned long *key_matrix) {
    
    bool ret = false;
    int i = 0;
    int j = 0;
    for(;j<4;j++) {
        for(;i<32;i++) {
            if(key_matrix[j] & (1<<i)) {
                ret = true;
                break;
            }
        }
        i = 0;
    }
    return ret;
}

void clear_key_matrix(unsigned long *key_matrix) {
    int i = 0;
    int j = 0;
    for(;j<4;j++) {
        for(;i<32;i++) {
            key_matrix[j] &= (~(1 << i));
        }
        i = 0;
    }
}

static void draw_keyboard(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    int width_t = attrs.width;
    int height_t = attrs.height;
    if (attrs.map_state != IsViewable) return;
    MidiKeyboard *keys = (MidiKeyboard*)w->parent_struct;
    
    cairo_rectangle(w->crb,0,0,width_t,height_t);
    set_pattern(w,&w->app->color_scheme->selected,&w->app->color_scheme->normal,BACKGROUND_);
    cairo_fill (w->crb);

    cairo_set_line_width(w->crb, 1.0);
    int space = 2;
    int set = 0;
    int i = 0;
    int k = 0;

    for(;i<width_t;i++) {
        cairo_rectangle(w->crb,i,height_t*0.3,25,height_t*0.7);
        if ( k+keys->octave == keys->active_key || is_key_in_matrix(keys->key_matrix,k+keys->octave)) {
            use_base_color_scheme(w, ACTIVE_);
            cairo_set_line_width(w->crb, 1.0);
        } else if ( k+keys->octave == keys->prelight_key) {
            use_base_color_scheme(w, PRELIGHT_);
            cairo_set_line_width(w->crb, 2.0);
        } else {
            use_fg_color_scheme(w, NORMAL_);
            cairo_set_line_width(w->crb, 1.0);
        }

        cairo_fill_preserve(w->crb);
        use_base_color_scheme(w, NORMAL_);
        cairo_stroke(w->crb);

        if (space!=4) {
            k++;
        } else {
            if (set <= 3) {
                space = 0;
                set = 0;
            } else if (set == 4) {
                space = 1;
                set = 0;
            }
        }

        if (k>127) break;
        i+=24;
        space++;
        set++;
        k++;
    }

    space = 1;
    set = 0;
    k = 1;
    i = 0;

    for(;i<width_t;i++) {

       if (space!=3) {
            cairo_set_line_width(w->crb, 1.0);
            cairo_rectangle(w->crb,i+15,height_t*0.3,20,height_t*0.4);
            if ( k+keys->octave == keys->active_key || is_key_in_matrix(keys->key_matrix,k+keys->octave)) {
                use_base_color_scheme(w, ACTIVE_);
                cairo_set_line_width(w->crb, 1.0);
            } else if ( k+keys->octave == keys->prelight_key) {
                use_base_color_scheme(w, PRELIGHT_);
                cairo_set_line_width(w->crb, 2.0);
            } else {
                use_bg_color_scheme(w, NORMAL_);
                cairo_set_line_width(w->crb, 1.0);
            }

            cairo_fill_preserve(w->crb);
            use_base_color_scheme(w, NORMAL_);
            cairo_stroke(w->crb);

            k++;
            space++;
            set++;

        } else {

            if (set == 2) {
                space = 0;
                set = 0;
            } else if (set == 3) {
                space = 1;
                set = 0;
            }

        }

        i+=24;
        k++;
        if(k>127)break;
    }
}

static void keyboard_motion(void *w_, void* xmotion_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = w->parent;
    MidiKeyboard *keys = (MidiKeyboard*)w->parent_struct;
    XMotionEvent *xmotion = (XMotionEvent*)xmotion_;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;

    bool catch = false;

    if(xmotion->y < height*0.3) {
        keys->active_key = keys->prelight_key = -1;
        expose_widget(w);
        return;
    }

    if(xmotion->y < height*0.7) {
        int space = 1;
        int set = 0;
        int set_key = 1;
        int i = 0;
        for(;i<width;i++) {
            if (space!=3) {
                if(xmotion->x > i+15 && xmotion->x < i+35) {
                    keys->prelight_key = set_key+keys->octave;
                    if(xmotion->state & Button1Mask) {
                        if (keys->active_key != keys->prelight_key) {
                            keys->send_key = keys->active_key;
                            keys->mk_send_note(p, &keys->send_key,false);
                            keys->active_key = keys->prelight_key;
                            keys->send_key = keys->active_key;
                            keys->mk_send_note(p, &keys->send_key,true);
                        }
                    }
                    catch = true;
                    expose_widget(w);
                    break;
                }
                space++;
                set++;
                set_key++;
            } else {
                if (set == 2) {
                    space = 0;
                    set = 0;
                } else if (set == 3) {
                    space = 1;
                    set = 0;
                }
            }
            i+=24;
            set_key++;
        }        
    }

    if (!catch) {
        int space = 2;
        int set = 0;
        int i = 0;
        int k = 0;

        for(;i<width;i++) {
            if(xmotion->x > i && xmotion->x < i+25) {
                keys->prelight_key = k+keys->octave;
                if(xmotion->state & Button1Mask) {
                    if (keys->active_key != keys->prelight_key) {
                        keys->send_key = keys->active_key;
                        keys->mk_send_note(p, &keys->send_key,false);
                        keys->active_key = keys->prelight_key;
                        keys->send_key = keys->active_key;
                        keys->mk_send_note(p, &keys->send_key,true);
                    }
                }
                expose_widget(w);
                break;
            }

            if (space!=4) {
                k++;
            } else {
                if (set <= 3) {
                    space = 0;
                    set = 0;
                } else if (set == 4) {
                    space = 1;
                    set = 0;
                }
            }

            i+=24;
            space++;
            set++;
            k++;
        }
    }
}

static void key_press(void *w_, void *key_, void *user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = w->parent;
    if (!w) return;
    MidiKeyboard *keys = (MidiKeyboard*)w->parent_struct;
    XKeyEvent *key = (XKeyEvent*)key_;
    if (!key) return;
    float outkey = 0.0;
    KeySym sym = XLookupKeysym (key, 0);
    switch(keys->layout) {
        case(0):keysym_qwertz_to_midi_key(sym, &outkey);
        break;
        case(1):keysym_qwerty_to_midi_key(sym, &outkey);
        break;
        case(2):keysym_azerty_to_midi_key(sym, &outkey);
        break;        
        default:keysym_qwertz_to_midi_key(sym, &outkey);
        break;
    }

    if ((int)outkey && !is_key_in_matrix(keys->key_matrix, (int)outkey+keys->octave)) {
        set_key_in_matrix(keys->key_matrix,(int)outkey+keys->octave,true);
        keys->send_key = (int)outkey+keys->octave;
        keys->mk_send_note(p, &keys->send_key,true);
        expose_widget(w);
    } 
    if (sym == XK_space) {
        clear_key_matrix(&keys->key_matrix[0]);
        keys->mk_send_all_sound_off(p, NULL);
        expose_widget(w);
    } 
}

static void key_release(void *w_, void *key_, void *user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = w->parent;
    if (!w) return;
    MidiKeyboard *keys = (MidiKeyboard*)w->parent_struct;
    XKeyEvent *key = (XKeyEvent*)key_;
    if (!key) return;
    float outkey = 0.0;
    KeySym sym = XLookupKeysym (key, 0);
    switch(keys->layout) {
        case(0):keysym_qwertz_to_midi_key(sym, &outkey);
        break;
        case(1):keysym_qwerty_to_midi_key(sym, &outkey);
        break;
        case(2):keysym_azerty_to_midi_key(sym, &outkey);
        break;
        default:keysym_qwertz_to_midi_key(sym, &outkey);
        break;
    }
    if ((int)outkey && is_key_in_matrix(keys->key_matrix, (int)outkey+keys->octave)) {
        set_key_in_matrix(keys->key_matrix,(int)outkey+keys->octave,false);
        keys->send_key = (int)outkey+keys->octave;
        keys->mk_send_note(p,&keys->send_key,false);
        expose_widget(w);
        if(!have_key_in_matrix(keys->key_matrix)) {
      //      adj_changed(w, GATE, 0.0);
            //keys->active_key = keys->prelight_key = -1;
        }
    }
}

static void leave_keyboard(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    MidiKeyboard *keys = (MidiKeyboard*)w->parent_struct;
    keys->prelight_key = -1;
    expose_widget(w);
}

static void button_pressed_keyboard(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = w->parent;
    if (w->flags & HAS_POINTER) {
        MidiKeyboard *keys = (MidiKeyboard*)w->parent_struct;
        XButtonEvent *xbutton = (XButtonEvent*)button_;
        if(xbutton->button == Button1) {
            keys->active_key = keys->prelight_key;
            keys->send_key = keys->active_key;
            keys->mk_send_note(p,&keys->send_key,true);
            expose_widget(w);
        }
    }
}

static void button_released_keyboard(void *w_, void* button_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = w->parent;
    if (w->flags & HAS_POINTER) {
        MidiKeyboard *keys = (MidiKeyboard*)w->parent_struct;
        XButtonEvent *xbutton = (XButtonEvent*)button_;
        if(xbutton->button == Button1) {
            keys->send_key = keys->active_key;
            keys->mk_send_note(p,&keys->send_key,false);
            keys->active_key = -1;
            expose_widget(w);
        }
    }
}

static void octave_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = w->parent;
    MidiKeyboard *keys = (MidiKeyboard*)p->parent_struct;
    keys->octave = (int)12*adj_get_value(w->adj);
}

static void layout_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = w->parent;
    MidiKeyboard *keys = (MidiKeyboard*)p->parent_struct;
    keys->layout = (int)adj_get_value(w->adj);
}

static void modwheel_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = w->parent;
    Widget_t *pa = p->parent;
    MidiKeyboard *keys = (MidiKeyboard*)p->parent_struct;
    keys->modwheel = (int)adj_get_value(w->adj);
    keys->mk_send_mod(pa, &keys->modwheel);
}

static void pitchwheel_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = w->parent;
    Widget_t *pa = p->parent;
    MidiKeyboard *keys = (MidiKeyboard*)p->parent_struct;
    keys->pitchwheel = (int)adj_get_value(w->adj);
    keys->mk_send_pitch(pa,&keys->pitchwheel);
}

static void pitchsensity_callback(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = w->parent;
    Widget_t *pa = p->parent;
    MidiKeyboard *keys = (MidiKeyboard*)p->parent_struct;
    keys->pitchsensity = (int)adj_get_value(w->adj);
    keys->mk_send_pitchsensity(pa,&keys->pitchsensity);
}

static void wheel_key_release(void *w_, void *key_, void *user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = w->parent;
    p->func.key_release_callback(p, key_, user_data);
}

static void wheel_key_press(void *w_, void *key_, void *user_data) {
    Widget_t *w = (Widget_t*)w_;
    Widget_t *p = w->parent;
    p->func.key_press_callback(p, key_, user_data);
}

/**
 * @brief keyboard_mem_free        - release additional used memory when destroy the Widget_t
 * @param *w_                      - void pointer to the Widget_t
 * @param *user_data               - void pointer to attached user_data
 * @return void
 */

static void keyboard_mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    MidiKeyboard *keys = (MidiKeyboard*)w->parent_struct;
    if(keys->icon) {
        XFreePixmap(w->app->dpy, (*keys->icon));
        keys->icon = NULL;
    }
    free(keys);
}

static void key_dummy(Widget_t *w,int *key, bool on_off) {
    //if (on_off)
    //fprintf(stderr, "send note on %i\n",(*key));
    //else
    //fprintf(stderr, "send note off %i\n",(*key));
}

static void wheel_dummy(Widget_t *w,int *value) {
    //fprintf(stderr, "send wheel value %i\n",(*value));
}

Widget_t *open_midi_keyboard(Widget_t *w) {
    Widget_t *wid = create_window(w->app, DefaultRootWindow(w->app->dpy), 0, 0, 700, 200);
    XSelectInput(wid->app->dpy, wid->widget,StructureNotifyMask|ExposureMask|KeyPressMask 
                    |EnterWindowMask|LeaveWindowMask|ButtonReleaseMask|KeyReleaseMask
                    |ButtonPressMask|Button1MotionMask|PointerMotionMask);
    MidiKeyboard *keys = (MidiKeyboard*)malloc(sizeof(MidiKeyboard));
    wid->parent_struct = keys;
    wid->parent = w;
    wid->flags |= HAS_MEM | NO_AUTOREPEAT;
    keys->prelight_key = -1;
    keys->active_key = -1;
    keys->send_key = -1;
    keys->octave = 12*2;
    keys->layout = 0;
    keys->modwheel = 64;
    keys->pitchwheel = 64;
    keys->pitchsensity = 64;
    keys->icon = NULL;
    int j = 0;
    for(;j<4;j++) {
        keys->key_matrix[j] = 0;
    }

    wid->func.expose_callback = draw_keyboard;
    wid->func.motion_callback = keyboard_motion;
    wid->func.leave_callback = leave_keyboard;
    wid->func.button_press_callback = button_pressed_keyboard;
    wid->func.button_release_callback = button_released_keyboard;
    wid->func.key_press_callback = key_press;
    wid->func.key_release_callback = key_release;
    wid->func.mem_free_callback = keyboard_mem_free;
    widget_set_icon_from_png(wid,keys->icon,LDVAR(midikeyboard_png));
    widget_set_title(wid, "Midi Keyboard");
    keys->mk_send_note = key_dummy;
    keys->mk_send_pitch = wheel_dummy;
    keys->mk_send_pitchsensity = wheel_dummy;
    keys->mk_send_mod = wheel_dummy;
    keys->mk_send_all_sound_off = wheel_dummy;

    Widget_t *b = add_hslider(wid, "Keyboard mapping", 10, 10, 160, 40);
    b->flags |= NO_AUTOREPEAT;
    set_adjustment(b->adj,2.0, 2.0, 0.0, 4.0, 1.0, CL_CONTINUOS);
    b->func.value_changed_callback = octave_callback;

    Widget_t *p = add_hslider(wid, "PitchWheel", 170, 10, 160, 40);
    p->flags |= NO_AUTOREPEAT;
    set_adjustment(p->adj,64.0, 64.0, 0.0, 127.0, 1.0, CL_CONTINUOS);
    p->func.value_changed_callback = pitchwheel_callback;
    p->func.key_press_callback = wheel_key_press;
    p->func.key_release_callback = wheel_key_release;
    
    Widget_t *s = add_hslider(wid, "PitchSensity", 330, 12, 90, 35);
    s->flags |= NO_AUTOREPEAT;
    set_adjustment(s->adj,64.0, 64.0, 0.0, 127.0, 1.0, CL_CONTINUOS);
    s->func.value_changed_callback = pitchsensity_callback;
    s->func.key_press_callback = wheel_key_press;
    s->func.key_release_callback = wheel_key_release;

    Widget_t *m = add_hslider(wid, "ModWheel", 420, 10, 160, 40);
    m->flags |= NO_AUTOREPEAT;
    set_adjustment(m->adj,64.0, 64.0, 0.0, 127.0, 1.0, CL_CONTINUOS);
    m->func.value_changed_callback = modwheel_callback;
    m->func.key_press_callback = wheel_key_press;
    m->func.key_release_callback = wheel_key_release;

    Widget_t *layout = add_combobox(wid, "", 590, 15, 90, 30);
    layout->flags |= NO_AUTOREPEAT;
    combobox_add_entry(layout,"qwertz");
    combobox_add_entry(layout,"qwerty");
    combobox_add_entry(layout,"azerty");
    combobox_set_active_entry(layout, 0);
    set_adjustment(layout->adj,0.0, 0.0, 0.0, 2.0, 1.0, CL_ENUM);
    layout->func.value_changed_callback = layout_callback;

 return wid;
}
