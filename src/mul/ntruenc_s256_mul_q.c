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
static void ntruenc_s256_mul_mod_q_small(short *r, short *a, short *b)
{
    int i, j;
    short *p;

    for (j=0; j<93; j++)
        r[j] = a[0] * b[j];
    for (i=1; i<93; i++)
    {
        r[i+93-1] = 0;
        p = &r[i];
        for (j=0; j<93; j++)
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
static void ntruenc_s256_mul_mod_q_186(short *r, short *a, short *b)
{
    int i;
    short t1[2*93-1];
    short t2[2*93-1];
    short t3[2*93-1];
    short aa[93];
    short bb[93];

    for (i=0; i<93; i++)
    {
        aa[i] = a[i+93];
        bb[i] = b[i+93];
    }
    ntruenc_s256_mul_mod_q_small(t3, aa, bb);

    for (i=0; i<93; i++)
    {
        aa[i] += a[i];
        bb[i] += b[i];
    }
    ntruenc_s256_mul_mod_q_small(t2, aa, bb);

    ntruenc_s256_mul_mod_q_small(t1, a, b);

    for (i=0; i<93; i++)
        r[i] = t1[i];
    for (i=0; i<93-1; i++)
        r[i+93] = (t1[i+93] + t2[i] - t1[i] - t3[i]);
    r[93*2-1] = (t2[93-1] - t1[93-1] - t3[93-1]);
    for (i=0; i<93-1; i++)
        r[i+2*93] = (t2[i+93] - t1[i+93] - t3[i+93] + t3[i]);
    for (; i<93*2-1; i++)
        r[i+2*93] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s256_mul_mod_q_372(short *r, short *a, short *b)
{
    int i;
    short t1[2*186-1];
    short t2[2*186-1];
    short t3[2*186-1];
    short aa[186];
    short bb[186];

    for (i=0; i<186; i++)
    {
        aa[i] = a[i+186];
        bb[i] = b[i+186];
    }
    ntruenc_s256_mul_mod_q_186(t3, aa, bb);

    for (i=0; i<186; i++)
    {
        aa[i] += a[i];
        bb[i] += b[i];
    }
    ntruenc_s256_mul_mod_q_186(t2, aa, bb);

    ntruenc_s256_mul_mod_q_186(t1, a, b);

    for (i=0; i<186; i++)
        r[i] = t1[i];
    for (i=0; i<186-1; i++)
        r[i+186] = (t1[i+186] + t2[i] - t1[i] - t3[i]);
    r[186*2-1] = (t2[186-1] - t1[186-1] - t3[186-1]);
    for (i=0; i<186-1; i++)
        r[i+2*186] = (t2[i+186] - t1[i+186] - t3[i+186] + t3[i]);
    for (; i<186*2-1; i++)
        r[i+2*186] = t3[i];
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
    int i, j, k;
    short t1[2*372-1];
    short t2[2*372-1];
    short t3[2*372-1];
    short aa[372];
    short bb[372];

    for (i=0; i<371; i++)
    {
        aa[i] = a[i+372];
        bb[i] = b[i+372];
    }
    aa[371] = 0;
    bb[371] = 0;

    ntruenc_s256_mul_mod_q_372(t3, aa, bb);

    for (i=0; i<372; i++)
    {
        aa[i] += a[i];
        bb[i] += b[i];
    }
    ntruenc_s256_mul_mod_q_372(t2, aa, bb);

    ntruenc_s256_mul_mod_q_372(t1, a, b);

    k = 743-372;
    r[0] = t1[0] + t2[k] - t1[k] - t3[k];
    for (i=1,j=0,k++; i<372; i++,j++,k++)
        r[i] = t1[i] + t3[j] + t2[k] - t1[k] - t3[k];
    for (k=0; i<743; i++,j++,k++)
        r[i] = t1[i] + t3[j] + t2[k] - t1[k] - t3[k];

    for (i=0; i<743; i++)
    {
        r[i] &= NTRU_S256_Q-1;
        r[i] |= 0 - (r[i] & (1<<(NTRU_S256_Q_BITS-1)));
    }
}
#endif /* NTRUENC_SMALL_CODE */

