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

#ifndef NTRUENC_KEY_H
#define NTRUENC_KEY_H

/** Private key data type.  */
typedef struct ntruenc_priv_key_st NTRUENC_PRIV_KEY;
/** Public key data type.  */
typedef struct ntruenc_pub_key_st NTRUENC_PUB_KEY;

int NTRUENC_PRIV_KEY_new(short strength, int flags, NTRUENC_PRIV_KEY **key);
int NTRUENC_PRIV_KEY_init(NTRUENC_PRIV_KEY *key, short strength, int flags);
void NTRUENC_PRIV_KEY_final(NTRUENC_PRIV_KEY *key);
void NTRUENC_PRIV_KEY_free(NTRUENC_PRIV_KEY *key);
int NTRUENC_PRIV_KEY_get_len(NTRUENC_PRIV_KEY *key, int *len);

int NTRUENC_PUB_KEY_new(short strength, int flags, NTRUENC_PUB_KEY **key);
int NTRUENC_PUB_KEY_init(NTRUENC_PUB_KEY *key, short strength, int flags);
void NTRUENC_PUB_KEY_final(NTRUENC_PUB_KEY *key);
void NTRUENC_PUB_KEY_free(NTRUENC_PUB_KEY *key);
int NTRUENC_PUB_KEY_get_len(NTRUENC_PUB_KEY *key, int *len);
int NTRUENC_PUB_KEY_get_enc_len(NTRUENC_PUB_KEY *key, int *len);

#endif

