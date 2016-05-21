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
static void ntruenc_s215_mul_mod_q_small(int32_t *r, int32_t *a, int32_t *b)
{
    int i, j;
    int64_t *p;
    int64_t t[6*2];

    for (j=0; j<6; j++)
        t[j] = (int64_t)a[0] * b[j];
    for (i=1; i<6; i++)
    {
        t[i+6-1] = 0;
        p = &t[i];
        for (j=0; j<6; j++)
            p[j] += (int64_t)a[i] * b[j];
    }
    for (i=0; i<6*2-1; i++)
        r[i] = t[i] % (NTRU_S215_Q);
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s215_mul_mod_q_12(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*6-1];
    int32_t t2[2*6-1];
    int32_t t3[2*6-1];
    int32_t aa[6];
    int32_t bb[6];

    for (i=0; i<6; i++)
    {
        aa[i] = a[i+6];
        bb[i] = b[i+6];
    }
    ntruenc_s215_mul_mod_q_small(t3, aa, bb);

    for (i=0; i<6; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s215_mul_mod_q_small(t2, aa, bb);

    ntruenc_s215_mul_mod_q_small(t1, a, b);

    for (i=0; i<6; i++)
        r[i] = t1[i];
    for (i=0; i<6-1; i++)
        r[i+6] = (t1[i+6] + t2[i] - t1[i] - t3[i]);
    r[6*2-1] = (t2[6-1] - t1[6-1] - t3[6-1]);
    for (i=0; i<6-1; i++)
        r[i+2*6] = (t2[i+6] - t1[i+6] - t3[i+6] + t3[i]);
    for (; i<6*2-1; i++)
        r[i+2*6] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s215_mul_mod_q_24(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*12-1];
    int32_t t2[2*12-1];
    int32_t t3[2*12-1];
    int32_t aa[12];
    int32_t bb[12];

    for (i=0; i<12; i++)
    {
        aa[i] = a[i+12];
        bb[i] = b[i+12];
    }
    ntruenc_s215_mul_mod_q_12(t3, aa, bb);

    for (i=0; i<12; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s215_mul_mod_q_12(t2, aa, bb);

    ntruenc_s215_mul_mod_q_12(t1, a, b);

    for (i=0; i<12; i++)
        r[i] = t1[i];
    for (i=0; i<12-1; i++)
        r[i+12] = (t1[i+12] + t2[i] - t1[i] - t3[i]);
    r[12*2-1] = (t2[12-1] - t1[12-1] - t3[12-1]);
    for (i=0; i<12-1; i++)
        r[i+2*12] = (t2[i+12] - t1[i+12] - t3[i+12] + t3[i]);
    for (; i<12*2-1; i++)
        r[i+2*12] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s215_mul_mod_q_47(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*24-1];
    int32_t t2[2*24-1];
    int32_t t3[2*24-1];
    int32_t aa[24];
    int32_t bb[24];

    for (i=0; i<23; i++)
    {
        aa[i] = a[i+24];
        bb[i] = b[i+24];
    }
    aa[23] = 0;
    bb[23] = 0;

    ntruenc_s215_mul_mod_q_24(t3, aa, bb);

    for (i=0; i<24; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s215_mul_mod_q_24(t2, aa, bb);

    ntruenc_s215_mul_mod_q_24(t1, a, b);

    t3[24*2-2] = 0;
    for (i=0; i<24; i++)
        r[i] = t1[i];
    for (i=0; i<24-1; i++)
        r[i+24] = (t1[i+24] + t2[i] - t1[i] - t3[i]);
    r[24*2-1] = (t2[24-1] - t1[24-1] - t3[24-1]);
    for (i=0; i<24-1; i++)
        r[i+2*24] = (t2[i+24] - t1[i+24] - t3[i+24] + t3[i]);
    for (; i<24*2-1; i++)
        r[i+2*24] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s215_mul_mod_q_93(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*47-1];
    int32_t t2[2*47-1];
    int32_t t3[2*47-1];
    int32_t aa[47];
    int32_t bb[47];

    for (i=0; i<46; i++)
    {
        aa[i] = a[i+47];
        bb[i] = b[i+47];
    }
    aa[46] = 0;
    bb[46] = 0;

    ntruenc_s215_mul_mod_q_47(t3, aa, bb);

    for (i=0; i<47; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s215_mul_mod_q_47(t2, aa, bb);

    ntruenc_s215_mul_mod_q_47(t1, a, b);

    t3[47*2-2] = 0;
    for (i=0; i<47; i++)
        r[i] = t1[i];
    for (i=0; i<47-1; i++)
        r[i+47] = (t1[i+47] + t2[i] - t1[i] - t3[i]);
    r[47*2-1] = (t2[47-1] - t1[47-1] - t3[47-1]);
    for (i=0; i<47-1; i++)
        r[i+2*47] = (t2[i+47] - t1[i+47] - t3[i+47] + t3[i]);
    for (; i<47*2-1; i++)
        r[i+2*47] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
static void ntruenc_s215_mul_mod_q_185(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*93-1];
    int32_t t2[2*93-1];
    int32_t t3[2*93-1];
    int32_t aa[93];
    int32_t bb[93];

    for (i=0; i<92; i++)
    {
        aa[i] = a[i+93];
        bb[i] = b[i+93];
    }
    aa[92] = 0;
    bb[92] = 0;

    ntruenc_s215_mul_mod_q_93(t3, aa, bb);

    for (i=0; i<93; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s215_mul_mod_q_93(t2, aa, bb);

    ntruenc_s215_mul_mod_q_93(t1, a, b);

    t3[93*2-2] = 0;
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
static void ntruenc_s215_mul_mod_q_370(int32_t *r, int32_t *a, int32_t *b)
{
    int i;
    int32_t t1[2*185-1];
    int32_t t2[2*185-1];
    int32_t t3[2*185-1];
    int32_t aa[185];
    int32_t bb[185];

    for (i=0; i<185; i++)
    {
        aa[i] = a[i+185];
        bb[i] = b[i+185];
    }
    ntruenc_s215_mul_mod_q_185(t3, aa, bb);

    for (i=0; i<185; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s215_mul_mod_q_185(t2, aa, bb);

    ntruenc_s215_mul_mod_q_185(t1, a, b);

    for (i=0; i<185; i++)
        r[i] = t1[i];
    for (i=0; i<185-1; i++)
        r[i+185] = (t1[i+185] + t2[i] - t1[i] - t3[i]);
    r[185*2-1] = (t2[185-1] - t1[185-1] - t3[185-1]);
    for (i=0; i<185-1; i++)
        r[i+2*185] = (t2[i+185] - t1[i+185] - t3[i+185] + t3[i]);
    for (; i<185*2-1; i++)
        r[i+2*185] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
void ntruenc_s215_mul_mod_q(short *r, short *a, short *b)
{
    int i, j;
    int32_t t1[2*370-1];
    int32_t t2[2*370-1];
    int32_t t3[2*370-1];
    int32_t aa[370];
    int32_t bb[370];

    for (i=0; i<369; i++)
    {
        aa[i] = a[i+370];
        bb[i] = b[i+370];
    }
    aa[369] = 0;
    bb[369] = 0;

    ntruenc_s215_mul_mod_q_370(t3, aa, bb);

    for (i=0; i<370; i++)
    {
        aa[i] = aa[i] + a[i];
        bb[i] = bb[i] + b[i];
    }
    ntruenc_s215_mul_mod_q_370(t2, aa, bb);

    for (i=0; i<369; i++)
    {
        aa[i] = a[i];
        bb[i] = b[i];
    }
    ntruenc_s215_mul_mod_q_370(t1, aa, bb);

    r[0] = t1[0];
    for (i=1,j=0; i<739; i++,j++)
        r[i] = (t1[i] + t3[j]) % NTRU_S215_Q;
    for (i=370,j=0; i<739; i++,j++)
        r[i] = (r[i] + t2[j] - t1[j] - t3[j]) % NTRU_S215_Q;
    for (i=0; j<370*2-1; i++,j++)
        r[i] = (r[i] + t2[j] - t1[j] - t3[j]) % NTRU_S215_Q;

    for (i=0; i<739; i++)
    {
        if (r[i] > NTRU_S215_Q / 2)
            r[i] = -(NTRU_S215_Q - r[i]);
        if (r[i] < -NTRU_S215_Q / 2)
            r[i] = NTRU_S215_Q + r[i];
    }
}
#endif /* NTRUENC_SMALL_CODE */

