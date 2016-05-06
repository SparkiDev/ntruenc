
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
static int valid[] = {112, 128, 192, 256};
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
 * @param [in] data     The vectorized message or key.
 * @param [in] pub_key  The public key.
 */
void enc_cycles(short *data, NTRUENC_PUB_KEY *pub_key)
{
    int i;
    uint64_t start, end, diff;
    int num_ops;
    short *enc;
    int len;

    NTRUENC_PUB_KEY_get_enc_len(pub_key, &len);
    enc = malloc(len);

    NTRUENC_encrypt_init(pub_key);

    /* Prime the caches, etc */
    for (i=0; i<200; i++)
        NTRUENC_encrypt(enc, data, pub_key);

    /* Approximate number of ops in a second. */
    start = get_cycles();
    for (i=0; i<100; i++)
        NTRUENC_encrypt(enc, data, pub_key);
    end = get_cycles();
    num_ops = cps/((end-start)/100);

    /* Perform about 1 seconds worth of operations. */
    start = get_cycles();
    for (i=0; i<num_ops; i++)
        NTRUENC_encrypt(enc, data, pub_key);
    end = get_cycles();

    diff = end - start;

    printf("enc: %7d %2.3f  %7"PRIu64" %6"PRIu64"\n", num_ops, diff/(cps*1.0),
        diff/num_ops, cps/(diff/num_ops));

    NTRUENC_encrypt_final(pub_key);

    free(enc);
}

/*
 * Determine the number of decryption operations that can be performed per
 * second.
 *
 * @param [in] enc       Encrypted data.
 * @param [in] priv_key  The private key.
 */
void dec_cycles(short *enc, NTRUENC_PRIV_KEY *priv_key)
{
    int i;
    uint64_t start, end, diff;
    int num_ops;
    short *dec;
    int n;

    NTRUENC_PRIV_KEY_get_len(priv_key, &n);
    dec = malloc(n * sizeof(enc));

    NTRUENC_decrypt_init(priv_key);

    /* Prime the caches, etc */
    for (i=0; i<200; i++)
        NTRUENC_decrypt(dec, enc, priv_key);

    /* Approximate number of ops in a second. */
    start = get_cycles();
    for (i=0; i<100; i++)
        NTRUENC_decrypt(dec, enc, priv_key);
    end = get_cycles();
    num_ops = cps/((end-start)/100);

    /* Perform about 1 seconds worth of operations. */
    start = get_cycles();
    for (i=0; i<num_ops; i++)
        NTRUENC_decrypt(dec, enc, priv_key);
    end = get_cycles();

    diff = end - start;

    printf("dec: %7d %2.3f  %7"PRIu64" %6"PRIu64"\n", num_ops, diff/(cps*1.0),
        diff/num_ops, cps/(diff/num_ops));

    NTRUENC_decrypt_final(priv_key);

    free(dec);
}

/*
 * Determine the number of key generation operations that can be performed per
 * second.
 *
 * @param [in] priv_key  The public key.
 * @param [in] pub_key   The public key.
 */
void keygen_cycles(NTRUENC_PRIV_KEY *priv_key, NTRUENC_PUB_KEY *pub_key)
{
    int i;
    uint64_t start, end, diff;
    int num_ops;

    NTRUENC_keygen_init(priv_key, pub_key);
    /* Prime the caches, etc */
    for (i=0; i<100; i++)
        NTRUENC_keygen(priv_key, pub_key);

    /* Approximate number of ops in a second. */
    start = get_cycles();
    for (i=0; i<100; i++)
        NTRUENC_keygen(priv_key, pub_key);
    end = get_cycles();
    num_ops = cps/((end-start)/100);

    /* Perform about 1 seconds worth of operations. */
    start = get_cycles();
    for (i=0; i<num_ops; i++)
        NTRUENC_keygen(priv_key, pub_key);
    end = get_cycles();

    diff = end - start;

    printf("kgn: %7d %2.3f  %7"PRIu64" %6"PRIu64"\n", num_ops, diff/(cps*1.0),
        diff/num_ops, cps/(diff/num_ops));

    NTRUENC_keygen_final(priv_key, pub_key);
}

