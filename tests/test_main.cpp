// 单元测试入口（M4）。聚合 tests/ 下所有注册的 TEST，统一跑并统计结果。
// 不依赖 SFML：被测单元全是 SFML-free 头文件，本目标也不链接 SFML。
#include "test_framework.h"

int main()
{
    const std::size_t total = TestRegistry().size();
    std::cout << "==== PlaneWar 单元测试 (" << total << " cases) ====\n";

    int failedCases = 0;
    for (const auto& t : TestRegistry())
    {
        TestFailures() = 0;
        std::cout << "[RUN] " << t.name << "\n";
        t.fn();
        if (TestFailures() > 0)
        {
            std::cout << "  -> FAILED (" << TestFailures() << " assertion(s))\n";
            ++failedCases;
        }
        else
        {
            std::cout << "  -> ok\n";
        }
    }

    std::cout << "\n==== " << (total - static_cast<std::size_t>(failedCases))
              << "/" << total << " 通过，失败 " << failedCases << " ====\n";
    return failedCases == 0 ? 0 : 1;
}
