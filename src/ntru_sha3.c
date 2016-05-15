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

#include <stdint.h>
#include "ntru_sha3.h"

/**
 * Rotate a 64-bit value left.
 *
 * @param [in] a  The number to rotate left.
 * @param [in] r  The number od bits to rotate left.
 * @return  The rotated number.
 */
#define ROL(a, n)    (((a)<<(n))|((a)>>(64-(n))))

/**
 * Put an array of bytes into a 64-bit number.
 * The bytes in big-endian byte order.
 *
 * @param [in] x  The array of bytes.
 * @return  A 64-bit number.
 */
static uint64_t ntru_keccak_le64(const uint8_t *x)
{
    uint64_t r=0;
    uint64_t i;

    for (i=0; i<8; i++)
        r |= (uint64_t)x[i] << (8*i);
    return r;
}

/** An array of masking values for block operation. */
static uint64_t ntru_keccak_r[24] = 
{
0x0000000000000001, 0x0000000000008082, 0x800000000000808a, 0x8000000080008000, 0x000000000000808b, 0x0000000080000001, 0x8000000080008081, 0x8000000000008009, 0x000000000000008a, 0x0000000000000088, 0x0000000080008009, 0x000000008000000a, 0x000000008000808b, 0x800000000000008b, 0x8000000000008089, 0x8000000000008003, 0x8000000000008002, 0x8000000000000080, 0x000000000000800a, 0x800000008000000a, 0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008, };

#define K_I_0	10
#define K_I_1	 7
#define K_I_2	11
#define K_I_3	17
#define K_I_4	18
#define K_I_5	 3
#define K_I_6	 5
#define K_I_7	16
#define K_I_8	 8
#define K_I_9	21
#define K_I_10	24
#define K_I_11	 4
#define K_I_12	15
#define K_I_13	23
#define K_I_14	19
#define K_I_15	13
#define K_I_16	12
#define K_I_17	 2
#define K_I_18	20
#define K_I_19	14
#define K_I_20	22
#define K_I_21	 9
#define K_I_22	 6
#define K_I_23	 1

#define K_R_0	 1
#define K_R_1	 3
#define K_R_2	 6
#define K_R_3	10
#define K_R_4	15
#define K_R_5	21
#define K_R_6	28
#define K_R_7	36
#define K_R_8	45
#define K_R_9	55
#define K_R_10	 2
#define K_R_11	14
#define K_R_12	27
#define K_R_13	41
#define K_R_14	56
#define K_R_15	 8
#define K_R_16	25
#define K_R_17	43
#define K_R_18	62
#define K_R_19	18
#define K_R_20	39
#define K_R_21	61
#define K_R_22	20
#define K_R_23	44

/**
 * Loop operation.
 *
 * @param [in] s   The state.
 * @param [in] t   Temporary value.
 * @param [in] t2  Second temporary value.
 * @param [in] j   The index of the loop.
 */
