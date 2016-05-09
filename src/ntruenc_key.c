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
 * The parameters for NTRU encryption at different security strengths.
 */
NTRUENC_PARAMS ntruenc_params[] =
{
    /* Security strength: 112-bits */
    { 112, NTRU_S112_N, NTRU_S112_DF, NTRU_S112_DG, NTRU_S112_Q },
    /* Security strength: 128-bits */
    { 128, NTRU_S128_N, NTRU_S128_DF, NTRU_S128_DG, NTRU_S112_Q },
    /* Security strength: 192-bits */
    { 192, NTRU_S192_N, NTRU_S192_DF, NTRU_S192_DG, NTRU_S112_Q },
    /* Security strength: 256-bits */
    { 256, NTRU_S256_N, NTRU_S256_DF, NTRU_S256_DG, NTRU_S112_Q }
};
/**
 * The number of parameters.
 */
#define NTRUENC_PARAMS_LEN	\
    ((int)(sizeof(ntruenc_params)/sizeof(*ntruenc_params)))

/**
 * Retrieve the parameters meeting the required security strength.
 *
 * @param [in]  strength  The security strength required.
 * @param [out] params    The parameters matching the requirements.
 * @return  0 on parameters found.<br>
 *          1 otherwise.
 */
int NTRUENC_PARAMS_get(short strength, NTRUENC_PARAMS **params)
{
    int ret = 0;
    int i;
    NTRUENC_PARAMS *p = NULL;

    if (params == NULL)
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    /* Start at the strongest and return the weakest meeting requirements. */
    for (i=NTRUENC_PARAMS_LEN-1; i>=0; i--)
    {
        if (ntruenc_params[i].strength >= strength)
            p = &ntruenc_params[i];
    }
    *params = p;
    if (p == NULL)
        ret = NTRU_ERR_NOT_FOUND;
end:
    return ret;
}

/**
 * Allocate and initialize a private key.
 *
 * @param [in]  params  The NTRU encryption parameters.
 * @param [out] key     The new private key.
 * @return  NTRU_ERR_PARAM_NULL when a calling parameter is NULL.<br>
 *          NTRU_ERR_ALLOC on failure to allocate memory.<br>
 *          0 otherwise.
 */
