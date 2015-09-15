loop-inline-asm
===============

Just some throwaway code to learn more about gcc's inline assembly. The point
is to create an endless loop by reading the program counter and then jumping
back to it.  The gist of it is

    __asm__ __volatile__
      ("call here\n"
        "here: pop %%rax\n"
        "      add $0xc, %%rax\n"
        "      mov %%rax, %[var]\n"
        : [var] "=r" (rip));

We use the call-pop trick to get the contents of the RIP register on x86-64.
When I tested this, I couldn't get any other tricks to work (the load effective
address trick doesn't seem to work on OSX with llvm 6.1).

The increment of eight is kinda tricky due to the fact that it relies on the
compiler not optimizing anything. That's why you probably should compile with
`-Os` instead of anything else. You can do an `objdump -d loop` on the code to
adjust this offset, but, at least, `-O3` usually garbles your code so
thoroughly that it won't work (next up: insert pragmas to deny optimizations
for the assembly blocks).  This can be seen directly by looking at the
disassembly:

      0000000100000f20 <_main>:
         100000f20:   55                      push   %rbp
         100000f21:   48 89 e5                mov    %rsp,%rbp
         100000f24:   53                      push   %rbx
         100000f25:   50                      push   %rax
         100000f26:   48 c7 45 f0 00 00 00    movq   $0x0,-0x10(%rbp)
         100000f2d:   00
    ==>  100000f2e:   e8 00 00 00 00          callq  100000f33 <here>

      0000000100000f33 <here>:
         100000f33:   58                      pop    %rax
         100000f34:   48 83 c0 08             add    $0x8,%rax
         100000f38:   48 89 c3                mov    %rax,%rbx
    ==>  100000f3b:   48 8b 75 f0             mov    -0x10(%rbp),%rsi
         100000f3f:   48 8d 46 01             lea    0x1(%rsi),%rax
         100000f43:   48 89 45 f0             mov    %rax,-0x10(%rbp)
         100000f47:   48 8d 3d 38 00 00 00    lea    0x38(%rip),%rdi        # 100000f86 <_printf$stub+0x20>
         100000f4e:   31 c0                   xor    %eax,%eax
         100000f50:   48 89 da                mov    %rbx,%rdx
         100000f53:   e8 0e 00 00 00          callq  100000f66 <_printf$stub>
         100000f58:   48 89 d8                mov    %rbx,%rax
         100000f5b:   ff e0                   jmpq   *%rax
         100000f5d:   31 c0                   xor    %eax,%eax
         100000f5f:   48 83 c4 08             add    $0x8,%rsp
         100000f63:   5b                      pop    %rbx
         100000f64:   5d                      pop    %rbp
         100000f65:   c3                      retq

Above, I've indicated (`==>`) the start and end of the first assembly block. If
you count instructions from `100000f33` to the next marker, you should arrive
at the same number as I (unless you *actually* know assembly; it still seems to
me that one store has been optimized away, and that it's just a happy
coincidence that I can reuse `rbx`. Let me know).

Later on, we just jump back to the location stored in `rip`:

    __asm__ __volatile__
      ("movq %[addr], %%rax\n"
       "jmpq *%%rax\n"
       :
       : [addr] "r" (rip));

I've tested this on 64-bit OSX using llvm 6.1 via the `gcc` command line
invocation. To test yourself, try `make run` and `make dis` to see the
disassembly.

License
-------
Put in the public domain by the author, Christian Stigen Larsen
2015-09
