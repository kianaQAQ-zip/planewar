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
#include "Boss.h"
#include "PowerUp.h"
#include "Particle.h"
#include <cstdlib>

class AssetManager;   // 前向声明：World 头文件不引入 SFML（保持 Game 层头文件干净）
class Renderer;

// 游戏世界编排器（Game 层的总指挥）。
// 持有玩家 + 玩家子弹池 + 敌弹池 + 敌机池 + 道具池 + 粒子池 + Boss +
// 事件总线 + 波次管理器 + 分数管理器 + HUD，负责：
// 加载资源、固定步推进、生成敌人/Boss/道具、碰撞结算、音效/粒子、渲染。
// App 层只跟它打交道——依赖注入式的「显式编排」，没有任何全局隐式状态。
class World
{
public:
    World()
        : bullets_(GameConfig::BULLET_POOL)
        , enemyBullets_(GameConfig::ENEMY_BULLET_POOL)
        , enemies_(GameConfig::ENEMY_POOL)
        , items_(GameConfig::ITEM_POOL)
        , particles_(GameConfig::PARTICLE_POOL)
    {
        std::srand(12345);              // 固定种子，demo 行为可复现
        scoreManager_.RegisterEvents(bus_);                                  // 分数订阅一次
        bus_.Subscribe<BossFireEvent>([this](const BossFireEvent& e)         // Boss 发射 → 生成敌弹
        {
            SpawnBossBullets(e);
        });
        Reset();
    }

    // 加载贴图与字体（缺失也不报错，Renderer/HUD 会用圆形占位 / 跳过文本）。
    // 音效在此登记，缺失同样静默降级。定义在 World.cpp。
    void LoadAssets(AssetManager& assets);

    void Update(float dt, const InputState& input);
    void Render(Renderer& renderer);

    int  GetScore()    const { return scoreManager_.GetScore(); }
    bool IsGameOver()  const { return gameOver_ || won_; }
    bool HasWon()      const { return won_; }
    void Reset();

private:
    void HandleCollisions();
    void SpawnBossBullets(const BossFireEvent& e);
    void SpawnExplosion(const Vec2& pos, unsigned int color, int count);
    void SpawnItem();

    Player                player_;
    ObjectPool<Bullet>    bullets_;        // 玩家子弹
    ObjectPool<Bullet>    enemyBullets_;   // 敌弹 / Boss 弹
    ObjectPool<Enemy>     enemies_;
    ObjectPool<PowerUp>   items_;
    ObjectPool<Particle>  particles_;
    Boss                  boss_;
    EventBus              bus_;            // 事件中枢：碰撞 → 加分 / 发射 解耦
    WaveManager           waveManager_;    // 数据驱动波次生成
    ScoreManager          scoreManager_;   // 分数（事件订阅者）
    Hud                   hud_;            // 分数/生命/波次/Boss血条
    bool                  gameOver_    = false;
    bool                  won_        = false;
    bool                  bossSpawned_ = false;
    float                 itemTimer_   = 0.f;
};
