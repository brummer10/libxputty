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

#pragma once

#ifndef XMIDI_KEYBOARD_H_
#define XMIDI_KEYBOARD_H_

#include "xwidgets.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*mk_midikeyfunc)(Widget_t *w,int *key, bool on_off);
typedef void (*mk_midiwheelfunc)(Widget_t *w,int *value);

typedef struct {
    int octave;
    int layout;
    int modwheel;
    int detune;
    int attack;
    int sustain;
    int release;
    int volume;
    int velocity;
    int pitchwheel;
    int pitchsensity;
    int prelight_key;
    int active_key;
    int send_key;
    unsigned long key_matrix[4];

    mk_midikeyfunc mk_send_note;
    mk_midiwheelfunc mk_send_pitch;
    mk_midiwheelfunc mk_send_pitchsensity;
    mk_midiwheelfunc mk_send_mod;
    mk_midiwheelfunc mk_send_detune;
    mk_midiwheelfunc mk_send_attack;
    mk_midiwheelfunc mk_send_sustain;
    mk_midiwheelfunc mk_send_release;
    mk_midiwheelfunc mk_send_volume;
    mk_midiwheelfunc mk_send_velocity;
    mk_midiwheelfunc mk_send_all_sound_off;
} MidiKeyboard_mk;

void mk_keysym_azerty_to_midi_key(long inkey, float *midi_key);

void mk_keysym_qwertz_to_midi_key(long inkey, float *midi_key);

void mk_keysym_qwerty_to_midi_key(unsigned int inkey, float *midi_key);

bool mk_is_key_in_matrix(unsigned long *key_matrix, int key);

bool mk_have_key_in_matrix(unsigned long *key_matrix);

void mk_clear_key_matrix(unsigned long *key_matrix);

Widget_t *mk_open_midi_keyboard(Widget_t *w);

#ifdef __cplusplus
}
#endif

#endif //XMIDI_KEYBOARD_H_
