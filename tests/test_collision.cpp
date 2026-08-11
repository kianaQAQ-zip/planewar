#include "test_framework.h"
#include "game/Collision.h"

TEST(Collision_Overlap)
{
    Vec2 a(0.f, 0.f), b(10.f, 0.f);
    EXPECT_TRUE(CircleIntersect(a, 6.f, b, 6.f));     // 距离 10 <= 半径和 12
}

TEST(Collision_NoOverlap)
{
    Vec2 a(0.f, 0.f), b(100.f, 0.f);
    EXPECT_FALSE(CircleIntersect(a, 6.f, b, 6.f));
}

TEST(Collision_BoundaryTouches)
{
    Vec2 a(0.f, 0.f), b(10.f, 0.f);
    EXPECT_TRUE(CircleIntersect(a, 5.f, b, 5.f));     // 距离 == 半径和
}

TEST(Collision_ZeroRadiusJustTouches)
{
    Vec2 a(0.f, 0.f), b(4.f, 0.f);
    EXPECT_TRUE(CircleIntersect(a, 0.f, b, 4.f));     // 一点接触也算相交
    EXPECT_FALSE(CircleIntersect(a, 0.f, b, 4.f - 0.01f));
}
