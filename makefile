# Copyright (c) 2016 Sean Parkinson
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

CC=gcc
CFLAGS=-O3 -m64 -Wall -DCPU_X86_64 -DCC_GCC -Iinclude
#-DNTRUENC_SMALL_CODE
#CFLAGS=-g -m64 -Wall -DCPU_X86_64 -DCC_GCC -Iinclude
LIBS=
#CFLAGS+=-DOPT_NTRU_OPENSSL_RAND
#LIBS+=-lcrypto

all: ntruenc_test

#ASM=src/asm/ntruenc_s112_x64.s src/asm/ntruenc_s128_x64.s src/asm/ntruenc_s192_x64.s src/asm/ntruenc_s256_x64.s
#ASM_OBJ=ntruenc_s112_x64.o ntruenc_s128_x64.o ntruenc_s192_x64.o ntruenc_s256_x64.o
NTRUENC_MUL_Q=ntruenc_s112_mul_q.o ntruenc_s128_mul_q.o ntruenc_s192_mul_q.o ntruenc_s256_mul_q.o
NTRUENC_IMPL=ntruenc_s112.o ntruenc_s128.o ntruenc_s192.o ntruenc_s256.o $(NTRUENC_MUL_Q)

NTRUENC_OP_OBJ=$(NTRUENC_IMPL) $(ASM_OBJ)

NTRUENC_OBJ=ntruenc.o $(NTRUENC_OP_OBJ) ntruenc_key.o random.o

%.o: src/%.c src/*.h include/*.h
	$(CC) -c $(CFLAGS) -o $@ $<

src/mul/ntruenc_s112_mul_q.c: src/mul/ntruenc_kara.rb
	ruby src/mul/ntruenc_kara.rb 112 >src/mul/ntruenc_s112_mul_q.c
src/mul/ntruenc_s128_mul_q.c: src/mul/ntruenc_kara.rb
	ruby src/mul/ntruenc_kara.rb 128 >src/mul/ntruenc_s128_mul_q.c
src/mul/ntruenc_s192_mul_q.c: src/mul/ntruenc_kara.rb
	ruby src/mul/ntruenc_kara.rb 192 >src/mul/ntruenc_s192_mul_q.c
src/mul/ntruenc_s256_mul_q.c: src/mul/ntruenc_kara.rb
	ruby src/mul/ntruenc_kara.rb 256 >src/mul/ntruenc_s256_mul_q.c
ntruenc_s112_mul_q.o: src/mul/ntruenc_s112_mul_q.c src/*.h include/*.h
	$(CC) -c $(CFLAGS) -Isrc -o $@ $<
ntruenc_s128_mul_q.o: src/mul/ntruenc_s128_mul_q.c src/*.h include/*.h
	$(CC) -c $(CFLAGS) -Isrc -o $@ $<
ntruenc_s192_mul_q.o: src/mul/ntruenc_s192_mul_q.c src/*.h include/*.h
	$(CC) -c $(CFLAGS) -Isrc -o $@ $<
ntruenc_s256_mul_q.o: src/mul/ntruenc_s256_mul_q.c src/*.h include/*.h
	$(CC) -c $(CFLAGS) -Isrc -o $@ $<

#src/asm/ntruenc_s112_x64.s: src/asm/ntruenc_asm.rb rubyasm/x86_asm.rb
#	ruby src/asm/ntruenc_asm.rb 112 > $@
#ntruenc_s112_x64.o: src/asm/ntruenc_s112_x64.s
#	$(CC) -c -o $@ $<
#
#src/asm/ntruenc_s128_x64.s: src/asm/ntruenc_asm.rb rubyasm/x86_asm.rb
#	ruby src/asm/ntruenc_asm.rb 128 > $@
#ntruenc_s128_x64.o: src/asm/ntruenc_s128_x64.s
#	$(CC) -c -o $@ $<
#
#src/asm/ntruenc_s192_x64.s: src/asm/ntruenc_asm.rb rubyasm/x86_asm.rb
#	ruby src/asm/ntruenc_asm.rb 192 > $@
#ntruenc_s192_x64.o: src/asm/ntruenc_s192_x64.s
#	$(CC) -c -o $@ $<
#
#src/asm/ntruenc_s256_x64.s: src/asm/ntruenc_asm.rb rubyasm/x86_asm.rb
#	ruby src/asm/ntruenc_asm.rb 256 > $@
#ntruenc_s256_x64.o: src/asm/ntruenc_s256_x64.s
#	$(CC) -c -o $@ $<
#
#asm: $(ASM)

ntruenc_test.o: test/ntruenc_test.c
	$(CC) -c $(CFLAGS) -Isrc -o $@ $<

ntruenc_test: ntruenc_test.o $(NTRUENC_OBJ)
	$(CC) -o $@ $^ $(LIBS)
clean:
	rm *.o
	rm ntruenc_test
