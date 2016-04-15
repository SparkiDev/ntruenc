/*
 * Copyright (c) 2016 Sean Parkinson (sparkinson@iprimus.com.au)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef NTRUENC_KEY_LCL_H
#define NTRUENC_KEY_LCL_H

#include "ntruenc_key.h"

/**
 * The method table for NTRU Encryption operations.
 */
typedef struct ntruenc_meths_st
{
    /** The maximum security strength supported by implementation. */
    int strength;
    /** Flags describing implementation. */
    int flags;
    /** Number of NTRU vectors required for encryption by implementation. */
    char enc_num;
    /** Number of NTRU vectors required for decryption by implementation. */
    char dec_num;
    /** Number of NTRU vectors required for key generation by implementation. */
    char keygen_num;
    /** Function to perform encryption. */
    int (*enc)(short *e, short *m, short *h, short *t);
    /** Function to perform decryption. */
    void (*dec)(short *c, short *e, short *f, short *t);
    /** Function to perform key generation. */
    int (*keygen)(short *f, short *h, short *t);
} NTRUENC_METHS;

/**
 * The parameters structure for NTRU.
 */
typedef struct ntruenc_params_st
{
    /** The maximum security strength supported by parameters. */
    short strength;
    /** The number of elements in a NTRU vector. */
    short n;
    /** The number of -1 elements in a random NTRU vector mod p. */
    short df;
    /** The number of -1 elements in a random NTRU vector mod p in g. */
    short dg;
    /** The larger modulus value. */
    short q;
} NTRUENC_PARAMS;

/**
 * The private key structure.
 */
struct ntruenc_priv_key_st
{
    /** NTRU parameters. */
    NTRUENC_PARAMS *params;
    /** Implementation of NTRU Encryption.  */
    NTRUENC_METHS *meths;
    /* Private key value. */
    short *f;
    /* Temprorary dynamicly allocated data. */
    short *t;
};

struct ntruenc_pub_key_st
{
    /** NTRU parameters. */
    NTRUENC_PARAMS *params;
    /** Implementation of NTRU Encryption.  */
    NTRUENC_METHS *meths;
    /* Public key value. */
    short *h;
    /* Temprorary dynamicly allocated data. */
    short *t;
};

#endif /* NTRUENC_KEY_LCL_H */

