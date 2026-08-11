#pragma once
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "ObjectPool.h"
#include "Collision.h"
#include "GameConfig.h"
#include "Input.h"
#include "events/EventBus.h"
#include "waves/WaveManager.h"
#include "ScoreManager.h"
#include "ui/Hud.h"
#include <cstdlib>

class AssetManager;   // 前向声明：World 头文件不引入 SFML（保持 Game 层头文件干净）
class Renderer;

// 游戏世界编排器（Game 层的总指挥）。
// 持有玩家 + 子弹池 + 敌机池 + 波次管理器 + 分数管理器 + HUD + 事件总线，
// 负责：加载资源、固定步推进、生成敌机、碰撞结算、渲染。
// App 层只跟它打交道，不直接碰任何实体——这就是依赖注入式的「显式编排」。
class World
{
public:
    World()
        : bullets_(GameConfig::BULLET_POOL)
        , enemies_(GameConfig::ENEMY_POOL)
    {
        std::srand(12345);              // 固定种子，demo 行为可复现
        scoreManager_.RegisterEvents(bus_);  // 事件订阅注册一次，贯穿全局
        Reset();
    }

    // 加载贴图与字体（缺失也不报错，Renderer/HUD 会用圆形占位 / 跳过文本）。定义在 World.cpp。
    void LoadAssets(AssetManager& assets);

    void Update(float dt, const InputState& input);
    void Render(Renderer& renderer);

    int  GetScore()    const { return scoreManager_.GetScore(); }
    bool IsGameOver()  const { return gameOver_; }
    void Reset();

private:
    void HandleCollisions();

    Player                player_;
    ObjectPool<Bullet>    bullets_;
    ObjectPool<Enemy>     enemies_;
    EventBus              bus_;          // 事件中枢：碰撞 → 加分 解耦
    WaveManager           waveManager_;  // 数据驱动波次生成
    ScoreManager          scoreManager_; // 分数（事件订阅者）
    Hud                   hud_;          // 分数/生命/波次展示
    bool                  gameOver_   = false;
};
