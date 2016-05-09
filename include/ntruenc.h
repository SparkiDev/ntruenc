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

#ifndef NTRUENC_H
#define NTRUENC_H

#include "ntruenc_key.h"

/* Error codes */
/** Failed to find the requested data. */
#define NTRU_ERR_NOT_FOUND	1
/** The data passed in is invalid for the operation. */
#define NTRU_ERR_BAD_DATA	2
/** The length passed in is invalid for the operation. */
#define NTRU_ERR_BAD_LEN	3
/** A function call parameter is NULL when this is not valid. */
#define NTRU_ERR_PARAM_NULL	4
/** The object was not initialized. */
#define NTRU_ERR_INIT		10
/** Failure to allocate dynamic memory. */
#define NTRU_ERR_ALLOC		20
/** Failed to generate require random data. */
#define NTRU_ERR_RANDOM		30
/** The operation failed to find an inverse value. */
#define NTRU_ERR_NO_INVERSE	31

typedef struct ntruenc_st NTRUENC;

int NTRUENC_new(int strength, int flags, NTRUENC **ne);
int NTRUENC_init(NTRUENC *ne, int strength, int flags);
void NTRUENC_final(NTRUENC *ne);
void NTRUENC_free(NTRUENC *ne);

int NTRUENC_encrypt_init(NTRUENC *ne, NTRUENC_PUB_KEY *pub);
int NTRUENC_encrypt(NTRUENC *ne, unsigned char *data, int len,
    unsigned char *enc, int elen);
void NTRUENC_encrypt_final(NTRUENC *ne);

int NTRUENC_decrypt_init(NTRUENC *ne, NTRUENC_PRIV_KEY *priv);
int NTRUENC_decrypt(NTRUENC *ne, unsigned char *enc, int elen,
    unsigned char *data, int len, int *olen);
void NTRUENC_decrypt_final(NTRUENC *ne);

int NTRUENC_keygen_init(NTRUENC *ne, NTRUENC_PARAMS *params);
int NTRUENC_keygen(NTRUENC *ne, NTRUENC_PRIV_KEY **priv, NTRUENC_PUB_KEY **pub);
void NTRUENC_keygen_final(NTRUENC *ne);

#endif

