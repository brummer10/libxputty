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


#include "xwaveview.h"
#include "xwaveview_private.h"


Widget_t* add_waveview(Widget_t *parent, const char * label,
                int x, int y, int width, int height) {

    Widget_t *wid = create_widget(parent->app, parent, x, y, width, height);
    wid->label = label;
    wid->flags |= HAS_MEM;
    wid->scale.gravity = CENTER;
    WaveView_t *wave_view;
    wave_view = (WaveView_t*)malloc(sizeof(WaveView_t));
    wave_view->wave = NULL;
    wave_view->size = 0;
    wid->private_struct = wave_view;
    wid->func.expose_callback = _draw_waveview;
    wid->func.mem_free_callback = waveview_mem_free;
    return wid;
}

void update_waveview(Widget_t *waveview, float* wave, int size) {
    WaveView_t *wave_view = (WaveView_t*)waveview->private_struct;
    if (size !=wave_view->size && size>1) {
        free(wave_view->wave);
        wave_view->wave = NULL;
        wave_view->size = size;
        wave_view->wave = (float*)malloc(size*sizeof(float));
    }
    if (wave_view->size>1) {
        memcpy(wave_view->wave, wave, size*sizeof(float));
        expose_widget(waveview);
    }
}

void waveview_mem_free(void *w_, void* user_data) {
    Widget_t *w = (Widget_t*)w_;
    WaveView_t *wave_view = (WaveView_t*)w->private_struct;
    free(wave_view->wave);
    free(wave_view);
}
