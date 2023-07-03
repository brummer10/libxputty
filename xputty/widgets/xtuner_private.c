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


#include "xtuner_private.h"


static const char *note_sharp[] = {"A","A#","B","C","C#","D","D#","E","F","F#","G","G#"};
static const char *note_fr[] = {"La","La#","Si","Do","Do#","Ré","Ré#","Mi","Fa","Fa#","Sol","Sol#"};
static const char *note_flat[] = {"A","Bb","B","C","Db","D","Eb","E","F","Gb","G","Ab"};
static const char *note_fr_flat[] = {"La","La♭","Si","Do","Do♭","Ré","Ré♭","Mi","Fa","Fa♭","Sol","Sol♭"};
static const char* note_19[19] = {"A","A♯","B♭","B","B♯","C","C♯","D♭","D","D♯","E♭","E","E♯","F","F♯","G♭","G","G♯","A♭"};
static const char* note_fr_19[19] = {"La","La♯","Si♭","Si","Si♯","Do","Do♯","Ré♭","Ré","Ré♯","Mi♭","Mi","Mi♯","Fa","Fa♯","Sol♭","Sol","Sol♯","La♭"};
static const char* note_24[24] = {"A","A¼♯","A♯","A¾♯","B","B¼♯","C","C¼♯","C♯","C¾♯","D","D¼♯","D♯","D¾♯","E","E¼♯","F","F¼♯","F♯","F¾♯","G","G¼♯","G♯","G¾♯"};
static const char* note_fr_24[24] = {"La","La¼♯","La♯","La¾♯","Si","Si¼♯","Do","Do¼♯","Do♯","Do¾♯","Ré","Ré¼♯","Ré♯","Ré¾♯","Mi","E¼♯","Fa","Fa¼♯","Fa♯","Fa¾♯","Sol","Sol¼♯","Sol♯","Sol¾♯"};
static const char* note_31[31] = {"A","B♭♭","A♯","B♭","A♯♯","B","C♭","B♯","C ","D♭♭","C♯","D♭","C♯♯","D","E♭♭","D♯","E♭","D♯♯","E","F♭","E♯","F","G♭♭","F♯","G♭","F♯♯","G","A♭♭","G♯","A♭","G♯♯"};
static const char* note_fr_31[31] = {"La","Si♭♭","La♯","Si♭","La♯♯","Si","Do♭","Si♯","Do ","Ré♭♭","Do♯","Ré♭","Do♯♯","Ré","Mi♭♭","Ré♯","Mi♭","Ré♯♯","Mi","Fa♭","Mi♯","Fa","Sol♭♭","Fa♯","Sol♭","Fa♯♯","Sol","La♭♭","Sol♯","La♭","Sol♯♯"};
static const char* note_53[53] = {"la","laa","lo","law","ta","teh","te","tu","tuh","ti","tih","to","taw","da","do","di","daw","ro","rih","ra","ru","ruh","reh","re ","ri","raw","ma","meh","me","mu","muh","mi","maa","mo","maw","fe","fa","fih","fu","fuh","fi","se","suh","su","sih","sol","si","saw","lo","leh","le","lu","luh"};
static const char *octave[] = {"0","1","2","3","4","5"," "};

static void _tuner_triangle(Widget_t *w, float posx, float posy, float width, float height)
{
    float h2 = height/2.0;
    cairo_move_to(w->crb, posx, posy-h2);
    if (width > 0) {
        cairo_curve_to(w->crb,posx, posy-h2, posx+10, posy, posx, posy+h2);
    } else {
        cairo_curve_to(w->crb,posx, posy-h2, posx-10, posy, posx, posy+h2);
    }
    cairo_curve_to(w->crb,posx, posy+h2, posx+width/2, posy+h2, posx+width, posy);
    cairo_curve_to(w->crb, posx+width, posy, posx+width/2, posy-h2, posx, posy-h2);
    cairo_fill(w->crb);
    cairo_new_path (w->crb);
}

static int _get_tuner_temperament(Widget_t *w) {
    XTuner *xt = (XTuner *)w->parent_struct;
    if((int)xt->temperament == 0) return 12;
    else if((int)xt->temperament == 1) return 19;
    else if((int)xt->temperament == 2) return 24;
    else if((int)xt->temperament == 3) return 31;
    else if((int)xt->temperament == 4) return 53;
    else return 12;
}

