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
 * The parameters structure for NTRU.
 */
struct ntruenc_params_st
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
};

/**
 * The private key structure.
 */
struct ntruenc_priv_key_st
{
    /** NTRU parameters. */
    NTRUENC_PARAMS *params;
    /* Private key value. */
    short *f;
};

struct ntruenc_pub_key_st
{
    /** NTRU parameters. */
    NTRUENC_PARAMS *params;
    /* Public key value. */
    short *h;
};

#endif /* NTRUENC_KEY_LCL_H */

