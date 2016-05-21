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
#include "ntruenc_lcl.h"

#ifndef NTRUENC_SMALL_CODE
/**
 * Simple multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s128_mul_mod_q_small(short *r, short *a, short *b)
{
    int i, j;
    short *p;

    for (j=0; j<110; j++)
        r[j] = a[0] * b[j];
    for (i=1; i<110; i++)
    {
        r[i+110-1] = 0;
        p = &r[i];
        for (j=0; j<110; j++)
            p[j] += a[i] * b[j];
    }
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s128_mul_mod_q_220(short *r, short *a, short *b)
{
    int i;
    short t1[2*110-1];
    short t2[2*110-1];
    short t3[2*110-1];
    short aa[110];
    short bb[110];

    for (i=0; i<110; i++)
    {
        aa[i] = a[i+110];
        bb[i] = b[i+110];
    }
    ntruenc_s128_mul_mod_q_small(t3, aa, bb);

    for (i=0; i<110; i++)
    {
        aa[i] += a[i];
        bb[i] += b[i];
    }
    ntruenc_s128_mul_mod_q_small(t2, aa, bb);

    ntruenc_s128_mul_mod_q_small(t1, a, b);

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
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
void ntruenc_s128_mul_mod_q(short *r, short *a, short *b)
{
    int i, j;
    short t1[2*220-1];
    short t2[2*220-1];
    short t3[2*220-1];
    short aa[220];
    short bb[220];

    for (i=0; i<219; i++)
    {
        aa[i] = a[i+220];
        bb[i] = b[i+220];
    }
    aa[219] = 0;
    bb[219] = 0;

    ntruenc_s128_mul_mod_q_220(t3, aa, bb);

    for (i=0; i<220; i++)
    {
        aa[i] += a[i];
        bb[i] += b[i];
    }
    ntruenc_s128_mul_mod_q_220(t2, aa, bb);

    ntruenc_s128_mul_mod_q_220(t1, a, b);

    r[0] = t1[0];
    for (i=1,j=0; i<439; i++,j++)
        r[i] = t1[i] + t3[j];
    for (i=220,j=0; i<439; i++,j++)
        r[i] += t2[j] - t1[j] - t3[j];
    for (i=0; j<220*2-1; i++,j++)
        r[i] += t2[j] - t1[j] - t3[j];

    for (i=0; i<439; i++)
    {
        r[i] &= NTRU_S128_Q-1;
        r[i] |= 0 - (r[i] & (1<<(NTRU_S128_Q_BITS-1)));
    }
}
#endif /* NTRUENC_SMALL_CODE */