int NTRUENC_PRIV_KEY_new(NTRUENC_PARAMS *params, NTRUENC_PRIV_KEY **key)
{
    int ret;
    NTRUENC_PRIV_KEY *k = NULL;

    if ((params == NULL) || (key == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    k = malloc(sizeof(NTRUENC_PRIV_KEY));
    if (k == NULL)
    {
        ret = NTRU_ERR_ALLOC;
        goto end;
    }

    ret = NTRUENC_PRIV_KEY_init(k, params);
    if (ret != 0)
        goto end;

    *key = k;
    k = NULL;
end:
    NTRUENC_PRIV_KEY_free(k);
    return ret;
}

/**
 * Initialize a private key object.
 *
 * @param [in] key     The private key.
 * @param [in] params  The NTRU encryption parameters.
 * @return  NTRU_ERR_PARAM_NULL when a calling parameter is NULL.<br>
 *          0 otherwise.
 */
int NTRUENC_PRIV_KEY_init(NTRUENC_PRIV_KEY *key, NTRUENC_PARAMS *params)
{
    int ret = 0;

    if ((key == NULL) || (params == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    memset(key, 0, sizeof(*key));

    key->params = params;
end:
    return ret;
}

/**
 * Finalize the private key object.
 * Disposes of any allocated data fields.
 *
 * @param [in] key  The private key.
 */
void NTRUENC_PRIV_KEY_final(NTRUENC_PRIV_KEY *key)
{
    if (key != NULL)
    {
        if (key->f != NULL) free(key->f);
    }
}

/**
 * Frees the private key fields and object.
 *
 * @param [in] key  The private key.
 */
void NTRUENC_PRIV_KEY_free(NTRUENC_PRIV_KEY *key)
{
    if (key != NULL)
    {
        NTRUENC_PRIV_KEY_final(key);
        free(key);
    }
}

/**
 * Retrieves the number of elements in an NTRU vector.
 *
 * @param [in]  key  The private key object.
 * @param [out] n    The number of elements of an NTRU vector.
 * @return  NTRU_ERR_PARAM_NULL when a calling parameter is NULL.<br>
 *          0 otherwise.
 */
int NTRUENC_PRIV_KEY_num_entries(NTRUENC_PRIV_KEY *key, int *n)
{
    int ret = 0;

    if ((key == NULL) || (n == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    *n = key->params->n;
end:
    return ret;
}


/**
 * Allocate and initialize a public key.
 *
 * @param [in]  params  The NTRU encryption parameters.
 * @param [out] key     The new public key.
 * @return  NTRU_ERR_PARAM_NULL when a calling parameter is NULL.<br>
 *          NTRU_ERR_ALLOC on failure to allocate memory.<br>
 *          0 otherwise.
 */
int NTRUENC_PUB_KEY_new(NTRUENC_PARAMS *params, NTRUENC_PUB_KEY **key)
{
    int ret;
    NTRUENC_PUB_KEY *k = NULL;

    if ((params == NULL) || (key == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    k = malloc(sizeof(NTRUENC_PUB_KEY));
    if (k == NULL)
    {
        ret = NTRU_ERR_ALLOC;
        goto end;
    }

    ret = NTRUENC_PUB_KEY_init(k, params);
    if (ret != 0)
        goto end;

    *key = k;
    k = NULL;
end:
    NTRUENC_PUB_KEY_free(k);
    return ret;
}

/**
 * Initialize a public key object.
 *
 * @param [in] key       The public key.
 * @param [in] params  The NTRU encryption parameters.
 * @return  NTRU_ERR_PARAM_NULL when a calling parameter is NULL.<br>
 *          0 otherwise.
 */
int NTRUENC_PUB_KEY_init(NTRUENC_PUB_KEY *key, NTRUENC_PARAMS *params)
{
    int ret = 0;

    if ((key == NULL) || (params == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    memset(key, 0, sizeof(*key));

    key->params = params;
end:
    return ret;
}

/**
 * Finalize the public key object.
 * Disposes of any allocated data fields.
 *
 * @param [in]  The public key.
 */
void NTRUENC_PUB_KEY_final(NTRUENC_PUB_KEY *key)
{
    if (key != NULL)
    {
        if (key->h != NULL) free(key->h);
    }
}

/**
 * Frees the public key fields and object.
 *
 * @param [in] key  The public key.
 */
void NTRUENC_PUB_KEY_free(NTRUENC_PUB_KEY *key)
{
    if (key != NULL)
    {
        NTRUENC_PUB_KEY_final(key);
        free(key);
    }
}

/**
 * Retrieves the number of elements in an NTRU vector.
 *
 * @param [in]  key  The public key object.
 * @param [out] n    The number of elements of an NTRU vector.
 * @return  NTRU_ERR_PARAM_NULL when a calling parameter is NULL.<br>
 *          0 otherwise.
 */
int NTRUENC_PUB_KEY_num_entries(NTRUENC_PUB_KEY *key, int *n)
{
    int ret = 0;

    if ((key == NULL) || (n == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    *n = key->params->n;
end:
    return ret;
}

/**
 * Retrieves the number of bytes in an NTRU Encryption.
 *
 * @param [in]  key  The public key object.
 * @param [out] len  The number of byes in an NTRU Encryption.
 * @return  NTRU_ERR_PARAM_NULL when a calling parameter is NULL.<br>
 *          0 otherwise.
 */
int NTRUENC_PUB_KEY_get_enc_len(NTRUENC_PUB_KEY *key, int *len)
{
    int ret = 0;

    if ((key == NULL) || (len == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    *len = (key->params->n * 12 + 7) / 8;
end:
    return ret;
}

