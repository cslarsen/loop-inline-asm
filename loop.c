/*
 * This is just to learn more about inline assembly. I'm getting the program
 * pointer and use that to create an infinite loop.
 *
 * Compile with: gcc -g -Os loop.c -oloop
 * Disassemble with: objdump -d loop
 *
 * Works only on x86-64.
 * Tested on Mac OSX with gcc/llvm 6.1.0.
 *
 * Written by Christian Stigen Larsen
 */

#include <stdio.h>

int main()
{
  volatile long n = 0;
  unsigned long rip;

  // Get current program counter
  asm("call here\n"
      "here: pop %[var]\n"
      : [var] "=r" (rip));

  /*
   * At this point, we can either subtract 5 from rip, so we jump back to the
   * "call" instruction (see objdump; depends on optimization level), or we can
   * advance it by 4 to get to the start of the printf call setup.  Obviously,
   * we only need to get the address once, so we choose to advance by four.
   */
  rip += 4;

  printf("n=%ld rip=0x%lx\n", n++, rip);

  // Jump back to original RIP
  asm("movq %[addr], %%rax\n"
      "jmpq *%%rax\n"
      :
      : [addr] "r" (rip));

  return 0;
}
