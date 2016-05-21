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
static void ntruenc_s112_mul_mod_q_small(int32_t *r, int32_t *a, int32_t *b)
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
        r[i] = t[i] % (NTRU_S112_Q);
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s112_mul_mod_q_14(int32_t *r, int32_t *a, int32_t *b)
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
    ntruenc_s112_mul_mod_q_small(t3, aa, bb);

    for (i=0; i<7; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s112_mul_mod_q_small(t2, aa, bb);

    ntruenc_s112_mul_mod_q_small(t1, a, b);

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
static void ntruenc_s112_mul_mod_q_28(int32_t *r, int32_t *a, int32_t *b)
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
    ntruenc_s112_mul_mod_q_14(t3, aa, bb);

    for (i=0; i<14; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s112_mul_mod_q_14(t2, aa, bb);

    ntruenc_s112_mul_mod_q_14(t1, a, b);

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
static void ntruenc_s112_mul_mod_q_55(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*28-1];
    int32_t t2[2*28-1];
    int32_t t3[2*28-1];
    int32_t aa[28];
    int32_t bb[28];

    for (i=0; i<27; i++)
    {
        aa[i] = a[i+28];
        bb[i] = b[i+28];
    }
    aa[27] = 0;
    bb[27] = 0;

    ntruenc_s112_mul_mod_q_28(t3, aa, bb);

    for (i=0; i<28; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s112_mul_mod_q_28(t2, aa, bb);

    ntruenc_s112_mul_mod_q_28(t1, a, b);

    t3[28*2-2] = 0;
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
static void ntruenc_s112_mul_mod_q_110(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*55-1];
    int32_t t2[2*55-1];
    int32_t t3[2*55-1];
    int32_t aa[55];
    int32_t bb[55];

    for (i=0; i<55; i++)
    {
        aa[i] = a[i+55];
        bb[i] = b[i+55];
    }
    ntruenc_s112_mul_mod_q_55(t3, aa, bb);

    for (i=0; i<55; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s112_mul_mod_q_55(t2, aa, bb);

    ntruenc_s112_mul_mod_q_55(t1, a, b);

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
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s112_mul_mod_q_220(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*110-1];
    int32_t t2[2*110-1];
    int32_t t3[2*110-1];
    int32_t aa[110];
    int32_t bb[110];

    for (i=0; i<110; i++)
    {
        aa[i] = a[i+110];
        bb[i] = b[i+110];
    }
    ntruenc_s112_mul_mod_q_110(t3, aa, bb);

    for (i=0; i<110; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s112_mul_mod_q_110(t2, aa, bb);

    ntruenc_s112_mul_mod_q_110(t1, a, b);

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
void ntruenc_s112_mul_mod_q(short *r, short *a, short *b)
{
    int i, j;
    int32_t t1[2*220-1];
    int32_t t2[2*220-1];
    int32_t t3[2*220-1];
    int32_t aa[220];
    int32_t bb[220];

    for (i=0; i<219; i++)
    {
        aa[i] = a[i+220];
        bb[i] = b[i+220];
    }
    aa[219] = 0;
    bb[219] = 0;

    ntruenc_s112_mul_mod_q_220(t3, aa, bb);

    for (i=0; i<220; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s112_mul_mod_q_220(t2, aa, bb);

    for (i=0; i<219; i++)
    {
        aa[i] = a[i];
        bb[i] = b[i];
    }
    ntruenc_s112_mul_mod_q_220(t1, aa, bb);

    r[0] = t1[0];
    for (i=1,j=0; i<439; i++,j++)
        r[i] = (t1[i] + t3[j]) % NTRU_S112_Q;
    for (i=220,j=0; i<439; i++,j++)
        r[i] = (r[i] + t2[j] - t1[j] - t3[j]) % NTRU_S112_Q;
    for (i=0; j<220*2-1; i++,j++)
        r[i] = (r[i] + t2[j] - t1[j] - t3[j]) % NTRU_S112_Q;

    for (i=0; i<439; i++)
    {
        if (r[i] > NTRU_S112_Q / 2)
            r[i] = -(NTRU_S112_Q - r[i]);
        if (r[i] < -NTRU_S112_Q / 2)
            r[i] = NTRU_S112_Q + r[i];
    }
}
#endif /* NTRUENC_SMALL_CODE */

