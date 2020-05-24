#pragma once
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cstring>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <map>
#include <iostream>

#include "DSL.h"
#include "x86commandset.h"

namespace Consts {
	constexpr int BUF_SIZE = 4096;
};

struct cmp_str {
	bool operator()(const char* a, const char* b){
		return strcmp(a, b) < 0;
	}
};
