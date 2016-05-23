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
static void ntruenc_s192_mul_mod_q_small(int64_t *r, int16_t *a, int16_t *b,
    void *t)
{
    int i, j;
    int64_t *p;

    for (j=0; j<42; j++)
        r[j] = (int32_t)a[0] * b[j];
    for (i=1; i<42; i++)
    {
        r[i+42-1] = 0;
        p = &r[i];
        for (j=0; j<42; j++)
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
static void ntruenc_s192_mul_mod_q_83(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + 2*42-1;
    int64_t *t3 = t2 + 2*42-1;
    int64_t *t = t3 + 2*42-1;
    int16_t aa[42];
    int16_t bb[42];

    for (i=0; i<41; i++)
    {
        aa[i] = a[i+42];
        bb[i] = b[i+42];
    }
    aa[41] = 0;
    bb[41] = 0;

    ntruenc_s192_mul_mod_q_small(t3, aa, bb, t);

    for (i=0; i<42; i++)
    {
        aa[i] = (aa[i] + a[i]) % NTRU_S192_Q;
        bb[i] = (bb[i] + b[i]) % NTRU_S192_Q;
    }
    ntruenc_s192_mul_mod_q_small(t2, aa, bb, t);

    ntruenc_s192_mul_mod_q_small(t1, a, b, t);

    for (i=0; i<42; i++)
        r[i] = t1[i];
    for (i=0; i<42-1; i++)
        r[i+42] = (t1[i+42] + t2[i] - t1[i] - t3[i]);
    r[42*2-1] = (t2[42-1] - t1[42-1] - t3[42-1]);
    for (i=0; i<42-1; i++)
        r[i+2*42] = (t2[i+42] - t1[i+42] - t3[i+42] + t3[i]);
    for (; i<42*2-3; i++)
        r[i+2*42] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
static void ntruenc_s192_mul_mod_q_165(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + 2*83-1;
    int64_t *t3 = t2 + 2*83-1;
    int64_t *t = t3 + 2*83-1;
    int16_t aa[83];
    int16_t bb[83];

    for (i=0; i<82; i++)
    {
        aa[i] = a[i+83];
        bb[i] = b[i+83];
    }
    aa[82] = 0;
    bb[82] = 0;

    ntruenc_s192_mul_mod_q_83(t3, aa, bb, t);

    for (i=0; i<83; i++)
    {
        aa[i] = (aa[i] + a[i]);
        bb[i] = (bb[i] + b[i]);
    }
    ntruenc_s192_mul_mod_q_83(t2, aa, bb, t);

    ntruenc_s192_mul_mod_q_83(t1, a, b, t);

    for (i=0; i<83; i++)
        r[i] = t1[i];
    for (i=0; i<83-1; i++)
        r[i+83] = (t1[i+83] + t2[i] - t1[i] - t3[i]);
    r[83*2-1] = (t2[83-1] - t1[83-1] - t3[83-1]);
    for (i=0; i<83-1; i++)
        r[i+2*83] = (t2[i+83] - t1[i+83] - t3[i+83] + t3[i]);
    for (; i<83*2-3; i++)
        r[i+2*83] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
static void ntruenc_s192_mul_mod_q_330(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + 2*165-1;
    int64_t *t3 = t2 + 2*165-1;
    int64_t *t = t3 + 2*165-1;
    int16_t aa[165];
    int16_t bb[165];

    for (i=0; i<165; i++)
    {
        aa[i] = a[i+165];
        bb[i] = b[i+165];
    }
    ntruenc_s192_mul_mod_q_165(t3, aa, bb, t);

    for (i=0; i<165; i++)
    {
        aa[i] = (aa[i] + a[i]) % NTRU_S192_Q;
        bb[i] = (bb[i] + b[i]) % NTRU_S192_Q;
    }
    ntruenc_s192_mul_mod_q_165(t2, aa, bb, t);

    ntruenc_s192_mul_mod_q_165(t1, a, b, t);

    for (i=0; i<165; i++)
        r[i] = t1[i];
    for (i=0; i<165-1; i++)
        r[i+165] = (t1[i+165] + t2[i] - t1[i] - t3[i]);
    r[165*2-1] = (t2[165-1] - t1[165-1] - t3[165-1]);
    for (i=0; i<165-1; i++)
        r[i+2*165] = (t2[i+165] - t1[i+165] - t3[i+165] + t3[i]);
    for (; i<165*2-1; i++)
        r[i+2*165] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
void ntruenc_s192_mul_mod_q(short *r, short *a, short *b,
    void *tp)
{
    int i, j, k;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + 2*330-1;
    int64_t *t3 = t2 + 2*330-1;
    int64_t *t = t3 + 2*330-1;
    int16_t aa[330];
    int16_t bb[330];

    for (i=0; i<329; i++)
    {
        aa[i] = a[i+330];
        bb[i] = b[i+330];
    }
    aa[329] = 0;
    bb[329] = 0;

    ntruenc_s192_mul_mod_q_330(t3, aa, bb, t);

    for (i=0; i<330; i++)
    {
        aa[i] = (aa[i] + a[i]);
        bb[i] = (bb[i] + b[i]);
    }
    ntruenc_s192_mul_mod_q_330(t2, aa, bb, t);

    ntruenc_s192_mul_mod_q_330(t1, a, b, t);

    k = 659-330;
    r[0] = (t1[0] + t2[k] - t1[k] - t3[k]) % NTRU_S192_Q;
    for (i=1,j=0,k++; i<330; i++,j++,k++)
        r[i] = (t1[i] + t3[j] + t2[k] - t1[k] - t3[k]) % NTRU_S192_Q;
    for (k=0; i<659; i++,j++,k++)
        r[i] = (t1[i] + t3[j] + t2[k] - t1[k] - t3[k]) % NTRU_S192_Q;

    for (i=0; i<659; i++)
    {
        if (r[i] > NTRU_S192_Q / 2)
            r[i] = -(NTRU_S192_Q - r[i]);
        if (r[i] < -NTRU_S192_Q / 2)
            r[i] = NTRU_S192_Q + r[i];
    }
}
#endif /* NTRUENC_SMALL_CODE */

