/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

// Visual Studio testing dependencies
#include "CppUnitTest.h"

// Testing dependencies
#include "test_spu.h"

#define TEST_METHOD_INSTRUCTION(method) \
    TEST_METHOD_CATEGORY(method, L"SPU Tests") { test.##method##(); }

TEST_CLASS(SPUTests) {
    SPUTestRunner test;

public:
#define INSTRUCTION(name) TEST_METHOD_INSTRUCTION(name)
#include "test_spu.inl"
#undef INSTRUCTION
};
