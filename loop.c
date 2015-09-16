/*
 * This is just to learn more about inline assembly. I'm getting the program
 * pointer and use that to create an infinite loop.
 *
 * Compile with: gcc -g -Os loop.c -oloop
 * Disassemble with: objdump -d loop
 *
 * Works only on x86-64.
 * Tested on Mac OSX with gcc/llvm 6.1.0 and Linux with gcc 4.4.7.
 *
 * Written by Christian Stigen Larsen
 */

#include <stdio.h>

int main()
{
  volatile long n = 0;
  unsigned long rip;

  // Get the current program counter.
  __asm__ __volatile__
    ("call here\n"
      "here: pop %%rax\n"
      "      add $0x8, %%rax\n"
      "      mov %%rax, %[var]\n"
      : [var] "=r" (rip));

  // Print an incrementing counter and the jump position
  printf("n=%ld rip=0x%lx\n", n++, rip);

  // Jump back to the first instruction after the assembly block.
  __asm__ __volatile__
    ("movq %[addr], %%rax\n"
     "jmpq *%%rax\n"
     :
     : [addr] "r" (rip));

  return 0;
}
