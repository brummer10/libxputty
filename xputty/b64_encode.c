
/* ------------------------------------------------------------------------ *
 * file:        base64_stringencode.c v1.0                                  *
 * purpose:     tests encoding/decoding strings with base64                 *
 * author:      02/23/2009 Frank4DD                                         *
 *                                                                          *
 * source:      http://base64.sourceforge.net/b64.c for encoding            *
 *              http://en.literateprograms.org/Base64_(C) for decoding      *
 * ------------------------------------------------------------------------ */

#include <stdio.h>
#include <string.h>

#include "b64_encode.h"

/* ---- Base64 Encoding/Decoding Table --- */
char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* decodeblock - decode 4 '6-bit' characters into 3 8-bit binary bytes */
void decodeblock(unsigned char in[], unsigned char **clrstr) {
     *((*clrstr) ++) = in[0] << 2 | in[1] >> 4;
     *((*clrstr) ++) = in[1] << 4 | in[2] >> 2;
     *((*clrstr) ++) = in[2] << 6 | in[3] >> 0;
}

void b64_decode(char *b64src, char *clrdst) {
    int c, phase, i;
    unsigned char in[4];
    char *p;

    clrdst[0] = '\0';
    phase = 0;
    i=0;
    while(b64src[i]) {
        c = (int) b64src[i];
        if(c == '=') {
            decodeblock(in, (unsigned char **) &clrdst);
            break;
        }
        p = strchr(b64, c);
        if(p) {
            in[phase] = p - b64;
            phase = (phase + 1) % 4;
            if(phase == 0) {
                decodeblock(in, (unsigned char **) &clrdst);
                in[0]=in[1]=in[2]=in[3]=0;
            }
        }
        i++;
    }
}

/* encodeblock - encode 3 8-bit binary bytes as 4 '6-bit' characters */
void encodeblock( unsigned char in[], char b64str[], int len ) {
    unsigned char out[5];
    out[0] = b64[ in[0] >> 2 ];
    out[1] = b64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? b64[ ((in[1] & 0x0f) << 2) |
                              ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? b64[ in[2] & 0x3f ] : '=');
    out[4] = '\0';
    strncat(b64str, (char*)out, sizeof(char)*5);
}

/* encode - base64 encode a stream, adding padding if needed */
void b64_encode(char *clrstr, char *b64dst) {
    unsigned char in[3];
    int i, len = 0;
    int j = 0;

    b64dst[0] = '\0';
    while(clrstr[j]) {
        len = 0;
        for(i=0; i<3; i++) {
            in[i] = (unsigned char) clrstr[j];
            if(clrstr[j]) {
                len++;
                j++;
            }
            else in[i] = 0;
        }
        if( len ) {
            encodeblock( in, b64dst, len );
        }
    }
}
