#pragma once
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"
#include "ObjectPool.h"
#include "Collision.h"
#include "GameConfig.h"
#include "Input.h"
#include <cstdlib>

class AssetManager;   // 前向声明：World 头文件不引入 SFML（保持 Game 层头文件干净）
class Renderer;

// 游戏世界编排器（Game 层的总指挥）。
// 持有玩家 + 子弹池 + 敌机池，负责：加载资源、固定步推进、生成敌机、碰撞结算、渲染。
// 它是 Game 层内部的总线，App 层只跟它打交道，不直接碰任何实体。
class World
{
public:
    World()
        : bullets_(GameConfig::BULLET_POOL)
        , enemies_(GameConfig::ENEMY_POOL)
    {
        std::srand(12345);   // 固定种子，demo 行为可复现
        Reset();
    }

    // 加载贴图（缺失也不报错，Renderer 会用圆形占位）。定义在 World.cpp。
    void LoadAssets(AssetManager& assets);

    void Update(float dt, const InputState& input);
    void Render(Renderer& renderer);

    int  GetScore()    const { return score_; }
    bool IsGameOver()  const { return gameOver_; }
    void Reset();

private:
    void SpawnEnemies(float dt);
    void HandleCollisions();

    Player                player_;
    ObjectPool<Bullet>    bullets_;
    ObjectPool<Enemy>     enemies_;
    int                   score_     = 0;
    float                 spawnTimer_ = 0.f;
    bool                  gameOver_   = false;
};
