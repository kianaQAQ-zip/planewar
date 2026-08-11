#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

// 极简单元测试框架（M4 自研，零第三方依赖）
// ---------------------------------------------------------------------------
// 设计取舍：不引入 Catch2 / GoogleTest，避免给入门 demo 增加网络与依赖负担。
// 一个 ~50 行的「注册表式」harness 足以覆盖本项目的纯逻辑单元。
// 被测单元（Math / ObjectPool / EventBus / Collision / GameConfig / WaveDef）
// 全部是 SFML-free 头文件，因此测试目标可脱离游戏 exe 独立编译运行——
// 这一点本身也是「Game 层不依赖 SFML」架构纪律的副产品。
//
// 用法：在 .cpp 里写 TEST(name) { EXPECT_*(...); }，main 由 test_main.cpp 提供。

struct TestCase
{
    const char* name;
    void (*fn)();
};

inline std::vector<TestCase>& TestRegistry()
{
    static std::vector<TestCase> r;
    return r;
}

inline int& TestFailures()
{
    static int n = 0;
    return n;
}

struct TestRegistrar
{
    TestRegistrar(const char* name, void(*fn)())
    {
        TestRegistry().push_back({name, fn});
    }
};

// 每个 TEST 通过静态对象在 main 之前完成注册。
#define TEST(name) \
    static void name(); \
    static ::TestRegistrar _reg_##name(#name, &name); \
    static void name()

#define TEST_FAIL(msg) \
    do { std::cout << "    ✗ " << (msg) << "  (" << __FILE__ << ":" << __LINE__ << ")\n"; \
         ++::TestFailures(); } while (0)

#define EXPECT_TRUE(cond) \
    do { if (!(cond)) TEST_FAIL("EXPECT_TRUE(" #cond ")"); } while (0)

#define EXPECT_FALSE(cond) \
    do { if ( (cond)) TEST_FAIL("EXPECT_FALSE(" #cond ")"); } while (0)

#define EXPECT_EQ(a, b) \
    do { auto _a = (a); auto _b = (b); \
         if (!(_a == _b)) { \
             std::cout << "    ✗ EXPECT_EQ(" #a ", " #b ")  got [" \
                       << _a << "] vs [" << _b << "]  (" \
                       << __FILE__ << ":" << __LINE__ << ")\n"; \
             ++::TestFailures(); } } while (0)

#define EXPECT_FLOAT_EQ(a, b) \
    do { auto _a = (a); auto _b = (b); \
         if (std::fabs(static_cast<double>(_a) - static_cast<double>(_b)) > 1e-5) \
             TEST_FAIL("EXPECT_FLOAT_EQ(" #a ", " #b ")"); } while (0)
