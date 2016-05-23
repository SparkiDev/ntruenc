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
static void ntruenc_s192_mul_mod_q_small(short *r, short *a, short *b)
{
    int i, j;
    short *p;

    for (j=0; j<75; j++)
        r[j] = a[0] * b[j];
    for (i=1; i<75; i++)
    {
        r[i+75-1] = 0;
        p = &r[i];
        for (j=0; j<75; j++)
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
static void ntruenc_s192_mul_mod_q_149(short *r, short *a, short *b)
{
    int i;
    short t1[2*75-1];
    short t2[2*75-1];
    short t3[2*75-1];
    short aa[75];
    short bb[75];

    for (i=0; i<74; i++)
    {
        aa[i] = a[i+75];
        bb[i] = b[i+75];
    }
    aa[74] = 0;
    bb[74] = 0;

    ntruenc_s192_mul_mod_q_small(t3, aa, bb);

    for (i=0; i<75; i++)
    {
        aa[i] += a[i];
        bb[i] += b[i];
    }
    ntruenc_s192_mul_mod_q_small(t2, aa, bb);

    ntruenc_s192_mul_mod_q_small(t1, a, b);

    for (i=0; i<75; i++)
        r[i] = t1[i];
    for (i=0; i<75-1; i++)
        r[i+75] = (t1[i+75] + t2[i] - t1[i] - t3[i]);
    r[75*2-1] = (t2[75-1] - t1[75-1] - t3[75-1]);
    for (i=0; i<75-1; i++)
        r[i+2*75] = (t2[i+75] - t1[i+75] - t3[i+75] + t3[i]);
    for (; i<75*2-3; i++)
        r[i+2*75] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s192_mul_mod_q_297(short *r, short *a, short *b)
{
    int i;
    short t1[2*149-1];
    short t2[2*149-1];
    short t3[2*149-1];
    short aa[149];
    short bb[149];

    for (i=0; i<148; i++)
    {
        aa[i] = a[i+149];
        bb[i] = b[i+149];
    }
    aa[148] = 0;
    bb[148] = 0;

    ntruenc_s192_mul_mod_q_149(t3, aa, bb);

    for (i=0; i<149; i++)
    {
        aa[i] += a[i];
        bb[i] += b[i];
    }
    ntruenc_s192_mul_mod_q_149(t2, aa, bb);

    ntruenc_s192_mul_mod_q_149(t1, a, b);

    for (i=0; i<149; i++)
        r[i] = t1[i];
    for (i=0; i<149-1; i++)
        r[i+149] = (t1[i+149] + t2[i] - t1[i] - t3[i]);
    r[149*2-1] = (t2[149-1] - t1[149-1] - t3[149-1]);
    for (i=0; i<149-1; i++)
        r[i+2*149] = (t2[i+149] - t1[i+149] - t3[i+149] + t3[i]);
    for (; i<149*2-3; i++)
        r[i+2*149] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
void ntruenc_s192_mul_mod_q(short *r, short *a, short *b)
{
    int i, j, k;
    short t1[2*297-1];
    short t2[2*297-1];
    short t3[2*297-1];
    short aa[297];
    short bb[297];

    for (i=0; i<296; i++)
    {
        aa[i] = a[i+297];
        bb[i] = b[i+297];
    }
    aa[296] = 0;
    bb[296] = 0;

    ntruenc_s192_mul_mod_q_297(t3, aa, bb);

    for (i=0; i<297; i++)
    {
        aa[i] += a[i];
        bb[i] += b[i];
    }
    ntruenc_s192_mul_mod_q_297(t2, aa, bb);

    ntruenc_s192_mul_mod_q_297(t1, a, b);

    k = 593-297;
    r[0] = t1[0] + t2[k] - t1[k] - t3[k];
    for (i=1,j=0,k++; i<297; i++,j++,k++)
        r[i] = t1[i] + t3[j] + t2[k] - t1[k] - t3[k];
    for (k=0; i<593; i++,j++,k++)
        r[i] = t1[i] + t3[j] + t2[k] - t1[k] - t3[k];

    for (i=0; i<593; i++)
    {
        r[i] &= NTRU_S192_Q-1;
        r[i] |= 0 - (r[i] & (1<<(NTRU_S192_Q_BITS-1)));
    }
}
#endif /* NTRUENC_SMALL_CODE */

