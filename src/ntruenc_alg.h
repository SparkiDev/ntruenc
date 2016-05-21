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

/**
 * The data needed to convert a number mod 3 to a number in the range: -1..1.
 */
static short neg_mod_3_data[5] = { 1, -1, 0, 1, -1 };
/**
 * The variable used to convert a number mod 3 to a number in the range: -1..1.
 */
static short *neg_mod_3 = &neg_mod_3_data[2];

/**
 * Makes number modulo 3 in the range: -1..1.
 *
 * @param [in] a  The number to make modulo 3.
 * @return  The number modulo 3 in the range: -1..1.
 */
#define ntruenc_neg_mod_3(a)	(neg_mod_3[(a) % 3])

#ifdef NTRUENC_SMALL_CODE
/**
 * Multiply to NTRU vectors.
 *
 * @param [in] r  The result of the multiplication.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
void NTRUENC_MUL_MOD_Q(short *r, short *a, short *b)
{
    int i, j;
    int64_t *p;
    int64_t t[NTRU_N*2];

    for (j=0; j<NTRU_N; j++)
        t[j] = (int32_t)a[0] * b[j];
    for (i=1; i<NTRU_N; i++)
    {
        t[i+NTRU_N-1] = 0;
        p = &t[i];
        for (j=0; j<NTRU_N; j++)
            p[j] += (int32_t)a[i] * b[j];
    }
    for (i=0; i<NTRU_N-1; i++)
    {
        r[i] = (t[i] + t[i+NTRU_N]) % NTRU_Q;
        if (r[i] > NTRU_Q / 2)
            r[i] = -(NTRU_Q - r[i]);
        if (r[i] < -NTRU_Q / 2)
            r[i] = NTRU_Q + r[i];
    }
    r[NTRU_N-1] = t[NTRU_N-1] % NTRU_Q;
    if (r[NTRU_N-1] > NTRU_Q / 2)
        r[NTRU_N-1] = -(NTRU_Q - r[NTRU_N-1]);
    if (r[NTRU_N-1] < -NTRU_Q / 2)
        r[NTRU_N-1] = NTRU_Q + r[NTRU_N-1];
}
#endif

/**
 * Generate public and private key values.
 *   f' = (random vector * p) mod q
 *   f  = 1 + p.f' mod q
 *   t  = f^-1 mod q
 *   g  = (random vector mod q) * p
 *   h  = g.t
 *
 * @param [in] f  The random private value f.
 * @param [in] h  The public value h.
 * @param [in] t  The temprorary buffer to use in generation.
 * @return  NTRU_ERR_RANDOM if generating random fails.<br>
 *          NTRU_ERR_NO_INVERSE if the f has no inverse.<br>
 *          0 on successful generation of a key pair.
 */
int NTRUENC_KEYGEN(short *f, short *h, short *t)
{
    int ret;
    short *g = &t[NTRU_N];

    ret = NTRUENC_RANDOM(f, NTRU_DF, NTRU_DF, 3);
    if (ret != 0) return ret;

    f[0] += 1;

    ret = NTRUENC_MOD_INV_Q(t, f);
    if (ret != 0) return ret;

    ret = NTRUENC_RANDOM(g, NTRU_DG, NTRU_DG, 3);
    if (ret != 0) return ret;

    NTRUENC_MUL_MOD_Q(h, t, g);

    return 0;
}

/**
 * Generate an encryption of the encoded message or key using the public value.
 *
 * @param [in] e  The encrypted value.
 * @param [in] m  The endocode message or key.
 * @param [in] h  The public vlaue.
 * @param [in] t  The temporary buffer to use in generation.
 * @return  NTRU_ERR_RANDOM if generating random fails.<br>
 *          0 on successful generation of a key pair.
 */
int NTRUENC_ENCRYPT(short *e, short *m, short *h, short *t)
{
    int ret;
    int i;

    ret = NTRUENC_RANDOM(t, NTRU_DF, NTRU_DF, 1);
    if (ret != 0) return ret;

    NTRUENC_MUL_MOD_Q(e, t, h);
    /* Add in message/key and ensure the values are in the right range. */
    for (i=0; i<NTRU_N; i++)
    {
        e[i] = (e[i] + m[i]) % NTRU_Q;
        if (e[i] > NTRU_Q / 2)
            e[i] = -(NTRU_Q - e[i]);
        if (e[i] < -NTRU_Q / 2)
            e[i] = NTRU_Q + e[i];
    }

    return 0;
}

/**
 * Decrypt the message/key using the private value.
 *
 * @param [in] c  The decrypted message/key.
 * @param [in] e  The encrypted value.
 * @param [in] f  The private vlaue.
 * @param [in] t  The temporary buffer to use in calculations.
 */
void NTRUENC_DECRYPT(short *c, short *e, short *f, short *t)
{
    int i;

    NTRUENC_MUL_MOD_Q(c, f, e);
    /* Calculate mod p to isolate the message/key. */
    for (i=0; i<NTRU_N; i++)
        c[i] = ntruenc_neg_mod_3(c[i]);
}

