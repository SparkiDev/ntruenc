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

class Register
  def initialize(name, size=8)
    @name = name
    @bytes = size
    case size
    when 1
      @p=""
      @name = name[0] + "l"
    when 2
      if @name.to_i(10) == 0
        @p=""
      else
        @p="r"
        @name = name[0] + "w"
      end 
    when 4
      if @name.to_i == 0
        @p="e"
      else
        @p="r"
        @name = name[0] + "d"
      end
    when 8
      @p="r"
    end
  end
  def r8()
    Register.new(@name, 1)
  end
  def r16()
    Register.new(@name, 2)
  end
  def r32()
    Register.new(@name, 4)
  end
  def r64()
    Register.new(@name, 8)
  end
  def to_s()
    "%"+@p+@name
  end
  def [](i)
    if Integer === i
      r = "(%#{@p}#{@name})"
      r = "#{i*@bytes}" + r if i > 0
    else
      r = "(%#{@p}#{@name},#{i})"
    end
    r
  end
  def +(i)
    "#{i}(%#{@p}#{@name})"
  end
end

class XMM_Register <Register
  def to_s()
    "%xmm"+@name
  end
end

class X86_64
  def nr(name)
    r = Register.new(name)
    @regs_list.each do |a|
      return r if r.to_s == a.to_s
    end
    throw "Unreserved register"
  end
  def gr(n)
    l = @regs_list[0..n-1]
    @regs_list = @regs_list[n..-1]
    l
  end
  def nxr(name)
    r = XMM_Register.new(name)
    @xmm_regs_list.each do |a|
      return r if r.to_s == a.to_s
    end
    throw "Unreserved register"
  end
  def gxr(n)
    l = @xmm_regs_list[0..n-1]
    @xmm_regs_list = @xmm_regs_list[n..-1]
    l
  end

  def grab_regs(cnt)
    s_cnt = safe_regs().length
    p_cnt = push_regs().length

    throw "To few registers available #{cnt}/#{s_cnt}+#{p_cnt}" if s_cnt+p_cnt < cnt

    s_cnt = cnt if s_cnt > cnt
    r = safe_regs()[0..s_cnt-1]
    return r if cnt == s_cnt
    @pr = push_regs()[0..cnt-s_cnt-1]
    save_regs()
    r + @pr
  end

  def grap_xmm_regs(cnt)
    x_cnt = xmm_regs().length
    throw "To few XMM registers available #{cnt}/#{x_cnt}" if x_cnt < cnt
    xmm_regs()
  end
end

class GCC_X86_64 <X86_64

  def method_missing(method_sym, *arguments, &block)
    case method_sym.to_s
    when /not_/
      print "\tnot\t"
    when /and_/
      print "\tand\t"
    when /or_/
      print "\tor\t"
    else
      print "\t#{method_sym}\t"
    end
    0.upto(arguments.length-1) do |i|
      if Integer === arguments[i]
        print "$"+arguments[i].to_s
      else
        print arguments[i]
      end
      print ", " if i < arguments.length-1
    end
    puts
  end

  def initialize()
    super
    @safe_regs = []
    @push_regs = []
    @xmm_regs = []
    @param_regs = []
    ["ax", "cx", "dx", "8", "9", "10", "11"].each do |n|
      @safe_regs << Register.new(n)
    end
    ["bx", "12", "13", "14", "15", "bp", "sp", "si", "di"].each do |n|
      @push_regs << Register.new(n)
    end
    ["0", "1", "2", "3", "4", "5", "6", "7"].each do |n|
      @xmm_regs << XMM_Register.new(n)
    end
    ["di", "si", "dx", "cx", "8", "9"].each do |n|
      @param_regs << Register.new(n)
    end
    @pr=[]
  end
  def safe_regs()
    @safe_regs
  end
  def push_regs()
    @push_regs
  end
  def xmm_regs()
    @xmm_regs
  end
  def param_regs()
    @param_regs
  end
  def param_reg(i)
    @param_regs[i]
  end

  def save_regs()
    @pr.each do |r|
      pushq r
    end
  end
  def restore_regs()
    return if not @pr
    @pr.reverse.each do |r|
      popq r
    end
  end

  def stack_p()
    Register.new("sp")
  end
  def reserve_stack()
    return if @stack == 0
    subq "$"+@stack.to_s, stack_p
  end
  def restore_stack()
    return if @stack == 0
    addq "$"+@stack.to_s, stack_p
  end

  def header()
    File.readlines(File.dirname(__FILE__)+'/license.s').each { |l| puts l }
    puts "\t.file\t\"#{@file}\""
  end

  def start_func()
    puts "\t.text"
    puts "\t.p2align 4,,15"
    puts "\t.globl\t#{@name}"
    puts "\t.type\t#{@name}, @function"
    puts "#{@name}:"
  end
  def end_func()
    puts "\t.size\t#{@name}, .-#{@name}"
  end

  def file(name)
    @file = name
    header
  end

  def func(name, reg_cnt, xmm_reg_cnt=0, stack=0)
    @name = name
    @stack = stack
    start_func
    @regs_list = grab_regs(reg_cnt)
    @xmm_regs_list = grap_xmm_regs(xmm_reg_cnt)
    reserve_stack()
  end

  def stack_align_16()
    if (@stack + @pr.length * 8) & 0x15
      subq 8, stack_p
      @stack += 8
    end
  end

  def func_done()
    restore_stack()
    restore_regs()
    puts "\tret"
    end_func
  end

  def ext_func(name)
    name
  end

  def set_label(name)
    puts ".L#{name}:"
    ".L#{name}"
  end
  def label(name)
    ".L#{name}"
  end
  def set_const(name)
    puts ".Lconst_#{name}:"
    ".Lconst_#{name}"
  end
  def const(name)
    ".Lconst_#{name}(%rip)"
  end
  def comment(a="")
    a = " "+a if a != ""
    puts "\t##{a}"
  end

  def start_data()
    puts "\t.section        .rodata.cst16,\"aM\",@progbits,16"
    puts "\t.align 16"
  end
  def set_value(size, val)
    puts "\t.value #{val}"
  end
end

class MacOSX_X86_64 <GCC_X86_64

  def start_func()
    puts "\t.section\t__TEXT,__text,regular,pure_instructions"
    puts "\t.macosx_version_min 10, 10"
    puts "\t.globl\t_#{@name}"
    puts "\t.align\t4, 0x90"
    puts "_#{@name}:\t\t\t## @#{@name}"
  end
  def end_func()
  end

  def stack_align_16()
    if (@stack + @pr.length * 8) & 0x15
      subq 8, stack_p
      @stack += 8
    end
  end

  def ext_func(name)
    "_"+name
  end

  def set_label(name)
    puts "L#{name}:"
    "L#{name}"
  end
  def label(name)
    "L#{name}"
  end
  def set_const(name)
    puts "Lconst_#{name}:"
    "Lconst_#{name}"
  end
  def const(name)
    "Lconst_#{name}(%rip)"
  end

  def start_data()
    puts "\t.section\t__DATA,__data"
    puts "\t.align 16"
  end
  def set_value(size, val)
    puts "\t.value #{val}"
  end
end
