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

/**
 * The data required for each pair of values.
 */
typedef struct ntruenc_mod_inv_st
{
    /* The data for the inverse value. */
    short bd[2*NTRU_N+1];
    /* The pointer to the start of the inverse value data. */
    short *b;
    /* The index of the end of the inverse value. */
    int be;

    /* The data for the f value. */
    short fd[NTRU_N+1];
    /* The pointer to start of the f value data. */
    short *f;
    /* The index of the start of the f value. */
    int fs;
} NTRUENC_MOD_INV;

int NTRUENC_MOD_INV_2(short *r, short *a)
{
    int i, j, k, d;
    NTRUENC_MOD_INV mi[2];
    NTRUENC_MOD_INV *m0 = &mi[0];
    NTRUENC_MOD_INV *m1 = &mi[1];
    NTRUENC_MOD_INV *tm;

    /* Set first inverse value to: 1 */
    m0->b = &m0->bd[NTRU_N];
    memset(m0->bd, 0, sizeof(*m0->bd)*NTRU_N);
    m0->b[0] = 1;
    m0->be = 0;

    /* Set second inverse value to: 0 */
    m1->b = &m1->bd[NTRU_N];
    memset(m1->bd, 0, sizeof(*m1->bd)*NTRU_N);
    m1->b[0] = 0;
    m1->be = 0;

    /* Set first f value to: input mod 2 */
    m0->f = &m0->fd[0];
    /* Start at offset 1 so that it ends at index NTRU_N. */
    for (i=0; i<NTRU_N; i++)
        m0->f[i+1] = a[i] & 1;
    m0->fs = 1;

    /* Set second value to: 0 (-1,...,1) */
    m1->f = &m1->fd[0];
    memset(m1->f, 0, sizeof(m1->fd));
    m1->f[0] = 1;
    m1->f[NTRU_N] = 1;
    m1->fs = 0;

    k = 0;
    while (1)
    {
        /* Find first non-zero element. */
        for (i=m0->fs; i<=NTRU_N && m0->f[i] == 0; i++) ;
        if (i == NTRU_N+1)
            return NTRU_ERR_NO_INVERSE;

        d = i - m0->fs;
        if (d > 0)
        {
            k += d;
            /* Move start position to index of first non-zero element. */
            m0->fs = i;
            /* Prepend other b a number of zeros. */
            m1->b -= d;
            /* Update end index. */
            m1->be += d;
        }

        /* Check whether we have one element of 1 - finished. */
        if ((m0->fs == NTRU_N) && (m0->f[NTRU_N] == 1))
            break;

        /* Make current index be the longer of the two - start is smaller. */
        if (m0->fs > m1->fs)
        {
            tm = m0;
            m0 = m1;
            m1 = tm;
        }

        /* Add the two. */
        for (i=m1->fs,j=m0->fs; i<=NTRU_N; i++,j++)
            m0->f[j] = (m0->f[j] ^ m1->f[i]) & 1;

        /* If the same length then we may have zero(s) at the end. */
        if ((m0->fs == m1->fs) && (m0->f[NTRU_N] == 0))
        {
            /* Find last non-zero element. */
            for (i=NTRU_N-1; i>=m0->fs && m0->f[i] == 0; i--) ;
            if (i < m0->fs)
                return NTRU_ERR_NO_INVERSE;
            /* Move pointer back and start index forward so last element's
             * index is at NTRU_N.
             */
            d = NTRU_N - i;
            m0->f -= d;
            m0->fs += d;
        }

        /* Append zeros to ensure the first is as long as the second. */
        for (i=m0->be+1; i<=m1->be; i++)
            m0->b[i] = 0;
        /* Update end index. */
        m0->be = i-1;

        /* Add the two. */
        for (i=0; i<=m1->be; i++)
            m0->b[i] = (m0->b[i] ^ m1->b[i]) & 1;

        /* If the same length then we may have zero(s) at the end. */
        if ((m0->be == m1->be) && (m0->b[m0->be] == 0))
        {
            /* Find last non-zero element. */
            for (i=m0->be-1; i>=0 && m0->b[i] == 0; i--) ;
            m0->be = i+1;
        }
    }

    /* Zero out the rest of the vector. */
    for (i=m0->be+1; i<=NTRU_N; i++)
        m0->b[i] = 0;
    /* The start of the result is: 0..NTRU_N-1 */
    if (k > NTRU_N) k -= NTRU_N;

    /* Move the values in but wrt the fact that there are NTRU_N+1 values. */
    for (i=k,j=0; i<NTRU_N; i++,j++)
        r[j] = m0->b[i];
    r[j] = (m0->b[0] ^ m0->b[NTRU_N]) & 1;
    j++;
    for (i=1; i<k; i++,j++)
        r[j] = m0->b[i];

    return 0;
}

