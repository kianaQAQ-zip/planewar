#include "Time.h"

void Time::Tick()
{
    float frame = clock_.restart().asSeconds();
    if (frame > 0.25f) frame = 0.25f;   // 防「螺旋死亡」：卡顿后不疯狂补步
    accumulator_ += frame;
}

bool Time::ShouldStep()
{
    if (accumulator_ >= FIXED_STEP)
    {
        accumulator_ -= FIXED_STEP;
        return true;
    }
    return false;
}

float Time::Alpha() const
{
    return accumulator_ / FIXED_STEP;
}
