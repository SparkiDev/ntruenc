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

static unsigned char data_dec_data[3] = { 0, 1, 1 };
static unsigned char *data_dec = &data_dec_data[1];

/**
 * Create a new NTRU Encryption operation object.
 *
 * @param [in]  strength  The security strength required of the implementation.
 * @param [in]  flags     Flags describing required features of the requested
 *                        implementation.
 * @param [out] ne        The new NTRU Encryption operation object.
 * @return  NTRU_ERR_PARAM_NULL when a parameter is NULL.<br>
 *          NTRU_ERR_ALLOC on failure to allocate memory.<br>
 *          NTRU_ERR_NOT_FOUND when no matching implementation available.<br>
 *          0 otherwise.
 */
int NTRUENC_new(int strength, int flags, NTRUENC **ne)
{
    int ret;
    NTRUENC *n = NULL;

    if (ne == NULL)
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    n = malloc(sizeof(*n));
    if (n == 0)
    {
        ret = NTRU_ERR_ALLOC;
        goto end;
    }

    ret = NTRUENC_init(n, strength, flags);
    if (ret != 0)
        goto end;

    *ne = n;
    n = NULL;
end:
    NTRUENC_free(n);
    return ret;
}

/**
 * Initialize an empty NTRU Encryption operation object.
 *
 * @param [in] ne        The NTRU Encryption operation object to initialize.
 * @param [in] strength  The security strength required of the implementation.
 * @param [in] flags     Flags describing required features of the requested
 *                       implementation.
 * @return  NTRU_ERR_PARAM_NULL when a parameter is NULL.<br>
 *          NTRU_ERR_NOT_FOUND when no matching implementation available.<br>
 *          0 otherwise.
 */
int NTRUENC_init(NTRUENC *ne, int strength, int flags)
{
    int ret;

    if (ne == NULL)
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    memset(ne, 0, sizeof(*ne));

    ret = ntruenc_meths_get(strength, flags, &ne->meths);
end:
    return ret;
}

/**
 * Finalize an NTRU Encryption operation object.
 *
 * @param [in] ne  The NTRU Encryption operation object.
 */
void NTRUENC_final(NTRUENC *ne)
{
}

/**
 * Free memory associated with the NTRU Encryption operation object.
 *
 * @param [in] ne  The NTRU Encryption operation object.
 */
void NTRUENC_free(NTRUENC *ne)
{
    if (ne != NULL)
    {
        NTRUENC_final(ne);
        free(ne);
    }
}

/**
 * Convert the message/key into an NTRU vector.
 * Each bit of the message/key becomes an element in the vector.
 * 0 -> -1
 * 1 -> +1
 *
 * @param [in] data  The message/key data.
 * @param [in] len   The length of the message/key data in bytes.
 * @param [in] n     The number of elements in the vector.
 * @param [in] m     The NTRU vector.
 * @return  NTRU_ERR_BAD_LEN if the message/key is too long to fit in the
 *          NTRU vector.<br>
 *          0 otherwise.
 */
static int ntruenc_encode_msg(unsigned char *data, int len, int n, short *m)
{
    int ret = 0;
    int i, j;
#ifdef NTRU_BENDIAN
    unsigned char l[2];
#else
    unsigned char *l = (unsigned char *)&len;
#endif
    int zlen;

    if ((len + 2) * 8 > n)
    {
        ret = NTRU_ERR_BAD_LEN;
        goto end;
    }
    zlen = (n - ((len + 2) * 8)) / 8;

#ifdef NTRU_BENDIAN
    l[0] = (len     ) & 0xff;
    l[1] = (len >> 8) & 0xff;
#endif
    for (i=0; i<2; i++)
    {
        for (j=0; j<8; j++)
            m[i*8+j] = (((l[i] >> j) & 1) << 1) - 1;
    }
    m += 2*8;
    for (i=0; i<len; i++)
    {
        for (j=0; j<8; j++)
            m[i*8+j] = (((data[i] >> j) & 1) << 1) - 1;
    }
    m += len*8;
    for (i=0; i<zlen; i++)
    {
        for (j=0; j<8; j++)
            m[i*8+j] = 0;
    }
    m += zlen*8;
    switch ((n - ((len + 2) * 8)) & 7)
    {
    case 7:
        m[6] = 0;
    case 6:
        m[5] = 0;
    case 5:
        m[4] = 0;
    case 4:
        m[3] = 0;
    case 3:
        m[2] = 0;
    case 2:
        m[1] = 0;
    case 1:
        m[0] = 0;
    default:
        break;
    }

end:
    return ret;
}

