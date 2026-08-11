#pragma once

// 全局游戏常量（Game 层，与引擎无关）。
// 实体出界判定、玩家移动范围都从这里取，方便统一调整。
namespace GameConfig
{
    constexpr float WORLD_WIDTH  = 800.f;
    constexpr float WORLD_HEIGHT = 600.f;

    constexpr int   PLAYER_LIVES      = 3;
    constexpr float PLAYER_SPEED      = 320.f;   // px/s
    constexpr float PLAYER_FIRE_RATE  = 0.18f;   // 两发间隔（秒），约 5–6 发/秒
    constexpr float PLAYER_RADIUS     = 18.f;

    constexpr float BULLET_SPEED   = 600.f;      // px/s，向上
    constexpr float BULLET_RADIUS  = 6.f;
    constexpr int   BULLET_POOL    = 256;

    constexpr float ENEMY_SPEED    = 130.f;      // px/s，向下
    constexpr float ENEMY_RADIUS   = 22.f;
    constexpr int   ENEMY_POOL     = 64;
    constexpr float ENEMY_SPAWN_INTERVAL = 1.2f; // 秒

    constexpr int   SCORE_PER_KILL = 100;
    constexpr int   BOSS_SCORE     = 5000;

    // ---- M3：敌弹 / 道具 / 粒子 / Boss ----
    constexpr int   ENEMY_BULLET_POOL    = 256;
    constexpr float ENEMY_BULLET_SPEED   = 300.f;
    constexpr float ENEMY_BULLET_RADIUS  = 7.f;

    constexpr int   ITEM_POOL            = 16;
    constexpr float ITEM_SPEED          = 90.f;
    constexpr float ITEM_RADIUS         = 16.f;
    constexpr float ITEM_SPAWN_INTERVAL = 10.f;   // 平均 ~10s 掉一个道具
    constexpr float ITEM_SHIELD_TIME    = 6.f;    // 护盾持续秒数

    constexpr int   PARTICLE_POOL        = 512;

    constexpr float BOSS_RADIUS         = 60.f;
    constexpr float BOSS_MAX_HP         = 60.f;   // 约 60 发命中
    constexpr float BOSS_SPEED_ENTER    = 80.f;   // 进场下移速度
    constexpr float BOSS_ENTER_Y        = 110.f;  // 进场停驻 Y
    constexpr float BOSS_FIRE_INTERVAL_P1 = 1.4f;
    constexpr float BOSS_FIRE_INTERVAL_P2 = 0.8f; // 狂暴阶段更密集

    constexpr float PLAYER_HIT_INVULN   = 1.2f;   // 受击后无敌时间，避免一帧多扣命
}
