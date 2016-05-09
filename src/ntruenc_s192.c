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
 */

#include "ntruenc_lcl.h"

#define NTRU_N			NTRU_S192_N
#define NTRU_DF			NTRU_S192_DF
#define NTRU_DG			NTRU_S192_DG
#define NTRU_Q			NTRU_S192_Q
#define NTRU_Q_BITS		NTRU_S192_Q_BITS
#define NTRUENC_KEYGEN		ntruenc_s192_keygen
#define NTRUENC_ENCRYPT		ntruenc_s192_encrypt
#define NTRUENC_DECRYPT		ntruenc_s192_decrypt
#define NTRUENC_MOD_INV_2	ntruenc_s192_mod_inv_2
#define NTRUENC_MOD_INV_Q	ntruenc_s192_mod_inv_q
#define NTRUENC_MUL_MOD_Q	ntruenc_s192_mul_mod_q
#define NTRUENC_RANDOM		ntruenc_s192_random

#include "ntruenc_inv.h"
#include "ntruenc_rand.h"
#include "ntruenc_alg.h"

