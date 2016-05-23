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
 * @param [in] t  Dynamicly allocated data used during life of function.
 */
static void ntruenc_s256_mul_mod_q_small(int64_t *r, int16_t *a, int16_t *b,
    void *t)
{
    int i, j;
    int64_t *p;

    for (j=0; j<28; j++)
        r[j] = (int32_t)a[0] * b[j];
    for (i=1; i<28; i++)
    {
        r[i+28-1] = 0;
        p = &r[i];
        for (j=0; j<28; j++)
            p[j] += (int32_t)a[i] * b[j];
    }
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
static void ntruenc_s256_mul_mod_q_56(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + (2*28+1);
    int64_t *t3 = t2 + (2*28+1);
    int64_t *t = t3 + (2*28+1);
    int16_t aa[28];
    int16_t bb[28];

    for (i=0; i<28; i++)
    {
        aa[i] = a[i+28];
        bb[i] = b[i+28];
    }
    ntruenc_s256_mul_mod_q_small(t3, aa, bb, t);

    for (i=0; i<28; i++)
    {
        aa[i] = (aa[i] + a[i]) % NTRU_S256_Q;
        bb[i] = (bb[i] + b[i]) % NTRU_S256_Q;
    }
    ntruenc_s256_mul_mod_q_small(t2, aa, bb, t);

    ntruenc_s256_mul_mod_q_small(t1, a, b, t);

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
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
static void ntruenc_s256_mul_mod_q_111(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + (2*56+1);
    int64_t *t3 = t2 + (2*56+1);
    int64_t *t = t3 + (2*56+1);
    int16_t aa[56];
    int16_t bb[56];

    for (i=0; i<55; i++)
    {
        aa[i] = a[i+56];
        bb[i] = b[i+56];
    }
    aa[55] = 0;
    bb[55] = 0;

    ntruenc_s256_mul_mod_q_56(t3, aa, bb, t);

    for (i=0; i<56; i++)
    {
        aa[i] = (aa[i] + a[i]);
        bb[i] = (bb[i] + b[i]);
    }
    ntruenc_s256_mul_mod_q_56(t2, aa, bb, t);

    ntruenc_s256_mul_mod_q_56(t1, a, b, t);

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
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
static void ntruenc_s256_mul_mod_q_221(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + (2*111+1);
    int64_t *t3 = t2 + (2*111+1);
    int64_t *t = t3 + (2*111+1);
    int16_t aa[111];
    int16_t bb[111];

    for (i=0; i<110; i++)
    {
        aa[i] = a[i+111];
        bb[i] = b[i+111];
    }
    aa[110] = 0;
    bb[110] = 0;

    ntruenc_s256_mul_mod_q_111(t3, aa, bb, t);

    for (i=0; i<111; i++)
    {
        aa[i] = (aa[i] + a[i]) % NTRU_S256_Q;
        bb[i] = (bb[i] + b[i]) % NTRU_S256_Q;
    }
    ntruenc_s256_mul_mod_q_111(t2, aa, bb, t);

    ntruenc_s256_mul_mod_q_111(t1, a, b, t);

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
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
static void ntruenc_s256_mul_mod_q_441(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + (2*221+1);
    int64_t *t3 = t2 + (2*221+1);
    int64_t *t = t3 + (2*221+1);
    int16_t aa[221];
    int16_t bb[221];

    for (i=0; i<220; i++)
    {
        aa[i] = a[i+221];
        bb[i] = b[i+221];
    }
    aa[220] = 0;
    bb[220] = 0;

    ntruenc_s256_mul_mod_q_221(t3, aa, bb, t);

    for (i=0; i<221; i++)
    {
        aa[i] = (aa[i] + a[i]);
        bb[i] = (bb[i] + b[i]);
    }
    ntruenc_s256_mul_mod_q_221(t2, aa, bb, t);

    ntruenc_s256_mul_mod_q_221(t1, a, b, t);

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
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
void ntruenc_s256_mul_mod_q(short *r, short *a, short *b,
    void *tp)
{
    int i, j, k;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + (2*441+1);
    int64_t *t3 = t2 + (2*441+1);
    int64_t *t = t3 + (2*441+1);
    int16_t aa[441];
    int16_t bb[441];

    for (i=0; i<440; i++)
    {
        aa[i] = a[i+441];
        bb[i] = b[i+441];
    }
    aa[440] = 0;
    bb[440] = 0;

    ntruenc_s256_mul_mod_q_441(t3, aa, bb, t);

    for (i=0; i<441; i++)
    {
        aa[i] = (aa[i] + a[i]) % NTRU_S256_Q;
        bb[i] = (bb[i] + b[i]) % NTRU_S256_Q;
    }
    ntruenc_s256_mul_mod_q_441(t2, aa, bb, t);

    ntruenc_s256_mul_mod_q_441(t1, a, b, t);

    k = 881-441;
    r[0] = (t1[0] + t2[k] - t1[k] - t3[k]) % NTRU_S256_Q;
    for (i=1,j=0,k++; i<441; i++,j++,k++)
        r[i] = (t1[i] + t3[j] + t2[k] - t1[k] - t3[k]) % NTRU_S256_Q;
    for (k=0; i<881; i++,j++,k++)
        r[i] = (t1[i] + t3[j] + t2[k] - t1[k] - t3[k]) % NTRU_S256_Q;

    for (i=0; i<881; i++)
    {
        if (r[i] > NTRU_S256_Q / 2)
            r[i] = -(NTRU_S256_Q - r[i]);
        if (r[i] < -NTRU_S256_Q / 2)
            r[i] = NTRU_S256_Q + r[i];
    }
}
#endif /* NTRUENC_SMALL_CODE */

