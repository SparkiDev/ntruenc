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
static void ntruenc_s192_mul_mod_q_small(int32_t *r, int32_t *a, int32_t *b)
{
    int i, j;
    int64_t *p;
    int64_t t[11*2];

    for (j=0; j<11; j++)
        t[j] = (int64_t)a[0] * b[j];
    for (i=1; i<11; i++)
    {
        t[i+11-1] = 0;
        p = &t[i];
        for (j=0; j<11; j++)
            p[j] += (int64_t)a[i] * b[j];
    }
    for (i=0; i<11*2-1; i++)
        r[i] = t[i] % (NTRU_S192_Q);
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s192_mul_mod_q_21(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*11-1];
    int32_t t2[2*11-1];
    int32_t t3[2*11-1];
    int32_t aa[11];
    int32_t bb[11];

    for (i=0; i<10; i++)
    {
        aa[i] = a[i+11];
        bb[i] = b[i+11];
    }
    aa[10] = 0;
    bb[10] = 0;

    ntruenc_s192_mul_mod_q_small(t3, aa, bb);

    for (i=0; i<11; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s192_mul_mod_q_small(t2, aa, bb);

    ntruenc_s192_mul_mod_q_small(t1, a, b);

    t3[11*2-2] = 0;
    for (i=0; i<11; i++)
        r[i] = t1[i];
    for (i=0; i<11-1; i++)
        r[i+11] = (t1[i+11] + t2[i] - t1[i] - t3[i]);
    r[11*2-1] = (t2[11-1] - t1[11-1] - t3[11-1]);
    for (i=0; i<11-1; i++)
        r[i+2*11] = (t2[i+11] - t1[i+11] - t3[i+11] + t3[i]);
    for (; i<11*2-1; i++)
        r[i+2*11] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s192_mul_mod_q_42(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*21-1];
    int32_t t2[2*21-1];
    int32_t t3[2*21-1];
    int32_t aa[21];
    int32_t bb[21];

    for (i=0; i<21; i++)
    {
        aa[i] = a[i+21];
        bb[i] = b[i+21];
    }
    ntruenc_s192_mul_mod_q_21(t3, aa, bb);

    for (i=0; i<21; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s192_mul_mod_q_21(t2, aa, bb);

    ntruenc_s192_mul_mod_q_21(t1, a, b);

    for (i=0; i<21; i++)
        r[i] = t1[i];
    for (i=0; i<21-1; i++)
        r[i+21] = (t1[i+21] + t2[i] - t1[i] - t3[i]);
    r[21*2-1] = (t2[21-1] - t1[21-1] - t3[21-1]);
    for (i=0; i<21-1; i++)
        r[i+2*21] = (t2[i+21] - t1[i+21] - t3[i+21] + t3[i]);
    for (; i<21*2-1; i++)
        r[i+2*21] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s192_mul_mod_q_83(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*42-1];
    int32_t t2[2*42-1];
    int32_t t3[2*42-1];
    int32_t aa[42];
    int32_t bb[42];

    for (i=0; i<41; i++)
    {
        aa[i] = a[i+42];
        bb[i] = b[i+42];
    }
    aa[41] = 0;
    bb[41] = 0;

    ntruenc_s192_mul_mod_q_42(t3, aa, bb);

    for (i=0; i<42; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s192_mul_mod_q_42(t2, aa, bb);

    ntruenc_s192_mul_mod_q_42(t1, a, b);

    t3[42*2-2] = 0;
    for (i=0; i<42; i++)
        r[i] = t1[i];
    for (i=0; i<42-1; i++)
        r[i+42] = (t1[i+42] + t2[i] - t1[i] - t3[i]);
    r[42*2-1] = (t2[42-1] - t1[42-1] - t3[42-1]);
    for (i=0; i<42-1; i++)
        r[i+2*42] = (t2[i+42] - t1[i+42] - t3[i+42] + t3[i]);
    for (; i<42*2-1; i++)
        r[i+2*42] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s192_mul_mod_q_165(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*83-1];
    int32_t t2[2*83-1];
    int32_t t3[2*83-1];
    int32_t aa[83];
    int32_t bb[83];

    for (i=0; i<82; i++)
    {
        aa[i] = a[i+83];
        bb[i] = b[i+83];
    }
    aa[82] = 0;
    bb[82] = 0;

    ntruenc_s192_mul_mod_q_83(t3, aa, bb);

    for (i=0; i<83; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s192_mul_mod_q_83(t2, aa, bb);

    ntruenc_s192_mul_mod_q_83(t1, a, b);

    t3[83*2-2] = 0;
    for (i=0; i<83; i++)
        r[i] = t1[i];
    for (i=0; i<83-1; i++)
        r[i+83] = (t1[i+83] + t2[i] - t1[i] - t3[i]);
    r[83*2-1] = (t2[83-1] - t1[83-1] - t3[83-1]);
    for (i=0; i<83-1; i++)
        r[i+2*83] = (t2[i+83] - t1[i+83] - t3[i+83] + t3[i]);
    for (; i<83*2-1; i++)
        r[i+2*83] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s192_mul_mod_q_330(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*165-1];
    int32_t t2[2*165-1];
    int32_t t3[2*165-1];
    int32_t aa[165];
    int32_t bb[165];

    for (i=0; i<165; i++)
    {
        aa[i] = a[i+165];
        bb[i] = b[i+165];
    }
    ntruenc_s192_mul_mod_q_165(t3, aa, bb);

    for (i=0; i<165; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s192_mul_mod_q_165(t2, aa, bb);

    ntruenc_s192_mul_mod_q_165(t1, a, b);

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
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
void ntruenc_s192_mul_mod_q(short *r, short *a, short *b)
{
    int i, j;
    int32_t t1[2*330-1];
    int32_t t2[2*330-1];
    int32_t t3[2*330-1];
    int32_t aa[330];
    int32_t bb[330];

    for (i=0; i<329; i++)
    {
        aa[i] = a[i+330];
        bb[i] = b[i+330];
    }
    aa[329] = 0;
    bb[329] = 0;

    ntruenc_s192_mul_mod_q_330(t3, aa, bb);

    for (i=0; i<330; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s192_mul_mod_q_330(t2, aa, bb);

    for (i=0; i<329; i++)
    {
        aa[i] = a[i];
        bb[i] = b[i];
    }
    ntruenc_s192_mul_mod_q_330(t1, aa, bb);

    r[0] = t1[0];
    for (i=1,j=0; i<659; i++,j++)
        r[i] = (t1[i] + t3[j]) % NTRU_S192_Q;
    for (i=330,j=0; i<659; i++,j++)
        r[i] = (r[i] + t2[j] - t1[j] - t3[j]) % NTRU_S192_Q;
    for (i=0; j<330*2-1; i++,j++)
        r[i] = (r[i] + t2[j] - t1[j] - t3[j]) % NTRU_S192_Q;

    for (i=0; i<659; i++)
    {
        if (r[i] > NTRU_S192_Q / 2)
            r[i] = -(NTRU_S192_Q - r[i]);
        if (r[i] < -NTRU_S192_Q / 2)
            r[i] = NTRU_S192_Q + r[i];
    }
}
#endif /* NTRUENC_SMALL_CODE */

