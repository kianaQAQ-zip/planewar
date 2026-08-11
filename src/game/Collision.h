#pragma once
#include "core/Math.h"

// 圆形碰撞：两个圆相交当且仅当圆心距 <= 半径之和。
// 用距离平方比较，避免每帧开方（性能细节，面试可讲）。
inline bool CircleIntersect(const Vec2& a, float ra, const Vec2& b, float rb)
{
    const float r = ra + rb;
    return DistanceSq(a, b) <= r * r;
}
