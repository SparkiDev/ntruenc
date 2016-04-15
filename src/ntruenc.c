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
#include "ntruenc_key_lcl.h"

/**
 * Initialize memory for the encryption operation.
 * Allocate memory if required.
 *
 * @param [in] priv  Private key.
 * @return  0 on success.<br>
 *          1 on failure to allocate.
 */
int NTRUENC_encrypt_init(NTRUENC_PUB_KEY *pub)
{
    int ret = 0;

    if (pub->meths->enc_num > 0)
    {
        pub->t = malloc(pub->params->n * sizeof(short) * pub->meths->enc_num);
        ret = (pub->t == NULL);
    }
    return ret;
}
/**
 * Perform the encryption operation.
 * Use the sign function from the method table.
 *
 * @param [in] enc   The buffer to hold encrypted vector.
 * @param [in] data  The encoded message or key to encrypt.
 * @param [in] pub   The public key to encrypt with.
 */
int NTRUENC_encrypt(short *enc, short *data, NTRUENC_PUB_KEY *pub)
{
    return pub->meths->enc(enc, data, pub->h, pub->t);
}
/**
 * Cleanup the dynamic memory from encryption.
 *
 * @param [in] pub  Public key.
 */
void NTRUENC_encrypt_final(NTRUENC_PUB_KEY *pub)
{
    if (pub->t != NULL)
    {
        free(pub->t);
        pub->t = NULL;
    }
}

/**
 * Initialize memory for the decryption operation.
 * Allocate memory if required.
 *
 * @param [in] priv  Private key.
 * @return  0 on success.<br>
 *          1 on failure to allocate.
 */
int NTRUENC_decrypt_init(NTRUENC_PRIV_KEY *priv)
{
    int ret = 0;

    if (priv->meths->dec_num > 0)
    {
        priv->t = malloc(priv->params->n * sizeof(short) *
            priv->meths->enc_num);
        ret = (priv->t == NULL);
    }
    return ret;
}
/**
 * Perform the decryption operation.
 * Use the decryption function from the method table.
 *
 * @param [in]  data  The decrypted data.
 * @param [in]  enc   The encrypted data.
 * @param [in]  priv  The private key to decrypt with.
 */
void NTRUENC_decrypt(short *data, short *enc, NTRUENC_PRIV_KEY *priv)
{
    priv->meths->dec(data, enc, priv->f, priv->t);
}
/**
 * Cleanup the dynamic memory from decryption.
 *
 * @param [in] priv  Private key.
 */
void NTRUENC_decrypt_final(NTRUENC_PRIV_KEY *priv)
{
    if (priv->t != NULL)
    {
        free(priv->t);
        priv->t = NULL;
    }
}

/**
 * Initialize memory for the key generation operation.
 * Allocate memory for each of the key fields.
 *
 * @param [in] priv  Private key.
 * @param [in] pub   Public key.
 * @return  0 on success.<br>
 *          1 on failure to allocate.
 */
int NTRUENC_keygen_init(NTRUENC_PRIV_KEY *priv, NTRUENC_PUB_KEY *pub)
{
    int ret = 0;
    short n = priv->params->n;

    if (priv->f == NULL) priv->f = malloc(n*sizeof(*priv->f));
    if (pub->h == NULL) pub->h = malloc(n*sizeof(*pub->h));
    if (priv->meths->keygen_num > 0)
    {
        priv->t = malloc(priv->params->n * sizeof(short) *
            priv->meths->keygen_num);
        ret = (priv->t == NULL);
    }

    return ret || (priv->f == NULL) || (pub->h == NULL);
}
/**
 * Perform the key generation operation.
 * Use the key generation function from the method table.
 *
 * @param [in] priv  The generated private key.
 * @param [in] pub   The generated public key.
 * @return  0 on success.<br>
 *          1 on failure.
 */
int NTRUENC_keygen(NTRUENC_PRIV_KEY *priv, NTRUENC_PUB_KEY *pub)
{
    return priv->meths->keygen(priv->f, pub->h, priv->t);
}

/**
 * Cleanup the dynamic memory from key generation.
 *
 * @param [in] priv  Private key.
 * @param [in] pub   Public key.
 */
void NTRUENC_keygen_final(NTRUENC_PRIV_KEY *priv, NTRUENC_PUB_KEY *pub)
{
    if (priv->t != NULL)
    {
        free(priv->t);
        priv->t = NULL;
    }
}

