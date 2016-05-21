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

#include <string.h>
#include <stdint.h>
#include "ntruenc_lcl.h"

#ifndef NTRUENC_SMALL_CODE
/**
 * Simple multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s256_mul_mod_q_small(int32_t *r, int32_t *a, int32_t *b)
{
    int i, j;
    int64_t *p;
    int64_t t[7*2];

    for (j=0; j<7; j++)
        t[j] = (int64_t)a[0] * b[j];
    for (i=1; i<7; i++)
    {
        t[i+7-1] = 0;
        p = &t[i];
        for (j=0; j<7; j++)
            p[j] += (int64_t)a[i] * b[j];
    }
    for (i=0; i<7*2-1; i++)
        r[i] = t[i] % (NTRU_S256_Q);
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s256_mul_mod_q_14(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*7-1];
    int32_t t2[2*7-1];
    int32_t t3[2*7-1];
    int32_t aa[7];
    int32_t bb[7];

    for (i=0; i<7; i++)
    {
        aa[i] = a[i+7];
        bb[i] = b[i+7];
    }
    ntruenc_s256_mul_mod_q_small(t3, aa, bb);

    for (i=0; i<7; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s256_mul_mod_q_small(t2, aa, bb);

    ntruenc_s256_mul_mod_q_small(t1, a, b);

    for (i=0; i<7; i++)
        r[i] = t1[i];
    for (i=0; i<7-1; i++)
        r[i+7] = (t1[i+7] + t2[i] - t1[i] - t3[i]);
    r[7*2-1] = (t2[7-1] - t1[7-1] - t3[7-1]);
    for (i=0; i<7-1; i++)
        r[i+2*7] = (t2[i+7] - t1[i+7] - t3[i+7] + t3[i]);
    for (; i<7*2-1; i++)
        r[i+2*7] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s256_mul_mod_q_28(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*14-1];
    int32_t t2[2*14-1];
    int32_t t3[2*14-1];
    int32_t aa[14];
    int32_t bb[14];

    for (i=0; i<14; i++)
    {
        aa[i] = a[i+14];
        bb[i] = b[i+14];
    }
    ntruenc_s256_mul_mod_q_14(t3, aa, bb);

    for (i=0; i<14; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s256_mul_mod_q_14(t2, aa, bb);

    ntruenc_s256_mul_mod_q_14(t1, a, b);

    for (i=0; i<14; i++)
        r[i] = t1[i];
    for (i=0; i<14-1; i++)
        r[i+14] = (t1[i+14] + t2[i] - t1[i] - t3[i]);
    r[14*2-1] = (t2[14-1] - t1[14-1] - t3[14-1]);
    for (i=0; i<14-1; i++)
        r[i+2*14] = (t2[i+14] - t1[i+14] - t3[i+14] + t3[i]);
    for (; i<14*2-1; i++)
        r[i+2*14] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s256_mul_mod_q_56(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*28-1];
    int32_t t2[2*28-1];
    int32_t t3[2*28-1];
    int32_t aa[28];
    int32_t bb[28];

    for (i=0; i<28; i++)
    {
        aa[i] = a[i+28];
        bb[i] = b[i+28];
    }
    ntruenc_s256_mul_mod_q_28(t3, aa, bb);

    for (i=0; i<28; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s256_mul_mod_q_28(t2, aa, bb);

    ntruenc_s256_mul_mod_q_28(t1, a, b);

    for (i=0; i<28; i++)
        r[i] = t1[i];
    for (i=0; i<28-1; i++)
        r[i+28] = (t1[i+28] + t2[i] - t1[i] - t3[i]);
    r[28*2-1] = (t2[28-1] - t1[28-1] - t3[28-1]);
    for (i=0; i<28-1; i++)
        r[i+2*28] = (t2[i+28] - t1[i+28] - t3[i+28] + t3[i]);
    for (; i<28*2-1; i++)
        r[i+2*28] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s256_mul_mod_q_111(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*56-1];
    int32_t t2[2*56-1];
    int32_t t3[2*56-1];
    int32_t aa[56];
    int32_t bb[56];

    for (i=0; i<55; i++)
    {
        aa[i] = a[i+56];
        bb[i] = b[i+56];
    }
    aa[55] = 0;
    bb[55] = 0;

    ntruenc_s256_mul_mod_q_56(t3, aa, bb);

    for (i=0; i<56; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s256_mul_mod_q_56(t2, aa, bb);

    ntruenc_s256_mul_mod_q_56(t1, a, b);

    t3[56*2-2] = 0;
    for (i=0; i<56; i++)
        r[i] = t1[i];
    for (i=0; i<56-1; i++)
        r[i+56] = (t1[i+56] + t2[i] - t1[i] - t3[i]);
    r[56*2-1] = (t2[56-1] - t1[56-1] - t3[56-1]);
    for (i=0; i<56-1; i++)
        r[i+2*56] = (t2[i+56] - t1[i+56] - t3[i+56] + t3[i]);
    for (; i<56*2-1; i++)
        r[i+2*56] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s256_mul_mod_q_221(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*111-1];
    int32_t t2[2*111-1];
    int32_t t3[2*111-1];
    int32_t aa[111];
    int32_t bb[111];

    for (i=0; i<110; i++)
    {
        aa[i] = a[i+111];
        bb[i] = b[i+111];
    }
    aa[110] = 0;
    bb[110] = 0;

    ntruenc_s256_mul_mod_q_111(t3, aa, bb);

    for (i=0; i<111; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s256_mul_mod_q_111(t2, aa, bb);

    ntruenc_s256_mul_mod_q_111(t1, a, b);

    t3[111*2-2] = 0;
    for (i=0; i<111; i++)
        r[i] = t1[i];
    for (i=0; i<111-1; i++)
        r[i+111] = (t1[i+111] + t2[i] - t1[i] - t3[i]);
    r[111*2-1] = (t2[111-1] - t1[111-1] - t3[111-1]);
    for (i=0; i<111-1; i++)
        r[i+2*111] = (t2[i+111] - t1[i+111] - t3[i+111] + t3[i]);
    for (; i<111*2-1; i++)
        r[i+2*111] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s256_mul_mod_q_441(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*221-1];
    int32_t t2[2*221-1];
    int32_t t3[2*221-1];
    int32_t aa[221];
    int32_t bb[221];

    for (i=0; i<220; i++)
    {
        aa[i] = a[i+221];
        bb[i] = b[i+221];
    }
    aa[220] = 0;
    bb[220] = 0;

    ntruenc_s256_mul_mod_q_221(t3, aa, bb);

    for (i=0; i<221; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s256_mul_mod_q_221(t2, aa, bb);

    ntruenc_s256_mul_mod_q_221(t1, a, b);

    t3[221*2-2] = 0;
    for (i=0; i<221; i++)
        r[i] = t1[i];
    for (i=0; i<221-1; i++)
        r[i+221] = (t1[i+221] + t2[i] - t1[i] - t3[i]);
    r[221*2-1] = (t2[221-1] - t1[221-1] - t3[221-1]);
    for (i=0; i<221-1; i++)
        r[i+2*221] = (t2[i+221] - t1[i+221] - t3[i+221] + t3[i]);
    for (; i<221*2-1; i++)
        r[i+2*221] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
void ntruenc_s256_mul_mod_q(short *r, short *a, short *b)
{
    int i, j;
    int32_t t1[2*441-1];
    int32_t t2[2*441-1];
    int32_t t3[2*441-1];
    int32_t aa[441];
    int32_t bb[441];

    for (i=0; i<440; i++)
    {
        aa[i] = a[i+441];
        bb[i] = b[i+441];
    }
    aa[440] = 0;
    bb[440] = 0;

    ntruenc_s256_mul_mod_q_441(t3, aa, bb);

    for (i=0; i<441; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s256_mul_mod_q_441(t2, aa, bb);

    for (i=0; i<440; i++)
    {
        aa[i] = a[i];
        bb[i] = b[i];
    }
    ntruenc_s256_mul_mod_q_441(t1, aa, bb);

    r[0] = t1[0];
    for (i=1,j=0; i<881; i++,j++)
        r[i] = (t1[i] + t3[j]) % NTRU_S256_Q;
    for (i=441,j=0; i<881; i++,j++)
        r[i] = (r[i] + t2[j] - t1[j] - t3[j]) % NTRU_S256_Q;
    for (i=0; j<441*2-1; i++,j++)
        r[i] = (r[i] + t2[j] - t1[j] - t3[j]) % NTRU_S256_Q;

    for (i=0; i<881; i++)
    {
        if (r[i] > NTRU_S256_Q / 2)
            r[i] = -(NTRU_S256_Q - r[i]);
        if (r[i] < -NTRU_S256_Q / 2)
            r[i] = NTRU_S256_Q + r[i];
    }
}
#endif /* NTRUENC_SMALL_CODE */

