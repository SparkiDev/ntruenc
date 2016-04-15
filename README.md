NTRU Sig
========

Open Source NTRU Encryption Algorithm and Implementation

Written by Sean Parkinson (sparkinson@iprimus.com.au)

For information on copyright see: COPYRIGHT.md

Contents of Repository
----------------------

This is the code that implements the NTRU Encryption algorithms.

There are key generation, encryption and decryption APIs.

Different parameters are used based on the security strength required.
Four security strengths are supported: 112, 128, 192, 256.
The parameters are based on those specified in:
Jeff Hoffstein, Jill Pipher, John M. Schanck, Joseph H. Silverman, William Whyte, and Zhenfei Zhang; "Choosing Parameters for NTRUEncrypt"; https://eprint.iacr.org/2015/708.pdf

The code is small and fast. Uses Karatsuba multiplication for speed.

The arithmetic operations are side-channel resistant.

NTRU Encryption
---------------------

```
Key:
  [priv] f = 1 - p.f' mod q
  [pub]  h = g.p.f^1 mod q

Encryption:
  t = random mod q
  e = h.t + m mod q

Decryption:
  d = f.e mod q mod p
```

Building
--------

make

Testing
-------

Run all tests: ntruenc_test -no_asm

Run all tests and calculate speed: ntruenc_test -speed -no_asm


Performance
-----------

Example of ntruenc_test output on a 3.4 GHz Intel Ivy Bridge CPU:

```
Cycles/sec: 3400248956
Strength: 112
Vector Length: 401 elements, Encrypted Length: 802 bytes
Data: 0, kg init: 0, kg: 0, enc init: 0, enc: 0, dec init: 0, decrypt: equal
 Op      ops  secs     c/op  ops/s
enc:   28136 0.413    49910  68127
dec:  115118 1.001    29563 115017
kgn:    8553 0.996   395867   8589

Strength: 128
Vector Length: 439 elements, Encrypted Length: 878 bytes
Data: 0, kg init: 0, kg: 0, enc init: 0, enc: 0, dec init: 0, decrypt: equal
 Op      ops  secs     c/op  ops/s
enc:   61390 1.000    55375  61404
dec:  101758 1.002    33475 101575
kgn:    7499 0.983   445911   7625

Strength: 192
Vector Length: 593 elements, Encrypted Length: 1186 bytes
Data: 0, kg init: 0, kg: 0, enc init: 0, enc: 0, dec init: 0, decrypt: equal
 Op      ops  secs     c/op  ops/s
enc:   37333 1.001    91153  37302
dec:   55322 1.002    61569  55226
kgn:    4365 0.988   769923   4416

Strength: 256
Vector Length: 743 elements, Encrypted Length: 1486 bytes
Data: 0, kg init: 0, kg: 0, enc init: 0, enc: 0, dec init: 0, decrypt: equal
 Op      ops  secs     c/op  ops/s
enc:   29113 1.003   117175  29018
dec:   42427 1.003    80365  42310
kgn:    2957 1.000  1150239   2956
```