static const char **_get_note_set(Widget_t *w) {
    XTuner *xt = (XTuner *)w->parent_struct;
    if((int)xt->temperament == 0) {
        if (xt->lang == 1) return note_fr;
        return note_sharp;
    } else if((int)xt->temperament == 1) {
        if (xt->lang == 1) return note_fr_19;
        return note_19;
    } else if((int)xt->temperament == 2)  {
        if (xt->lang == 1) return note_fr_24;
        return note_24;
    } else if((int)xt->temperament == 3)  {
        if (xt->lang == 1) return note_fr_31;
        return note_31;
    } else if((int)xt->temperament == 4) return note_53;
    else {
        if (xt->lang == 1) return note_fr_flat;
        return note_flat;
    }
    
}

void _draw_tuner(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    if (!w) return;
    XTuner *xt = (XTuner *)w->parent_struct;
    XWindowAttributes attrs;
    XGetWindowAttributes(w->app->dpy, (Window)w->widget, &attrs);
    if (attrs.map_state != IsViewable) return;
    int width = attrs.width;
    int height = attrs.height;
    float x0 = width/2;
    float y0 = height/2;

    use_bg_color_scheme(w, NORMAL_);
    cairo_paint (w->crb);

    cairo_pattern_t* pat = cairo_pattern_create_linear (0, 0,x0, 0);
    cairo_pattern_set_extend(pat, CAIRO_EXTEND_REFLECT);
    cairo_pattern_add_color_stop_rgb (pat, 1, 0.1, 0.8, 0.1);
    cairo_pattern_add_color_stop_rgb (pat, 0.7, 0.8, 0.8, 0.1);
    cairo_pattern_add_color_stop_rgb (pat, 0, 1, 0.1, 0.1);

    cairo_set_source_rgb(w->crb,0.2,0.2,0.2);
    int i = 0;
    for (; i < width/20 +1; ++i) {
        cairo_rectangle(w->crb,(width/2)+i*10, 5, 5, 20);
        cairo_fill(w->crb);
    }
    for (; i >0; --i) {
        cairo_rectangle(w->crb,(width/2)-i*10, 5, 5, 20);
        cairo_fill(w->crb);
    }
    

    cairo_set_source_rgb(w->crb,0.2,0.2,0.2);
    _tuner_triangle(w, width/5.8,y0 , 25/w->scale.ascale, 20/w->scale.ascale);
    _tuner_triangle(w, width/1.2, y0, -25/w->scale.ascale, 20/w->scale.ascale);

    float value = adj_get_value(w->adj);
    float scale = -0.5;

    float fvis = _get_tuner_temperament(w) * (log2f(value/xt->ref_freq) + 4);
    float fvisr = round(fvis);
    int vis = fvisr;
    int indicate_oc = round((fvisr+xt->temp_adjust)/_get_tuner_temperament(w));
    const int octsz = sizeof(octave) / sizeof(octave[0]);
    if (indicate_oc < 0 || indicate_oc >= octsz) {
        // just safety, should not happen with current parameters
        // (pitch tracker output 23 .. 999 Hz)
        indicate_oc = octsz - 1;
    }

    scale = (fvis-vis) / 4;
    float cent = (scale * 10000) / 25;
    vis = vis % _get_tuner_temperament(w);
    if (vis < 0) {
        vis += _get_tuner_temperament(w);
    }
    
    if (value > 24.0 && value < 999.0) {
        if (scale < 0.004) {
            cairo_set_source_rgb(w->crb,0.80,0.79,0.01);
            _tuner_triangle(w, (width/5.8),y0 , 25/w->scale.ascale, 20/w->scale.ascale);
            _tuner_triangle(w, (width/5.8)-(300*scale)/w->scale.ascale,y0 , 25/w->scale.ascale, 20/w->scale.ascale);
            _tuner_triangle(w, (width/5.8)-(600*scale)/w->scale.ascale,y0 , 25/w->scale.ascale, 20/w->scale.ascale);
        }
        if (scale > -0.004) {
            cairo_set_source_rgb(w->crb,0.80,0.79,0.01);
            _tuner_triangle(w, (width/1.2), y0, -25/w->scale.ascale, 20/w->scale.ascale);
            _tuner_triangle(w, (width/1.2)-(300*scale)/w->scale.ascale, y0, -25/w->scale.ascale, 20/w->scale.ascale);
            _tuner_triangle(w, (width/1.2)-(600*scale)/w->scale.ascale, y0, -25/w->scale.ascale, 20/w->scale.ascale);
        }
    }
    // display note
    cairo_select_font_face (w->crb, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_source_rgba(w->crb, fabsf(scale)*2, 1-(scale*scale*4), 0.2,max(0,1-(fabsf(scale)*6)));
    cairo_set_font_size(w->crb, (w->app->big_font*2)/w->scale.ascale);
    cairo_text_extents_t extents;

    if (value > 24.0 && value < 999.0) {
        cairo_text_extents(w->crb,_get_note_set(w)[vis] , &extents);
        float c = (extents.width/2)+10.0;
        cairo_move_to(w->crb,x0-c , y0+10 );
        cairo_show_text(w->crb, _get_note_set(w)[vis]);
        cairo_set_font_size(w->crb, (w->app->small_font*2)/w->scale.ascale);
        cairo_move_to(w->crb,x0+c-10.0, y0+10+ extents.height/2 );
        cairo_show_text(w->crb, octave[indicate_oc]);

        char s[64];
        char sc[64];
        snprintf(s, 63, "%.2f Hz", value);
        use_text_color_scheme(w, NORMAL_);
        cairo_set_font_size (w->crb, w->app->big_font/w->scale.ascale);
        cairo_text_extents(w->crb,s , &extents);
        cairo_move_to (w->crb, width/1.1-extents.width, height-extents.height );
        cairo_show_text(w->crb, s);
        snprintf(sc, 63, "%.2f cent", cent);
        cairo_text_extents(w->crb,sc , &extents);
        cairo_move_to (w->crb, width/4.1-extents.width, height-extents.height );
        cairo_show_text(w->crb, sc);
    
        cairo_set_source(w->crb,pat);
        int m = 1000*scale;
        int n = 100*scale;
        int o = 500*scale;
        int p = 250*scale;
        //if (m==0 && xt->smove !=0) xt->move=width/20;
        xt->smove = m;
        xt->move +=m;
        static int cmove = 0;
        static int omove = 0;
        static int pmove = 0;
        cmove += n;
        omove += o;
        pmove += p;
        if(xt->move<-width/20) xt->move=width/20;
        else if(xt->move>width/20) xt->move=-width/20;
        if(cmove<-width/20) cmove=width/20;
        else if(cmove>width/20) cmove=-width/20;
        if(omove<-width/20) omove=width/20;
        else if(omove>width/20) omove=-width/20;
        if(pmove<-width/20) pmove=width/20;
        else if(pmove>width/20) pmove=-width/20;
        if (m==0) {
            if(xt->move<0) xt->move+=1;
            else if(xt->move>0) xt->move-=1;
        }
        if (n==0) {
            if(cmove<0) cmove+=1;
            else if(cmove>0) cmove-=1;
        }
        if (o==0) {
            if(omove<0) omove+=1;
            else if(omove>0) omove-=1;
        }
        if (p==0) {
            if(pmove<0) pmove+=1;
            else if(pmove>0) pmove-=1;
        }
        for (int i = 0; i < 4; ++i) {
            cairo_rectangle(w->crb,(width/2)-20 + (xt->move+i)*10, 5, 5, 5);
            cairo_rectangle(w->crb,(width/2)-20 + (omove+i)*10, 10, 5, 5);
            cairo_rectangle(w->crb,(width/2)-20 + (pmove+i)*10, 15, 5, 5);
            cairo_rectangle(w->crb,(width/2)-20 + (cmove+i)*10, 20, 5, 5);
            cairo_fill(w->crb);
        }
    } else {
        cairo_set_source_rgb(w->crb,0.2,0.2,0.2);

        cairo_set_font_size (w->crb, w->app->big_font/w->scale.ascale);
        cairo_text_extents(w->crb, "000.00 Hz", &extents);
        cairo_move_to (w->crb, width/1.1-extents.width, height-extents.height );
        cairo_show_text(w->crb, "000.00 Hz");

        cairo_text_extents(w->crb, "0.00 cent", &extents);
        cairo_move_to (w->crb, width/4.1-extents.width, height-extents.height );
        cairo_show_text(w->crb, "0.00 cent");        

        cairo_set_font_size(w->crb, (w->app->big_font*2)/w->scale.ascale);
        cairo_text_extents(w->crb,"#" , &extents);
        float c = (extents.width/2)+10.0;
        cairo_move_to(w->crb,x0-c , y0+10 );
        cairo_show_text(w->crb, "#");
        cairo_set_font_size(w->crb, (w->app->small_font*2)/w->scale.ascale);
        cairo_move_to(w->crb,x0+c-10.0, y0+10+ extents.height/2 );
        cairo_show_text(w->crb, "0");
    }
    cairo_pattern_destroy(pat);
}