/*
 * Generate a random vector as the hash.
 *
 * @param [in] hash      The NTRU vector to fill.
 * @param [in] n         The number of elements in the vector.
 * @param [in] strength  The strength of the hash.
 */
int random_data(short *hash, int n, int strength)
{
    int ret;
    int i;

    memset(hash, 0, n*sizeof(*hash));
    ret = pseudo_random((unsigned char *)hash, strength*4);
    if (ret != 0)
        goto end;

    for (i=0; i<strength*2; i++)
        hash[i] = ((hash[i] & 1) << 1) - 1;

end:
    return ret;
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
    NTRUENC_PRIV_KEY *priv_key = NULL;
    NTRUENC_PUB_KEY *pub_key = NULL;
    short *data = NULL;
    short *dec = NULL;
    short *enc = NULL;
    int n;
    int len;
    int i;

    printf("Strength: %d\n", strength);

    /* Create empty private and public keys. */
    ret = NTRUENC_PRIV_KEY_new(strength, flags, &priv_key);
    if (ret != 0)
        goto end;
    ret = NTRUENC_PUB_KEY_new(strength, flags, &pub_key);
    if (ret != 0)
        goto end;

    NTRUENC_PUB_KEY_get_len(pub_key, &n);
    NTRUENC_PUB_KEY_get_enc_len(pub_key, &len);
    printf("Vector Length: %d elements, Encrypted Length: %d bytes\n", n, len);

    /* Allocate memory for hash and signature. */
    data = malloc(n*sizeof(short));
    enc = malloc(len);
    dec = malloc(len);
    if ((data == NULL) || (enc == NULL) || (dec == NULL))
        goto end;

    /* Create vectorized data. */
    ret = random_data(data, n, strength);
    fprintf(stderr, "Data: %d", ret);
    if (ret != 0)
        goto end;

    /* Generate private and public keys. */
    ret = NTRUENC_keygen_init(priv_key, pub_key);
    fprintf(stderr, ", kg init: %d", ret);
    if (ret != 0)
        goto end;
    ret = NTRUENC_keygen(priv_key, pub_key);
    fprintf(stderr, ", kg: %d", ret);
    NTRUENC_keygen_final(priv_key, pub_key);
    if (ret != 0)
        goto end;

    ret = NTRUENC_encrypt_init(pub_key);
    fprintf(stderr, ", enc init: %d", ret);
    if (ret != 0)
        goto end;
    ret = NTRUENC_encrypt(enc, data, pub_key);
    fprintf(stderr, ", enc: %d", ret);
    NTRUENC_encrypt_final(pub_key);
    if (ret != 0)
        goto end;

    ret = NTRUENC_decrypt_init(priv_key);
    fprintf(stderr, ", dec init: %d", ret);
    if (ret != 0)
        goto end;
    NTRUENC_decrypt(dec, enc, priv_key);
    fprintf(stderr, ", decrypt: ");
    NTRUENC_decrypt_final(priv_key);
    for (i=0; i<n; i++)
    {
        if (dec[i] != data[i])
        {
            printf("Diff (%d, %d/%d)", i, dec[i], data[i]);
            ret = 1;
            goto end;
        }
    }
    fprintf(stderr, "equal");

    if (speed)
    {
        printf("\n");
        printf(" Op  %7s %5s  %7s %6s\n", "ops", "secs", "c/op", "ops/s");
        enc_cycles(data, pub_key);
        dec_cycles(enc, priv_key);
        keygen_cycles(priv_key, pub_key);
    }

end:
    printf("\n");
    /* Cleanup dynamic memory. */
    if (dec != NULL) free(dec);
    if (enc != NULL) free(enc);
    if (data != NULL) free(data);
    NTRUENC_PUB_KEY_free(pub_key);
    NTRUENC_PRIV_KEY_free(priv_key);

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

