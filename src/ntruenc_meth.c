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
 *
 */

#include <stdlib.h>
#include "ntruenc_lcl.h"

/**
 * NTRU Encrypt implementations.
 */
NTRUENC_METHS ntruenc_meths[] =
{
    /* Security strength 112 in C. */
    { 112, 0,
      1, 1, 2,
      ntruenc_s112_encrypt, ntruenc_s112_decrypt, ntruenc_s112_keygen },
    /* Security strength 128 in C. */
    { 128, 0,
      1, 1, 2,
      ntruenc_s128_encrypt, ntruenc_s128_decrypt, ntruenc_s128_keygen },
    /* Security strength 192 in C. */
    { 192, 0,
      1, 1, 2,
      ntruenc_s192_encrypt, ntruenc_s192_decrypt, ntruenc_s192_keygen },
    /* Security strength 256 in C. */
    { 256, 0,
      1, 1, 2,
      ntruenc_s256_encrypt, ntruenc_s256_decrypt, ntruenc_s256_keygen },
};
/**
 * The number of implementations.
 */
#define NTRUENC_METHS_LEN ((int)(sizeof(ntruenc_meths)/sizeof(*ntruenc_meths)))

/**
 * Retrieve the methods for the required security strength and flags.
 *
 * @param [in]  strength  The security strength required.
 * @param [in]  flags     The extra requirements on the methods to choose.
 * @param [out] meths     The method table matching the requirements.
 * @return  0 on table found.<br>
 *          1 otherwise.
 */
int ntruenc_meths_get(short strength, int flags, NTRUENC_METHS **meths)
{
    int ret = 0;
    int i;
    NTRUENC_METHS *m = NULL;

    /* Start at the strongest and return the weakest meeting requirements. */
    for (i=NTRUENC_METHS_LEN-1; i>=0; i--)
    {
        if ((ntruenc_meths[i].strength >= strength) &&
            ((ntruenc_meths[i].flags & flags) == flags))
        {
            m = &ntruenc_meths[i];
        }
    }
    *meths = m;

    if (m == NULL)
        ret = NTRU_ERR_NOT_FOUND;

    return ret;
}

