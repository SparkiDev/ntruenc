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
static void ntruenc_s112_mul_mod_q_small(short *r, short *a, short *b)
{
    int i, j;
    short *p;

    for (j=0; j<101; j++)
        r[j] = a[0] * b[j];
    for (i=1; i<101; i++)
    {
        r[i+101-1] = 0;
        p = &r[i];
        for (j=0; j<101; j++)
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
static void ntruenc_s112_mul_mod_q_201(short *r, short *a, short *b)
{
    int i;
    short t1[2*101-1];
    short t2[2*101-1];
    short t3[2*101-1];
    short aa[101];
    short bb[101];

    for (i=0; i<100; i++)
    {
        aa[i] = a[i+101];
        bb[i] = b[i+101];
    }
    aa[100] = 0;
    bb[100] = 0;

    ntruenc_s112_mul_mod_q_small(t3, aa, bb);

    for (i=0; i<101; i++)
    {
        aa[i] += a[i];
        bb[i] += b[i];
    }
    ntruenc_s112_mul_mod_q_small(t2, aa, bb);

    ntruenc_s112_mul_mod_q_small(t1, a, b);

    memset(r, 0, (2*201-1)*sizeof(*r));
    for (i=0; i<101*2-2; i++)
    {
        r[i] += t1[i];
        r[i+101] += t2[i] - t1[i] - t3[i];
        r[i+2*101] += t3[i];
    }
    r[101*2-2] += t1[101*2-2];
    r[101*2-2+101] += t2[101*2-2] - t1[101*2-2];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
void ntruenc_s112_mul_mod_q(short *r, short *a, short *b)
{
    int i, j;
    short t1[2*201-1];
    short t2[2*201-1];
    short t3[2*201-1];
    short aa[201];
    short bb[201];

    for (i=0; i<200; i++)
    {
        aa[i] = a[i+201];
        bb[i] = b[i+201];
    }
    aa[200] = 0;
    bb[200] = 0;

    ntruenc_s112_mul_mod_q_201(t3, aa, bb);

    for (i=0; i<201; i++)
    {
        aa[i] += a[i];
        bb[i] += b[i];
    }
    ntruenc_s112_mul_mod_q_201(t2, aa, bb);

    ntruenc_s112_mul_mod_q_201(t1, a, b);

    memcpy(r, t1, 401*sizeof(*r));
    for (i=201,j=0; i<401; i++,j++)
        r[i] += t2[j] - t1[j] - t3[j];
    for (i=0; j<201*2-1; i++,j++)
        r[i] += t2[j] - t1[j] - t3[j];
    for (i=1,j=0; i<401; i++,j++)
        r[i] += t3[j];

    for (i=0; i<401; i++)
    {
        r[i] &= NTRU_S112_Q-1;
        r[i] |= 0 - (r[i] & (1<<(NTRU_S112_Q_BITS-1)));
    }
}
#endif /* NTRUENC_SMALL_CODE */

