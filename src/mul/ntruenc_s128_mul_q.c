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
static void ntruenc_s128_mul_mod_q_small(int64_t *r, int16_t *a, int16_t *b,
    void *t)
{
    int i, j;
    int64_t *p;

    for (j=0; j<31; j++)
        r[j] = (int32_t)a[0] * b[j];
    for (i=1; i<31; i++)
    {
        r[i+31-1] = 0;
        p = &r[i];
        for (j=0; j<31; j++)
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
static void ntruenc_s128_mul_mod_q_62(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + (2*31+1);
    int64_t *t3 = t2 + (2*31+1);
    int64_t *t = t3 + (2*31+1);
    int16_t aa[31];
    int16_t bb[31];

    for (i=0; i<31; i++)
    {
        aa[i] = a[i+31];
        bb[i] = b[i+31];
    }
    ntruenc_s128_mul_mod_q_small(t3, aa, bb, t);

    for (i=0; i<31; i++)
    {
        aa[i] = (aa[i] + a[i]) % NTRU_S128_Q;
        bb[i] = (bb[i] + b[i]) % NTRU_S128_Q;
    }
    ntruenc_s128_mul_mod_q_small(t2, aa, bb, t);

    ntruenc_s128_mul_mod_q_small(t1, a, b, t);

    for (i=0; i<31; i++)
        r[i] = t1[i];
    for (i=0; i<31-1; i++)
        r[i+31] = (t1[i+31] + t2[i] - t1[i] - t3[i]);
    r[31*2-1] = (t2[31-1] - t1[31-1] - t3[31-1]);
    for (i=0; i<31-1; i++)
        r[i+2*31] = (t2[i+31] - t1[i+31] - t3[i+31] + t3[i]);
    for (; i<31*2-1; i++)
        r[i+2*31] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
static void ntruenc_s128_mul_mod_q_123(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + (2*62+1);
    int64_t *t3 = t2 + (2*62+1);
    int64_t *t = t3 + (2*62+1);
    int16_t aa[62];
    int16_t bb[62];

    for (i=0; i<61; i++)
    {
        aa[i] = a[i+62];
        bb[i] = b[i+62];
    }
    aa[61] = 0;
    bb[61] = 0;

    ntruenc_s128_mul_mod_q_62(t3, aa, bb, t);

    for (i=0; i<62; i++)
    {
        aa[i] = (aa[i] + a[i]);
        bb[i] = (bb[i] + b[i]);
    }
    ntruenc_s128_mul_mod_q_62(t2, aa, bb, t);

    ntruenc_s128_mul_mod_q_62(t1, a, b, t);

    t3[62*2-2] = 0;
    for (i=0; i<62; i++)
        r[i] = t1[i];
    for (i=0; i<62-1; i++)
        r[i+62] = (t1[i+62] + t2[i] - t1[i] - t3[i]);
    r[62*2-1] = (t2[62-1] - t1[62-1] - t3[62-1]);
    for (i=0; i<62-1; i++)
        r[i+2*62] = (t2[i+62] - t1[i+62] - t3[i+62] + t3[i]);
    for (; i<62*2-1; i++)
        r[i+2*62] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
static void ntruenc_s128_mul_mod_q_246(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + (2*123+1);
    int64_t *t3 = t2 + (2*123+1);
    int64_t *t = t3 + (2*123+1);
    int16_t aa[123];
    int16_t bb[123];

    for (i=0; i<123; i++)
    {
        aa[i] = a[i+123];
        bb[i] = b[i+123];
    }
    ntruenc_s128_mul_mod_q_123(t3, aa, bb, t);

    for (i=0; i<123; i++)
    {
        aa[i] = (aa[i] + a[i]) % NTRU_S128_Q;
        bb[i] = (bb[i] + b[i]) % NTRU_S128_Q;
    }
    ntruenc_s128_mul_mod_q_123(t2, aa, bb, t);

    ntruenc_s128_mul_mod_q_123(t1, a, b, t);

    for (i=0; i<123; i++)
        r[i] = t1[i];
    for (i=0; i<123-1; i++)
        r[i+123] = (t1[i+123] + t2[i] - t1[i] - t3[i]);
    r[123*2-1] = (t2[123-1] - t1[123-1] - t3[123-1]);
    for (i=0; i<123-1; i++)
        r[i+2*123] = (t2[i+123] - t1[i+123] - t3[i+123] + t3[i]);
    for (; i<123*2-1; i++)
        r[i+2*123] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
void ntruenc_s128_mul_mod_q(short *r, short *a, short *b,
    void *tp)
{
    int i, j, k;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + (2*246+1);
    int64_t *t3 = t2 + (2*246+1);
    int64_t *t = t3 + (2*246+1);
    int16_t aa[246];
    int16_t bb[246];

    for (i=0; i<245; i++)
    {
        aa[i] = a[i+246];
        bb[i] = b[i+246];
    }
    aa[245] = 0;
    bb[245] = 0;

    ntruenc_s128_mul_mod_q_246(t3, aa, bb, t);

    for (i=0; i<246; i++)
    {
        aa[i] = (aa[i] + a[i]);
        bb[i] = (bb[i] + b[i]);
    }
    ntruenc_s128_mul_mod_q_246(t2, aa, bb, t);

    ntruenc_s128_mul_mod_q_246(t1, a, b, t);

    k = 491-246;
    r[0] = (t1[0] + t2[k] - t1[k] - t3[k]) % NTRU_S128_Q;
    for (i=1,j=0,k++; i<246; i++,j++,k++)
        r[i] = (t1[i] + t3[j] + t2[k] - t1[k] - t3[k]) % NTRU_S128_Q;
    for (k=0; i<491; i++,j++,k++)
        r[i] = (t1[i] + t3[j] + t2[k] - t1[k] - t3[k]) % NTRU_S128_Q;

    for (i=0; i<491; i++)
    {
        if (r[i] > NTRU_S128_Q / 2)
            r[i] = -(NTRU_S128_Q - r[i]);
        if (r[i] < -NTRU_S128_Q / 2)
            r[i] = NTRU_S128_Q + r[i];
    }
}
#endif /* NTRUENC_SMALL_CODE */

