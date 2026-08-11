#include "test_framework.h"
#include "game/GameConfig.h"
#include "game/waves/WaveDef.h"

TEST(Config_Sanity)
{
    EXPECT_TRUE(GameConfig::WORLD_WIDTH  > 0.f);
    EXPECT_TRUE(GameConfig::WORLD_HEIGHT > 0.f);
    EXPECT_TRUE(GameConfig::PLAYER_LIVES > 0);
    EXPECT_TRUE(GameConfig::BULLET_POOL  > 0);
    EXPECT_TRUE(GameConfig::ENEMY_POOL   > 0);
    EXPECT_TRUE(GameConfig::ENEMY_BULLET_POOL > 0);
    EXPECT_TRUE(GameConfig::ITEM_POOL    > 0);
    EXPECT_TRUE(GameConfig::PARTICLE_POOL > 0);
    EXPECT_TRUE(GameConfig::BOSS_MAX_HP  > 0.f);
    // 玩法合理性：玩家子弹必须比敌机快，否则打不到；护盾必须持续到有意义
    EXPECT_TRUE(GameConfig::BULLET_SPEED > GameConfig::ENEMY_SPEED);
    EXPECT_TRUE(GameConfig::ITEM_SHIELD_TIME > 0.f);
}

TEST(Config_PoolsFitBudget)
{
    // 各池容量上限应足以支撑同屏峰值，这里只做「非负且有限」的健全性断言
    EXPECT_TRUE(GameConfig::BULLET_POOL    >= 64);
    EXPECT_TRUE(GameConfig::ENEMY_BULLET_POOL >= 64);
    EXPECT_TRUE(GameConfig::PARTICLE_POOL  >= 128);
}

TEST(WaveDef_DefaultsValid)
{
    WaveDef w;
    EXPECT_TRUE(w.count   >= 0);
    EXPECT_TRUE(w.interval > 0.f);
    EXPECT_TRUE(w.speed   > 0.f);
    EXPECT_TRUE(w.radius  > 0.f);
    EXPECT_TRUE(w.score   >= 0);
}
