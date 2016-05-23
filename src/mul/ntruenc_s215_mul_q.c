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
static void ntruenc_s215_mul_mod_q_small(int64_t *r, int16_t *a, int16_t *b,
    void *t)
{
    int i, j;
    int64_t *p;

    for (j=0; j<47; j++)
        r[j] = (int32_t)a[0] * b[j];
    for (i=1; i<47; i++)
    {
        r[i+47-1] = 0;
        p = &r[i];
        for (j=0; j<47; j++)
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
static void ntruenc_s215_mul_mod_q_93(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + (2*47+1);
    int64_t *t3 = t2 + (2*47+1);
    int64_t *t = t3 + (2*47+1);
    int16_t aa[47];
    int16_t bb[47];

    for (i=0; i<46; i++)
    {
        aa[i] = a[i+47];
        bb[i] = b[i+47];
    }
    aa[46] = 0;
    bb[46] = 0;

    ntruenc_s215_mul_mod_q_small(t3, aa, bb, t);

    for (i=0; i<47; i++)
    {
        aa[i] = (aa[i] + a[i]) % NTRU_S215_Q;
        bb[i] = (bb[i] + b[i]) % NTRU_S215_Q;
    }
    ntruenc_s215_mul_mod_q_small(t2, aa, bb, t);

    ntruenc_s215_mul_mod_q_small(t1, a, b, t);

    for (i=0; i<47; i++)
        r[i] = t1[i];
    for (i=0; i<47-1; i++)
        r[i+47] = (t1[i+47] + t2[i] - t1[i] - t3[i]);
    r[47*2-1] = (t2[47-1] - t1[47-1] - t3[47-1]);
    for (i=0; i<47-1; i++)
        r[i+2*47] = (t2[i+47] - t1[i+47] - t3[i+47] + t3[i]);
    for (; i<47*2-2; i++)
        r[i+2*47] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
static void ntruenc_s215_mul_mod_q_185(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + (2*93+1);
    int64_t *t3 = t2 + (2*93+1);
    int64_t *t = t3 + (2*93+1);
    int16_t aa[93];
    int16_t bb[93];

    for (i=0; i<92; i++)
    {
        aa[i] = a[i+93];
        bb[i] = b[i+93];
    }
    aa[92] = 0;
    bb[92] = 0;

    ntruenc_s215_mul_mod_q_93(t3, aa, bb, t);

    for (i=0; i<93; i++)
    {
        aa[i] = (aa[i] + a[i]) % NTRU_S215_Q;
        bb[i] = (bb[i] + b[i]) % NTRU_S215_Q;
    }
    ntruenc_s215_mul_mod_q_93(t2, aa, bb, t);

    ntruenc_s215_mul_mod_q_93(t1, a, b, t);

    for (i=0; i<93; i++)
        r[i] = t1[i];
    for (i=0; i<93-1; i++)
        r[i+93] = (t1[i+93] + t2[i] - t1[i] - t3[i]);
    r[93*2-1] = (t2[93-1] - t1[93-1] - t3[93-1]);
    for (i=0; i<93-1; i++)
        r[i+2*93] = (t2[i+93] - t1[i+93] - t3[i+93] + t3[i]);
    for (; i<93*2-2; i++)
        r[i+2*93] = t3[i];
}

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
static void ntruenc_s215_mul_mod_q_370(int64_t *r, int16_t *a, int16_t *b,
    void *tp)
{
    int i;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + (2*185+1);
    int64_t *t3 = t2 + (2*185+1);
    int64_t *t = t3 + (2*185+1);
    int16_t aa[185];
    int16_t bb[185];

    for (i=0; i<185; i++)
    {
        aa[i] = a[i+185];
        bb[i] = b[i+185];
    }
    ntruenc_s215_mul_mod_q_185(t3, aa, bb, t);

    for (i=0; i<185; i++)
    {
        aa[i] = (aa[i] + a[i]) % NTRU_S215_Q;
        bb[i] = (bb[i] + b[i]) % NTRU_S215_Q;
    }
    ntruenc_s215_mul_mod_q_185(t2, aa, bb, t);

    ntruenc_s215_mul_mod_q_185(t1, a, b, t);

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
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
void ntruenc_s215_mul_mod_q(short *r, short *a, short *b,
    void *tp)
{
    int i, j, k;
    int64_t *t1 = tp;
    int64_t *t2 = t1 + (2*370+1);
    int64_t *t3 = t2 + (2*370+1);
    int64_t *t = t3 + (2*370+1);
    int16_t aa[370];
    int16_t bb[370];

    for (i=0; i<369; i++)
    {
        aa[i] = a[i+370];
        bb[i] = b[i+370];
    }
    aa[369] = 0;
    bb[369] = 0;

    ntruenc_s215_mul_mod_q_370(t3, aa, bb, t);

    for (i=0; i<370; i++)
    {
        aa[i] = (aa[i] + a[i]) % NTRU_S215_Q;
        bb[i] = (bb[i] + b[i]) % NTRU_S215_Q;
    }
    ntruenc_s215_mul_mod_q_370(t2, aa, bb, t);

    ntruenc_s215_mul_mod_q_370(t1, a, b, t);

    k = 739-370;
    r[0] = (t1[0] + t2[k] - t1[k] - t3[k]) % NTRU_S215_Q;
    for (i=1,j=0,k++; i<370; i++,j++,k++)
        r[i] = (t1[i] + t3[j] + t2[k] - t1[k] - t3[k]) % NTRU_S215_Q;
    for (k=0; i<739; i++,j++,k++)
        r[i] = (t1[i] + t3[j] + t2[k] - t1[k] - t3[k]) % NTRU_S215_Q;

    for (i=0; i<739; i++)
    {
        if (r[i] > NTRU_S215_Q / 2)
            r[i] = -(NTRU_S215_Q - r[i]);
        if (r[i] < -NTRU_S215_Q / 2)
            r[i] = NTRU_S215_Q + r[i];
    }
}
#endif /* NTRUENC_SMALL_CODE */