/**
 * Convert the NTRU vector into a message/key.
 * Each element of the NTRU vector becomes a bit in the message/key.
 * -1 -> 0
 * +1 -> 1
 *
 * @param [in]  m     The NTRU vector.
 * @param [in]  n     The number of elements in the vector.
 * @param [in]  data  The buffer for the message/key.
 * @param [in]  len   The length of the buffer in bytes.
 * @param [out] olen  The length of the data in bytes.
 * @return  NTRU_ERR_BAD_DATA if the encoded length is too long for the
 *          NTRU_ERR_BAD_LEN if the message/key is too long to fit in the
 *          buffer.<br>
 *          0 otherwise.
 */
static int ntruenc_decode_msg(short *m, int n, unsigned char *data, int len,
    int *olen)
{
    int ret = 0;
    int i, j;
    short dlen;
#ifdef NTRU_BENDIAN
    unsigned char l[2];
#else
    unsigned char *l = (unsigned char *)&dlen;
#endif
    short *zero = NULL;
    int zlen;
    char r = 0;

    for (i=0; i<2; i++)
    {
        l[i] = data_dec[m[(i*8)+0]];
        for (j=1; j<8; j++)
            l[i] |= data_dec[m[(i*8)+j]] << j;
    }
#ifdef NTRU_BENDIAN
    dlen = l[0] | ((short)l[1]) << 8;
#endif

    *olen = dlen;
    if (data == NULL)
        goto end;

    if (((dlen + 2) * 8 > n) || (dlen < 0))
    {
        ret = NTRU_ERR_BAD_DATA;
        dlen = (n / 8) - 2 - 2;
        if (len < dlen)
            dlen = len;
    }

    zlen = (n - (dlen + 2) * 8) / 8;
    zero = malloc(zlen * sizeof(*zero));
    if (zero == NULL)
    {
        ret = NTRU_ERR_ALLOC;
        goto end;
    }

    m += 2*8;
    for (i=0; i<dlen; i++)
    {
        r |= m[(i*8)+0] == 0;
        data[i] = data_dec[m[(i*8)+0]];
        for (j=1; j<8; j++)
        {
            r |= m[(i*8)+j] == 0;
            data[i] |= data_dec[m[(i*8)+j]] << j;
        }
    }
    m += dlen*8;
    for (i=0; i<zlen; i++)
    {
        r |= m[(i*8)+0] != 0;
        zero[i] = data_dec[m[(i*8)+0]];
        for (j=1; j<8; j++)
        {
            r |= m[(i*8)+j] != 0;
            zero[i] |= data_dec[m[(i*8)+j]] << j;
        }
    }
    m += zlen*8;
    switch ((n - ((dlen + 2) * 8)) & 7)
    {
    case 7:
        r |= m[6] != 0;
    case 6:
        r |= m[5] != 0;
    case 5:
        r |= m[4] != 0;
    case 4:
        r |= m[3] != 0;
    case 3:
        r |= m[2] != 0;
    case 2:
        r |= m[1] != 0;
    case 1:
        r |= m[0] != 0;
    default:
        break;
    }

    if (r)
        ret = NTRU_ERR_BAD_DATA;
end:
    if (zero != NULL) free(zero);
    return ret;
}

/**
 * Convert the encrypted data as an NTRU vector into a packed buffer.
 * Each entry has 12-bits of data.
 *
 * @param [in] enc   The encrypted data as an NTRU vector.
 * @param [in] n     The number of elements in the NTRU vector.
 * @param [in] data  The encrypted data buffer.
 * @param [in] len   The length of the buffer in bytes.
 * @return  NTRU_ERR_BAD_LEN if the buffer is too small to hold the
 *          encrypted data.<br>
 *          0 otherwise.
 */
