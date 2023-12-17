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

#ifndef XB64_ENCODE_H_
#define XB64_ENCODE_H_

#include "xputty.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief b64_encode             - encode a b64 based char
 * @param *clrstr                - the char to encode
 * @param *b64dst                - the encoded char
 */

void b64_encode(char *clrstr, char *b64dst);

/**
 * @brief b64_decode             - decode to a b64 based char
 * @param *b64src                - the char to decode
 * @param *clrdst                - the decoded char
 */

void b64_decode(char *b64src, char *clrdst);

#ifdef __cplusplus
}
#endif

#endif //XB64_ENCODE_H_
