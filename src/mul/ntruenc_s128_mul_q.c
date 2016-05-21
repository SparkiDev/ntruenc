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
static void ntruenc_s128_mul_mod_q_small(int32_t *r, int32_t *a, int32_t *b)
{
    int i, j;
    int64_t *p;
    int64_t t[8*2];

    for (j=0; j<8; j++)
        t[j] = (int64_t)a[0] * b[j];
    for (i=1; i<8; i++)
    {
        t[i+8-1] = 0;
        p = &t[i];
        for (j=0; j<8; j++)
            p[j] += (int64_t)a[i] * b[j];
    }
    for (i=0; i<8*2-1; i++)
        r[i] = t[i] % (NTRU_S128_Q);
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s128_mul_mod_q_16(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*8-1];
    int32_t t2[2*8-1];
    int32_t t3[2*8-1];
    int32_t aa[8];
    int32_t bb[8];

    for (i=0; i<8; i++)
    {
        aa[i] = a[i+8];
        bb[i] = b[i+8];
    }
    ntruenc_s128_mul_mod_q_small(t3, aa, bb);

    for (i=0; i<8; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s128_mul_mod_q_small(t2, aa, bb);

    ntruenc_s128_mul_mod_q_small(t1, a, b);

    for (i=0; i<8; i++)
        r[i] = t1[i];
    for (i=0; i<8-1; i++)
        r[i+8] = (t1[i+8] + t2[i] - t1[i] - t3[i]);
    r[8*2-1] = (t2[8-1] - t1[8-1] - t3[8-1]);
    for (i=0; i<8-1; i++)
        r[i+2*8] = (t2[i+8] - t1[i+8] - t3[i+8] + t3[i]);
    for (; i<8*2-1; i++)
        r[i+2*8] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s128_mul_mod_q_31(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*16-1];
    int32_t t2[2*16-1];
    int32_t t3[2*16-1];
    int32_t aa[16];
    int32_t bb[16];

    for (i=0; i<15; i++)
    {
        aa[i] = a[i+16];
        bb[i] = b[i+16];
    }
    aa[15] = 0;
    bb[15] = 0;

    ntruenc_s128_mul_mod_q_16(t3, aa, bb);

    for (i=0; i<16; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s128_mul_mod_q_16(t2, aa, bb);

    ntruenc_s128_mul_mod_q_16(t1, a, b);

    t3[16*2-2] = 0;
    for (i=0; i<16; i++)
        r[i] = t1[i];
    for (i=0; i<16-1; i++)
        r[i+16] = (t1[i+16] + t2[i] - t1[i] - t3[i]);
    r[16*2-1] = (t2[16-1] - t1[16-1] - t3[16-1]);
    for (i=0; i<16-1; i++)
        r[i+2*16] = (t2[i+16] - t1[i+16] - t3[i+16] + t3[i]);
    for (; i<16*2-1; i++)
        r[i+2*16] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s128_mul_mod_q_62(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*31-1];
    int32_t t2[2*31-1];
    int32_t t3[2*31-1];
    int32_t aa[31];
    int32_t bb[31];

    for (i=0; i<31; i++)
    {
        aa[i] = a[i+31];
        bb[i] = b[i+31];
    }
    ntruenc_s128_mul_mod_q_31(t3, aa, bb);

    for (i=0; i<31; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s128_mul_mod_q_31(t2, aa, bb);

    ntruenc_s128_mul_mod_q_31(t1, a, b);

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
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s128_mul_mod_q_123(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*62-1];
    int32_t t2[2*62-1];
    int32_t t3[2*62-1];
    int32_t aa[62];
    int32_t bb[62];

    for (i=0; i<61; i++)
    {
        aa[i] = a[i+62];
        bb[i] = b[i+62];
    }
    aa[61] = 0;
    bb[61] = 0;

    ntruenc_s128_mul_mod_q_62(t3, aa, bb);

    for (i=0; i<62; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s128_mul_mod_q_62(t2, aa, bb);

    ntruenc_s128_mul_mod_q_62(t1, a, b);

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
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s128_mul_mod_q_246(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*123-1];
    int32_t t2[2*123-1];
    int32_t t3[2*123-1];
    int32_t aa[123];
    int32_t bb[123];

    for (i=0; i<123; i++)
    {
        aa[i] = a[i+123];
        bb[i] = b[i+123];
    }
    ntruenc_s128_mul_mod_q_123(t3, aa, bb);

    for (i=0; i<123; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s128_mul_mod_q_123(t2, aa, bb);

    ntruenc_s128_mul_mod_q_123(t1, a, b);

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
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
void ntruenc_s128_mul_mod_q(short *r, short *a, short *b)
{
    int i, j;
    int32_t t1[2*246-1];
    int32_t t2[2*246-1];
    int32_t t3[2*246-1];
    int32_t aa[246];
    int32_t bb[246];

    for (i=0; i<245; i++)
    {
        aa[i] = a[i+246];
        bb[i] = b[i+246];
    }
    aa[245] = 0;
    bb[245] = 0;

    ntruenc_s128_mul_mod_q_246(t3, aa, bb);

    for (i=0; i<246; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s128_mul_mod_q_246(t2, aa, bb);

    for (i=0; i<245; i++)
    {
        aa[i] = a[i];
        bb[i] = b[i];
    }
    ntruenc_s128_mul_mod_q_246(t1, aa, bb);

    r[0] = t1[0];
    for (i=1,j=0; i<491; i++,j++)
        r[i] = (t1[i] + t3[j]) % NTRU_S128_Q;
    for (i=246,j=0; i<491; i++,j++)
        r[i] = (r[i] + t2[j] - t1[j] - t3[j]) % NTRU_S128_Q;
    for (i=0; j<246*2-1; i++,j++)
        r[i] = (r[i] + t2[j] - t1[j] - t3[j]) % NTRU_S128_Q;

    for (i=0; i<491; i++)
    {
        if (r[i] > NTRU_S128_Q / 2)
            r[i] = -(NTRU_S128_Q - r[i]);
        if (r[i] < -NTRU_S128_Q / 2)
            r[i] = NTRU_S128_Q + r[i];
    }
}
#endif /* NTRUENC_SMALL_CODE */

