
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "ntruenc.h"
#include "ntruenc_key_lcl.h"
#include "random.h"

#ifdef CC_CLANG
#define PRIu64 "llu"
#else
#define PRIu64 "lu"
#endif

/* Valid strength values for test. */
static int valid[] = {112, 128, 192, 215, 256};
/* The number of valid values for test. */
#define VALID_NUM    (int)(sizeof(valid)/sizeof(*valid))

/* Number of cycles/sec. */
uint64_t cps = 0;

/*
 * Get the current cycle count from the CPU.
 *
 * @return  Cycle counter from CPU.
 */
uint64_t get_cycles()
{
    unsigned int hi, lo;

    asm volatile ("rdtsc\n\t" : "=a" (lo), "=d"(hi));
    return ((uint64_t)lo) | (((uint64_t)hi) << 32);
}

/*
 * Calculate the number of cycles/second.
 */
void calc_cps()
{
    uint64_t end, start = get_cycles();
    sleep(1);
    end = get_cycles();
    cps = end-start;
    printf("Cycles/sec: %"PRIu64"\n", cps);
}

/*
 * Determine the number of encryption operations that can be performed per
 * second.
 *
 * @param [in] ne       The NTRU Encryption operation object.
 * @param [in] data     The message or key data.
 * @param [in] len      The length of the message or key data.
 * @param [in] pub_key  The public key.
 */
void enc_cycles(NTRUENC *ne, unsigned char *data, int len,
    NTRUENC_PUB_KEY *pub_key)
{
    int i;
    uint64_t start, end, diff;
    int num_ops;
    unsigned char *enc;
    int elen;

    NTRUENC_PUB_KEY_get_enc_len(pub_key, &elen);
    enc = malloc(elen);

    NTRUENC_encrypt_init(ne, pub_key);

    /* Prime the caches, etc */
    for (i=0; i<200; i++)
        NTRUENC_encrypt(ne, data, len, enc, elen);

    /* Approximate number of ops in a second. */
    start = get_cycles();
    for (i=0; i<100; i++)
        NTRUENC_encrypt(ne, data, len, enc, elen);
    end = get_cycles();
    num_ops = cps/((end-start)/100);

    /* Perform about 1 seconds worth of operations. */
    start = get_cycles();
    for (i=0; i<num_ops; i++)
        NTRUENC_encrypt(ne, data, len, enc, elen);
    end = get_cycles();

    diff = end - start;

    printf("enc: %7d %2.3f  %7"PRIu64" %6"PRIu64"\n", num_ops, diff/(cps*1.0),
        diff/num_ops, cps/(diff/num_ops));

    NTRUENC_encrypt_final(ne);

    free(enc);
}

/*
 * Determine the number of decryption operations that can be performed per
 * second.
 *
 * @param [in] ne        The NTRU Encryption operation object.
 * @param [in] enc       Encrypted data.
 * @param [in] elen      The length of the encrypted data.
 * @param [in] priv_key  The private key.
 */
void dec_cycles(NTRUENC *ne, unsigned char *enc, int elen,
    NTRUENC_PRIV_KEY *priv_key)
{
    int i;
    uint64_t start, end, diff;
    int num_ops;
    unsigned char *dec;
    int len;
    int olen;
    int n;

    NTRUENC_PRIV_KEY_num_entries(priv_key, &n);
    len = (n + 7) / 8;
    dec = malloc(len);

    NTRUENC_decrypt_init(ne, priv_key);

    /* Prime the caches, etc */
    for (i=0; i<200; i++)
        NTRUENC_decrypt(ne, enc, elen, dec, len, &olen);

    /* Approximate number of ops in a second. */
    start = get_cycles();
    for (i=0; i<100; i++)
        NTRUENC_decrypt(ne, enc, elen, dec, len, &olen);
    end = get_cycles();
    num_ops = cps/((end-start)/100);

    /* Perform about 1 seconds worth of operations. */
    start = get_cycles();
    for (i=0; i<num_ops; i++)
        NTRUENC_decrypt(ne, enc, elen, dec, len, &olen);
    end = get_cycles();

    diff = end - start;

    printf("dec: %7d %2.3f  %7"PRIu64" %6"PRIu64"\n", num_ops, diff/(cps*1.0),
        diff/num_ops, cps/(diff/num_ops));

    NTRUENC_decrypt_final(ne);

    free(dec);
}