static int ntruenc_encode_encrypted(short *enc, int n, unsigned char *data,
    int len)
{
#if 0
    int ret = 0;
    int i, j;

    if (len < (n*12+7)/8)
    {
        ret = NTRU_ERR_BAD_LEN;
        goto end;
    }

    for (i=0,j=0; i<(n/2)*2; i+=2,j+=3)
    {
        data[j+0] =                             ((enc[i+0]        ) << 0);
        data[j+1] = ((enc[i+0] & 0xf00) >> 8) | ((enc[i+1] & 0x00f) << 4);
        data[j+2] = ((enc[i+1] & 0xff0) >> 4)                            ;
    }
    if (i < n)
    {
        data[j+0] =                             ((enc[i+0] & 0x0ff) << 0);
        data[j+1] = ((enc[i+0] & 0xf00) >> 8)                            ;
    }

end:
    return ret;
#else
    int ret = 0;

    if (len < (n * 2))
    {
        ret = NTRU_ERR_BAD_LEN;
        goto end;
    }
    memcpy(data, enc, n * 2);
end:
    return ret;
#endif
}

/**
 * Convert the packed buffer encrypted data into an NTRU vector.
 * Each entry is 12-bits of data.
 *
 * @param [in] data  The signature buffer.
 * @param [in] len   The length of the buffer in bytes.
 * @param [in] n     The number of elements in the NTRU vector.
 * @param [in] enc   The signature as NTRU vectors.
 * @return  NTRU_ERR_BAD_LEN if the buffer is too small to hold the
 *          signature.<br>
 *          0 otherwise.
 */
static int ntruenc_decode_encrypted(unsigned char *data, int len, int n,
    short *enc)
{
#if 0
    int ret = 0;
    int i, j;

    if (len < (n*12+7)/8)
    {
        ret = NTRU_ERR_BAD_LEN;
        goto end;
    }

    for (i=0,j=0; i<(n/2)*2; i+=2,j+=3)
    {
        enc[i+0] = ((data[j+0]       ) >> 0) | ((short)(data[j+1] & 0x0f) << 8);
        enc[i+1] = ((data[j+1] & 0xf0) >> 4) | ((short)(data[j+2]       ) << 4);
    }
    if (i < n)
        enc[i+0] = ((data[j+0]       ) >> 0) | ((short)(data[j+1] & 0x0f) << 8);

end:
    return ret;
#else
    int ret = 0;

    if (len < (n*2))
    {
        ret = NTRU_ERR_BAD_LEN;
        goto end;
    }
    memcpy(enc, data, n * 2);
end:
    return ret;
#endif
}

/**
 * Initialize memory for the encryption operation.
 * Allocate memory if required.
 *
 * @param [in] ne    The NTRU Encryption operation object.
 * @param [in] priv  Private key.
 * @return  NTRU_ERR_PARAM_NULL when a parameter is NULL.<br>
 *          NTRU_ERR_ALLOC on failure to allocate.<br>
 *          0 otheriwise.
 */
