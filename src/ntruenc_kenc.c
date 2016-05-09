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
#include <string.h>
#include "ntruenc_lcl.h"
#include "ntruenc_key_lcl.h"

/**
 * Encodes an NTRU vector as packed 12-bit data.
 *
 * @param [in]  a     The NTRU vector.
 * @param [in]  n     The number of elements in the NTRU vector.
 * @param [in]  data  The buffer to hold encoded data.
 * @param [out] len   The number of bytes of encoded data.
 */
static void ntruenc_encode_12bits(short *a, int n, unsigned char *data,
    int *len)
{
    int i, j;

    for (i=0,j=0; i<(n/2)*2; i+=2,j+=3)
    {
        data[j+0] =                           ((a[i+0]        ) << 0);
        data[j+1] = ((a[i+0] & 0xf00) >> 8) | ((a[i+1] & 0x00f) << 4);
        data[j+2] = ((a[i+1] & 0xff0) >> 4);
    }
    if ((n & 1) == 1)
    {
        data[j+0] =                           ((a[i+0] & 0x0ff) << 0);
        data[j+1] = ((a[i+0] & 0xf00) >> 8);
        j += 2;
    }
    if (len != NULL)
        *len = j;
}

/**
 * Decodes packed 12-bit data into an NTRU vector.
 *
 * @param [in]  data  The buffer holding the encoded data.
 * @param [in]  n     The number of elements in the NTRU vector.
 * @param [in]  a     The NTRU vector.
 * @param [out] len   The number of bytes of encoded data.
 */
static void ntruenc_decode_12bits(unsigned char *data, int n, short *a,
    int *len)
{
    int i, j;

    for (i=0,j=0; i<(n/2)*2; i+=2,j+=3)
    {
        a[i+0] = ((data[j+0]       ) >> 0) | ((short)(data[j+1] & 0x0f) << 8);
        a[i+1] = ((data[j+1] & 0xf0) >> 4) | ((short)(data[j+2]       ) << 4);
    }
    if ((n & 1) == 1)
    {
        a[i+0] = ((data[j+0]       ) >> 0) | ((short)(data[j+1] & 0x0f) << 8);
        j += 2;
    }
    if (len != NULL)
        *len = j;
}

/**
 * Retrieves the length of an encoded private key.
 *
 * @param [in]  key  The private key object.
 * @param [out] len  The length of an encoded private key.
 * @return  NTRU_ERR_PARAM_NULL when a calling parameter is NULL.<br>
 *          0 otherwise.
 */
