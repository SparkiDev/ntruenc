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

#include "ntruenc.h"
#include "ntruenc_key.h"

/**
 * The method table for NTRU Encryption operations.
 */
typedef struct ntruenc_meths_st
{
    /** The maximum security strength supported by implementation. */
    int strength;
    /** Flags describing implementation. */
    int flags;
    /** Number of NTRU vectors required for encryption by implementation. */
    char enc_num;
    /** Number of NTRU vectors required for decryption by implementation. */
    char dec_num;
    /** Number of NTRU vectors required for key generation by implementation. */
    char keygen_num;
    /** Function to perform encryption. */
    int (*enc)(short *e, short *m, short *h, short *t);
    /** Function to perform decryption. */
    void (*dec)(short *c, short *e, short *f, short *t);
    /** Function to perform key generation. */
    int (*keygen)(short *f, short *h, short *t);
} NTRUENC_METHS;


struct ntruenc_st
{
    /** Implementation of NTRU Encryption. */
    NTRUENC_METHS *meths;
    /** The private key to use with the operation. */
    NTRUENC_PRIV_KEY *priv;
    /** The public key to use with the operation. */
    NTRUENC_PUB_KEY *pub;
    /** The parameters to use with the operation. */
    NTRUENC_PARAMS *params;
    /** Message as an NTRU vector. */
    short *m;
    /** Encrypted data as NTRU vectors. */
    short *enc;
    /** Temprorary dynamicly allocated data. */
    short *t;
};

int ntruenc_meths_get(short strength, int flags, NTRUENC_METHS **meths);

/* Common parameter */
#define NTRU_P		3

/**
 * The number of -1 elements in a random NTRU vector mod p for a security
 * strength of 112-bits.
 */
#define NTRU_S112_DF		142
/**
 * The number of -1 elements in a random NTRU vector mod p for a security
 * strength of 112-bits in g.
 */
#define NTRU_S112_DG		146
/**
 * The number of elements in an NTRU vector for a security strength of 112-bits.
 */
#define NTRU_S112_N		439
/**
 * The largest modulus value when using a security strength of 112-bits.
 */
#define NTRU_S112_Q		6833
int ntruenc_s112_random(short *a, int df1, int df2, short v);
int ntruenc_s112_keygen(short *f, short *h, short *t);
int ntruenc_s112_encrypt(short *e, short *m, short *h, short *t);
void ntruenc_s112_decrypt(short *c, short *e, short *f, short *t);
int ntruenc_s112_mod_inv_q(short *r, short *a);
void ntruenc_s112_mul_mod_q(short *r, short *a, short *b, void *t);


/**
 * The number of -1 elements in a random NTRU vector mod p for a security
 * strength of 128-bits.
 */
#define NTRU_S128_DF		131
/**
 * The number of -1 elements in a random NTRU vector mod p for a security
 * strength of 128-bits in g.
 */
#define NTRU_S128_DG		163
/**
 * The number of elements in an NTRU vector for a security strength of 128-bits.
 */
#define NTRU_S128_N		491
/**
 * The largest modulus value when using a security strength of 128-bits.
 */
#define NTRU_S128_Q		6287
int ntruenc_s128_random(short *a, int df1, int df2, short v);
int ntruenc_s128_keygen(short *f, short *h, short *t);
int ntruenc_s128_encrypt(short *e, short *m, short *h, short *t);
void ntruenc_s128_decrypt(short *c, short *e, short *f, short *t);
int ntruenc_s128_mod_inv_q(short *r, short *a);
void ntruenc_s128_mul_mod_q(short *r, short *a, short *b, void *t);

/**
 * The number of -1 elements in a random NTRU vector mod p for a security
 * strength of 192.
 */
#define NTRU_S192_DF		155
/**
 * The number of -1 elements in a random NTRU vector mod p for a security
 * strength of 192 in g.
 */
#define NTRU_S192_DG		219
/**
 * The number of elements in an NTRU vector for a security strength of 192-bits.
 */
#define NTRU_S192_N		659
/**
 * The largest modulus value when using a security strength of 192-bits.
 */
#define NTRU_S192_Q		7481
int ntruenc_s192_random(short *a, int df1, int df2, short v);
int ntruenc_s192_keygen(short *f, short *h, short *t);
int ntruenc_s192_encrypt(short *e, short *m, short *h, short *t);
void ntruenc_s192_decrypt(short *c, short *e, short *f, short *t);
int ntruenc_s192_mod_inv_q(short *r, short *a);
void ntruenc_s192_mul_mod_q(short *r, short *a, short *b, void *t);

/**
 * The number of -1 elements in a random NTRU vector mod p for a security
 * strength of 215.
 */
#define NTRU_S215_DF		204
/**
 * The number of -1 elements in a random NTRU vector mod p for a security
 * strength of 215 in g.
 */
#define NTRU_S215_DG		246
/**
 * The number of elements in an NTRU vector for a security strength of 215-bits.
 */
#define NTRU_S215_N		739
/**
 * The largest modulus value when using a security strength of 215-bits.
 */
#define NTRU_S215_Q		9829
int ntruenc_s215_random(short *a, int df1, int df2, short v);
int ntruenc_s215_keygen(short *f, short *h, short *t);
int ntruenc_s215_encrypt(short *e, short *m, short *h, short *t);
void ntruenc_s215_decrypt(short *c, short *e, short *f, short *t);
int ntruenc_s215_mod_inv_q(short *r, short *a);
void ntruenc_s215_mul_mod_q(short *r, short *a, short *b, void *t);

/**
 * The number of -1 elements in a random NTRU vector mod p for a security
 * strength of 256.
 */
#define NTRU_S256_DF		159
/**
 * The number of -1 elements in a random NTRU vector mod p for a security
 * strength of 256 in g.
 */
#define NTRU_S256_DG		293
/**
 * The number of elements in an NTRU vector for a security strength of 256-bits.
 */
#define NTRU_S256_N		881
/**
 * The largest modulus value when using a security strength of 256-bits.
 */
#define NTRU_S256_Q		7673
int ntruenc_s256_random(short *a, int df1, int df2, short v);
int ntruenc_s256_keygen(short *f, short *h, short *t);
int ntruenc_s256_encrypt(short *e, short *m, short *h, short *t);
void ntruenc_s256_decrypt(short *c, short *e, short *f, short *t);
int ntruenc_s256_mod_inv_q(short *r, short *a);
void ntruenc_s256_mul_mod_q(short *r, short *a, short *b, void *t);

