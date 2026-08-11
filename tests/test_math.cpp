#include "test_framework.h"
#include "core/Math.h"

TEST(Vec2_Arithmetic)
{
    Vec2 a(1.f, 2.f);
    Vec2 b(3.f, 4.f);

    auto s = a + b;
    EXPECT_FLOAT_EQ(s.x, 4.f);
    EXPECT_FLOAT_EQ(s.y, 6.f);

    auto d = b - a;
    EXPECT_FLOAT_EQ(d.x, 2.f);
    EXPECT_FLOAT_EQ(d.y, 2.f);

    auto m = a * 2.f;
    EXPECT_FLOAT_EQ(m.x, 2.f);
    EXPECT_FLOAT_EQ(m.y, 4.f);

    Vec2 acc = a;
    acc += b;
    EXPECT_FLOAT_EQ(acc.x, 4.f);
    EXPECT_FLOAT_EQ(acc.y, 6.f);
}

TEST(Vec2_Length)
{
    Vec2 v(3.f, 4.f);
    EXPECT_FLOAT_EQ(v.LengthSq(), 25.f);
    EXPECT_FLOAT_EQ(v.Length(), 5.f);
}

TEST(Vec2_DistanceSq)
{
    Vec2 a(0.f, 0.f);
    Vec2 b(3.f, 4.f);
    EXPECT_FLOAT_EQ(DistanceSq(a, b), 25.f);
}