int NTRUENC_PRIV_KEY_get_len(NTRUENC_PRIV_KEY *key, int *len)
{
    int ret = 0;
    int n = key->params->n;

    if ((key == NULL) || (len == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    *len = (n*12+6)/8;
end:
    return ret;
}

/**
 * Encode the private key.
 * The required length for encoding is available through:
 *   NTRUENC_PRIV_KEY_get_len()
 *
 * @param [in] key   The key to encode.
 * @param [in] data  The buffer to hold the encoded data.
 * @param [in] len   The length of the buffer.
 * @return  NTRU_ERR_PARAM_NULL when a required parameter is NULL.<br>
 *          NTRU_ERR_BAD_LEN when the buffer is too small for encoded data.<br>
 *          NTRU_ERR_BAD_DATA when the key is missing data to encode.<br>
 *          0 otherwise.
 */
int NTRUENC_PRIV_KEY_encode(NTRUENC_PRIV_KEY *key, unsigned char *data, int len)
{
    int ret = 0;
    int n = key->params->n;

    if ((key == NULL) || (data == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    /* f is 12-bits */
    if (len < (n*12+6)/8)
    {
        ret = NTRU_ERR_BAD_LEN;
        goto end;
    }

    if (key->f == NULL)
    {
        ret = NTRU_ERR_BAD_DATA;
        goto end;
    }

    ntruenc_encode_12bits(key->f, n, data, NULL);
end:
    return ret;
}

/**
 * Decodes the private key data into a private key object.
 *
 * @param [in] key   The key to encode.
 * @param [in] data  The buffer to hold the encoded data.
 * @param [in] len   The length of the buffer.
 * @return  NTRU_ERR_PARAM_NULL when a required parameter is NULL.<br>
 *          NTRU_ERR_BAD_LEN when the buffer is too small.<br>
 *          NTRU_ERR_ALLOC when unable to allocate memory.<br>
 *          0 otherwise.
 */
int NTRUENC_PRIV_KEY_decode(NTRUENC_PRIV_KEY *key, unsigned char *data, int len)
{
    int ret = 0;
    int n = key->params->n;

    if ((key == NULL) || (data == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    /* f is 12-bits */
    if (len < (n*12+6)/8)
    {
        ret = NTRU_ERR_BAD_LEN;
        goto end;
    }

    if (key->f == NULL) key->f = malloc(sizeof(*key->f) * n);
    if (key->f == NULL)
    {
        ret = NTRU_ERR_ALLOC;
        goto end;
    }

    ntruenc_decode_12bits(data, n, key->f, NULL);
end:
    return ret;
}


/**
 * Retrieves the length of an encoded public key.
 *
 * @param [in]  key  The public key object.
 * @param [out] len  The length of an encoded public key.
 * @return  NTRU_ERR_PARAM_NULL when a calling parameter is NULL.<br>
 *          0 otherwise.
 */
int NTRUENC_PUB_KEY_get_len(NTRUENC_PUB_KEY *key, int *len)
{
    int ret = 0;
    int n = key->params->n;

    if ((key == NULL) || (len == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    *len = (n*12+6)/8;
end:
    return ret;
}

/**
 * Encode the public key.
 * The required length for encoding is available through:
 *   NTRUENC_PUB_KEY_get_len()
 *
 * @param [in] key   The key to encode.
 * @param [in] data  The buffer to hold the encoded data.
 * @param [in] len   The length of the buffer.
 * @return  NTRU_ERR_PARAM_NULL when a required parameter is NULL.<br>
 *          NTRU_ERR_BAD_LEN when the buffer is too small for encoded data.<br>
 *          NTRU_ERR_BAD_DATA when the key is missing data to encode.<br>
 *          0 otherwise.
 */
int NTRUENC_PUB_KEY_encode(NTRUENC_PUB_KEY *key, unsigned char *data, int len)
{
    int ret = 0;
    int n = key->params->n;

    if ((key == NULL) || (data == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    /* h is 12-bits */
    if (len < (n*12+6)/8)
    {
        ret = NTRU_ERR_BAD_LEN;
        goto end;
    }

    if (key->h == NULL)
    {
        ret = NTRU_ERR_BAD_DATA;
        goto end;
    }

    ntruenc_encode_12bits(key->h, n, data, NULL);
end:
    return ret;
}

/**
 * Decodes the public key data into a public key object.
 *
 * @param [in] key   The key to encode.
 * @param [in] data  The buffer to hold the encoded data.
 * @param [in] len   The length of the buffer.
 * @return  NTRU_ERR_PARAM_NULL when a required parameter is NULL.<br>
 *          NTRU_ERR_BAD_LEN when the buffer is too small.<br>
 *          NTRU_ERR_ALLOC when unable to allocate memory.<br>
 *          0 otherwise.
 */
int NTRUENC_PUB_KEY_decode(NTRUENC_PUB_KEY *key, unsigned char *data, int len)
{
    int ret = 0;
    int n = key->params->n;

    if ((key == NULL) || (data == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    /* y is 12-bits, r1 is 2-bits */
    if (len < (n*12+6)/8)
    {
        ret = NTRU_ERR_BAD_LEN;
        goto end;
    }

    if (key->h == NULL) key->h = malloc(sizeof(*key->h) * n);
    if (key->h == NULL)
    {
        ret = NTRU_ERR_ALLOC;
        goto end;
    }

    ntruenc_decode_12bits(data, n, key->h, NULL);
end:
    return ret;
}

