#include "test_framework.h"
#include "game/events/EventBus.h"

TEST(EventBus_SubscribeAndEmit)
{
    EventBus bus;
    int hits = 0;
    bus.Subscribe<int>([&](const int&) { ++hits; });
    bus.Emit(42);
    EXPECT_EQ(hits, 1);
    bus.Emit(7);
    EXPECT_EQ(hits, 2);
}

TEST(EventBus_MultipleSubscribers)
{
    EventBus bus;
    int a = 0, b = 0;
    bus.Subscribe<int>([&](const int&) { ++a; });
    bus.Subscribe<int>([&](const int&) { ++b; });
    bus.Emit(1);
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 1);
}

TEST(EventBus_TypeSafe)
{
    EventBus bus;
    int intHits = 0;
    bus.Subscribe<int>([&](const int&) { ++intHits; });
    bus.Emit(3.14f);                         // float 事件不会分发到 int 订阅者
    EXPECT_EQ(intHits, 0);
}

TEST(EventBus_NoSubscriberSilent)
{
    EventBus bus;
    bus.Emit(123);                           // 无订阅者：应静默返回，不崩溃
}

TEST(EventBus_Clear)
{
    EventBus bus;
    int hits = 0;
    bus.Subscribe<int>([&](const int&) { ++hits; });
    bus.Clear();
    bus.Emit(1);
    EXPECT_EQ(hits, 0);
}

TEST(EventBus_BossFireEvent)
{
    EventBus bus;
    int fires = 0;
    Vec2 lastOrigin{};
    bus.Subscribe<BossFireEvent>([&](const BossFireEvent& e) {
        ++fires;
        lastOrigin = e.origin;
    });
    bus.Emit(BossFireEvent{ {10.f, 20.f}, 2, {5.f, 5.f} });
    EXPECT_EQ(fires, 1);
    EXPECT_FLOAT_EQ(lastOrigin.x, 10.f);
    EXPECT_FLOAT_EQ(lastOrigin.y, 20.f);
}
