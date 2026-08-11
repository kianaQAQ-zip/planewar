#pragma once
#include <SFML/System.hpp>

// 固定步长时钟：逻辑按恒定 60Hz 更新，与渲染帧率彻底解耦。
class Time
{
public:
    static constexpr float FIXED_STEP = 1.0f / 60.0f;  // 固定逻辑步长（秒）

    Time() = default;

    void Tick();              // 每帧调用：累加真实帧时间
    bool ShouldStep();        // 返回是否该推进一次固定逻辑步
    float Alpha() const;      // 渲染插值因子 [0,1)，渲染平滑用

private:
    sf::Clock clock_;
    float accumulator_ = 0.0f;
};
