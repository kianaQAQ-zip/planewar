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
}