#define L(s, t, t2, j)						\
do								\
{								\
    t2 = s[K_I_##j]; s[K_I_##j] = ROL(t, K_R_##j); t = t2;	\
}								\
while (0)

/**
 * The block operation performed on the state.
 *
 * @param [in] s  The state.
 */
static void ntru_keccak_block(uint64_t *s)
{
    uint8_t x, y, n;
    uint64_t t, t2;
    uint64_t b[5];

    for (n=0; n<24; n++)
    {
        for (x=0; x<5; x++)
        {
            b[x] = 0;
            for (y=0; y<5; y++)
                b[x] ^= s[x+y*5];
        }
        for (x=0; x<5; x++)
        {
            t = b[(x+4) % 5] ^ ROL(b[(x+1) % 5], 1);
            for (y=0; y<5; y++)
                s[x+y*5] ^= t;
        }

        t = s[1];
        L(s, t, t2,  0);
        L(s, t, t2,  1);
        L(s, t, t2,  2);
        L(s, t, t2,  3);
        L(s, t, t2,  4);
        L(s, t, t2,  5);
        L(s, t, t2,  6);
        L(s, t, t2,  7);
        L(s, t, t2,  8);
        L(s, t, t2,  9);
        L(s, t, t2, 10);
        L(s, t, t2, 11);
        L(s, t, t2, 12);
        L(s, t, t2, 13);
        L(s, t, t2, 14);
        L(s, t, t2, 15);
        L(s, t, t2, 16);
        L(s, t, t2, 17);
        L(s, t, t2, 18);
        L(s, t, t2, 19);
        L(s, t, t2, 20);
        L(s, t, t2, 21);
        L(s, t, t2, 22);
        L(s, t, t2, 23);

        for (y=0; y<5; y++)
        {
            for (x=0; x<5; x++)
                b[x] = s[x+y*5];
            for (x=0; x<5; x++)
                s[x+y*5] = b[x] ^ (~b[(x+1) % 5] & b[(x+2) % 5]);
        }
        s[0] ^= ntru_keccak_r[n];
    }
}

/**
 * Single shot hash operation.
 *
 * @param [in] r  The number of bytes of message to put in.
 * @param [in] m  The message data to hash.
 * @param [in] n  The length of the message data.
 * @param [in] p  The padding byte at the end of the message.
 * @param [in] h  The message digest data.
 * @param [in] b  The maximum length of output for one block.
 * @param [in] d  The number of bytes to output.
 */
static void ntru_keccak(uint8_t r, const uint8_t *m, uint64_t n, uint8_t p,
    uint8_t *h, uint64_t b, uint64_t d)
{
    uint64_t i, j;
    uint64_t s[25];
    uint8_t *s8 = (uint8_t *)s;
    uint8_t t[200];

    for (i=0; i<25; i++)
        s[i] = 0;
    while (n >= r)
    {
        for (i=0; i<r/8; i++)
            s[i] ^= ntru_keccak_le64(m+8*i);
        ntru_keccak_block(s);
        n -= r;
        m += r;
    }
    for (i=0; i<n; i++)
        t[i] = m[i];
    for (; i<r; i++)
        t[i] = 0;
    t[n] = p;
    t[r-1] |= 0x80;
    for (i=0; i<r/8; i++)
        s[i] ^= ntru_keccak_le64(t+8*i);
    ntru_keccak_block(s);
    for (i=0,j=0; i<d; i++,j++)
    {
        if (j == b)
        {
            j = 0;
            ntru_keccak_block(s);
        }
        h[i] = s8[j];
    }
}

/**
 * Initialize the SHA-3 digest context for hashing.
 *
 * @param [in] ctx  The context of the hash operation.
 * @return  1 on success.
 */
int ntru_sha3_init(NTRU_SHA3 *ctx)
{
    int i;

    for (i=0; i<25; i++)
        ctx->s[i] = 0;
    ctx->i = 0;

    return 1;
}

/**
 * Update the SHA-3 digest with more data.
 *
 * @param [in] ctx   The context of the hash operation.
 * @param [in] data  The data to digest.
 * @param [in] len   The length of the data to digest.
 * @return  1 on success.
 */
static int ntru_sha3_update(NTRU_SHA3 *ctx, const uint8_t *data, size_t len,
    uint8_t p)
{
    uint8_t i;
    uint8_t l;
    uint8_t *t;

    if (ctx->i > 0)
    {
        l = p*8 - ctx->i;
        if (l > len) l = len;

        t = &ctx->t[ctx->i];
        for (i=0; i<l; i++)
            t[i] = data[i];
        data += i;
        len -= i;

        if (i+ctx->i == p*8)
        {
            for (i=0; i<p; i++)
                ctx->s[i] ^= ntru_keccak_le64(ctx->t+8*i);
            ntru_keccak_block(ctx->s);
            ctx->i = 0;
        }
    }
    while (len >= p*8)
    {
        for (i=0; i<p; i++)
            ctx->s[i] ^= ntru_keccak_le64(data+8*i);
        ntru_keccak_block(ctx->s);
        len -= p*8;
        data += p*8;
    }
    for (i=0; i < len; i++)
        ctx->t[i] = data[i];
    ctx->i = i;
    
    return 1;
}

/**
 * Calculate the SHA-3 message digest based on all the message data seen.
 *
 * @param [in] md   The buffer to hold the message digest.
 * @param [in] ctx  The context of the hash operation.
 * @param [in] r    The number of 64-bit words from message data used in blocks.
 * @param [in] l    The length of the message digest.
 * @return  1 on success.
 */
static int ntru_sha3_final(unsigned char *md, NTRU_SHA3 *ctx, uint8_t r,
    uint8_t l)
{
    uint8_t i;
    uint8_t *s8 = (uint8_t *)ctx->s;

    ctx->t[r*8-1] = 0x00;
    ctx->t[ctx->i] = 0x06;
    ctx->t[r*8-1] |= 0x80;
    for (i=ctx->i+1; i<r*8-1; i++)
        ctx->t[i] = 0;
    for (i=0; i<r; i++)
        ctx->s[i] ^= ntru_keccak_le64(ctx->t+8*i);
    ntru_keccak_block(ctx->s);
    for (i=0; i<l; i++)
        md[i] = s8[i];

    return 1;
}

/**
 * Update the SHA-3_224 digest with more data.
 *
 * @param [in] ctx   The context of the hash operation.
 * @param [in] data  The data to digest.
 * @param [in] len   The length of the data to digest.
 * @return  1 on success.
 */
int ntru_sha3_224_update(NTRU_SHA3 *ctx, const uint8_t *data, size_t len)
{
    return ntru_sha3_update(ctx, data, len, 18);
}

/**
 * Calculate the SHA-3_224 message digest based on all the message data seen.
 *
 * @param [in] md   The buffer to hold the message digest.
 * @param [in] ctx  The context of the hash operation.
 * @param [in] r    The number of 64-bit words from message data used in blocks.
 * @param [in] l    The length of the message digest.
 * @return  1 on success.
 */
int ntru_sha3_224_final(unsigned char *md, NTRU_SHA3 *ctx)
{
    return ntru_sha3_final(md, ctx, 18, 28);
}

/**
 * Update the SHA-3_256 digest with more data.
 *
 * @param [in] ctx   The context of the hash operation.
 * @param [in] data  The data to digest.
 * @param [in] len   The length of the data to digest.
 * @return  1 on success.
 */
int ntru_sha3_256_update(NTRU_SHA3 *ctx, const uint8_t *data, size_t len)
{
    return ntru_sha3_update(ctx, data, len, 17);
}

/**
 * Calculate the SHA-3_256 message digest based on all the message data seen.
 *
 * @param [in] md   The buffer to hold the message digest.
 * @param [in] ctx  The context of the hash operation.
 * @param [in] r    The number of 64-bit words from message data used in blocks.
 * @param [in] l    The length of the message digest.
 * @return  1 on success.
 */
int ntru_sha3_256_final(unsigned char *md, NTRU_SHA3 *ctx)
{
    return ntru_sha3_final(md, ctx, 17, 32);
}

/**
 * Update the SHA-3_384 digest with more data.
 *
 * @param [in] ctx   The context of the hash operation.
 * @param [in] data  The data to digest.
 * @param [in] len   The length of the data to digest.
 * @return  1 on success.
 */
int ntru_sha3_384_update(NTRU_SHA3 *ctx, const uint8_t *data, size_t len)
{
    return ntru_sha3_update(ctx, data, len, 13);
}

/**
 * Calculate the SHA-3_384 message digest based on all the message data seen.
 *
 * @param [in] md   The buffer to hold the message digest.
 * @param [in] ctx  The context of the hash operation.
 * @param [in] r    The number of 64-bit words from message data used in blocks.
 * @param [in] l    The length of the message digest.
 * @return  1 on success.
 */
int ntru_sha3_384_final(unsigned char *md, NTRU_SHA3 *ctx)
{
    return ntru_sha3_final(md, ctx, 13, 48);
}

/**
 * Update the SHA-3_512 digest with more data.
 *
 * @param [in] ctx   The context of the hash operation.
 * @param [in] data  The data to digest.
 * @param [in] len   The length of the data to digest.
 * @return  1 on success.
 */
int ntru_sha3_512_update(NTRU_SHA3 *ctx, const uint8_t *data, size_t len)
{
    return ntru_sha3_update(ctx, data, len, 9);
}

/**
 * Calculate the SHA-3_512 message digest based on all the message data seen.
 *
 * @param [in] md   The buffer to hold the message digest.
 * @param [in] ctx  The context of the hash operation.
 * @param [in] r    The number of 64-bit words from message data used in blocks.
 * @param [in] l    The length of the message digest.
 * @return  1 on success.
 */
int ntru_sha3_512_final(unsigned char *md, NTRU_SHA3 *ctx)
{
    return ntru_sha3_final(md, ctx, 9, 64);
}

/**
 * Single shot hash operation of SHAKE-128.
 *
 * @param [in] h  The message digest data.
 * @param [in] l  The number of bytes to output.
 * @param [in] m  The message data to hash.
 * @param [in] n  The length of the message data.
 * @return  1 on success.
 */
int ntru_shake128(uint8_t *h, uint64_t l, const uint8_t *m, uint64_t n)
{
    ntru_keccak(21*8, m, n, 0x1f, h, 168, l);
    return 1;
}
/**
 * Single shot hash operation of SHAKE-256.
 *
 * @param [in] h  The message digest data.
 * @param [in] l  The number of bytes to output.
 * @param [in] m  The message data to hash.
 * @param [in] n  The length of the message data.
 * @return  1 on success.
 */
int ntru_shake256(uint8_t *h, uint64_t l, const uint8_t *m, uint64_t n)
{
    ntru_keccak(17*8, m, n, 0x1f, h, 136, l);
    return 1;
}
/**
 * Single shot hash operation of SHA-3_224.
 *
 * @param [in] h  The message digest data.
 * @param [in] l  The number of bytes to output.
 * @param [in] m  The message data to hash.
 * @param [in] n  The length of the message data.
 * @return  1 on success.
 */
int ntru_sha3_224(uint8_t *h, const uint8_t *m, uint64_t n)
{
    ntru_keccak(18*8, m, n, 0x06, h, 28, 28);
    return 1;
}
/**
 * Single shot hash operation of SHA-3_256.
 *
 * @param [in] h  The message digest data.
 * @param [in] l  The number of bytes to output.
 * @param [in] m  The message data to hash.
 * @param [in] n  The length of the message data.
 * @return  1 on success.
 */
int ntru_sha3_256(uint8_t *h, const uint8_t *m, uint64_t n)
{
    ntru_keccak(17*8, m, n, 0x06, h, 32, 32);
    return 1;
}
/**
 * Single shot hash operation of SHA-3_384.
 *
 * @param [in] h  The message digest data.
 * @param [in] l  The number of bytes to output.
 * @param [in] m  The message data to hash.
 * @param [in] n  The length of the message data.
 * @return  1 on success.
 */
int ntru_sha3_384(uint8_t *h, const uint8_t *m, uint64_t n)
{
    ntru_keccak(13*8, m, n, 0x06, h, 48, 48);
    return 1;
}
/**
 * Single shot hash operation of SHA-3_512.
 *
 * @param [in] h  The message digest data.
 * @param [in] l  The number of bytes to output.
 * @param [in] m  The message data to hash.
 * @param [in] n  The length of the message data.
 * @return  1 on success.
 */
int ntru_sha3_512(uint8_t *h, const uint8_t *m, uint64_t n)
{
    ntru_keccak(9*8, m, n, 0x06, h, 64, 64);
    return 1;
}