/*
 * Determine the number of key generation operations that can be performed per
 * second.
 *
 * @param [in] priv_key  The public key.
 * @param [in] pub_key   The public key.
 */
void keygen_cycles(NTRUENC *ne, NTRUENC_PARAMS *params,
    NTRUENC_PRIV_KEY *priv_key, NTRUENC_PUB_KEY *pub_key)
{
    int i;
    uint64_t start, end, diff;
    int num_ops;

    NTRUENC_keygen_init(ne, params);
    /* Prime the caches, etc */
    for (i=0; i<20; i++)
        NTRUENC_keygen(ne, &priv_key, &pub_key);

    /* Approximate number of ops in a second. */
    start = get_cycles();
    for (i=0; i<20; i++)
        NTRUENC_keygen(ne, &priv_key, &pub_key);
    end = get_cycles();
    num_ops = cps/((end-start)/20);

    /* Perform about 1 seconds worth of operations. */
    start = get_cycles();
    for (i=0; i<num_ops; i++)
        NTRUENC_keygen(ne, &priv_key, &pub_key);
    end = get_cycles();

    diff = end - start;

    printf("kgn: %7d %2.3f  %7"PRIu64" %6"PRIu64"\n", num_ops, diff/(cps*1.0),
        diff/num_ops, cps/(diff/num_ops));

    NTRUENC_keygen_final(ne);
}

/*
 * Generate a random vector as the hash.
 *
 * @param [in] msg       The message buffer.
 * @param [in] len       The length of the buffer.
 */
int random_data(unsigned char *msg, int len)
{
    return pseudo_random(msg, len);
}

/*
 * Test an implementation of the NTRU Encryption scheme.
 *
 * @param [in] strength  The rquired strength of the implementation.
 * @param [in] flags     The extra requirements on the methods to choose.
 * @param [in] speed     Indicates whether to calculate speed of operations.
 * @return  0 on successful testing.<br>
 *          1 otherwise.
 */
