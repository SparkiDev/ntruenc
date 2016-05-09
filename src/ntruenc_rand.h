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

#include "string.h"
#include "random.h"

#if 1
#define NTRUENC_RANDOM_CONSTANT_TIME
#endif

int NTRUENC_RANDOM(short *a, int df1, int df2, short v)
{
#ifdef NTRUENC_RANDOM_CONSTANT_TIME
    int ret = 0;
    int i;
    unsigned short r[NTRU_N];
    unsigned short j;
    short t;

    /* Generate a large number of indices. */
    if (pseudo_random((unsigned char *)r, sizeof(r)) != 0)
    {
        ret = NTRU_ERR_RANDOM;
        goto end;
    }
    /* Put in the require number values: v and -v */
    for (i=0; i<df1; i++)
        a[i] = v;
    for (; i<df1+df2; i++)
        a[i] = NTRU_Q - v;
    /* Zero out the rest of the values. */
    memset(&a[i], 0, 2*(NTRU_N-df1-df2));

    /* Randomly mix the values. */
    for (i=NTRU_N-1; i>0; i--)
    {
        j = r[i] % (i+1);
        t = a[i];
        a[i] = a[j];
        a[j] = t;
    }

end:
    return ret;
#else
    int ret = 0;
    int i, j;
    unsigned short r[NTRU_DF*3];
    unsigned short k;

    memset(a, 0, sizeof(*a)*NTRU_N);
    /* Generate a large number of indices. */
    if (pseudo_random((unsigned char *)r, sizeof(r)) != 0)
    {
        ret = NTRU_ERR_RANDOM;
        goto end;
    }

    /* Fill vector with as many 1s as required. */
    for (i=0,j=0; j<df1; i++)
    {
        /* Generate more data if there is no more random data. */
        if (i == NTRU_DF*3)
        {
            /* Re-mill the buffer. */
            if (pseudo_random((unsigned char *)r, sizeof(r)) != 0)
            {
                ret = NTRU_ERR_RANDOM;
                goto end;
            }
            i = 0;
        }

        k = r[i] % NTRU_N;
        /* Check if the random index is unused as of yet. */
        if (a[k] == 0)
        {
            a[k] = v;
            j++;
        }
    }

    /* Fill vector with as many 2s (-1) as required. */
    for (i=0,j=0; j<df2; i++)
    {
        /* Generate more data if there is no more random data. */
        if (i == NTRU_DF*3)
        {
            /* Fill the buffer up so that little is wasted. */
            if (pseudo_random((unsigned char *)r, sizeof(*r)*(df2-j)*3) != 0)
            {
                ret = NTRU_ERR_RANDOM;
                goto end;
            }
            i = 0;
        }

        k = r[i] % NTRU_N;
        /* Check if the random index is unused as of yet. */
        if (a[k] == 0)
        {
            a[k] = NTRU_Q - v;
            j++;
        }
    }

end:
    return ret;
#endif
}