int NTRUENC_encrypt_init(NTRUENC *ne, NTRUENC_PUB_KEY *pub)
{
    int ret = 0;

    if ((ne == NULL) || (pub == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    ne->m = malloc(pub->params->n * sizeof(*ne->m));
    ne->enc = malloc(pub->params->n * sizeof(*ne->enc));
    if ((ne->m == NULL) || (ne->enc == NULL))
    {
        ret = NTRU_ERR_ALLOC;
        goto end;
    }

    if (ne->meths->enc_num > 0)
    {
        ne->t = malloc(pub->params->n * sizeof(short) * ne->meths->enc_num);
        if (ne->t == NULL)
        {
            ret = NTRU_ERR_ALLOC;
            goto end;
        }
    }

    ne->pub = pub;
end:
    if (ret == NTRU_ERR_ALLOC)
    {
        if (ne->t != NULL) { free(ne->t); ne->t = NULL; }
        if (ne->enc != NULL) { free(ne->enc); ne->enc = NULL; }
        if (ne->m != NULL) { free(ne->m); ne->m = NULL; }
    }
    return ret;
}

/**
 * Perform the encryption operation.
 * Use the encryption function from the method table.
 *
 * @param [in] ne    The NTRU Encryption operation object.
 * @param [in] data  The encoded message or key to encrypt.
 * @param [in] len   The length of the encoded message or key.
 * @param [in] enc   The buffer to hold encrypted data.
 * @param [in] elen  The length of the buffer.
 * @return  NTRU_ERR_PARAM_NULL when a parameter is NULL.<br>
 *          NTRU_ERR_INIT when NTRU_encrypt_init() has not been called.<br>
 *          NTRU_ERR_ALLOC on failure to allocate.<br>
 *          NTRU_ERR_BAD_LEN when buffer is too short.<br>
 *          0 otheriwise.
 */
int NTRUENC_encrypt(NTRUENC *ne, unsigned char *data, int len,
    unsigned char *enc, int elen)
{
    int ret;

    if ((ne == NULL) || (data == NULL) || (enc == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }
    if (ne->pub == NULL)
    {
        ret = NTRU_ERR_INIT;
        goto end;
    }

    ret = ntruenc_encode_msg(data, len, ne->pub->params->n, ne->m);
    if (ret != 0)
        goto end;

    ret = ne->meths->enc(ne->enc, ne->m, ne->pub->h, ne->t);
    if (ret != 0)
        goto end;

    ret = ntruenc_encode_encrypted(ne->enc, ne->pub->params->n, enc, elen);
end:
    return ret;
}

/**
 * Cleanup the dynamic memory from encryption.
 *
 * @param [in] ne  The NTRU Encryption operation object.
 */
void NTRUENC_encrypt_final(NTRUENC *ne)
{
    if (ne != NULL)
    {
        ne->pub = NULL;
        if (ne->t != NULL) { free(ne->t); ne->t = NULL; }
        if (ne->enc != NULL) { free(ne->enc); ne->enc = NULL; }
        if (ne->m != NULL) { free(ne->m); ne->m = NULL; }
    }
}

/**
 * Initialize memory for the decryption operation.
 * Allocate memory if required.
 *
 * @param [in] ne    The NTRU Encryption operation object.
 * @param [in] priv  Private key.
 * @return  NTRU_ERR_PARAM_NULL when a parameter is NULL.<br>
 *          NTRU_ERR_ALLOC on failure to allocate.<br>
 *          0 otheriwise.
 */
int NTRUENC_decrypt_init(NTRUENC *ne, NTRUENC_PRIV_KEY *priv)
{
    int ret = 0;

    if ((ne == NULL) || (priv == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    ne->m = malloc(priv->params->n * sizeof(*ne->m));
    ne->enc = malloc(priv->params->n * sizeof(*ne->enc));
    if ((ne->m == NULL) || (ne->enc == NULL))
    {
        ret = NTRU_ERR_ALLOC;
        goto end;
    }

    if (ne->meths->dec_num > 0)
    {
        ne->t = malloc(priv->params->n * sizeof(short) * ne->meths->dec_num);
        if (ne->t == NULL)
        {
            ret = NTRU_ERR_ALLOC;
            goto end;
        }
    }

    ne->priv = priv;
end:
    if (ret == NTRU_ERR_ALLOC)
    {
        if (ne->t != NULL) { free(ne->t); ne->t = NULL; }
        if (ne->enc != NULL) { free(ne->enc); ne->enc = NULL; }
        if (ne->m != NULL) { free(ne->m); ne->m = NULL; }
    }
    return ret;
}

/**
 * Perform the decryption operation.
 * Use the decryption function from the method table.
 *
 * @param [in]  ne    The NTRU Encryption operation object.
 * @param [in]  enc   The encrypted data.
 * @param [in]  elen  The length of the encrypted data.
 * @param [in]  data  The decrypted data.
 * @param [in]  len   The length of the decrypted data buffer.
 * @param [out] olen  The length of the decrypted data.
 * @return  NTRU_ERR_PARAM_NULL when a parameter is NULL.<br>
 *          NTRU_ERR_INIT when NTRU_decrypt_init() has not been called.<br>
 *          NTRU_ERR_BAD_LEN when buffer is too short.<br>
 *          0 otheriwise.
 */
int NTRUENC_decrypt(NTRUENC *ne, unsigned char *enc, int elen,
    unsigned char *data, int len, int *olen)
{
    int ret = 0;

    *olen = 0;

    if ((ne == NULL) || (enc == NULL) || (data == NULL) || (olen == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }
    if (ne->priv == NULL)
    {
        ret = NTRU_ERR_INIT;
        goto end;
    }

    ret = ntruenc_decode_encrypted(enc, elen, ne->priv->params->n, ne->enc);
    if (ret != 0)
        goto end;

    ne->meths->dec(ne->m, ne->enc, ne->priv->f, ne->t);

    ret = ntruenc_decode_msg(ne->m, ne->priv->params->n, data, len, olen);
end:
    return ret;
}

/**
 * Cleanup the dynamic memory from decryption.
 *
 * @param [in] ne  The NTRU Encryption operation object.
 */
void NTRUENC_decrypt_final(NTRUENC *ne)
{
    if (ne != NULL)
    {
        ne->priv = NULL;
        if (ne->t != NULL) { free(ne->t); ne->t = NULL; }
        if (ne->enc != NULL) { free(ne->enc); ne->enc = NULL; }
        if (ne->m != NULL) { free(ne->m); ne->m = NULL; }
    }
}

/**
 * Initialize memory for the key generation operation.
 * Allocate memory for each of the key fields.
 *
 * @param [in] ne      The NTRU Encryption operation object.
 * @param [in] params  NTRU parameters.
 * @return  NTRU_ERR_PARAM_NULL when a parameter is NULL.<br>
 *          NTRU_ERR_ALLOC on failure to allocate.<br>
 *          0 otheriwise.
 */
int NTRUENC_keygen_init(NTRUENC *ne, NTRUENC_PARAMS *params)
{
    int ret = 0;

    if ((ne == NULL) || (params == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }

    if (ne->meths->keygen_num > 0)
    {
        ne->t = malloc(params->n * sizeof(short) * ne->meths->keygen_num);
        if (ne->t == NULL)
        {
            ret = NTRU_ERR_ALLOC;
            goto end;
        }
    }

    ne->params = params;
end:
    return ret;
}

/**
 * Perform the key generation operation.
 * Use the key generation function from the method table.
 *
 * @param [in]  ne        The NTRU Encryption operation object.
 * @param [out] priv_key  The generated private key.
 * @param [out] pub_key   The generated public key.
 * @return  NTRU_ERR_PARAM_NULL when a parameter is NULL.<br>
 *          NTRU_ERR_INIT when NTRU_decrypt_init() has not been called.<br>
 *          NTRU_ERR_ALLOC on failure to allocate.<br>
 *          0 otheriwise.
 */
int NTRUENC_keygen(NTRUENC *ne, NTRUENC_PRIV_KEY **priv_key,
    NTRUENC_PUB_KEY **pub_key)
{
    int ret;
    NTRUENC_PRIV_KEY *priv = NULL;
    NTRUENC_PUB_KEY *pub = NULL;
    short n = ne->params->n;

    if ((ne == NULL) || (priv_key == NULL) || (pub_key == NULL))
    {
        ret = NTRU_ERR_PARAM_NULL;
        goto end;
    }
    if (ne->params == NULL)
    {
        ret = NTRU_ERR_INIT;
        goto end;
    }

    if (*priv_key == NULL)
    {
        ret = NTRUENC_PRIV_KEY_new(ne->params, &priv);
        if (ret != 0)
            goto end;
    }
    else
        priv = *priv_key;
    if (*pub_key == NULL)
    {
        ret = NTRUENC_PUB_KEY_new(ne->params, &pub);
        if (ret != 0)
            goto end;
    }
    else
        pub = *pub_key;

    if (priv->f == NULL) priv->f = malloc(n*sizeof(*priv->f));
    if (pub->h == NULL) pub->h = malloc(n*sizeof(*pub->h));
    if ((priv->f == NULL) || (pub->h == NULL))
    {
        ret = NTRU_ERR_ALLOC;
        goto end;
    }

    ret = ne->meths->keygen(priv->f, pub->h, ne->t);
    if (ret != 0)
        goto end;

    *priv_key = priv;
    *pub_key = pub;
    priv = NULL;
    pub = NULL;
end:
    NTRUENC_PUB_KEY_free(pub);
    NTRUENC_PRIV_KEY_free(priv);
    return ret;
}

/**
 * Cleanup the dynamic memory from key generation.
 *
 * @param [in] ne  The NTRU Encryption operation object.
 */
void NTRUENC_keygen_final(NTRUENC *ne)
{
    if (ne != NULL)
    {
        ne->params = NULL;
        if (ne->t != NULL)
        {
            free(ne->t);
            ne->t = NULL;
        }
    }
}

