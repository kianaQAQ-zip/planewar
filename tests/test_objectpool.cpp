#include "test_framework.h"
#include "game/ObjectPool.h"
#include <cstddef>

// 满足 ObjectPool 契约的最小哑类型：SetActive / IsActive / OnSpawn。
struct Dummy
{
    bool active = false;
    int  spawns = 0;
    void SetActive(bool b) { active = b; }
    bool IsActive() const  { return active; }
    void OnSpawn()         { ++spawns; }
};

TEST(ObjectPool_AcquireAndRelease)
{
    ObjectPool<Dummy> pool(4);
    EXPECT_EQ(pool.Capacity(), static_cast<std::size_t>(4));
    EXPECT_EQ(pool.ActiveCount(), static_cast<std::size_t>(0));

    Dummy* p = pool.Acquire();
    EXPECT_TRUE(p != nullptr);
    EXPECT_TRUE(p->IsActive());
    EXPECT_EQ(p->spawns, 1);                 // Acquire 调用了 OnSpawn
    EXPECT_EQ(pool.ActiveCount(), static_cast<std::size_t>(1));

    pool.Release(p);
    EXPECT_FALSE(p->IsActive());
    EXPECT_EQ(pool.ActiveCount(), static_cast<std::size_t>(0));
}

TEST(ObjectPool_FullReturnsNull)
{
    ObjectPool<Dummy> pool(2);
    Dummy* a = pool.Acquire();
    Dummy* b = pool.Acquire();
    EXPECT_TRUE(a != nullptr && b != nullptr);
    EXPECT_EQ(pool.ActiveCount(), static_cast<std::size_t>(2));

    Dummy* c = pool.Acquire();               // 池满
    EXPECT_TRUE(c == nullptr);
}

TEST(ObjectPool_ReusesFreedSlot)
{
    ObjectPool<Dummy> pool(2);
    Dummy* a = pool.Acquire();
    pool.Release(a);

    Dummy* a2 = pool.Acquire();              // 应复用同一个槽位（零分配）
    EXPECT_TRUE(a2 == a);
    EXPECT_EQ(pool.ActiveCount(), static_cast<std::size_t>(1));
}

TEST(ObjectPool_ForEachActiveOnly)
{
    ObjectPool<Dummy> pool(3);
    Dummy* a = pool.Acquire();
    Dummy* b = pool.Acquire();
    pool.Release(b);                         // 仅 a 仍激活

    int visited = 0;
    pool.ForEachActive([&](Dummy& d) { ++visited; (void)d; });
    EXPECT_EQ(visited, 1);
    EXPECT_TRUE(a->IsActive());
}
