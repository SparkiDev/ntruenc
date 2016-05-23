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
static void ntruenc_s112_mul_mod_q_small(int64_t *r, int16_t *a, int16_t *b,
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
static void ntruenc_s112_mul_mod_q_55(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + 2*28-1;
    int64_t *t3 = t2 + 2*28-1;
    int64_t *t = t3 + 2*28-1;
    int16_t aa[28];
    int16_t bb[28];

    for (i=0; i<27; i++)
    {
        aa[i] = a[i+28];
        bb[i] = b[i+28];
    }
    aa[27] = 0;
    bb[27] = 0;

    ntruenc_s112_mul_mod_q_small(t3, aa, bb, t);

    for (i=0; i<28; i++)
    {
        aa[i] = (aa[i] + a[i]) % NTRU_S112_Q;
        bb[i] = (bb[i] + b[i]) % NTRU_S112_Q;
    }
    ntruenc_s112_mul_mod_q_small(t2, aa, bb, t);

    ntruenc_s112_mul_mod_q_small(t1, a, b, t);

    for (i=0; i<28; i++)
        r[i] = t1[i];
    for (i=0; i<28-1; i++)
        r[i+28] = (t1[i+28] + t2[i] - t1[i] - t3[i]);
    r[28*2-1] = (t2[28-1] - t1[28-1] - t3[28-1]);
    for (i=0; i<28-1; i++)
        r[i+2*28] = (t2[i+28] - t1[i+28] - t3[i+28] + t3[i]);
    for (; i<28*2-3; i++)
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
static void ntruenc_s112_mul_mod_q_110(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + 2*55-1;
    int64_t *t3 = t2 + 2*55-1;
    int64_t *t = t3 + 2*55-1;
    int16_t aa[55];
    int16_t bb[55];

    for (i=0; i<55; i++)
    {
        aa[i] = a[i+55];
        bb[i] = b[i+55];
    }
    ntruenc_s112_mul_mod_q_55(t3, aa, bb, t);

    for (i=0; i<55; i++)
    {
        aa[i] = (aa[i] + a[i]);
        bb[i] = (bb[i] + b[i]);
    }
    ntruenc_s112_mul_mod_q_55(t2, aa, bb, t);

    ntruenc_s112_mul_mod_q_55(t1, a, b, t);

    for (i=0; i<55; i++)
        r[i] = t1[i];
    for (i=0; i<55-1; i++)
        r[i+55] = (t1[i+55] + t2[i] - t1[i] - t3[i]);
    r[55*2-1] = (t2[55-1] - t1[55-1] - t3[55-1]);
    for (i=0; i<55-1; i++)
        r[i+2*55] = (t2[i+55] - t1[i+55] - t3[i+55] + t3[i]);
    for (; i<55*2-1; i++)
        r[i+2*55] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
static void ntruenc_s112_mul_mod_q_220(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + 2*110-1;
    int64_t *t3 = t2 + 2*110-1;
    int64_t *t = t3 + 2*110-1;
    int16_t aa[110];
    int16_t bb[110];

    for (i=0; i<110; i++)
    {
        aa[i] = a[i+110];
        bb[i] = b[i+110];
    }
    ntruenc_s112_mul_mod_q_110(t3, aa, bb, t);

    for (i=0; i<110; i++)
    {
        aa[i] = (aa[i] + a[i]) % NTRU_S112_Q;
        bb[i] = (bb[i] + b[i]) % NTRU_S112_Q;
    }
    ntruenc_s112_mul_mod_q_110(t2, aa, bb, t);

    ntruenc_s112_mul_mod_q_110(t1, a, b, t);

    for (i=0; i<110; i++)
        r[i] = t1[i];
    for (i=0; i<110-1; i++)
        r[i+110] = (t1[i+110] + t2[i] - t1[i] - t3[i]);
    r[110*2-1] = (t2[110-1] - t1[110-1] - t3[110-1]);
    for (i=0; i<110-1; i++)
        r[i+2*110] = (t2[i+110] - t1[i+110] - t3[i+110] + t3[i]);
    for (; i<110*2-1; i++)
        r[i+2*110] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
void ntruenc_s112_mul_mod_q(short *r, short *a, short *b,
    void *tp)
{
    int i, j, k;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + 2*220-1;
    int64_t *t3 = t2 + 2*220-1;
    int64_t *t = t3 + 2*220-1;
    int16_t aa[220];
    int16_t bb[220];

    for (i=0; i<219; i++)
    {
        aa[i] = a[i+220];
        bb[i] = b[i+220];
    }
    aa[219] = 0;
    bb[219] = 0;

    ntruenc_s112_mul_mod_q_220(t3, aa, bb, t);

    for (i=0; i<220; i++)
    {
        aa[i] = (aa[i] + a[i]);
        bb[i] = (bb[i] + b[i]);
    }
    ntruenc_s112_mul_mod_q_220(t2, aa, bb, t);

    ntruenc_s112_mul_mod_q_220(t1, a, b, t);

    k = 439-220;
    r[0] = (t1[0] + t2[k] - t1[k] - t3[k]) % NTRU_S112_Q;
    for (i=1,j=0,k++; i<220; i++,j++,k++)
        r[i] = (t1[i] + t3[j] + t2[k] - t1[k] - t3[k]) % NTRU_S112_Q;
    for (k=0; i<439; i++,j++,k++)
        r[i] = (t1[i] + t3[j] + t2[k] - t1[k] - t3[k]) % NTRU_S112_Q;

    for (i=0; i<439; i++)
    {
        if (r[i] > NTRU_S112_Q / 2)
            r[i] = -(NTRU_S112_Q - r[i]);
        if (r[i] < -NTRU_S112_Q / 2)
            r[i] = NTRU_S112_Q + r[i];
    }
}
#endif /* NTRUENC_SMALL_CODE */

