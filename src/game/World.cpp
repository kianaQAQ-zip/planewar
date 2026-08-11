#include "World.h"
#include "core/AssetManager.h"

void World::LoadAssets(AssetManager& assets)
{
    // 缺失也不报错：Renderer 会用彩色圆占位，没有美术素材也能看到 demo 跑起来
    assets.LoadTexture(SpriteId::Player,       "assets/player.png");
    assets.LoadTexture(SpriteId::EnemyBasic,   "assets/enemy-basic.png");
    assets.LoadTexture(SpriteId::BulletPlayer, "assets/bullet.png");
}

void World::Reset()
{
    player_.Reset({GameConfig::WORLD_WIDTH * 0.5f, GameConfig::WORLD_HEIGHT - 60.f});

    bullets_.ForEachActive([](Bullet& b) { b.SetActive(false); });
    enemies_.ForEachActive([](Enemy& e)  { e.SetActive(false);  });

    score_      = 0;
    spawnTimer_ = 0.f;
    gameOver_   = false;
}

void World::Update(float dt, const InputState& input)
{
    if (gameOver_)
        return;

    // 1) 玩家输入 + 自身移动
    player_.HandleInput(input, bullets_);
    player_.Update(dt);

    // 2) 池内对象各自推进
    bullets_.ForEachActive([dt](Bullet& b) { b.Update(dt); });
    SpawnEnemies(dt);
    enemies_.ForEachActive([dt](Enemy& e) { e.Update(dt); });

    // 3) 碰撞结算（子弹×敌机计分；玩家×敌机扣命）
    HandleCollisions();
}

void World::SpawnEnemies(float dt)
{
    spawnTimer_ -= dt;
    if (spawnTimer_ > 0.f)
        return;
    spawnTimer_ = GameConfig::ENEMY_SPAWN_INTERVAL;

    if (Enemy* e = enemies_.Acquire())
    {
        const float r = e->GetRadius();
        const float x = r + (static_cast<float>(std::rand()) / RAND_MAX) *
                            (GameConfig::WORLD_WIDTH - 2.f * r);
        e->SetPosition({x, -r});
    }
}

void World::HandleCollisions()
{
    auto& bs = bullets_.Raw();
    auto& es = enemies_.Raw();

    // 子弹 × 敌机：命中则双方回收并加分
    for (auto& b : bs)
    {
        if (!b.IsActive()) continue;
        for (auto& e : es)
        {
            if (!e.IsActive()) continue;
            if (CircleIntersect(b.GetPosition(), b.GetRadius(),
                                e.GetPosition(), e.GetRadius()))
            {
                b.SetActive(false);
                e.SetActive(false);
                score_ += GameConfig::SCORE_PER_KILL;
                break;   // 这颗子弹已消失，停止检查它
            }
        }
    }

    // 玩家 × 敌机：命中则敌机回收、玩家扣命；命数耗尽则游戏结束
    if (!player_.IsDead())
    {
        for (auto& e : es)
        {
            if (!e.IsActive()) continue;
            if (CircleIntersect(player_.GetPosition(), player_.GetRadius(),
                                e.GetPosition(), e.GetRadius()))
            {
                e.SetActive(false);
                player_.LoseLife();
                if (player_.IsDead())
                    gameOver_ = true;
                break;
            }
        }
    }
}

void World::Render(Renderer& renderer)
{
    player_.Draw(renderer);
    bullets_.ForEachActive([&](const Bullet& b) { b.Draw(renderer); });
    enemies_.ForEachActive([&](const Enemy&  e) { e.Draw(renderer);  });
}
