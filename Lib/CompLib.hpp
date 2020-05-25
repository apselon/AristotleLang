#pragma once
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <cassert>
#include <cstdint>

#include "DSL.h"
#include "x86commandset.h"
#include "../Vector/Vector.hpp"
#include "../HashTable/HashTable.cpp"

namespace Consts {
	constexpr int BUF_SIZE = 4096;
};

inline unsigned long hash(const char* data){
    unsigned long h = 0;

    asm(R"(
        .intel_syntax noprefix
        lea rax, [%1]
        xor %0, %0
    looop:
        crc32 %0, byte ptr [rax]
        inc rax
        cmp byte ptr [rax], 0
        jne looop
        .att_syntax prefix
    )"
        : "=r"(h)
        : "r"(data)
        : "rax", "rcx"
	);

    return h;
}
