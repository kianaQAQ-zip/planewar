#include "World.h"
#include "core/AssetManager.h"

void World::LoadAssets(AssetManager& assets)
{
    // 资源登记集中在此：缺图不报错（Renderer 用圆形占位），缺字体不报错（HUD 跳过文本）。
    assets.LoadFont("assets/font.ttf");

    assets.LoadTexture(SpriteId::Background,   "assets/background.png");
    assets.LoadTexture(SpriteId::Player,       "assets/player.png");
    assets.LoadTexture(SpriteId::EnemyBasic,   "assets/enemy-basic.png");
    assets.LoadTexture(SpriteId::EnemyFast,    "assets/enemy-fast.png");
    assets.LoadTexture(SpriteId::EnemyTank,    "assets/enemy-tank.png");
    assets.LoadTexture(SpriteId::BulletPlayer, "assets/bullet.png");
}

void World::Reset()
{
    player_.Reset({GameConfig::WORLD_WIDTH * 0.5f, GameConfig::WORLD_HEIGHT - 60.f});

    bullets_.ForEachActive([](Bullet& b) { b.SetActive(false); });
    enemies_.ForEachActive([](Enemy& e)  { e.SetActive(false);  });

    scoreManager_.Reset();
    waveManager_.Reset();
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
    waveManager_.Update(dt, enemies_);   // 数据驱动生成敌机（取代 M1 的随机生成）
    enemies_.ForEachActive([dt](Enemy& e) { e.Update(dt); });

    // 3) 碰撞结算（子弹×敌机计分；玩家×敌机扣命）
    HandleCollisions();
}

void World::HandleCollisions()
{
    auto& bs = bullets_.Raw();
    auto& es = enemies_.Raw();

    // 子弹 × 敌机：命中则双方回收，并 Emit 击杀事件（由 ScoreManager 订阅加分）
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
                // 不直接加分，而是发事件——碰撞系统和计分系统互不认识（解耦）。
                bus_.Emit(EnemyKilledEvent{ GameConfig::SCORE_PER_KILL });
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
                bus_.Emit(PlayerHitEvent{ player_.GetLives() });  // 事件留给后续音效/闪屏
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

    // HUD：分数 / 生命 / 波次（World 把当前数值传给 HUD，HUD 只负责画）
    hud_.Draw(renderer, scoreManager_.GetScore(), player_.GetLives(), waveManager_.GetCurrentWave());
}
