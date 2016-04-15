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

#if 0
#ifdef CPU_X86_64
    /* Security strength 112 in Intel x86 64-bit assembly. */
    { 112, NTRUENC_METHS_FLAG_ASM, 1, 1, 2,
      ntruenc_s112_encrypt_x64, ntruenc_s112_decrypt_x64, ntruenc_s112_keygen_x64 },
    /* Security strength 128 in Intel x86 64-bit assembly. */
    { 128, NTRUENC_METHS_FLAG_ASM, 1, 1, 2,
      ntruenc_s128_encrypt_x64, ntruenc_s128_decrypt_x64, ntruenc_s128_keygen_x64 },
    /* Security strength 192 in Intel x86 64-bit assembly. */
    { 192, NTRUENC_METHS_FLAG_ASM, 1, 1, 2,
      ntruenc_s192_encrypt_x64, ntruenc_s192_decrypt_x64, ntruenc_s192_keygen_x64 },
    /* Security strength 256 in Intel x86 64-bit assembly. */
    { 256, NTRUENC_METHS_FLAG_ASM, 1, 1, 2,
      ntruenc_s256_encrypt_x64, ntruenc_s256_decrypt_x64, ntruenc_s256_keygen_x64 },
#endif
#endif
};
/**
 * The number of implementations.
 */
#define NTRUENC_METHS_LEN ((int)(sizeof(ntruenc_meths)/sizeof(*ntruenc_meths)))

/**
 * The parameters for NTRU encryption at different security strengths.
 */
NTRUENC_PARAMS ntruenc_params[] =
{
    { 112, NTRU_S112_N, NTRU_S112_DF, NTRU_S112_DG, NTRU_S112_Q },
    { 128, NTRU_S128_N, NTRU_S128_DF, NTRU_S128_DG, NTRU_S112_Q },
    { 192, NTRU_S192_N, NTRU_S192_DF, NTRU_S192_DG, NTRU_S112_Q },
    { 256, NTRU_S256_N, NTRU_S256_DF, NTRU_S256_DG, NTRU_S112_Q }
};
/**
 * The number of parameters.
 */
#define NTRUENC_PARAMS_LEN ((int)(sizeof(ntruenc_params)/sizeof(*ntruenc_params)))

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
    return (m == NULL);
}

/**
 * Retrieve the parameters meeting the required security strength.
 *
 * @param [in]  strength  The security strength required.
 * @param [out] params    The parameters matching the requirements.
 * @return  0 on parameters found.<br>
 *          1 otherwise.
 */
int ntruenc_params_get(short strength, NTRUENC_PARAMS **params)
{
    int i;
    NTRUENC_PARAMS *p = NULL;

    /* Start at the strongest and return the weakest meeting requirements. */
    for (i=NTRUENC_PARAMS_LEN-1; i>=0; i--)
    {
        if (ntruenc_params[i].strength >= strength)
            p = &ntruenc_params[i];
    }
    *params = p;
    return (p == NULL);
}

/**
 * Allocate and initialize a private key.
 *
 * @param [in]  strength  The security strength required.
 * @param [in]  flags     Requirements on methods and parameters.  
 * @param [out] key       The new private key.  
 * @return  0 on success.<br>
 *          1 otherwise.
 */
int NTRUENC_PRIV_KEY_new(short strength, int flags, NTRUENC_PRIV_KEY **key)
{
    int ret = 1;
    *key = malloc(sizeof(NTRUENC_PRIV_KEY));
    if (*key == NULL)
        goto end;
    ret = NTRUENC_PRIV_KEY_init(*key, strength, flags);
end:
    return ret;
}

/**
 * Initialize a private key object.
 *
 * @param [in] key       The private key.
 * @param [in] strength  The security strength required.
 * @param [in] flags     Requirements on methods and parameters.  
 * @return  0 on success.<br>
 *          1 otherwise.
 */
int NTRUENC_PRIV_KEY_init(NTRUENC_PRIV_KEY *key, short strength, int flags)
{
    int ret = 1;

    if (key == NULL)
        goto end;
    memset(key, 0, sizeof(*key));

    ret = ntruenc_meths_get(strength, flags, &key->meths);
    if (ret != 0)
        goto end;
    ret = ntruenc_params_get(strength, &key->params);
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
    if (key->f != NULL) free(key->f);
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
 * @param [out] len  The number of elements of an NTRU vector.
 * @return  0 on success.<br>
 *          1 when a parameter is NULL.
 */
int NTRUENC_PRIV_KEY_get_len(NTRUENC_PRIV_KEY *key, int *len)
{
    int ret = (key == NULL) || (len == NULL);
    if (ret == 0)
        *len = key->params->n;
    return ret;
}


/**
 * Allocate and initialize a public key.
 *
 * @param [in]  strength  The security strength required.
 * @param [in]  flags     Requirements on methods and parameters.  
 * @param [out] key       The new public key.  
 * @return  0 on success.<br>
 *          1 otherwise.
 */
int NTRUENC_PUB_KEY_new(short strength, int flags, NTRUENC_PUB_KEY **key)
{
    int ret = 1;
    *key = malloc(sizeof(NTRUENC_PUB_KEY));
    if (*key == NULL)
        goto end;
    ret = NTRUENC_PUB_KEY_init(*key, strength, flags);
end:
    return ret;
}

/**
 * Initialize a public key object.
 *
 * @param [in] key       The public key.
 * @param [in] strength  The security strength required.
 * @param [in] flags     Requirements on methods and parameters.  
 * @return  0 on success.<br>
 *          1 otherwise.
 */
int NTRUENC_PUB_KEY_init(NTRUENC_PUB_KEY *key, short strength, int flags)
{
    int ret;
    memset(key, 0, sizeof(*key));
    ret = ntruenc_meths_get(strength, flags, &key->meths);
    if (ret != 0) return ret;
    return ntruenc_params_get(strength, &key->params);
}

/**
 * Finalize the public key object.
 * Disposes of any allocated data fields.
 *
 * @param [in]  The public key.
 */
void NTRUENC_PUB_KEY_final(NTRUENC_PUB_KEY *key)
{
    if (key->h != NULL) free(key->h);
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
 * @param [out] len  The number of elements of an NTRU vector.
 * @return  0 on success.<br>
 *          1 when a parameter is NULL.
 */
int NTRUENC_PUB_KEY_get_len(NTRUENC_PUB_KEY *key, int *len)
{
    int ret = (key == NULL) || (len == NULL);
    if (ret == 0)
        *len = key->params->n;
    return ret;
}

/**
 * Retrieves the number of bytes in an NTRU Encryption.
 *
 * @param [in]  key  The public key object.
 * @param [out] len  The number of byes in an NTRU Encryption.
 * @return  0 on success.<br>
 *          1 when a parameter is NULL.
 */
int NTRUENC_PUB_KEY_get_enc_len(NTRUENC_PUB_KEY *key, int *len)
{
    int ret = (key == NULL) || (len == NULL);
    if (ret == 0)
        *len = key->params->n * sizeof(*key->h);
    return ret;
}