int test_ntruenc(int strength, int flags, int speed)
{
    int ret;
    NTRUENC_PARAMS *params = NULL;
    NTRUENC_PRIV_KEY *priv_key = NULL;
    NTRUENC_PUB_KEY *pub_key = NULL;
    NTRUENC_PRIV_KEY *priv_key_gen = NULL;
    NTRUENC_PUB_KEY *pub_key_gen = NULL;
    NTRUENC *ne = NULL;
    unsigned char *data = NULL;
    int len;
    unsigned char *dec = NULL;
    int olen;
    unsigned char *enc = NULL;
    int elen;
    int n;
    unsigned char *priv = NULL;
    unsigned char *pub = NULL;
    int priv_len, pub_len;
    int i;

    printf("Strength: %d\n", strength);

    ret = NTRUENC_PARAMS_get(strength, &params);
    if (ret != 0)
        goto end;
    /* Create empty private and public keys. */
    ret = NTRUENC_PRIV_KEY_new(params, &priv_key);
    if (ret != 0)
        goto end;
    ret = NTRUENC_PUB_KEY_new(params, &pub_key);
    if (ret != 0)
        goto end;
    ret = NTRUENC_PRIV_KEY_new(params, &priv_key_gen);
    if (ret != 0)
        goto end;
    ret = NTRUENC_PUB_KEY_new(params, &pub_key_gen);
    if (ret != 0)
        goto end;

    NTRUENC_PUB_KEY_num_entries(pub_key, &n);
    NTRUENC_PUB_KEY_get_enc_len(pub_key, &elen);
    NTRUENC_PRIV_KEY_get_len(priv_key, &priv_len);
    NTRUENC_PUB_KEY_get_len(pub_key, &pub_len);
    printf("Vector Length: %d elements, Encrypted Length: %d bytes\n", n, elen);
    printf("Private Key Length: %d bytes, Public Key Length: %d bytes\n",
        priv_len, pub_len);

    len = strength / 4;
    /* Allocate memory for hash and signature. */
    data = malloc(n*sizeof(short));
    enc = malloc(elen);
    dec = malloc(len);
    priv = malloc(priv_len);
    pub = malloc(pub_len);
    if ((data == NULL) || (enc == NULL) || (dec == NULL) || (priv == NULL) ||
        (pub == NULL))
        goto end;

    /* Create vectorized data. */
    ret = random_data(data, len);
    fprintf(stderr, "Data: %d", ret);
    if (ret != 0)
        goto end;

    ret = NTRUENC_new(strength, flags, &ne);
    if (ret != 0)
        goto end;

    /* Generate private and public keys. */
    ret = NTRUENC_keygen_init(ne, params);
    fprintf(stderr, ", kg init: %d", ret);
    if (ret != 0)
        goto end;
    ret = NTRUENC_keygen(ne, &priv_key_gen, &pub_key_gen);
    fprintf(stderr, ", kg: %d", ret);
    NTRUENC_keygen_final(ne);
    if (ret != 0)
        goto end;

    ret = NTRUENC_PRIV_KEY_encode(priv_key_gen, priv, priv_len);
    if (ret != 0)
        goto end;
    ret = NTRUENC_PRIV_KEY_decode(priv_key, priv, priv_len);
    if (ret != 0)
        goto end;

    ret = NTRUENC_PUB_KEY_encode(pub_key_gen, pub, pub_len);
    if (ret != 0)
        goto end;
    ret = NTRUENC_PUB_KEY_decode(pub_key, pub, pub_len);
    if (ret != 0)
        goto end;

    ret = NTRUENC_encrypt_init(ne, pub_key);
    fprintf(stderr, ", enc init: %d", ret);
    if (ret != 0)
        goto end;
    ret = NTRUENC_encrypt(ne, data, len, enc, elen);
    fprintf(stderr, ", enc: %d", ret);
    NTRUENC_encrypt_final(ne);
    if (ret != 0)
        goto end;

    ret = NTRUENC_decrypt_init(ne, priv_key);
    fprintf(stderr, ", dec init: %d", ret);
    if (ret != 0)
        goto end;
    ret = NTRUENC_decrypt(ne, enc, elen, dec, len, &olen);
    fprintf(stderr, ", decrypt: %d", ret);
    NTRUENC_decrypt_final(ne);
    for (i=0; i<olen; i++)
    {
        if (dec[i] != data[i])
        {
            printf(",Diff (%d, %d/%d)", i, dec[i], data[i]);
            ret = 1;
            goto end;
        }
    }
    fprintf(stderr, ",%d", olen);

    if (speed)
    {
        printf("\n");
        printf(" Op  %7s %5s  %7s %6s\n", "ops", "secs", "c/op", "ops/s");
        enc_cycles(ne, data, len, pub_key);
        dec_cycles(ne, enc, elen, priv_key);
        keygen_cycles(ne, params, priv_key, pub_key);
    }

end:
    printf("\n");
    /* Cleanup dynamic memory. */
    if (pub != NULL) free(pub);
    if (priv != NULL) free(priv);
    if (dec != NULL) free(dec);
    if (enc != NULL) free(enc);
    if (data != NULL) free(data);
    NTRUENC_free(ne);
    NTRUENC_PUB_KEY_free(pub_key);
    NTRUENC_PRIV_KEY_free(priv_key);
    NTRUENC_PUB_KEY_free(pub_key_gen);
    NTRUENC_PRIV_KEY_free(priv_key_gen);

    return ret;
}

/*
 * Main entry point of program.<br>
 *  -speed   Test the speed of operations in cycles and per second.<br>
 *  <strength>  The algorithm strength to test.
 *
 * @param [in] argc  The count of command line arguments.
 * @param [in] argv  The command line arguments.
 * @return  0 on success.<br>
 *          1 on test failure.
 */
int main(int argc, char *argv[])
{
    int ret = 0;
    int i;
    int s;
    int which = 0;
    int speed = 0;
    int flags = 0;

    while (--argc)
    {
        argv++;

        if (strcmp(*argv, "-speed") == 0)
            speed = 1;
        else
        {
            s = atoi(*argv);
            /* If strength value is valid then set corresponding bit. */
            for (i=0; i<VALID_NUM; i++)
            {
                if (valid[i] == s)
                    which |= 1<<i;
            }
        }
    }

    if (speed)
        calc_cps();

    /* Test all  */
    for (i=0; i<VALID_NUM; i++)
    {
        if ((which == 0) || ((which & (1<<i)) != 0))
            ret |= test_ntruenc(valid[i], flags, speed);
    }

    return (ret != 0);
}

