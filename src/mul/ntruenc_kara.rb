#!/usr/bin/ruby
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


class NTRUENC_Karatsuba

  def initialize(str, q, n, r)
    @str = str
    @q = q
    @n = n
    @r = r
    @s = [n]
    t = @n
    1.upto(r+1) do
      @s << (t+1)/2
      t = (t+1)/2
    end
  end

  def write_small()
    n = @s[@r]
    puts <<EOF
/**
 * Simple multiplication of two NTRU vectors.
 *
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 * @param [in] t  Dynamicly allocated data used during life of function.
 */
static void ntruenc_s#{@str}_mul_mod_q_small(int64_t *r, int16_t *a, int16_t *b,
    void *t)
{
    int i, j;
    int64_t *p;

    for (j=0; j<#{n}; j++)
        r[j] = (int32_t)a[0] * b[j];
    for (i=1; i<#{n}; i++)
    {
        r[i+#{n}-1] = 0;
        p = &r[i];
        for (j=0; j<#{n}; j++)
            p[j] += (int32_t)a[i] * b[j];
    }
}
EOF
  end

  def write_karatsuba(c)
    n = @s[c]
    nf = @s[c-1]
    nm = n
    extra_ops = ""
    if nf & 1 == 1
      nm -= 1
      extra_ops = <<EOF

    aa[#{nm}] = 0;
    bb[#{nm}] = 0;
EOF
    end

    func = "ntruenc_s#{@str}_mul_mod_q"
    func += "_#{nf}" if c != 1
    static_decl = ""
    static_decl = "static " if c != 1
    res_type = "short"
    res_type = "int64_t" if c != 1
    op_type = "short"
    op_type = "int16_t" if c != 1
    next_func = "ntruenc_s#{@str}_mul_mod_q_"
    if c == @r
      next_func += "small"
    else
      next_func += @s[c].to_s
    end
    dec_j = ""
    dec_j = ", j, k" if c == 1
    add_mod = ""
    add_mod = " % NTRU_S#{@str}_Q" if @q > 8192 or (@r - c) & 1 == 0

    add_ops = ""
    if c == 1
       add_ops = <<EOF
    k = #{nf}-#{n};
    r[0] = (t1[0] + t2[k] - t1[k] - t3[k]) % NTRU_S#{@str}_Q;
    for (i=1,j=0,k++; i<#{n}; i++,j++,k++)
        r[i] = (t1[i] + t3[j] + t2[k] - t1[k] - t3[k]) % NTRU_S#{@str}_Q;
    for (k=0; i<#{nf}; i++,j++,k++)
        r[i] = (t1[i] + t3[j] + t2[k] - t1[k] - t3[k]) % NTRU_S#{@str}_Q;

    for (i=0; i<#{nf}; i++)
    {
        if (r[i] > NTRU_S#{@str}_Q / 2)
            r[i] = -(NTRU_S#{@str}_Q - r[i]);
        if (r[i] < -NTRU_S#{@str}_Q / 2)
            r[i] = NTRU_S#{@str}_Q + r[i];
    }
EOF
    elsif nf & 1 == 0
       add_ops = <<EOF
    for (i=0; i<#{n}; i++)
        r[i] = t1[i];
    for (i=0; i<#{n}-1; i++)
        r[i+#{n}] = (t1[i+#{n}] + t2[i] - t1[i] - t3[i]);
    r[#{n}*2-1] = (t2[#{n}-1] - t1[#{n}-1] - t3[#{n}-1]);
    for (i=0; i<#{n}-1; i++)
        r[i+2*#{n}] = (t2[i+#{n}] - t1[i+#{n}] - t3[i+#{n}] + t3[i]);
    for (; i<#{n}*2-1; i++)
        r[i+2*#{n}] = t3[i];
EOF
    else
        add_ops = <<EOF
    t3[#{n}*2-2] = 0;
    for (i=0; i<#{n}; i++)
        r[i] = t1[i];
    for (i=0; i<#{n}-1; i++)
        r[i+#{n}] = (t1[i+#{n}] + t2[i] - t1[i] - t3[i]);
    r[#{n}*2-1] = (t2[#{n}-1] - t1[#{n}-1] - t3[#{n}-1]);
    for (i=0; i<#{n}-1; i++)
        r[i+2*#{n}] = (t2[i+#{n}] - t1[i+#{n}] - t3[i+#{n}] + t3[i]);
    for (; i<#{n}*2-1; i++)
        r[i+2*#{n}] = t3[i];
EOF
    end

    # Write out function
    puts <<EOF

/**
 * Karatsuba multiplication of two NTRU vectors.
 *
 * @param [in] r   The multiplication result.
 * @param [in] a   The first operand.
 * @param [in] b   The second operand.
 * @param [in] tp  Dynamicly allocated data used during life of function.
 */
#{static_decl}void #{func}(#{res_type} *r, #{op_type} *a, #{op_type} *b,
    void *tp)
{
    int i#{dec_j};
    int64_t *t1 = tp;
    int64_t *t2 = t1 + (2*#{n}+1);
    int64_t *t3 = t2 + (2*#{n}+1);
    int64_t *t = t3 + (2*#{n}+1);
    int16_t aa[#{n}];
    int16_t bb[#{n}];

    for (i=0; i<#{nm}; i++)
    {
        aa[i] = a[i+#{n}];
        bb[i] = b[i+#{n}];
    }#{extra_ops}
    #{next_func}(t3, aa, bb, t);

    for (i=0; i<#{n}; i++)
    {
        aa[i] = (aa[i] + a[i])#{add_mod};
        bb[i] = (bb[i] + b[i])#{add_mod};
    }
    #{next_func}(t2, aa, bb, t);

    #{next_func}(t1, a, b, t);

#{add_ops}}
EOF
  end

  def write_mul()
    File.readlines(File.dirname(__FILE__)+'/../../rubyasm/license.c').each { |l| puts l }
    puts "#include <string.h>"
    puts "#include <stdint.h>"
    puts "#include \"ntruenc_lcl.h\""
    puts
    puts "#ifndef NTRUENC_SMALL_CODE"
    write_small()
    @r.downto(1) do |i|
      write_karatsuba(i)
    end
    puts "#endif /* NTRUENC_SMALL_CODE */"
    puts
  end
end

n=0
r=0
s = ARGV[0].to_i

case s
when 112
  q=6833
  n=439
  r=4
when 128
  q=6287
  n=491
  r=4
when 192
  q=7481
  n=659
  r=4
when 215
  q=9829
  n=739
  r=4
when 256
  q=7673
  n=881
  r=5
else
  throw "Invalid strength: #{ARGV[0]} (112|128|192|215|256)"
end

nek = NTRUENC_Karatsuba.new(s, q, n, r)
nek.write_mul()

