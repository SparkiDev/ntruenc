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

  def initialize(str, n, r)
    @str = str
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
 */
static void ntruenc_s#{@str}_mul_mod_q_small(short *r, short *a, short *b)
{
    int i, j;
    short *p;

    for (j=0; j<#{n}; j++)
        r[j] = a[0] * b[j];
    for (i=1; i<#{n}; i++)
    {
        r[i+#{n}-1] = 0;
        p = &r[i];
        for (j=0; j<#{n}; j++)
            p[j] += a[i] * b[j];
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
    next_func = "ntruenc_s#{@str}_mul_mod_q_"
    if c == @r
      next_func += "small"
    else
      next_func += @s[c].to_s
    end
    dec_j = ""
    dec_j = ", j" if c == 1

    add_ops = ""
    if c == 1
       add_ops = <<EOF
    r[0] = t1[0];
    for (i=1,j=0; i<#{nf}; i++,j++)
        r[i] = t1[i] + t3[j];
    for (i=#{n},j=0; i<#{nf}; i++,j++)
        r[i] += t2[j] - t1[j] - t3[j];
    for (i=0; j<#{n}*2-1; i++,j++)
        r[i] += t2[j] - t1[j] - t3[j];

    for (i=0; i<#{nf}; i++)
    {
        r[i] &= NTRU_S#{@str}_Q-1;
        r[i] |= 0 - (r[i] & (1<<(NTRU_S#{@str}_Q_BITS-1)));
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
 * @param [in] r  The multiplication result.
 * @param [in] a  The first operand.
 * @param [in] b  The second operand.
 */
#{static_decl}void #{func}(short *r, short *a, short *b)
{
    int i#{dec_j};
    short t1[2*#{n}-1];
    short t2[2*#{n}-1];
    short t3[2*#{n}-1];
    short aa[#{n}];
    short bb[#{n}];

    for (i=0; i<#{nm}; i++)
    {
        aa[i] = a[i+#{n}];
        bb[i] = b[i+#{n}];
    }#{extra_ops}
    #{next_func}(t3, aa, bb);

    for (i=0; i<#{n}; i++)
    {
        aa[i] += a[i];
        bb[i] += b[i];
    }
    #{next_func}(t2, aa, bb);

    #{next_func}(t1, a, b);

#{add_ops}}
EOF
  end

  def write_mul()
    File.readlines(File.dirname(__FILE__)+'/../../rubyasm/license.c').each { |l| puts l }
    puts "#include <string.h>"
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
  n=401
  r=2
when 128
  n=439
  r=2
when 192
  n=593
  r=3
when 256
  n=743
  r=3
else
  throw "Invalid strength: #{ARGV[0]} (112|128|192|256)"
end

nek = NTRUENC_Karatsuba.new(s, n, r)
nek.write_mul()

