#pragma once
#include "xorstr.h"

// assembler opcode defines for inline asm
#define ASM_OP_JMP_EB 0xEB
// #define ASM_OPCODE_NOP            0x90

// size of the full assembler instruction in bytes
#define ASM_OP_JMP_EB_SIZE 0x2
// #define ASM_INSTR_SIZE_PUSH        0x1
// #define ASM_INSTR_SIZE_NOP        0x1

// length of your pattern in bytes
#define MARKER_SIZE 10

// the pattern of the marker and its mask
#define MARKER_BYTES xorstr("\x52\x90\x5A\x9C\x9D\x90\x50\x58\x60\x61").crypt_get()
#define MARKER_MASK xorstr("xxxxxxxxxx").crypt_get()

// inline macro assembler | \x52\x90\x5A\x9C\x9D\x90\x50\x58\x60\x61 | 52 90 5A 9C 9D 90 50 58 60 61
#define ROLLED __asm    \
{						\
    __asm push edx      \
    __asm nop			\
    __asm pop edx		\
    __asm pushfd		\
    __asm popfd         \
    __asm nop			\
    __asm push eax		\
    __asm pop eax		\
    __asm pushad		\
    __asm popad         \
}